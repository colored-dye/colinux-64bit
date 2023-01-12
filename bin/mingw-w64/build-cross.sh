. ./build-common.sh

./mingw-w64/mingw-w64-build.sh \
    i686 \
    --gcc-branch gcc-$GCC_VERSION \
    --mingw-w64-branch v$MINGW_VERSION \
    --linked-runtime msvcrt \
    -r $PREFIX \
    --disable-threads \
    -j $(nproc) \
    --keep-artifacts \
    --cached-sources
