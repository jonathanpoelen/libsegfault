# LibSegFault

Implementation of `/lib/libSegFault.so` with `Boost.stracktrace`

$ `./crash`

```
crash: /crash.cpp:5: void foo(int*): Assertion `p' failed.
zsh: abort (core dumped)  ./crash
```
$ `LD_PRELOAD=./libsegfault.so ./crash`

```
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

$ `SEGFAULT_SIGNALS=segv LD_PRELOAD=./libsegfault.so ./crash`

```
crash: ../../projects/sigabort/crash.cpp:5: void foo(int*): Assertion `p' failed.
zsh: abort (core dumped)  SEGFAULT_SIGNALS=segv LD_PRELOAD=./libsegfault.so ./crash
```

## stacktrace.sh

$ `./stacktrace.sh prog args...`

Wrapper for use `libsegfault` with `libasan`.

## `SEGFAULT_SIGNALS`

Comma-separated values:

- segv
- abrt
- bus
- ill
- fpe
- sys
- all

## Build

```bash
meson build
```

### Disable libbacktrace

```bash
meson build -Dnobacktrace=true
```

### Configure libbacktrace

```bash
meson build -Dbacktrace-include='"my/backtrace.h"' -Dbacktrace-library-path=my-backtrace-dir
```
