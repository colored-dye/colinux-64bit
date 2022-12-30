#!/bin/sh

# We need to find our self here, or in subdir bin
# BINDIR is bin directory with scripts
if [ -f bin/build-common.sh ]; then
	BINDIR=`pwd`/bin
elif [ -f build-common.sh ]; then
	BINDIR=`pwd`
else
	echo "build-common.sh: Can't detect bin directory!" >&2
	exit 1
fi

# TOPDIR is the directory ./configure is in.
TOPDIR=`dirname $BINDIR`

# Default Kernel version we are targeting, can overwrite in CFG file.
# Remember: Please update also conf/kernel-*-config
#

# Use User config, if exist
# you probably don't need to change anything from here down
if [ -f $BINDIR/user-build.cfg ] ; then
	# Users directories
	. $BINDIR/user-build.cfg
else
	# fall back to default config
	. $BINDIR/sample.user-build.cfg
fi

case $OSTYPE in
    darwin*)
	TARGET=$MACHTYPE
	BUILD=$TARGET

	# Tweaks for Darwin bash.
	set +o posix 2>/dev/null
	;;
    *)
	# what flavor are we building?
	case $COLINUX_HOST_ARCH in
	    x86_64)
		TARGET=x86_64-w64-mingw32
		;;
	    *)
		TARGET=i686-pc-mingw32
		# TARGET=i686-w64-mingw32
		;;
	esac
	;;
esac

# ARCH must overwrite for builds on 64 bit (target kernel)
TARGET_GUEST_ARCH=i386

# MINGW="mingw-runtime-$MINGW_VERSION"
MINGW="mingw-runtime-$MINGW_VERSION"
MINGW_ARCHIVE="$MINGW.tar.gz"
MINGW_URL="http://sourceforge.net/projects/mingw/files/OldFiles/$MINGW"

BINUTILS_RELEASE="$BINUTILS_VERSION"
BINUTILS=binutils-$BINUTILS_RELEASE
BINUTILS_ARCHIVE=$BINUTILS.tar.bz2
BINUTILS_URL="http://ftp.gnu.org/pub/gnu/binutils"

GCC_RELEASE="$GCC_VERSION"
GCC=gcc-$GCC_RELEASE
GCC_ARCHIVE1="gcc-core-$GCC_RELEASE.tar.bz2"
GCC_ARCHIVE2="gcc-g++-$GCC_RELEASE.tar.bz2"
GCC_URL="http://ftp.gnu.org/pub/gnu/gcc/gcc-$GCC_VERSION"

W32API_PART="w32api-$W32API_VERSION"
W32API="$W32API_PART-mingw32"
W32API_ARCHIVE="$W32API-dev.tar.gz"
W32API_URL="http://sourceforge.net/projects/mingw/files/OldFiles/$W32API_PART"

FLTK_URL="http://www.fltk.org/pub/fltk"
FLTK=fltk-$FLTK_VERSION
FLTK_ARCHIVE=$FLTK-source.tar.bz2

WINPCAP_SRC=WpdPack
WINPCAP_URL="http://www.winpcap.org/archive"
WINPCAP_SRC_ARCHIVE="$WINPCAP_VERSION-$WINPCAP_SRC.zip"

# KERNEL_VERSION: full kernel version (e.g. 2.6.11)
# KERNEL_DIR: sub-dir in www.kernel.org for the download (e.g. v2.6)
KERNEL_DIR=`echo $KERNEL_VERSION | sed -e 's/^\([0-9]*\.[0-9]*\)\..*$/v\1/'`

KERNEL=linux-$KERNEL_VERSION
KERNEL_URL="http://www.kernel.org/pub/linux/kernel/$KERNEL_DIR"
KERNEL_ARCHIVE=$KERNEL.tar.bz2

CO_VERSION=`cat $TOPDIR/src/colinux/VERSION`
COMPLETE_KERNEL_NAME=$KERNEL_VERSION-co-$CO_VERSION

# Get variables only? Then end here.
if [ "$1" = "--get-vars" ]; then
	return
