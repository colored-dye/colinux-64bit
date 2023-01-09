. ./build-common.sh

./mingw-w64/mingw-w64-build.sh i686 -r $PREFIX --disable-threads -j $(nproc) --keep-artifacts
