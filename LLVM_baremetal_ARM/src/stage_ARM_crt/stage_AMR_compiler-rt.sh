#!/bin/bash

homedir=/home/durian
arminstallprefix=$homedir/arm-none-eabi
LLVMinstallprefix=$homedir/llvm

cmake ../compiler-rt -G Ninja \
	-DLLVM_CMAKE_DIR=$homedir/backup_data/workspace/llvm-project-18.1.8.src \
	-DLLVM_ENABLE_PIC=ON \
	-DCMAKE_AR=$homedir/arm-none-eabi/bin/llvm-ar \
	-DCMAKE_ASM_COMPILER_TARGET="arm-none-eabi" \
	-DCMAKE_ASM_FLAGS="--target=arm-none-eabi -march=armv6m -mfloat-abi=soft -mfpu=none" \
	-DCMAKE_C_COMPILER=$homedir/arm-none-eabi/bin/clang \
	-DCMAKE_C_COMPILER_TARGET="arm-none-eabi" \
 	-DCMAKE_C_FLAGS="--target=arm-none-eabi -march=armv6m -mfloat-abi=soft -mfpu=none --sysroot=$homedir/backup_data/workspace/sysroot -I$homedir/backup_data/workspace/sysroot/libc/usr/include" \
	-DCMAKE_CXX_FLAGS="--target=arm-none-eabi -march=armv6m -mfloat-abi=soft -mfpu=none --sysroot=$homedir/backup_data/workspace/sysroot -I$homedir/backup_data/workspace/sysroot/libc/usr/include" \
	-DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld" \
	-DCMAKE_NM=$homedir/arm-none-eabi/bin/llvm-nm \
	-DCMAKE_RANLIB=$homedir/arm-none-eabi/bin/llvm-ranlib \
	-DCOMPILER_RT_BUILD_BUILTINS=ON \
	-DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
	-DCOMPILER_RT_BUILD_MEMPROF=OFF \
	-DCOMPILER_RT_BUILD_PROFILE=OFF \
	-DCOMPILER_RT_BUILD_SANITIZERS=OFF \
	-DCOMPILER_RT_BUILD_XRAY=OFF \
	-DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
	-DLLVM_CONFIG_PATH=$homedir/arm-none-eabi/bin/llvm-config \
	-DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
	-DCOMPILER_RT_OS_DIR="baremetal" \
	-DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$homedir/arm-none-eabi \
	-DLLVM_TARGETS_TO_BUILD="ARM"

