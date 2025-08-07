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

# Ensure the resulting toolchain is fully relocatable by rewriting
# RPATH entries to use paths relative to each binary's location.
find "$PREFIX" -type f -print0 | while IFS= read -r -d '' file; do
  if file "$file" | grep -q ELF; then
    patchelf --set-rpath '$ORIGIN/../lib' "$file" 2>/dev/null || true
  fi
done
