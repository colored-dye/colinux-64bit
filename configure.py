#-*-coding: UTF-8-*-

import os
import subprocess

def check_exec_tool(name, cmd, min_ver, max_ver):
  print(f"Checking {name} ...", end='')
  if min_ver == "" and max_ver == "":
    return True
  ver = subprocess.check_output(f"{cmd} | sed -n -e " + r"'s/^[^0-9]*\([0-9]\{1,\}\)\.\([0-9]\{1,\}\)\.\([0-9]\{1,\}\).*/i1=\1 i2=\2 i3=\3/p'", shell=True).decode()
  ver = ver[:-1].split(' ')
  min_ver = min_ver.split('.')
  for i in range(min(len(ver), len(min_ver))):
    v = int(ver[i].split('=')[-1])
    min_ver_ = int(min_ver[i])
    if v < min_ver_:
      print("Missing")
      return False
  print("Found")
  return True

if __name__ == "__main__":
  from bin.build_common import build_common_get_vars
  build_common_get_vars()

  from bin.build_common import TOPDIR, TARGET, BUILD
  target = TARGET
  prefix = None
  distdir = os.path.join(TOPDIR, "dist")
  downloaddir = None
  builddir = os.path.join(TOPDIR, "build")

  check_exec_tool('wget', 'wget --version', '1.20.0', '')
