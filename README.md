# Python 3.2.5 Repack

> ⚠️ **Security Notice**: Python 3.2.5 and its dependencies (including OpenSSL 1.0.2u and GDBM 1.23) are **extremely outdated** and should **not** be used in production or on systems requiring strong security. These components are no longer supported and have known vulnerabilities. This build is provided **only** to aid in the temporary maintenance of legacy infrastructure or for migration purposes.

Reproducible rebuild of Python 3.2.5 for Ubuntu 20.04 systems with statically linked dependencies.

## Components
- Python 3.2.5
- OpenSSL 1.0.2u
- GDBM 1.23
- site-packages
  - pip 7.1.2
  - wheel 0.29.0
  - python-magic 1.2

## Binary Installation
```bash
# Example for Ubuntu 20.04
wget https://github.com/phdye/python-3.2.5-repack/releases/download/v3.2.5/python-3.2.5-ubuntu-20.04.tar.xz
sudo tar -C /opt -xf python-3.2.5-ubuntu-20.04.tar.xz
export PATH="/opt/python-3.2.5/bin:$PATH"
```
You can also use the provided `install.sh` script:
```bash
./install.sh python-3.2.5-ubuntu-20.04.tar.xz
```

Artifacts are named `python-3.2.5-<os>.tar.xz`, where `<os>` matches
the target platform (e.g., `ubuntu-22.04`).

## Use the Environment

### With the portable execution wrapper, just run Python normally:
```bash
export PATH=/opt/python-3.2.5/bin
python3.2 ...
```

### Without the portable execution wrapper, one needed to setup the environment a bit more first :
```bash
export PATH=/opt/python-3.2.5/bin
export LD_LIBRARY_PATH=/opt/python-3.2.5/lib
python3.2 ...
```

## Minimal Package Support

`pip` and `wheel` let one easily install wheels :  `python3.2 -m pip install <wheel-file>`

With this ancient, long past end-of-life Python, please do expect this `pip` to generally be able to download and install packages.  Most often one needs to Google which version of a given package last worked with this version of Python, download the wheel and try it out.  If it doesn't work, go back a revision or two and try again.  And sometimes, one then has to correspondingly downgrade other packages to have a good working set.

## Build from Source
```bash
git clone https://github.com/phdye/python-3.2.5-repack.git
cd python-3.2.5-repack
chmod +x scripts/*.sh
./scripts/build-openssl.sh
./scripts/build-gdbm.sh
./scripts/build-python.sh
./scripts/populate-site-packages.sh
./scripts/apply-portable-executable-wrapper.sh
```
## Notes
- Requires: Ubuntu 20.04, standard build tools
- No global interference with system Python
- No custom toolchains needed
- Intended only for legacy support and migration workflows

## License
MIT
