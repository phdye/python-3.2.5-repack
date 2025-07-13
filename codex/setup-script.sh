#!/usr/bin/env bash
set -euo pipefail

ASSET="python-3.2.5.ubuntu-20.04.v1.1.4.tar.xz"
REPO="phdye/python-3.2.5-repack"
PYENV_ROOT=$(pyenv root)
INSTALL_DIR="$PYENV_ROOT/versions/3.2.5"
TARBALL="$ASSET"

# Prerequisite: jq must be installed
command -v jq >/dev/null || { echo "❌ jq is required. Please install it."; exit 1; }

echo "🔍 Locating release asset for $ASSET..."

# Use full /releases API to find asset
DOWNLOAD_URL=$(curl -s "https://api.github.com/repos/$REPO/releases" |
  jq -r --arg name "$ASSET" '.[] | .assets[]? | select(.name == $name) | .browser_download_url' | head -n1)

if [[ -z "$DOWNLOAD_URL" ]]; then
  echo "❌ Asset $ASSET not found in any public releases of $REPO"
  exit 1
fi

echo "✅ Found asset:"
echo "  $DOWNLOAD_URL"

echo "⬇️ Downloading..."
curl -L -o "$TARBALL" "$DOWNLOAD_URL"

echo "📦 Extracting to $INSTALL_DIR..."
mkdir -p "$INSTALL_DIR"
tar -xf "$TARBALL" -C "$INSTALL_DIR" --strip-components=1
rm "$TARBALL"

echo "✅ Python 3.2.5 installed at:"
echo "  $INSTALL_DIR"

echo "🔄 Setting pyenv global version to 3.2.5..."
pyenv global 3.2.5
pyenv rehash

python --version
