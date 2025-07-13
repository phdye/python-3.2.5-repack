#!/bin/bash
set -euo pipefail

version=v1.1.2

PREFIX="/opt/python-3.2.5"

TARBALL="${1:-python-3.2.5-ubuntu-20.04.${version}.tar.xz}"

echo "Installing Python 3.2.5 to ${PREFIX} using $TARBALL..."
sudo mkdir -p "$PREFIX"
sudo tar -xf "$TARBALL" -C /opt

echo ""
echo "✅ Python 3.2.5 installed at $PREFIX"
echo "To use it, add to your shell profile:"
echo "  export PATH="$PATH:$PREFIX/bin"

# It is not recommended to put it first unless you
# intend it to be the default 'python'
