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

#
# Build guest binutils
#

cd $BUILD
tar -xvf ../download/binutils.tar.xz
cd binutils-2.34
mkdir build
cd build
../configure \
    --program-prefix=i686-co-linux- \
    --target=i686-linux-gnu \
    --disable-multilib \
    --prefix=$PREFIX --with-sysroot=$PREFIX \
    --disable-nls --disable-shared --enable-static \
    --enable-lto \
    >>$LOG 2>>$ERR
make -j8 >>$LOG 2>>$ERR
make install >>$LOG 2>>$ERR

#
# Build guest GCC
#

cd $BUILD
tar -xvf ../download/gcc.tar.xz
cd gcc-12.1.0
mkdir build
cd build
../configure \
    --program-prefix=i686-co-linux- \
    --target=i686-linux-gnu \
    --disable-shared --enable-static \
    --disable-multilib \
    --prefix=$PREFIX \
    --enable-languages="c,c++" --disable-nls \
    >>$LOG 2>>$ERR
make -j 8 all-gcc >>$LOG 2>>$ERR
make install-gcc >>$LOG 2>>$ERR
