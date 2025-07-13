#!/bin/bash

set -euo pipefail

OPENSSL_VERSION="1.0.2u"

ARCHIVE="$HOME/archive"
SRC="$HOME/src"
PREFIX="$HOME/sysroot"

TARBALL="$ARCHIVE/openssl-${OPENSSL_VERSION}.tar.gz"
SRC_DIR="$SRC/openssl-${OPENSSL_VERSION}"

mkdir -p "$ARCHIVE" "$SRC" "$PREFIX"

if [ ! -f "$TARBALL" ]; then
  wget -O "$TARBALL" "https://www.openssl.org/source/old/1.0.2/openssl-${OPENSSL_VERSION}.tar.gz"
fi

tar -xzf "$TARBALL" -C "$SRC"
cd "$SRC_DIR"

CFLAGS="-fPIC -O2" ./Configure linux-x86_64 --prefix="$PREFIX" --openssldir="$PREFIX/ssl" no-ssl2 no-ssl3 shared
make -j"$(nproc)"
make install_sw
