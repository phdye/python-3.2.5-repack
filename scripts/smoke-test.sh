#!/bin/bash

set -euo pipefail

PREFIX="$HOME/sysroot"
export PATH="$PREFIX/bin:$PATH"

python3.2 --version
