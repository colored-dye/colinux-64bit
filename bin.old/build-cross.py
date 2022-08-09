import os
import subprocess
import shutil
from build_common import *
# from ConfigParser import ConfigParser

BUILD_FLAGS = "CFLAGS=-O2 LDFLAGS=-s"

CONFIG_VARS = ".config_vars"
g_configparser = None

def build_cross_download_files():
  download_file(g_configparser["GCC_ARCHIVE"], g_configparser["GCC_URL"])
  download_file(g_configparser["BINUTILS_ARCHIVE"], g_configparser["BINUTILS_URL"])
  download_file(g_configparser["MINGW_ARCHIVE"], g_configparser["MINGW_URL"])
  return

def check_installed():
  print("Check cross compiler:")
  os.chdir(g_configparser["TOPDIR"])
  PREFIX = g_configparser["PREFIX"]
  TARGET = g_configparser["TARGET"]
  if os.path.exists(f"{PREFIX}/bin/{TARGET}-gcc") \
      and os.path.exists(f"{PREFIX}/bin/{TARGET}-ld") \
      and os.path.exists(f"{PREFIX}/bin/{TARGET}-windres") \
      and os.path.exists(f"{PREFIX}/bin/{TARGET}-strip"):
    ret = subprocess.run(f"{TARGET}-gcc -dumpfullversion", shell=True, stdout=subprocess.PIPE)
    GCC_VERSION = g_configparser["GCC_VERSION"]
    if ret.stdout.decode().strip() != GCC_VERSION:
      print(f"{TARGET}-gcc {GCC_VERSION} not installed")
      return
    BINUTILS_VERSION = g_configparser["BINUTILS_VERSION"]
    if os.system(f"{TARGET}-ld --version | egrep -q {BINUTILS_VERSION}") == 0:
      print(f"Skip {TARGET}-gcc, {TARGET}-ld")
      print(f" - already installed in {PREFIX}/bin")
      sys.exit(0)
    else:
      print(f"{TARGET}-ld {BINUTILS_VERSION} not installed")
      return
  print("No executable, rebuilding")
  return

def install_libs():
  print("Installing cross libs mingw")
  PREFIX = g_configparser["PREFIX"]
  TARGET = g_configparser["TARGET"]
  tar_unpack_to(g_configparser["MINGW_ARCHIVE"], f"{PREFIX}/{TARGET}")
  return

def extract_binutils():
  print("Extracting binutils")
  BUILD_DIR = g_configparser["BUILD_DIR"]
  BINUTILS = g_configparser["BINUTILS"]
  shutil.rmtree(f"{BUILD_DIR}/{BINUTILS}", ignore_errors=True)
  tar_unpack_to(g_configparser["BINUTILS_ARCHIVE"], BUILD_DIR)
  return

# def patch_binutils():
#   BUILD_DIR = g_configparser["BUILD_DIR"]
#   BINUTILS = g_configparser["BINUTILS"]
#   if not os.path.isdir(f"{BUILD_DIR}/{BINUTILS}-src"):
#     shutil.move(f"{BUILD_DIR}/{BINUTILS}-src", f"{BUILD_DIR}/{BINUTILS}")
#   return

def configure_binutils():
  print("Configuring binutils")
  os.chdir(g_configparser["BUILD_DIR"])
  TARGET = g_configparser["TARGET"]
  BINUTILS = g_configparser["BINUTILS"]
  PREFIX = g_configparser["PREFIX"]
  log = g_configparser["COLINUX_BUILD_LOG"]
  err = g_configparser["COLINUX_BUILD_ERR"]
  shutil.rmtree(f"binutils-{TARGET}", ignore_errors=True)
  os.mkdir(f"binutils-{TARGET}")
  os.chdir(f"binutils-{TARGET}")
  if os.system(f"../{BINUTILS}/configure --prefix={PREFIX} --target={TARGET} \
    --disable-nls >>{log} 2>>{err}") != 0:
    error_exit(1, "Configure binutils failed")
  return

def build_binutils():
  print("Building binutils")
  BUILD_DIR = g_configparser["BUILD_DIR"]
  TARGET = g_configparser["TARGET"]
  log = g_configparser["COLINUX_BUILD_LOG"]
  err = g_configparser["COLINUX_BUILD_ERR"]
  os.chdir(f"{BUILD_DIR}/binutils-{TARGET}")
  if os.system(f"make {BUILD_FLAGS} >>{log} 2>>{err}") != 0:
    error_exit(1, "Make binutils failed")
  return

def install_binutils():
  print("Installing binutils")
  os.chdir(g_configparser["BUILD_DIR"]+"/binutils-"+g_configparser["TARGET"])
  if os.system("make install >>"+g_configparser["COLINUX_BUILD_LOG"]+" 2>>"+g_configparser["COLINUX_BUILD_ERR"]) != 0:
    error_exit(1, "Install binutils failed")
  return

def clean_binutils():
  print("Clean binutils")
  os.chdir(g_configparser["BUILD_DIR"])
  shutil.rmtree(g_configparser["BINUTILS"], ignore_errors=True)
  shutil.rmtree("binutils-"+g_configparser["TARGET"], ignore_errors=True)

def check_binutils_guest():
  print("Check guest binutils " + g_configparser["BINUTILS_VERSION"] + ":")
  os.environ["PATH"] = os.environ["PATH"] + ":" + g_configparser["COLINUX_GCC_GUEST_PATH"]
  ret = subprocess.run(g_configparser["COLINUX_GCC_GUEST_TARGET"]+"-as --version 2>/dev/null", shell=True, stdout=subprocess.PIPE)
  if ret.stdout == b"":
    print("No executables, build now")
    return False
  ret = subprocess.run(g_configparser["COLINUX_GCC_GUEST_TARGET"]+"-as --version 2>/dev/null | grep Binutils", shell=True, stdout=subprocess.PIPE)
  ver = ret.stdout.decode().strip().split(' ')[6]
  if ver != g_configparser["BINUTILS_VERSION"]:
    print(f"Wrong version {ver}, build now")
    return False
  
  print("Found")
  
  return True

