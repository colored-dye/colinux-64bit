#-*-coding: UTF-8-*-

import os
import sys
from ConfigParser import ConfigParser

CONFIG_VARS = ".config_vars"

g_configparser = None

def get_var(key: str):
  return g_configparser[key]

def set_var(key: str, val: str):
  g_configparser[key] = val

def build_common_get_vars():
  if os.path.exists("bin/build_common.py"):
    BINDIR = os.path.abspath(os.path.join(os.curdir, "bin"))
  else:
    BINDIR = os.path.abspath(os.curdir)

  # Default top directory. Can be overwritten by user config.
  TOPDIR = os.path.dirname(BINDIR)

  user_config = ConfigParser(os.path.join(BINDIR, "user-build.cfg"))

  COLINUX_HOST_OS = user_config["COLINUX_HOST_OS"]

  global CONFIG_VARS
  global g_configparser
  CONFIG_VARS = os.path.join(user_config["TOPDIR"], CONFIG_VARS)
  g_configparser = ConfigParser(CONFIG_VARS)

  for k in user_config.keys():
    set_var(k, user_config[k])

  if user_config["TOPDIR"] != "":
    TOPDIR = user_config["TOPDIR"]
  DOWNLOAD_DIR = user_config["DOWNLOADS"]
  BUILD_DIR = user_config["BUILD_DIR"]
  PREFIX = user_config["PREFIX"]
  KERNEL_VERSION = user_config["KERNEL_VERSION"]
  KERNEL_SOURCE = user_config["COLINUX_TARGET_KERNEL_SOURCE"]
  KERNEL_BUILD = user_config["COLINUX_TARGET_KERNEL_BUILD"]
  MODULE_PATH = user_config["MODULE_PATH"]

  MINGW_VERSION = user_config["MINGW_VERSION"]
  MINGW_URL = "https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release"
  MINGW_ARCHIVE = f"mingw-w64-v{MINGW_VERSION}.tar.bz2"

  BINUTILS_VERSION = user_config["BINUTILS_VERSION"]
  BINUTILS_URL = "https://ftp.gnu.org/pub/gnu/binutils"
  BINUTILS = f"binutils-{BINUTILS_VERSION}"
  BINUTILS_ARCHIVE = f"{BINUTILS}.tar.bz2"

  GCC_VERSION = user_config["GCC_VERSION"]
  GCC_URL = "https://ftp.gnu.org/pub/gnu/gcc"
  GCC = f"gcc-{GCC_VERSION}"
  GCC_ARCHIVE = f"{GCC}.tar.gz"

  FLTK_VERSION = user_config["FLTK_VERSION"]
  FLTK_URL = "https://www.fltk.org/pub/fltk"
  FLTK_ARCHIVE = f"{FLTK_VERSION}/fltk-{FLTK_VERSION}-source.tar.bz2"

  WINPCAP_VERSION = user_config["WINPCAP_VERSION"]
  WINPCAP_URL = "https://winpcap.org/archive"
  WINPCAP_ARCHIVE = f"{WINPCAP_VERSION}-WpdPack.zip"

  MD5DIR = BUILD_DIR
  W32LIBS_CHECKSUM = os.path.join(MD5DIR, ".build-colinux-libs.md5")
  KERNEL_CHECKSUM = os.path.join(MD5DIR, ".build-kernel.md5")

  TARGET = "x86_64-w64-mingw32"
  TARGET_ARCH = TARGET.split('-')[0]
  # BUILD = "x86_64-pc-linux" # Current developing build system

  COLINUX_GCC_GUEST_TARGET = "x86_64-co-linux"
  COLINUX_GCC_GUEST_PATH = f"{PREFIX}/{COLINUX_GCC_GUEST_TARGET}/bin"

  CO_VERSION = open(os.path.join(TOPDIR, "VERSION"), 'r').read()
  COMPLETE_KERNEL_NAME = KERNEL_VERSION + "-co-" + CO_VERSION

  """Set intermediate variables.
  """

  set_var("BINDIR", BINDIR)
  set_var("TOPDIR", TOPDIR)
  set_var("COLINUX_HOST_OS", COLINUX_HOST_OS)
  set_var("DOWNLOAD_DIR", DOWNLOAD_DIR)
  set_var("BUILD_DIR", BUILD_DIR)
  set_var("PREFIX", PREFIX)

  set_var("KERNEL_VERSION", KERNEL_VERSION)
  set_var("KERNEL_SOURCE", KERNEL_SOURCE)
  set_var("KERNEL_BUILD", KERNEL_BUILD)
  set_var("MODULE_PATH", MODULE_PATH)

  set_var("MINGW_VERSION", MINGW_VERSION)
  set_var("MINGW_URL", MINGW_URL)
  set_var("MINGW_ARCHIVE", MINGW_ARCHIVE)

  set_var("BINUTILS_VERSION", BINUTILS_VERSION)
  set_var("BINUTILS_URL", BINUTILS_URL)
  set_var("BINUTILS_ARCHIVE", BINUTILS_ARCHIVE)
  set_var("BINUTILS", BINUTILS)

  set_var("GCC_VERSION", GCC_VERSION)
  set_var("GCC_URL", GCC_URL)
  set_var("GCC", GCC)
  set_var("GCC_ARCHIVE", GCC_ARCHIVE)

  set_var("FLTK_VERSION", FLTK_VERSION)
  set_var("FLTK_URL", FLTK_URL)
  set_var("FLTK_ARCHIVE", FLTK_ARCHIVE)

  set_var("WINPCAP_VERSION", WINPCAP_VERSION)
  set_var("WINPCAP_URL", WINPCAP_URL)
  set_var("WINPCAP_ARCHIVE", WINPCAP_ARCHIVE)

  set_var("MD5DIR",MD5DIR)
  set_var("W32LIBS_CHECKSUM", W32LIBS_CHECKSUM)
  set_var("KERNEL_CHECKSUM", KERNEL_CHECKSUM)

  set_var("TARGET", TARGET)
  set_var("TARGET_ARCH", TARGET_ARCH)
  # set_var("BUILD", BUILD)

  set_var("COLINUX_GCC_GUEST_TARGET", COLINUX_GCC_GUEST_TARGET)
  set_var("COLINUX_GCC_GUEST_PATH", COLINUX_GCC_GUEST_PATH)

  set_var("CO_VERSION", CO_VERSION)
  set_var("COMPLETE_KERNEL_NAME", COMPLETE_KERNEL_NAME)

  return

