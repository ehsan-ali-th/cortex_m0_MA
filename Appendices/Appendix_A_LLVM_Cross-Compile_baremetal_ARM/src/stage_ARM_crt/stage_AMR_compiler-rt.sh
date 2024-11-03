#!/bin/bash

homedir=/home/durian
arminstallprefix=$homedir/arm-unknown-eabi
LLVMinstallprefix=$homedir/llvm

cmake ../compiler-rt -G Ninja \
    -DLLVM_CMAKE_DIR=$homedir/backup_data/workspace/llvm-project-18.1.8.src \
    -DLLVM_ENABLE_PIC=ON \
    -DCMAKE_AR=$homedir/arm-unknown-eabi/bin/llvm-ar \
    -DCMAKE_ASM_COMPILER_TARGET="armv6m-unknown-unknown-eabi" \
    -DCMAKE_ASM_FLAGS="--target=armv6m-unknown-unknown-eabi --sysroot=$homedir/backup_data/workspace/sysroot -march=armv6m -mcpu=cortex-m0 -mfpu=none -mfloat-abi=soft" \
    -DCMAKE_C_COMPILER=$homedir/arm-unknown-eabi/bin/clang \
    -DCMAKE_C_COMPILER_TARGET="armv6m-unknown-unknown-eabi" \
    -DCMAKE_C_FLAGS="--sysroot=$homedir/backup_data/workspace/sysroot -march=armv6m -mcpu=cortex-m0 -mfpu=none -mfloat-abi=soft -I$homedir/backup_data/workspace/sysroot/aarch64-none-linux-gnu/libc/usr/include"  \
    -DCMAKE_CXX_FLAGS="--sysroot=$homedir/backup_data/workspace/sysroot -march=armv6m -mcpu=cortex-m0 -mfpu=none -mfloat-abi=soft"  \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld" \
    -DCMAKE_NM=$homedir/arm-unknown-eabi/bin/llvm-nm \
    -DCMAKE_RANLIB=$homedir/arm-unknown-eabi/bin/llvm-ranlib \
    -DCOMPILER_RT_BUILD_BUILTINS=ON \
    -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
    -DCOMPILER_RT_BUILD_MEMPROF=OFF \
    -DCOMPILER_RT_BUILD_PROFILE=OFF \
    -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
    -DCOMPILER_RT_BUILD_XRAY=OFF \
    -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
    -DLLVM_CONFIG_PATH=$homedir/arm-unknown-eabi/bin/llvm-config \
    -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
    -DCOMPILER_RT_OS_DIR="baremetal" \
    -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$arminstallprefix \
    -DLLVM_TARGETS_TO_BUILD="ARM"

