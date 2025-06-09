# Python 3.2.5 Repack

> ⚠️ **Security Notice**: Python 3.2.5 and its dependencies (including OpenSSL 1.0.2u and GDBM 1.23) are **extremely outdated** and should **not** be used in production or on systems requiring strong security. These components are no longer supported and have known vulnerabilities. This build is provided **only** to aid in the temporary maintenance of legacy infrastructure or for migration purposes.

Reproducible rebuild of Python 3.2.5 for Ubuntu 20.04 systems with statically linked dependencies.

## Components
- Python 3.2.5
- OpenSSL 1.0.2u
- GDBM 1.23

## Binary Installation
```bash
wget https://github.com/phdye/python-3.2.5-repack/releases/download/v3.2.5/python-3.2.5.tar.xz
sudo tar -C /opt -xf python-3.2.5.tar.xz
export PATH="/opt/python-3.2.5/bin:$PATH"
```

## Build from Source
```bash
git clone https://github.com/phdye/python-3.2.5-repack.git
cd python-3.2.5-repack
chmod +x scripts/*.sh
./scripts/build-openssl.sh
./scripts/build-gdbm.sh
./scripts/build-python.sh
./scripts/apply-portable-executable-wrapper.sh
```

## Use the Environment
```bash
scripts/with-env.sh python3.2
```

## Notes
- Requires: Ubuntu 20.04, standard build tools
- No global interference with system Python
- No custom toolchains needed
- Intended only for legacy support and migration workflows

## License
MIT or similar