fi

# where does it go?
if [ -z "$PREFIX" ] ; then
	echo "Please specify the $""PREFIX directory in user-build.cfg (e.g, /home/$USER/mingw32)"
	exit -1
fi

# where does it go?
if [ -z "$DOWNLOADS" ] ; then
	echo "Please specify the $""DOWNLOADS directory in user-build.cfg (e.g, /tmp/$USER/download)"
	exit -1
fi

# where does it go?
if [ -z "$BUILD_DIR" ] ; then
	echo "Please specify the $""BUILD_DIR directory in user-build.cfg (e.g, /tmp/$USER/build)"
	exit -1
fi

# Default path to modules
if [ -z "$COLINUX_TARGET_MODULE_PATH" ] ; then
	COLINUX_TARGET_MODULE_PATH="$COLINUX_TARGET_KERNEL_BUILD/_install"
fi

# Default logfile of building (Append), can overwrite in user-build.cfg
if [ -z "$COLINUX_BUILD_LOG" ] ; then
    COLINUX_BUILD_LOG="$TOPDIR/build-colinux-$$.log"
fi

if [ -z "$COLINUX_BUILD_ERR" ] ; then
    COLINUX_BUILD_ERR="$TOPDIR/build-colinux-$$.err"
fi

# Install directory set?
if [ -z "$COLINUX_INSTALL_DIR" ] ; then
    echo "Please specify the $""COLINUX_INSTALL_DIR in user-build.cfg (e.g, /home/$USER/colinux/dist)"
    exit -1
fi

# These are the files from the SDL website
# need install directory first on the path so gcc can find binutils
# Fairly for ccache: Add the cross path at end.

PATH="$PATH:$PREFIX/bin"

#
# download a file from a given url, only if it is not present
#

download_file()
{
	# Make sure wget is installed
	if [ "x`which wget`" = "x" ]
	then
		echo "You need to install wget." >&2
		exit 1
	fi

	if [ ! -f "$DOWNLOADS/$1" ]
	then
		mkdir -p "$DOWNLOADS"
		cd "$DOWNLOADS"
		echo "Downloading $1"
		if ! wget "$2/$1" --no-check-certificate
		then
			echo "Could not download $1"
			# move broken download
			test -f $1 && mv $1 $1.incomplete
			exit 1
		fi
		cd "$BINDIR"
	else
		echo "Found $1 in the srcdir $DOWNLOADS"
	fi
}

# Arg1: gzip or bzip2 tar archive
# Arg2: destination directory
tar_unpack_to()
{
	local tool

	case "$DOWNLOADS/$1" in
		*.tar.gz|*.tgz)
			tool=gzip
		;;
		*.tar.bz2)
			tool=bunzip2
		;;
		*)
			echo "$FUNCNAME($LINENO): unknown extension for $1" >&2
			exit 1
		;;
	esac

	mkdir -p "$2"
	cd "$2"
	$tool -dc "$DOWNLOADS/$1" | tar x \
	|| { echo "unpack failed for $1" >&2; exit 1; }
}

#
# Show errors from actual logfile, than exit build process
# Arg1: Errorlevel
# Arg2: Error message
#
error_exit()
{
	# Show errors in log file with tail, if errorlevel < 10
	if [ $1 -lt 10 ]; then
		if [ -s $COLINUX_BUILD_LOG ]; then
			echo -e "\n  --- BUILD LOG $COLINUX_BUILD_LOG:"
			tail -n 20 $COLINUX_BUILD_LOG
		fi
		if [ -s $COLINUX_BUILD_ERR ]; then
			echo -e "\n  --- ERROR LOG $COLINUX_BUILD_ERR:"
			tail -n 20 $COLINUX_BUILD_ERR
		fi
	fi

	echo "$2"
	exit $1
}

