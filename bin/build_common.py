#-*-coding: UTF-8-*-

import os
import configparser

# Global parameters
BINDIR = ""
TOPDIR = ""
COLINUX_HOST_OS = ""
DOWNLOAD_DIR = ""
BUILD_DIR = ""
PREFIX = ""
KERNEL_VERSION = ""
KERNEL_SOURCE = ""
KERNEL_BUILD = ""
MODULE_PATH = ""

MINGW_VERSION = ""
MINGW_ARCHIVE = ""
MINGW_URL = ""

BINUTILS_VERSION = ""
BINUTILS_ARCHIVE = ""
BINUTILS_URL = ""

GCC_VERSION = ""
GCC_ARCHIVE = ""
GCC_URL = ""

FLTK_VERSION = ""
FLTK_URL = ""
FLTK_ARCHIVE = ""

WINPCAP_VERSION = ""
WINPCAP_URL = ""
WINPCAP_ARCHIVE = ""

TARGET = ""
BUILD = ""

def build_common_get_vars():
  global BINDIR
  global TOPDIR
  global COLINUX_HOST_OS
  global DOWNLOAD_DIR
  global BUILD_DIR
  global PREFIX
  global KERNEL_VERSION
  global KERNEL_SOURCE
  global KERNEL_BUILD
  global MODULE_PATH
  global TARGET
  global BUILD

  global MINGW_VERSION
  global MINGW_URL
  global MINGW_ARCHIVE

  global BINUTILS_VERSION
  global BINUTILS_URL
  global BINUTILS_ARCHIVE

  global GCC_VERSION
  global GCC_URL
  global GCC_ARCHIVE

  global FLTK_VERSION
  global FLTK_URL
  global FLTK_ARCHIVE

  global WINPCAP_VERSION
  global WINPCAP_URL
  global WINPCAP_ARCHIVE

  if os.path.exists("bin/build_common.py"):
    BINDIR = os.path.abspath(os.path.join(os.curdir, "bin"))
  else:
    BINDIR = os.path.abspath(os.curdir)

  # Default top directory. Can be overwritten by user config.
  TOPDIR = os.path.dirname(BINDIR)

  user_config = configparser.ConfigParser()
  user_config.read(os.path.join(BINDIR, "user-build.cfg"))
  user_config = user_config["config"]

  COLINUX_HOST_OS = user_config["colinux_host_os"]
  if user_config["topdir"] != "":
    TOPDIR = user_config["topdir"]
    if TOPDIR[-1] == '/':
      TOPDIR = TOPDIR[:-1]
  DOWNLOAD_DIR = user_config["downloads"].replace("topdir", TOPDIR)
  BUILD_DIR = user_config["build_dir"].replace("topdir", TOPDIR)
  PREFIX = user_config["prefix"].replace("topdir", TOPDIR)
  KERNEL_VERSION = user_config["kernel_version"]
  KERNEL_SOURCE = user_config["kernel_source"].replace("build_dir", BUILD_DIR)
  KERNEL_BUILD = user_config["kernel_build"].replace("build_dir", BUILD_DIR)
  MODULE_PATH = user_config["module_path"].replace("kernel_build", KERNEL_BUILD)

  MINGW_VERSION = user_config["mingw_version"]
  MINGW_URL = "https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release"
  MINGW_ARCHIVE = f"mingw-w64-v{MINGW_VERSION}.tar.bz2"

  BINUTILS_VERSION = user_config["binutils_version"]
  BINUTILS_URL = "https://ftp.gnu.org/pub/gnu/binutils"
  BINUTILS_ARCHIVE = f"binutils-{BINUTILS_VERSION}.tar.bz2"

  GCC_VERSION = user_config["gcc_version"]
  GCC_URL = "https://ftp.gnu.org/pub/gnu/gcc"
  GCC_ARCHIVE = f"gcc-{GCC_VERSION}/gcc-{GCC_VERSION}.tar.gz"

  FLTK_VERSION = user_config["fltk_version"]
  FLTK_URL = "https://www.fltk.org/pub/fltk"
  FLTK_ARCHIVE = f"{FLTK_VERSION}/fltk-{FLTK_VERSION}-source.tar.bz2"

  WINPCAP_VERSION = user_config["winpcap_version"]
  WINPCAP_URL = "https://winpcap.org/archive"
  WINPCAP_ARCHIVE = f"{WINPCAP_VERSION}-WpdPack.zip"

  TARGET = "x86_64-pc-mingw32"
  TARGET_ARCH = TARGET.split('-')[0]
  BUILD = "x86_64-pc-linux"
