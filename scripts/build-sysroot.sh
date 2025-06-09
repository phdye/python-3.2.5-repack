#!/bin/bash

set -eou pipefile

export CFLAGS=
export CPPFLAGS=
export LDFLAGS=
export LD_LIBRARY_PATH=

./scripts/install-deps.sh

./scripts/build-openssl.sh

./scripts/build-gdbm.sh

./scripts/build-python.sh

./scripts/populate-site-packages.sh

./scripts/apply-portable-executable-wrapper.sh
