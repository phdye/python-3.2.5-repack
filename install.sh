#!/bin/bash
set -euo pipefail

PREFIX="/opt/python-3.2.5"

echo "Installing Python 3.2.5 to ${PREFIX}..."
sudo mkdir -p "$PREFIX"
sudo tar -xf python-3.2.5.tar.xz -C /opt

echo ""
echo "✅ Python 3.2.5 installed at $PREFIX"
echo "To use it, add to your shell profile:"
echo "  export PATH="$PREFIX/bin:$PATH""
