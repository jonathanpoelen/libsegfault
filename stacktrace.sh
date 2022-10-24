#!/bin/bash

p="${LIBSEGFAULT:-/lib/local/libsegfault.so}"

if [[ ! -f "$p" ]]; then
  echo "$0: $p not found. Please, set LIBSEGFAULT environment"
  exit 1
fi

while read lib arrow path addr ; do
  if [[ $lib =~ libasan.so* ]]; then
    SEGFAULT_SIGNALS="${SEGFAULT_SIGNALS:-abrt}" LD_PRELOAD+=":$path:$p" exec "$@"
  fi
done < <(ldd "$1")

LD_PRELOAD+=":$p" exec "$@"
