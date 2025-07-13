#!/bin/bash

set -euo pipefail

PYTHON_VERSION="3.2.5"

ARCHIVE="$HOME/archive"
SRC="$HOME/src"
PREFIX="$HOME/sysroot"

TARBALL="$ARCHIVE/Python-${PYTHON_VERSION}.tgz"
SRC_DIR="$SRC/Python-${PYTHON_VERSION}"

mkdir -p "$ARCHIVE" "$SRC" "$PREFIX"

if [ ! -f "$TARBALL" ]; then
  wget -O "$TARBALL" "https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz"
fi

set -x

rm -rf "$SRC_DIR"

tar -xzf "$TARBALL" -C "$SRC"
cd "$SRC_DIR"

export CFLAGS="-fPIC -O2 -I${PREFIX}/include -DHAVE_MEMMOVE"
export LDFLAGS="-L${PREFIX}/lib"
export PATH="$PREFIX/bin:$PATH"
export LD_LIBRARY_PATH="$PREFIX/lib:$LD_LIBRARY_PATH"

set +x

echo
# echo "===== ===== ===== ===== ===== ===== ===== ===== "
# env
echo "===== ===== ===== ===== ===== ===== ===== ===== "
echo

( set -x && ./configure --prefix="$PREFIX" --enable-shared --with-system-ffi --disable-ipv6 \
  CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" LD_LIBRARY_PATH="$LD_LIBRARY_PATH" )

echo
echo "===== ===== ===== ===== ===== ===== ===== ===== "
echo

( set -x && make -j"$(nproc)" )

echo
echo "===== ===== ===== ===== ===== ===== ===== ===== "
echo

# ( set -x && make install )

pushd ${PREFIX}/bin

target=python3.2m
if [ -x python3.2m ] ; then
    for suffix in "" 3 3.2 ; do
        name=python${suffix}
        rm -f ${name}
        ( set -x && ln -s ${target} ${name} )
    done
fi

popd

echo
echo "===== ===== ===== ===== ===== ===== ===== ===== "
echo
