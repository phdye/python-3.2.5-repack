#!/bin/bash

set -euo pipefail

PREFIX="$HOME/sysroot"
export PATH="$PREFIX/bin:$PATH"
export LD_LIBRARY_PATH="$PREFIX/lib:$LD_LIBRARY_PATH"
export OPENSSL_DIR="$PREFIX"

exec "$@"
