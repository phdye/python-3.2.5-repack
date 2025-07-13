#!/bin/bash

set -euo pipefail

export BUILD_SUBJECT=build-deps

#----------------------------------------------------------------------------

source scripts/builder.sh

# Compression
build zlib     1.2.13  https://zlib.net/fossils/zlib-1.2.13.tar.gz -xzf ""
build bzip2    1.0.8   https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz -xzf ""

# Interactive Shell Support -- oddly, this now breaks the build
build ncurses  6.4     https://ftp.gnu.org/gnu/ncurses/ncurses-6.4.tar.gz -xzf \
    "--with-shared --without-ada --without-debug --with-termlib"

SAVE_LDFLAGS="${LDFLAGS}"
export LDFLAGS="${LDFLAGS} -L${PREFIX}/lib -Wl,--no-as-needed -ltinfo"
build readline 8.2     https://ftp.gnu.org/gnu/readline/readline-8.2.tar.gz -xzf \
    "--with-curses"
LDFLAGS="${SAVE_LDFLAGS}"


# Storage Backends
build sqlite   3.45.3  https://www.sqlite.org/2024/sqlite-autoconf-3450300.tar.gz -xzf ""
build gdbm     1.23    https://ftp.gnu.org/gnu/gdbm/gdbm-1.23.tar.gz -xzf "--enable-libgdbm-compat"

# Dynamic Linking and XML Support
build libffi   3.4.4   https://github.com/libffi/libffi/releases/download/v3.4.4/libffi-3.4.4.tar.gz -xzf ""
build expat    2.6.2   https://github.com/libexpat/libexpat/releases/download/R_2_6_2/expat-2.6.2.tar.gz -xzf ""

echo
echo "Python dependencies built"
echo
