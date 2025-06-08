#!/bin/bash

set -euo pipefail

PYTHON_VERSION="3.2.5"

ARCHIVE="$HOME/archive"
SRC="$HOME/src"
PREFIX="$HOME/sysroot"

TARBALL="$ARCHIVE/Python-${PYTHON_VERSION}.tgz"
SRC_DIR="$SRC/python-${PYTHON_VERSION}"

mkdir -p "$ARCHIVE" "$SRC" "$PREFIX"

if [ ! -f "$TARBALL" ]; then
  wget -O "$TARBALL" "https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz"
fi

tar -xzf "$TARBALL" -C "$SRC"
mv "$SRC/Python-${PYTHON_VERSION}" "$SRC_DIR"
cd "$SRC_DIR"

export CFLAGS="-I${PREFIX}/include"
export LDFLAGS="-L${PREFIX}/lib"
export PATH="$PREFIX/bin:$PATH"
export LD_LIBRARY_PATH="$PREFIX/lib:$LD_LIBRARY_PATH"

./configure --prefix="$PREFIX" --enable-shared --with-system-ffi --disable-ipv6 \
  CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

make -j"$(nproc)"
make install
