# LibSegFault

Get a call trace when your application crashes.

Use `Boost.stracktrace` as a dependency.

```console
$ g++ crash.cpp -o crash
$ ./crash
crash: /crash.cpp:5: void foo(int*): Assertion `p' failed.
zsh: abort (core dumped)  ./crash
```

```console
$ LD_PRELOAD=./libsegfault.so ./crash
crash: /crash.cpp:5: void foo(int*): Assertion `p' failed.
 0# (anonymous namespace)::segabort_handler(int) at /libsegfault.cpp:39
 1# 0x00007FCCEA604E00 in /usr/lib/libc.so.6
 2# raise in /usr/lib/libc.so.6
 3# __GI_abort in /usr/lib/libc.so.6
 4# _nl_load_domain.cold.0 in /usr/lib/libc.so.6
 5# 0x00007FCCEA5FD396 in /usr/lib/libc.so.6
 6# foo(int*) at /crash.cpp:6
 7# main at /crash.cpp:12
 8# __libc_start_main in /usr/lib/libc.so.6
 9# _start in ./crash
zsh: abort (core dumped)  LD_PRELOAD=./libsegfault.so ./crash
```

Specify a list of signals with `SEGFAULT_SIGNALS` (default is `segv,abrt`).

```console
$ SEGFAULT_SIGNALS=segv LD_PRELOAD=./libsegfault.so ./crash
crash: ../../projects/sigabort/crash.cpp:5: void foo(int*): Assertion `p' failed.
zsh: abort (core dumped)  SEGFAULT_SIGNALS=segv LD_PRELOAD=./libsegfault.so ./crash
```

No backtrace since the signal is abrt, not segv.


## stacktrace.sh

$ `./stacktrace.sh prog args...`

Wrapper for use `libsegfault` with `libasan`.

## `SEGFAULT_SIGNALS`

Value case-insensitive separated by space or comma:

- segv
- abrt
- bus
- ill
- fpe
- trap
- sys
- all

## Installation

```bash
meson setup --buildtype=release build_dir
cd build_dir
meson compile
meson install
```

### Disable libbacktrace

```bash
meson setup --reconfigure -Dnobacktrace=false project-path
```

### Configure libbacktrace

```bash
meson setup --reconfigure \
  -Dbacktrace-include='"my/backtrace.h"' \
  -Dbacktrace-library-path=my-backtrace-dir \
  project-path
```
