#!/bin/sh

# Build cross platform mingw32.

. ./build-common.sh

# Flags for building gcc (not for target)
BUILD_FLAGS="CFLAGS=-O2 LDFLAGS=-s"

# until release, disable checking for faster builds:
DISABLE_CHECKING=--disable-checking

download_files()
{
	download_file "$GCC_ARCHIVE1" "$GCC_URL"
	download_file "$GCC_ARCHIVE2" "$GCC_URL"
	download_file "$BINUTILS_ARCHIVE" "$BINUTILS_URL"
	download_file "$MINGW_ARCHIVE" "$MINGW_URL"
	download_file "$W32API_ARCHIVE" "$W32API_URL"
}

check_installed()
{
	echo -n "Check cross compiler: "
	cd "$TOPDIR"
	if [ -x $PREFIX/bin/$TARGET-gcc -a \
	     -x $PREFIX/bin/$TARGET-ld -a \
	     -x $PREFIX/bin/$TARGET-windres -a \
	     -x $PREFIX/bin/$TARGET-strip ]
	then
		# Verify version of installed GCC and LD
		if [ `$TARGET-gcc -dumpversion` != $GCC_VERSION ]
		then
			echo "$TARGET-gcc $GCC_VERSION not installed"
			return
		fi

		# if $TARGET-ld --version | egrep -q "$BINUTILS_VERSION"
		# then
		# 	echo "Skip $TARGET-gcc, $TARGET-ld"
		# 	echo " - already installed on $PREFIX/bin"
		# 	exit 0
		# else
		# 	echo "$TARGET-ld $BINUTILS_VERSION not installed"
		# 	return
		# fi

	fi
	echo "No executable, rebuilding"
}


install_libs()
{
	echo "Installing cross libs and includes"
	tar_unpack_to $MINGW_ARCHIVE "$PREFIX/$TARGET"
	tar_unpack_to $W32API_ARCHIVE "$PREFIX/$TARGET"
}

extract_mingw()
{
	echo "Extracting MinGW"
	tar_unpack_to $MINGW_ARCHIVE $BUILD_DIR
	return 0
}

#
# Cross binutils
#

check_binutils()
{
	echo -n "Check cross binutils $BINUTILS_VERSION: "
	local PATH="$PATH:$PREFIX/bin"
	${TARGET}-ld -v 2>/dev/null 1>2
	if [ $? -ne 0 ]
	then
		echo "No executables, build now"
		return 1
	fi
	
	regex="^gnu ld \(.*\) (${BINUTILS_VERSION})"
	ver=`${TARGET}-ld -v 2>/dev/null | \
		egrep -i -c "${regex}"`

	# Verify version of installed AS
	if [ $ver -ne 1 ]
	then
		echo "Wrong version ($ver), build now"
		return 1
	fi

	echo "found"

	return 0
}

extract_binutils()
{
	echo "Extracting binutils"
	rm -rf "$BUILD_DIR/$BINUTILS"
	tar_unpack_to "$BINUTILS_ARCHIVE" "$BUILD_DIR"
	return 0
}

patch_binutils()
{
	# Fixup wrong path in tar
	test -d $BUILD_DIR/${BINUTILS}-src && mv $BUILD_DIR/${BINUTILS}-src $BUILD_DIR/${BINUTILS}
	if [ "$BINUTILS_PATCH" != "" ]; then
		echo "Patching binutils"
		cd "$BUILD_DIR/$BINUTILS"
		patch -p1 < "$TOPDIR/$BINUTILS_PATCH"
		test $? -ne 0 && error_exit 10 "patch binutils failed"
	fi
}

configure_binutils()
{
	echo "Configuring binutils"
	cd "$BUILD_DIR"
	rm -rf "binutils-$TARGET"
	mkdir "binutils-$TARGET"
	cd "binutils-$TARGET"
	"../$BINUTILS/configure" \
		--prefix="$PREFIX" --target=$TARGET \
		--disable-nls --disable-multilib \
		>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "configure binutils failed"
	return 0
}

