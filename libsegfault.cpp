#include <boost/stacktrace.hpp>
#include <csignal>
#include <strings.h>
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

struct S { char const* name; std::size_t len; int num; };
#define D(s) S{#s, sizeof(#s)-1, SIG##s},
constexpr S signal_defs[] = { X_SIGNALS(D) };
#undef D

int signal_num(char const* sig, std::size_t n)
{
  for (auto const& s : signal_defs)
  {
    if (s.len == n && strncasecmp(s.name, sig, n) == 0)
    {
      return s.num;
    }
  }
  return -1;
}

void segabort_handler(int signum)
{
  ::signal(signum, SIG_DFL);
  auto s = to_string(boost::stacktrace::stacktrace());
  write(STDERR_FILENO, s.data(), s.size());
  ::raise(signum);
}

int
__attribute__((constructor))
segabort_init(void)
{
  const char *signals = getenv("SEGFAULT_SIGNALS");
  int error = 0;
  if (!signals)
  {
    signal(SIGSEGV, segabort_handler);
    signal(SIGABRT, segabort_handler);
  }
  else if (strcasecmp(signals, "all") == 0)
  {
    signal(SIGSEGV, segabort_handler);
    signal(SIGBUS, segabort_handler);
    signal(SIGILL, segabort_handler);
    signal(SIGABRT, segabort_handler);
    signal(SIGFPE, segabort_handler);
    signal(SIGTRAP, segabort_handler);
    signal(SIGSYS, segabort_handler);
  }
  else if (*signals)
  {
    char const* sig = signals;
    while (*sig)
    {
      while (*sig == ',')
      {
        ++sig;
      }

      char const* end = sig + 1;
      while (*end && *end != ',')
      {
        ++end;
      }

      int signum = signal_num(sig, end-sig);
      if (signum != -1)
      {
        ::signal(signum, segabort_handler);
      }
      else
      {
        auto wa = [](auto& a) { ::write(STDERR_FILENO, a, sizeof(a)-1); };
        auto ws = [](auto* s, std::size_t n) { ::write(STDERR_FILENO, s, n); };
        wa("Unknown signal '");
        ws(sig, end-sig);
        #define D(s) #s ", "
        wa("' in " X_SIGNALS(D) "all\n");
        #undef D
        error = 1;
      }

      sig = end;
      if (*sig)
      {
        ++sig;
      }
    }
  }

  return error;
}

}
