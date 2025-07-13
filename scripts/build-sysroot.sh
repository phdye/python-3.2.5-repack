#!/bin/bash

set -eou pipefail

export CFLAGS=
export CPPFLAGS=
export LDFLAGS=
export LD_LIBRARY_PATH=

# ./scripts/diagnose-env.sh

./scripts/install-deps.sh

./scripts/build-openssl.sh

./scripts/build-gdbm.sh

./scripts/build-python.sh

./scripts/populate-site-packages.sh

# ( cd ~/sysroot/bin && ln -s python3.2 python3 )

( cd ~/sysroot/bin && if [ ! -e python ] ; then ln -s python3.2 python ; fi )

./scripts/apply-portable-executable-wrapper.sh
