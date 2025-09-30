#include <boost/stacktrace.hpp>
#include <csignal>
#include <unistd.h>

namespace
{

#define X_SIGNALS(f) \
  f(SEGV) \
  f(ABRT) \
  f(BUS) \
  f(ILL) \
  f(FPE) \
  f(TRAP) \
  f(SYS)

struct Str
{
  char const* ptr;
  std::size_t len;

  constexpr Str(char const* s, std::size_t len)
    : ptr(s)
    , len(len)
  {}

  template<std::size_t N>
  constexpr Str(char const(&s)[N])
    : ptr(s)
    , len(N - 1)
  {}

  bool ieq(Str other) const
  {
    if (other.len != len)
    {
      return false;
    }

    auto lower = [](char c) { return static_cast<uint8_t>(c) | 0b100000; };

    #define CASE(n) case n: if (lower(ptr[n-1]) != lower(other.ptr[n-1])) return false;

    switch (len)
    {
      CASE(4) [[fallthrough]];
      CASE(3) [[fallthrough]];
      CASE(2) [[fallthrough]];
      CASE(1)
    }

    #undef CASE

    return true;
  }
};

struct Signal
{
  Str name;
  int num;
};

#define D(s) Signal {#s, SIG##s},
constexpr Signal signal_defs[] = { X_SIGNALS(D) };
#undef D

constexpr auto signal_name_len_max = []{
  std::size_t n = 0;
  for (auto s : signal_defs)
  {
    n = (n < s.name.len) ? s.name.len : n;
  }
  return n;
}();
static_assert(signal_name_len_max <= 4, "ieq() update required");


int signal_num_of(Str name)
{
  auto find = [](Str name){
    for (auto const& sig : signal_defs)
    {
      if (sig.name.ieq(name))
      {
        return sig.num;
      }
    }
    return -1;
  };

  int ret = find(name);
  // not fount, skip SIG prefix
  if (ret == -1 && name.len > 3 && Str(name.ptr, 3).ieq("sig"))
  {
    ret = find({name.ptr + 3, name.len - 3});
  }
  return ret;
}

void print_err(Str s)
{
  write(STDERR_FILENO, s.ptr, s.len);
}

void sigabort_handler(int signum)
{
  ::signal(signum, SIG_DFL);
  auto s = to_string(boost::stacktrace::stacktrace());
  print_err({s.data(), s.size()});
  ::raise(signum);
}

int
__attribute__((constructor))
segabort_init(void)
{
  const char *signals = getenv("SEGFAULT_SIGNALS");
  int error = 0;

  // no signal specified, use default
  if (!signals)
  {
    signal(SIGSEGV, sigabort_handler);
    signal(SIGABRT, sigabort_handler);
  }
  // enable user-configured signals
  else
  {
    auto is_sep = [](char c) { return c == ',' || c == ' '; };
    int signum;

    while (*signals)
    {
      // skip separator
      while (is_sep(*signals))
      {
        ++signals;
      }

      // no more value, stop the parser
      if (!*signals)
      {
        break;
      }

      // find separator or end of line
      char const* end = signals + 1;
      while (*end && !is_sep(*end))
      {
        ++end;
      }

      Str name { signals, static_cast<std::size_t>(end-signals) };
      signals = end;

      // enable all signals
      if (Str("all").ieq(name))
      {
        #define D(s) signal(SIG##s, sigabort_handler);
        X_SIGNALS(D)
        #undef D
      }
      // signal found
      else if ((signum = signal_num_of(name)) != -1)
      {
        ::signal(signum, sigabort_handler);
      }
      // signal not found
      else
      {
        print_err("Unknown signal '");
        print_err(name);
        #define D(s) #s ", "
        print_err("' in " X_SIGNALS(D) "all\n");
        #undef D
        error = 1;
      }
    }
  }

  return error;
}

}
