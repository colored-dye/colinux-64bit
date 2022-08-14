#!/bin/sh

set -e

BUILDROOT=$HOME/mingw-build
PREFIX=$HOME/mingw64
DOWNLOADS=$BUILDROOT/downloads
BUILD_DIR=$BUILDROOT/build
TARGET=x86_64-w64-mingw32
LOG=$BUILDROOT/build.log
ERR=$BUILDROOT/build.err
BUILD_FLAGS="CFLAGS=-O2 LDFLAGS=-s"

BINUTILS_VERSION="2.23.1"
BINUTILS_URL=https://ftp.gnu.org/pub/gnu/binutils
BINUTILS_RELEASE="$BINUTILS_VERSION"
BINUTILS=binutils-$BINUTILS_RELEASE
BINUTILS_ARCHIVE=$BINUTILS.tar.bz2

GCC_VERSION="4.7.0"
GCC_RELEASE="$GCC_VERSION"
GCC=gcc-$GCC_RELEASE
GCC_ARCHIVE=gcc-$GCC_RELEASE.tar.bz2
GCC_URL=https://ftp.gnu.org/pub/gnu/gcc/gcc-$GCC_VERSION

MINGW_VERSION=4.7.0
MINGW_ARCHIVE=$TARGET-gcc-$MINGW_VERSION-release-linux64_rubenvb.tar.lzma
MINGW_URL=https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/rubenvb/gcc-4.7-release

download() {
	echo -n "Downloading ... "
	mkdir -p $DOWNLOADS

	echo -n "binutils "
	test -f $DOWNLOADS/$BINUTILS_ARCHIVE || wget $BINUTILS_URL/$BINUTILS_ARCHIVE -P $DOWNLOADS >>$LOG 2>>$ERR

	echo -n "MinGW "
	test -f $DOWNLOADS/$MINGW_ARCHIVE || wget $MINGW_URL/$MINGW_ARCHIVE -P $DOWNLOADS >>$LOG 2>>$ERR

	echo "GCC"
	test -f $DOWNLOADS/$GCC_ARCHIVE || wget $GCC_URL/$GCC_ARCHIVE -P $DOWNLOADS >>$LOG 2>>$ERR
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
		*.tar.lzma)
			tool=lzma
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

build_binutils() {
	echo "Extracting binutils"
	rm -rf $BUILD_DIR/$BINUTILS
	tar_unpack_to $BINUTILS_ARCHIVE $BUILD_DIR

	echo "Configuring binutils"
	cd $BUILD_DIR
	rm -rf binutils-$TARGET
	mkdir binutils-$TARGET
	cd binutils-$TARGET
	../$BINUTILS/configure -v --prefix=$PREFIX --with-sysroot=$PREFIX \
		--target=$TARGET --disable-nls --disable-werror \
		--disable-shared --enable-static \
		>>$LOG 2>>$ERR

	echo "Building binutils"
	make $BUILD_FLAGS -j 8 >>$LOG 2>>$ERR

	echo "Installing binutils"
	make install >>$LOG 2>>$ERR

	echo "Cleaning binutils"
	cd $BUILD_DIR
	rm -rf $BINUTILS binutils-$TARGET
}

build_mingw() {
	echo "Extracting MinGW"
	tar_unpack_to $MINGW_ARCHIVE $PREFIX/..
}

###############################################

export LDFLAGS="-s"

mkdir -p $BUILD_DIR
download
# build_mingw
build_binutils

