#!/bin/bash

set -euo pipefail

export BUILD_SUBJECT=install-deps

#----------------------------------------------------------------------------

sudo apt update

sudo apt upgrade -y

sudo apt install -y build-essential libc6-dev

#----------------------------------------------------------------------------

echo
echo "Python fundamental build tools installed"
echo

#----------------------------------------------------------------------------