build_binutils()
{
	echo "Building binutils"
	cd "$BUILD_DIR/binutils-$TARGET"
	make $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "make binutils failed"
	return 0
}

install_binutils()
{
	echo "Installing binutils"
	cd "$BUILD_DIR/binutils-$TARGET"
	make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "install binutils failed"
	return 0
}

clean_binutils()
{
	echo "Clean binutils"
	cd $BUILD_DIR
	rm -rf "$BINUTILS" binutils-$TARGET binutils-$COLINUX_GCC_GUEST_TARGET
}

#
# Cross GCC
#

check_gcc()
{
	echo -n "Check cross compiler $GCC_VERSION: "

	# Get version number
	local PATH="$PATH:$PREFIX/bin"
	ver=`${TARGET}-gcc -dumpversion 2>/dev/null`

	if [ -z "$ver" ]
	then
		echo "No executables, build now"
		return 1
	fi

	# Verify version of installed GCC
	if [ $ver != $GCC_VERSION ]
	then
		echo "Wrong version ($ver), build now"
		return 1
	fi

	echo "found"
	return 0
}

extract_gcc()
{
	echo "Extracting gcc"
	rm -rf "$BUILD_DIR/$GCC"
	tar_unpack_to "$GCC_ARCHIVE1" "$BUILD_DIR"
	tar_unpack_to "$GCC_ARCHIVE2" "$BUILD_DIR"

	return 0
}

patch_gcc()
{
	if [ "$GCC_PATCH" != "" ]; then
		echo "Patching gcc"
		cd "$BUILD_DIR/$GCC"
		patch -p1 < "$TOPDIR/$GCC_PATCH"
		test $? -ne 0 && error_exit 10 "patch gcc failed"
	fi
}

check_gmp_mpfr()
{
	echo "Checking GMP and MPFR"
	if test ! -f $PREFIX/gmp/include/gmp.h || test -f $PREFIX/mpfr/include/mpfr.h
	then
		echo "No GMP or MPFR, rebuild"
		return 1
	fi
	return 0
}

build_gmp()
{
	echo "Building GMP"
	tar_unpack_to "$GMP_ARCHIVE" "$BUILD_DIR"
	cd "$BUILD_DIR"
	rm -rf gmp-build
	mkdir gmp-build
	cd gmp-build
	"../$GMP/configure" -v \
		--prefix="$PREFIX/gmp" \
		--disable-shared --enable-static \
		>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "configure gmp failed"
	make >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "make gmp failed"
	make check >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "check gmp failed"
	make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "install gmp failed"
	return 0
}

build_mpfr()
{
	echo "Building MPFR"
	tar_unpack_to "$MPFR_ARCHIVE" "$BUILD_DIR"
	cd $BUILD_DIR
	rm -rf mpfr-build
	mkdir mpfr-build
	cd mpfr-build
	"../$MPFR/configure" -v \
		--prefix="$PREFIX/mpfr" \
		--disable-shared --enable-static \
		--with-gmp="$PREFIX/gmp" \
		>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "configure mpfr failed"
	make >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "make mpfr failed"
	make check >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "check mpfr failed"
	make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "install mpfr failed"
	return 0
}

configure_gcc()
{
	echo "Configuring gcc"
	cd "$BUILD_DIR"
	rm -rf "gcc-$TARGET"
	mkdir "gcc-$TARGET"
	cd "gcc-$TARGET"
	"../$GCC/configure" -v \
		--prefix="$PREFIX" --target=$TARGET \
        --with-headers="$PREFIX/$TARGET/include" \
        --with-gnu-as --with-gnu-ld \
		--disable-nls --disable-multilib --without-newlib \
		--enable-languages="c,c++" \
		$DISABLE_CHECKING \
		>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR

	# GMP and MPFR are needed. Here I use libgmp-dev and libmpfr-dev in Ubuntu 14.04
	# "../$GCC/configure" -v \
	# 	--prefix=$PREFIX --target=$TARGET \
	# 	--disable-nls --disable-multilib \
	# 	--enable-languages="c,c++" \
	# 	$DISABLE_CHECKING \
	# 	>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "configure gcc failed"
	return 0
}

