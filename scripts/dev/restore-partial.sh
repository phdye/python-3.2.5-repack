#!/bin/bash

set -euo pipefail

PREFIX=$HOME/sysroot

set -x

scripts/clear-sysroot.sh

cd ${PREFIX}

tar xf ~/archive/sysroot/partial.tar.xz

tar xf ~/archive/sysroot/ncurses-6.4.tar.xz