#
# Strip kernel image file 'vmlinux'
# Arg1: input file
# Arg2: stripped output file
#
strip_kernel()
{
	local STRIP="strip --strip-all"
	local FROM_SOURCE="src/colinux/user/daemon.c"
	local KEEP

	# Build the list of needed symbols: Grep from loader function in daemon, this lines
	# --> rc = co_daemon_load_symbol(daemon, "init_thread_union", &import->kernel_init_task_union);
	# --> rc = co_daemon_load_symbol_and_data(daemon, "co_arch_info", &import->kernel_co_arch_info,
	#     _____^^^^^^^^^^^^^^^^^^^^^__________________^************^___^^^^^^______________________

	KEEP=`grep "co_daemon_load_symbol" $TOPDIR/$FROM_SOURCE | \
	  sed -n -e 's/^.*daemon[^"]*"\([^"]*\)".*import.*$/ --keep-symbol=\1/p' | tr -d "\n"`
	if [ -n "$KEEP" ]
	then
		# Kernel strip
		$STRIP $KEEP -o $2 $1 || exit $?
	else
		# Function not found by grep
		echo -e "\nWARNING: $FROM_SOURCE" >&2
		echo -e "Can't get symbols for stripping! Don't strip vmlinux\n" >&2

		# Fallback into copy mode
		cp -a $1 $2
	fi
}

# Create ZIP packages (for "autobuild")
build_package()
{
	local name bname oname
	local DATE=`LANG=C TZ="UTC" date +%G%m%d`
	local SYMBOLS_ZIP=$COLINUX_INSTALL_DIR/daemons-$CO_VERSION-$DATE.dbg.zip
	local DAEMONS_ZIP=$COLINUX_INSTALL_DIR/daemons-$CO_VERSION-$DATE.zip
	local VMLINUX_ZIP=$COLINUX_INSTALL_DIR/vmlinux-$COMPLETE_KERNEL_NAME-$DATE.zip
	local MODULES_TGZ=$COLINUX_INSTALL_DIR/modules-$COMPLETE_KERNEL_NAME-$DATE.tgz
	local EXE_DIR="$TOPDIR/src/colinux/os/winnt/build"
	local PREMAID="$TOPDIR/src/colinux/os/winnt/user/install/premaid"

	echo "Create ZIP packages into $COLINUX_INSTALL_DIR"
	mkdir -p $COLINUX_INSTALL_DIR

	# remove old zip files
	rm -f $SYMBOLS_ZIP $DAEMONS_ZIP

	# Add files with debugging symbols into zip
	zip -j "$SYMBOLS_ZIP" $EXE_DIR/*.exe $EXE_DIR/*.sys || exit $?

	# Use stripped files from installer and add to zip
	zip -j $DAEMONS_ZIP $PREMAID/*.exe $PREMAID/*.sys || exit $?

	# Exist Kernel and is newer?
	if [ $COLINUX_TARGET_KERNEL_BUILD/vmlinux -nt $VMLINUX_ZIP ]
	then
		echo "Installing Kernel $KERNEL_VERSION in $COLINUX_INSTALL_DIR"

		# remove old zip file
		rm -f $VMLINUX_ZIP

		if [ "$COLINUX_KERNEL_STRIP" = "yes" ]
		then
			name=vmlinux
			oname=$COLINUX_INSTALL_DIR/$name

			# Create map file with symbols, add to zip
			map=$COLINUX_INSTALL_DIR/$name.map
			nm $COLINUX_TARGET_KERNEL_BUILD/$name | sort | uniq > $map
			zip -j $VMLINUX_ZIP $map || exit $?
			rm $map

			# Strip kernel and add to ZIP
			strip_kernel $COLINUX_TARGET_KERNEL_BUILD/$name $oname
			zip -j $VMLINUX_ZIP $oname || exit $?
			rm $oname
		else
			# Add kernel to ZIP (not stripped)
			zip -j $VMLINUX_ZIP $COLINUX_TARGET_KERNEL_BUILD/vmlinux || exit $?
		fi
	fi

	# Copy modules file
	echo "Installing Modules $KERNEL_VERSION in $COLINUX_INSTALL_DIR"
	cp -p $COLINUX_TARGET_KERNEL_BUILD/vmlinux-modules.tar.gz $MODULES_TGZ
}