def download_file(archive: str, url: str):
  DOWNLOADS = g_configparser["DOWNLOAD_DIR"]
  if not os.path.exists(os.path.join(DOWNLOADS, archive)):
    os.makedirs(DOWNLOADS, exist_ok=True)
    os.chdir(DOWNLOADS)
    print(f"Downloading {archive}")
    os.system(f"wget {url}/{archive}")
    os.chdir(g_configparser["BINDIR"])
  else:
    print(f"Found {archive} in {DOWNLOADS}")
  return

def tar_unpack_to(archive: str, build_dir: str):
  ext = os.path.splitext(archive)[-1]
  if ext == '.gz' or ext == '.tgz':
    tool = "gzip"
  elif ext == '.bz2':
    tool = "bunzip2"
  os.makedirs(build_dir, exist_ok=True)
  os.chdir(build_dir)

  DOWNLOADS = g_configparser["DOWNLOAD_DIR"]
  os.system(f"{tool} -dc {DOWNLOADS}/{archive} | tar x")
  return

def error_exit(error_level, error_msg):
  log = g_configparser["COLINUX_BUILD_LOG"]
  err = g_configparser["COLINUX_BUILD_ERR"]
  if error_level < 10:
    if os.path.isfile(log) and os.path.getsize(log) > 0:
      print(f"\n  --- BUILD LOG {log}:")
      os.system(f"tail -n 20 {log}")
    if os.path.isfile(err) and os.path.getsize(err) > 0:
      print(f"\n  --- ERROR LOG {err}:")
      os.system(f"tail -n 20 {err}")
  print(error_msg)
  sys.exit(error_level)

def build_common_post_vars():
  if g_configparser["PREFIX"] == "":
    print("Please specify `PREFIX'")
    sys.exit(-1)
  os.environ["PATH"] = os.environ["PATH"] + ":" + g_configparser["PREFIX"]+"/bin"
  return

def build_common_main():
  build_common_get_vars()
  build_common_post_vars()

build_common_get_vars()
