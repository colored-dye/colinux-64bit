if test -f bin/build-common.sh
then
    ROOT=$(realpath `pwd`)
else
    ROOT=$(realpath ../)
fi
PREFIX=$ROOT/mingw/i686/i686-co-linux
BUILD=$ROOT/mingw/bld
LOG=$BUILD/log
ERR=$BUILD/err

mkdir -p $BUILD

#
# Build guest binutils
#

echo "Building guest binutils"
cd $BUILD
tar -xvf ../download/binutils.tar.xz >/dev/null
cd binutils-2.34
mkdir build
cd build
echo "Configuring guest binutils"
../configure \
    --program-prefix=i686-co-linux- \
    --target=i686-linux-gnu \
    --disable-multilib \
    --prefix=$PREFIX --with-sysroot=$PREFIX \
    --disable-nls --disable-shared --enable-static \
    --enable-lto \
    >>$LOG 2>>$ERR
echo "Making guest binutils"
make -j8 >>$LOG 2>>$ERR
echo "Installing guest binutils"
make install >>$LOG 2>>$ERR

#
# Build guest GCC
#

echo "Building guest GCC"
cd $BUILD
tar -xvf ../download/gcc.tar.xz >/dev/null
cd gcc-12.1.0
mkdir build
cd build
echo "Configuring guest GCC"
../configure \
    --program-prefix=i686-co-linux- \
    --target=i686-linux-gnu \
    --disable-shared --enable-static \
    --disable-multilib \
    --prefix=$PREFIX \
    --enable-languages="c,c++" --disable-nls \
    >>$LOG 2>>$ERR
echo "Making guest GCC"
make -j 8 all-gcc >>$LOG 2>>$ERR
echo "Installing guest GCC"
make install-gcc >>$LOG 2>>$ERR
