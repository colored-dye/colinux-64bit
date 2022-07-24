#-*-coding: UTF-8-*-

import os
import subprocess
from bin.ConfigParser import ConfigParser

CONFIG_VARS = ".config_vars"

g_configparser = ConfigParser(CONFIG_VARS)
g_configparser.read()

def check_exec_tool(name, cmd, min_ver):
  """Check mininum version requirement only.
  """
  print(f"Checking {name} ... ", end='')
  if min_ver == "":
    return True
  ver = subprocess.check_output(f"{cmd} | sed -n -e " + r"'s/^[^0-9]*\([0-9]\{1,\}\)\.\([0-9]\{1,\}\)\.\([0-9]\{1,\}\).*/i1=\1 i2=\2 i3=\3/p'", shell=True).decode()
  ver = ver[:-1].split(' ')
  min_ver = min_ver.split('.')
  for i in range(min(len(ver), len(min_ver))):
    v = int(ver[i].split('=')[-1])
    min_ver_ = int(min_ver[i])
    if v < min_ver_:
      print("missing")
      return False
  print("found")
  return True

if __name__ == "__main__":
  from bin.build_common import build_common_get_vars
  build_common_get_vars()
  target = g_configparser["TARGET"]
  
