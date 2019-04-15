project('libsegfault', 'cpp', default_options : ['cpp_std=c++17'])

deps = []
cxx_args = []
nobt = get_option('nobacktrace')
if not nobt
  comp = meson.get_compiler('cpp')
  deps += comp.find_library('dl')
  cxx_args += '-DBOOST_STACKTRACE_USE_BACKTRACE'

  btinc = get_option('backtrace-include')
  if btinc != ''
    cxx_args += '-DBOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE=' + btinc
  endif

  btlibpath = get_option('backtrace-library-path')
  if btlibpath != ''
    deps += comp.find_library('backtrace', dirs: btlibpath)
  else
    deps += comp.find_library('backtrace')
  endif
endif

shared_library('segfault', 'libsegfault.cpp',
  native: true,
  cpp_args: cxx_args,
  dependencies: deps,
)
executable('crash', 'crash.cpp', cpp_args: ['-g', '-rdynamic'])
