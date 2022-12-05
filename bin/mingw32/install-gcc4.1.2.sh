#!/usr/bin/bash
# Install gcc-4.1.2 through apt on Ubuntu 10.04

sudo mv /etc/apt/sources.list /etc/apt/sources.list.bak
sudo echo -e "deb http://old-releases.ubuntu.com/ubuntu/ lucid main restricted universe multiverse\ndeb http://old-releases.ubuntu.com/ubuntu/ lucid-security main restricted universe multiverse\ndeb http://old-releases.ubuntu.com/ubuntu/ lucid-updates main restricted universe multiverse\ndeb http://old-releases.ubuntu.com/ubuntu/ lucid-proposed main restricted universe multiverse\ndeb http://old-releases.ubuntu.com/ubuntu/ lucid-backports main restricted universe multiverse\ndeb-src http://old-releases.ubuntu.com/ubuntu/ lucid main restricted universe multiverse\n" > /etc/apt/sources.list
sudo apt-get install gcc-4.1

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.1 40

sudo ln -s /usr/include/i386-linux-gnu/bits /usr/include/bits
sudo ln -s /usr/include/i386-linux-gnu/asm /usr/include/asm
sudo ln -s /usr/include/i386-linux-gnu/sys /usr/include/sys
sudo ln -s /usr/include/i386-linux-gnu/gnu /usr/include/gnu
sudo ln -s /usr/lib/i386-linux-gnu/crti.o /usr/lib/crti.o
sudo ln -s /usr/lib/i386-linux-gnu/crt1.o /usr/lib/crt1.o
sudo ln -s /usr/lib/i386-linux-gnu/crtn.o /usr/lib/crtn.o

. ./build-common.sh

GCCPREFIX="/opt/$GCC"

download_file $GCC_URL/$GCC_ARCHIVE1
download_file $GCC_URL/$GCC_ARCHIVE2

# tar_unpack_to $BINUTILS_ARCHIVE $BUILD_DIR
tar_unpack_to $GCC_ARCHIVE1 $BUILD_DIR
tar_unpack_to $GCC_ARCHIVE2 $BUILD_DIR

BUILD_FLAGS="CFLAGS=-O2 LDFLAGS=-s -j4"

# Don't build binutils! Use system's built-in binutils.
# cd $BUILD_DIR/$BINUTILS
# mkdir -p build
# cd build
# ../configure --prefix=$GCCPREFIX --disable-nls --disable-multilib >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
# make $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
# sudo make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR

# PATH="$GCCPREFIX:$PATH"

cd $BUILD_DIR/$GCC
# patch -p1 < $TOPDIR/patch/gcc-4.1.2.patch
mkdir -p build
cd build
../configure --prefix=$GCCPREFIX --disable-nls --enable-languages=c,c++ >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
make $BUILD_FLAGS >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR
sudo make install >>$COLINUX_BUILD_LOG 2>>$COLINUX_BUILD_ERR

rm -rf $BINUTILS/build $GCC/build

echo -e "Finished building $GCC.\nAdd $GCCPREFIX to path."

