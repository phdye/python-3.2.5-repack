#!/bin/false

if [ -z ${BUILD_SUBJECT} ] ; then
  echo
  echo "Error:  BUILD_SUBJECT not defined.  Aborting."
  echo
  echo "Usage:  bootstrap-build.sh"
  echo
  echo "  BUILD_SUBJECT must be defined in order to define the build area."
  echo
  exit 1
fi

set -euo pipefail

SRC="$HOME/src"
BUILD="$HOME/build"
PREFIX="$HOME/sysroot"
ARCHIVE="$HOME/archive"

mkdir -p "$ARCHIVE" "$SRC" "$PREFIX"

INSTALLED_DIR="$PREFIX/.installed"

export MAKEFLAGS="-j$(nproc)"
export PATH="$PREFIX/bin:$PATH"
export CFLAGS="${CFLAGS} -O2 -fPIC"
export CPPFLAGS="${CFLAGS} ${CPPFLAGS}"
# Embed a relative RPATH so binaries remain relocatable
export LDFLAGS="-Wl,-rpath,\$ORIGIN/../lib -L${PREFIX}/lib ${LDFLAGS}"

export MAKE="make"

# Record packages that have been properly installed
mkdir -p "$INSTALLED_DIR"
mkdir -p "$BUILD"
cd "$BUILD"

# Return true if anything for $name is already installed
function is_pkg_installed() {
  local name="$1"
  local srcdir="$2"
  [[ -f "$INSTALLED_DIR/$srcdir"	]] && return 0
  [[ -d "$PREFIX/include/$name"		]] && return 0
  [[ -f "$PREFIX/include/$name.h"	]] && return 0
  [[ -f "$PREFIX/lib/lib$name.so"	]] && return 0
  [[ -x "$PREFIX/bin/$name"		]] && return 0
  return 1
}

function do-prep() {
  local name="$1"
  local version="$2"
  local url="$3"
  local tarflags="$4"
  local config_flags="$5"
  local srcdir=${name}-${version}

  cd "$BUILD"

  local tarball="$(echo $url | sed -e 's:.*/::' )"
  local archive="$ARCHIVE/${tarball}"
  [ ! -f ${archive} ] && curl -L -o ${archive} "$url"
  rm -rf ${srcdir}
  tar $tarflags ${archive}
  tarname="$(basename -s .tar $(basename -s .gz $(basename -s .xz ${tarball})))"
  if [ "${tarname}" != "${srcdir}" ] ; then
    mv "${tarname}" "${srcdir}"
  fi

  cd "$BUILD"
}

function do-config() {
  local name="$1"
  local version="$2"
  local url="$3"
  local tarflags="$4"
  local config_flags="$5"
  local srcdir=${name}-${version}

  echo "Configuring:  ${name} ${version} ${url} ${tarflags} ${config_flags}"

  cd "$BUILD/${srcdir}"

  if [[ ! -x configure ]]; then
    if [[ -f configure.ac || -f configure.in ]]; then
      autoreconf -fi
    fi
  fi
  if [[ -x configure ]]; then
    OPTS=
    if [ -f .config.guess ] ; then
      OPTS="${OPTS} --build=$(./config.guess)"
    fi
    if ./configure --help 2>&1 | grep -q -- '--host='; then
      OPTS="${OPTS} --host=x86_64-pc-linux-gnu"
    fi
    ./configure --prefix="$PREFIX" ${OPTS} ${config_flags}
  elif [[ -x Configure ]]; then
    ./Configure linux-x86_64 --prefix="$PREFIX" --openssldir="$PREFIX" ${config_flags}
    if egrep '^\s*depend\*s:' Makefile 2>&1 | grep -q -- 'depend'; then
      make clean || true
      make depend
    fi
  elif [[ -f Makefile ]]; then
    echo -n
  else
    echo "No recognized build system for $name — aborting"
    exit 1
  fi

  echo "Configured:  $name"

  cd "$BUILD"
}

function do-build() {
  local name="$1"
  local version="$2"
  local url="$3"
  local tarflags="$4"
  local config_flags="$5"
  local srcdir=${name}-${version}

  echo "Building:  ${name} ${version} ${url} ${tarflags} ${config_flags}"

  set -x

  cd "$BUILD/${srcdir}"

  if [[ -r Makefile ]]; then
    ${MAKE} $MAKEFLAGS
  else
    echo "No Makefile for $name — unable to build — aborting"
    exit 1
  fi

  echo "Built:  $name"

  cd "$BUILD"
}

function do-install() {
  local name="$1"
  local version="$2"
  local url="$3"
  local tarflags="$4"
  local config_flags="$5"
  local srcdir=${name}-${version}

  echo "Installing:  ${name} ${version} ${url} ${tarflags} ${config_flags}"

  cd "$BUILD/${srcdir}"

  if [[ -x Configure ]]; then
    ${MAKE} PREFIX=${PREFIX} install_sw
  elif [[ -x configure || -r Makefile ]]; then
    ${MAKE} PREFIX=${PREFIX} install
  else
    echo "No Makefile for $name — unable to build — aborting"
    exit 1
  fi

  echo "Installed:  $name"
  touch "$PREFIX"/.installed/${srcdir}

  cd "$BUILD"
}

build(){
  local name="$1"
  local version="$2"
  local url="$3"
  local tarflags="$4"
  local config_flags="$5"
  local srcdir=${name}-${version}

  if is_pkg_installed "$name" "${srcdir}"; then
    echo "Already installed: ${srcdir}"
    touch "$PREFIX"/.installed/${srcdir}
    return 0
  fi

  do-prep "$@"
  do-config "$@"
  do-build "$@"
  do-install "$@"
}

config-only(){
  local name="$1"
  local version="$2"
  local url="$3"
  local tarflags="$4"
  local config_flags="$5"
  local srcdir=${name}-${version}

  if is_pkg_installed "$name" "${srcdir}"; then
    echo "Already installed: ${srcdir}"
    touch "$PREFIX"/.installed/${srcdir}
    return 0
  fi

  do-prep "$@"
  do-config "$@"
  # do-build "$@"
  # do-install "$@"
}