build_header()
{
	echo "Building MinGW headers"
	cd "$BUILD_DIR/$MINGW/mingw-w64-headers"
	rm -rf build
	mkdir build
	cd build
	../configure --prefix=$PREFIX \
		--host=$TARGET --build=$BUILD \
		>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "configure MinGW header failed"
	make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "build MinGW header failed"
	return 0
}

build_gcc()
{
	echo "Building gcc"
	cd "$BUILD_DIR/gcc-$TARGET"
	make all-gcc $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "make gcc base failed"
	make install-gcc $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "install gcc base failed"
	return 0
}

build_crt()
{
	echo "Building MinGW C-Runtime"
	cd $BUILD_DIR/$MINGW/mingw-w64-crt
	rm -rf build
	mkdir build
	cd build
	../configure --prefix=$PREFIX \
		--host=$TARGET --enable-lib32 --disable-lib64 \
		>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "configure crt failed"

	make $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "make crt failed"
	make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "install crt failed"

	return 0
}

install_gcc()
{
	echo "Installing gcc (final stage)"
	cd "$BUILD_DIR/gcc-$TARGET"
	make $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "make gcc failed"
	make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "install gcc failed"
	return 0
}

clean_gcc()
{
	echo "Clean gcc"
	cd $BUILD_DIR
	rm -rf "$GCC" gcc-$TARGET gcc-$COLINUX_GCC_GUEST_TARGET
}

#
# Build compiler for coLinux GUEST system kernel compiling.
#

check_binutils_guest()
{
	test -z "$COLINUX_GCC_GUEST_TARGET" && return 0
	echo -n "Check guest binutils $BINUTILS_VERSION: "

	# Get version number
	local PATH="$PATH:$COLINUX_GCC_GUEST_PATH"

	${COLINUX_GCC_GUEST_TARGET}-ld -v 2>/dev/null 1>2
	if [ $? -ne 0 ]
	then
		echo "No executables, build now"
		return 1
	fi

	regex="^gnu ld \(.*\) (${BINUTILS_VERSION})"
	ver=`${COLINUX_GCC_GUEST_TARGET}-ld -v 2>/dev/null | \
		egrep -i -c "${regex}"`

	# Verify version of installed AS
	if [ $ver -ne 1 ]
	then
		echo "Wrong version ($ver), build now"
		return 1
	fi

	echo "found"

	# Must have prefix for kernel build
	for name in ar as ld nm objdump objcopy strip
	do
		if ! which ${COLINUX_GCC_GUEST_TARGET}-$name >/dev/null 2>/dev/null
		then
			mkdir -p $COLINUX_GCC_GUEST_PATH
			ln -s `which $name` $COLINUX_GCC_GUEST_PATH/${COLINUX_GCC_GUEST_TARGET}-$name
			echo " softlink for ${COLINUX_GCC_GUEST_TARGET}-$name"
		fi
	done

	return 0
}

build_binutils_guest()
{
	echo "Configuring guest binutils"
	cd "$BUILD_DIR"
	rm -rf "binutils-$COLINUX_GCC_GUEST_TARGET"
	mkdir "binutils-$COLINUX_GCC_GUEST_TARGET"
	cd "binutils-$COLINUX_GCC_GUEST_TARGET"
	"../$BINUTILS/configure" \
		--program-prefix="${COLINUX_GCC_GUEST_TARGET}-" \
		--prefix="$PREFIX/$COLINUX_GCC_GUEST_TARGET" \
		--disable-nls \
		>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "configure guest binutils failed"

	echo "Building guest binutils"
	make $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "make guest binutils failed"

	echo "Installing guest binutils"
	make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "install guest binutils failed"
	return 0
}

