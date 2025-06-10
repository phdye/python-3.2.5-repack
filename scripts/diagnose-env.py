#!/usr/bin/env python3
import json
import os
import platform
import subprocess
import sys

def run(cmd):
    return subprocess.getoutput(cmd)

info = {
    "platform": platform.platform(),
    "uname": run("uname -a"),
}

print("\n" + "=" * 80 + "\n\n")

if os.path.exists('/etc/os-release'):
    info["os_release"] = open('/etc/os-release').read()

info.update({
    "gcc": run("gcc --version | head -n 1"),
    "make": run("make --version | head -n 1"),
    "ld": run("ld --version | head -n 1"),
    "pkg_config": run("pkg-config --version 2>/dev/null"),
    "openssl": run("openssl version 2>/dev/null"),
    "ldd": run("ldd --version | head -n 1"),
    "cpuinfo": run("grep -m1 'model name' /proc/cpuinfo"),
    "cpu_flags": run("grep -m1 'flags' /proc/cpuinfo"),
    "installed_libs": run("dpkg -l | egrep '(gcc|libc6|openssl|zlib|gdbm)' | head"),
    "env_PATH": os.environ.get('PATH', ''),
    "env_LD_LIBRARY_PATH": os.environ.get('LD_LIBRARY_PATH', ''),
})

for binary in sys.argv[1:]:
    info[f'ldd_{binary}'] = run(f'ldd {binary}')

print(json.dumps(info, indent=2))

print("\n" + "=" * 80 + "\n\n")
