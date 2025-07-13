#!/bin/bash

set -euo pipefail

GDBM_VERSION="1.23"

ARCHIVE="$HOME/archive"
SRC="$HOME/src"
PREFIX="$HOME/sysroot"

TARBALL="$ARCHIVE/gdbm-${GDBM_VERSION}.tar.gz"
SRC_DIR="$SRC/gdbm-${GDBM_VERSION}"

mkdir -p "$ARCHIVE" "$SRC" "$PREFIX"

if [ ! -f "$TARBALL" ]; then
  wget -O "$TARBALL" "https://ftp.gnu.org/gnu/gdbm/gdbm-${GDBM_VERSION}.tar.gz"
fi

tar -xf "$TARBALL" -C "$SRC"
cd "$SRC_DIR"

CFLAGS="-fPIC -O2" ./configure --prefix="$PREFIX" --enable-libgdbm-compat
make -j"$(nproc)"
make install

mkdir -p "$PREFIX/include/gdbm"
ln -sf ../ndbm.h "$PREFIX/include/gdbm/ndbm.h"
