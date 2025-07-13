#!/bin/bash

set -eou pipefail

export CFLAGS=
export CPPFLAGS=
export LDFLAGS=
export LD_LIBRARY_PATH=

# ./scripts/diagnose-env.sh

( set -x && rm -rf ~/sysroot && mkdir ~/sysroot )

./scripts/install-deps.sh

./scripts/build-deps.sh

./scripts/build-openssl.sh

# Now handled in install-deps.sh # ./scripts/build-gdbm.sh

./scripts/build-python.sh

./scripts/populate-site-packages.sh

./scripts/apply-portable-executable-wrapper.sh
