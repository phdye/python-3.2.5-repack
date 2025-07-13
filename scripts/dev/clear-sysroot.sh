#!/bin/bash

set -euo pipefail

PREFIX=$HOME/sysroot

set -x

rm -rf  ${PREFIX}
mkdir   ${PREFIX}
