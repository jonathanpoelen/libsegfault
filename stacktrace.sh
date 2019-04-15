#!/bin/bash

p=${LIBSEGFAULT:-/lib/local/libsegfault.so}

if [ ! -f "$p" ]; then
  echo "$0: $p not found. Please, set LIBSEGFAULT environment"
  exit 1
fi

asan_path=$(ldd "$1" | while read lib arrow path addr ; do
  if [[ $lib =~ libasan.so* ]]; then
    echo "$path"
    break
  fi
done)
if [ -n "$asan_path" ]; then
  SEGFAULT_SIGNALS="${SEGFAULT_SIGNALS:-abrt}" LD_PRELOAD+=" $asan_path $p" exec "$@"
else
  LD_PRELOAD+=" $p" exec "$@"
fi
