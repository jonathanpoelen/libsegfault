#!/bin/sh

meson \
 --buildtype=release \
 -Db_lto=true \
 -Db_ndebug=if-release \
 -Dcpp_link_args="['-s', '-Wl,--gc-sections', '-Wl,--strip-all', '-fuse-ld=gold']" \
 -Dcpp_args="['-fno-rtti']" \
 "$@"