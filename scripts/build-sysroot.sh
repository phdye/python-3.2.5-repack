#!/bin/bash

./scripts/build-openssl.sh

./scripts/build-gdbm.sh

./scripts/build-python.sh

./scripts/populate-site-packages.sh

./scripts/apply-portable-executable-wrapper.sh
