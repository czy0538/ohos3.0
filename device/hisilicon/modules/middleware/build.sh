#!/bin/bash
# Copyright 2020-2020, Huawei Technologies Co. Ltd.
#
# ALL RIGHTS RESERVED
#
# Compile mpp/sample project, this is the entrance script

# error out on errors
set -e
OUT_DIR="$1"
BOARD_NAME="$2"
HOS_KERNEL_TYPE="$3"
HOS_BUILD_COMPILER="$4"
STORAGE_TYPE="$5"
CC_PATH="$6"

function main(){
    CUR_DIR=$(cd $(dirname "$0");pwd)
    echo "BOARD_NAME=${BOARD_NAME} HOS_BUILD_COMPILER=${HOS_BUILD_COMPILER} STORAGE_TYPE=${STORAGE_TYPE} HOS_KERNEL_TYPE=${HOS_KERNEL_TYPE} CC_PATH=${CC_PATH}"
    COMPILER_TYPE=gcc
    if [ "$HOS_BUILD_COMPILER" == "clang" ]; then
        COMPILER_TYPE=llvm
    fi
    COMPILER_VER="himix100"
    if [ "$BOARD_NAME" = "hi3516dv300" ];then
        COMPILER_VER="himix200"
    fi
    if [ "$HOS_KERNEL_TYPE" == "liteos_a" ]; then
        if [ "$COMPILER_TYPE" == "llvm" ]; then
            COMPILER_VER="himix410"
        fi
    elif [ "$HOS_KERNEL_TYPE" == "linux" ]; then
        if [ "$STORAGE_TYPE" == "emmc" ]; then
	          COMPILER_VER="himix410"
        fi
    fi

    cd $CUR_DIR/source && ./build.sh $OUT_DIR $BOARD_NAME $HOS_KERNEL_TYPE $COMPILER_TYPE $CC_PATH $COMPILER_VER
}

if [ "x" != "x$7" ]; then
export SYSROOT_PATH=$7
fi
if [ "x" != "x$8" ]; then
export ARCH_CFLAGS="$8"
fi

main "$@"

