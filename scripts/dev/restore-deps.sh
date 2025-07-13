#!/bin/bash

set -euo pipefail

PREFIX=$HOME/sysroot

set -x

scripts/clear-sysroot.sh

scripts/restore-partial.sh

cd ${PREFIX}

tar xf ~/archive/sysroot/readline-8.2.tar.xz

tar xf ~/archive/sysroot/openssl-1.0.2u.tar.xz