check_gcc_guest()
{
	test -z "$COLINUX_GCC_GUEST_TARGET" && return 0
	echo -n "Check guest compiler $GCC_VERSION: "

	# Get version number
	local PATH="$PATH:$COLINUX_GCC_GUEST_PATH"
	ver=`${COLINUX_GCC_GUEST_TARGET}-gcc -dumpversion 2>/dev/null`

	if [ -z "$ver" ]
	then
		echo "No executables, build now"
		return 1
	fi

	# Verify version of installed GCC
	if [ $ver != $GCC_VERSION ]
	then
		echo "Wrong version ($ver), build now"
		return 1
	fi

	echo "found"
	return 0
}

build_gcc_guest()
{
	echo "Configuring guest gcc"
	cd "$BUILD_DIR"
	rm -rf "gcc-$COLINUX_GCC_GUEST_TARGET"
	mkdir "gcc-$COLINUX_GCC_GUEST_TARGET"
	cd "gcc-$COLINUX_GCC_GUEST_TARGET"
	"../$GCC/configure" -v \
		--program-prefix="${COLINUX_GCC_GUEST_TARGET}-" \
		--prefix="$PREFIX/$COLINUX_GCC_GUEST_TARGET" \
		--with-gmp=$PREFIX/gmp --with-gmp-include=$PREFIX/gmp/include --with-gmp-lib=$PREFIX/gmp/lib \
		--with-mpfr=$PREFIX/mpfr --with-mpfr-include=$PREFIX/mpfr/include --with-mpfr-lib=$PREFIX/mpfr/lib \
		--disable-nls \
		--enable-languages="c,c++" \
		$DISABLE_CHECKING \
		>>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "configure gcc failed"

	echo "Building guest gcc"
	make all-gcc $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "make guest gcc failed"

	echo "Installing guest gcc"
	make install-gcc >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
	test $? -ne 0 && error_exit 1 "install guest gcc failed"

	# remove info and man pages
	rm -rf "$PREFIX/$COLINUX_GCC_GUEST_TARGET/info" "$PREFIX/$COLINUX_GCC_GUEST_TARGET/man"
	return 0
}

final_tweaks()
{
	echo "Finalizing installation"

	# remove gcc build headers
	rm -rf "$PREFIX/$TARGET/sys-include"

	# remove info and man pages
	rm -rf "$PREFIX/info" "$PREFIX/man"

	echo "Installation complete!"
}

build_cross()
{
	# do not check files, if rebuild forced
	test "$1" = "--rebuild" || check_installed

	download_files
	# Only Download
	test "$1" = "--download-only" && exit 0

	echo "log: $COLINUX_BUILD_LOG"
	mkdir -p `dirname $COLINUX_BUILD_LOG`
	echo "err: $COLINUX_BUILD_ERR"
	mkdir -p `dirname $COLINUX_BUILD_ERR`

	# Build cross binutils
	# install_libs

	check_binutils
	test $? -eq 0 || \
		(extract_binutils && \
		configure_binutils && \
		build_binutils && \
		install_binutils)

	# Build binutils for kernel
	check_binutils_guest || (extract_binutils && build_binutils_guest)

	clean_binutils

	# Build cross MinGW gcc
	check_gcc
	test $? -eq 0 || \
		(extract_mingw && \
		build_header && \
		ln -sf $PREFIX/TARGET $PREFIX/mingw && \
		extract_gcc && \
		(check_gmp_mpfr || (build_gmp && build_mpfr) ) && \
		configure_gcc && \
		build_gcc && \
		build_crt && \
		install_gcc)
	
	# Build GCC for kernel
	check_gcc_guest
	test $? -eq 0 || ( \
		(check_gmp_mpfr || (build_gmp && build_mpfr) ) && extract_gcc && build_gcc_guest
	)
	
	clean_gcc

	final_tweaks
	echo "Installation complete!"
}

build_cross $1
