. ./build-common.sh

./build-cross.sh $1 && \
    ./build-colinux-libs.sh $1 && \
    ./build-kernel.sh $1 && \
    ./build-colinux.sh $1 && \
    echo "Build-all $1 DONE"
