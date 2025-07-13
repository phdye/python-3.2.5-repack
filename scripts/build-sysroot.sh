#!/bin/bash

set -eou pipefail

export PREFIX=${HOME}/sysroot
export CFLAGS=-I${PREFIX}/include
export CPPFLAGS=
export LDFLAGS="-Wl,-rpath,\\\$\$ORIGIN/../lib -L${PREFIX}/lib"
export LD_LIBRARY_PATH=

# ./scripts/diagnose-env.sh

( set -x && rm -rf ~/sysroot && mkdir ~/sysroot )

./scripts/install-deps.sh

./scripts/build-deps.sh

./scripts/build-openssl.sh

# Now handled in install-deps.sh # ./scripts/build-gdbm.sh

./scripts/build-python.sh

./scripts/populate-site-packages.sh