def build_binutils_guest():
  print("Configuring guest binutils")
  os.chdir(g_configparser["BUILD_DIR"])
  shutil.rmtree("binutils-" + g_configparser["COLINUX_GCC_GUEST_TARGET"], ignore_errors=True)
  os.mkdir("binutils-" + g_configparser["COLINUX_GCC_GUEST_TARGET"])
  os.chdir("binutils-" + g_configparser["COLINUX_GCC_GUEST_TARGET"])

  log = g_configparser["COLINUX_BUILD_LOG"]
  err = g_configparser["COLINUX_BUILD_ERR"]
  if os.system("../"+g_configparser["BINUTILS"]+"/configure \
      --program-prefix=" + g_configparser["COLINUX_GCC_GUEST_TARGET"] + "- \
      --prefix=" + g_configparser["PREFIX"] + "/" + g_configparser["COLINUX_GCC_GUEST_TARGET"] + 
      " --disable-nls >>" + log + 
      " 2>>" + err) != 0:
    error_exit(1, "Configure guest binutils failed")
  
  print("Building guest binutils")
  if os.system("make " + BUILD_FLAGS + " >>" + log + " 2>>" + err) != 0:
    error_exit(1, "Make guest binutils failed")
  
  print("Installing guest binutils")
  if os.system("make install >>" + log + " 2>>" + err) != 0:
    error_exit(1, "Install guest binutils failed")
  return

def extract_gcc():
  print("Extracting gcc")
  BUILD_DIR = g_configparser["BUILD_DIR"]
  shutil.rmtree(BUILD_DIR + "/" + g_configparser["GCC"], ignore_errors=True)
  tar_unpack_to(g_configparser["GCC_ARCHIVE"], BUILD_DIR)
  return

def configure_gcc():
  print("Configuring gcc")
  os.chdir(g_configparser["BUILD_DIR"])
  shutil.rmtree("gcc-{}".format(g_configparser["TARGET"]), ignore_errors=True)
  os.mkdir("gcc-{}".format(g_configparser["TARGET"]))
  os.chdir("gcc-{}".format(g_configparser["TARGET"]))

  if os.system("../{}/configure -v --prefix={} --target={} \
      --with-headers={}/{}/include --with-gnu-as --with-gnu-ld \
      --disable-nls --without-newlib --disable-multilib \
      --enable-languages=\"c,c++\" --disable-checking \
      >>{} 2>>{}".format(
            g_configparser["GCC"], g_configparser["PREFIX"], g_configparser["TARGET"],
            g_configparser["PREFIX"], g_configparser["TARGET"],
            g_configparser["COLINUX_BUILD_LOG"], g_configparser["COLINUX_BUILD_ERR"]
          )) != 0:
    error_exit(1, "Configure gcc failed")
  return

def build_gcc():
  print("Building gcc")
  os.chdir("{}/gcc-{}".format(g_configparser["BUILD_DIR"], g_configparser["TARGET"]))
  if os.system("make {} >>{} 2>>{}".format(
    BUILD_FLAGS, g_configparser["COLINUX_BUILD_LOG"], g_configparser["COLINUX_BUILD_ERR"]
  )) != 0:
    error_exit(1, "Make gcc failed")
  return

def install_gcc():
  print("Installing gcc")
  os.chdir("{}/gcc-{}".format(g_configparser["BUILD_DIR"], g_configparser["TARGET"]))
  if os.system("make install >>{} 2>>{}".format(
    g_configparser["COLINUX_BUILD_LOG"], g_configparser["COLINUX_BUILD_ERR"]
  )) != 0:
    error_exit(1, "Install gcc failed")
  return

def clean_gcc():
  print("Clean gcc")
  os.chdir(g_configparser["BUILD_DIR"])
  shutil.rmtree(g_configparser["GCC"], ignore_errors=True)
  shutil.rmtree("gcc-{}".format(g_configparser["TARGET"]), ignore_errors=True)
  shutil.rmtree("gcc-{}".format(g_configparser["COLINUX_GCC_GUEST_TARGET"]), ignore_errors=True)
  return

# def check_gcc_guest():


# def build_gcc_guest():

# def final_tweaks():

def build_cross_main(args):
  global CONFIG_VARS, g_configparser
  if os.path.exists("bin/build-cross.py"):
    CONFIG_VARS = os.path.abspath('.') + "/" + CONFIG_VARS
  elif os.path.exists("build-cross.py"):
    CONFIG_VARS = os.path.abspath('../') + "/" + CONFIG_VARS
  g_configparser = ConfigParser(CONFIG_VARS)
  g_configparser.read()

  build_cross_download_files()

  print("log:", g_configparser["COLINUX_BUILD_LOG"])
  os.makedirs(os.path.dirname(g_configparser["COLINUX_BUILD_LOG"]), exist_ok=True)
  print("err:", g_configparser["COLINUX_BUILD_ERR"])
  os.makedirs(os.path.dirname(g_configparser["COLINUX_BUILD_ERR"]), exist_ok=True)

  # install_libs()

  # extract_binutils()
  # configure_binutils()
  # build_binutils()
  # install_binutils()
  # if not check_binutils_guest():
  #   build_binutils_guest()
  # clean_binutils()

  extract_gcc()
  configure_gcc()
  build_gcc()
  install_gcc()
  # clean_gcc()

  return

if __name__ == "__main__":
  build_common_main()
  build_cross_main(sys.argv[1:])
