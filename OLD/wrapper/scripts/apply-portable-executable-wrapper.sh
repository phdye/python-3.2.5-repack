#!/bin/bash

set -euo pipefail

PREFIX="$HOME/sysroot"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

cd "$REPO_ROOT/src/wrapper"
make
scripts/apply-wrapper -w wrapper "$PREFIX/bin"
