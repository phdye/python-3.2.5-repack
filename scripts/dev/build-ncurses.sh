#!/bin/bash

set -euo pipefail

export BUILD_SUBJECT=readline-8.2

#----------------------------------------------------------------------------

source scripts/builder.sh

PREFIX=${PREFIX}/one

# Interactive Shell Support -- oddly, this now breaks the build
build ncurses  6.4     https://ftp.gnu.org/gnu/ncurses/ncurses-6.4.tar.gz -xzf \
    "--with-shared --without-ada --without-debug --with-termlib"

# set -x
# export LDFLAGS="-L${PREFIX}/lib -Wl,--no-as-needed -ltinfo"
# set +x

# build readline 8.2     https://ftp.gnu.org/gnu/readline/readline-8.2.tar.gz -xzf \
#     "--with-curses"

#   ... system awk dies due to 'UP' feature missing. ... libinfo not linked

echo
echo "Python ${BUILD_SUBJECT} built"
echo
