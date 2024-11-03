#!/bin/bash

    homedir=/home/durian
    arminstallprefix=$homedir/arm-none-eabi
    LLVMinstallprefix=$homedir/llvm

    cmake -G Ninja ../llvm \
        -DLLVM_INSTALL_UTILS=ON \
        -DLLVM_ENABLE_PIC=ON \
        -DLLVM_ENABLE_BINDINGS=OFF \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$LLVMinstallprefix \
        -DLLVM_TARGETS_TO_BUILD="Native" \
        -DLLVM_ENABLE_PROJECTS="clang;lld" \
        -DLLVM_ENABLE_RUNTIMES="compiler-rt;libcxx;libcxxabi;libunwind" \
        -DLLVM_BUILD_DOCS=OFF \
        -DLLVM_ENABLE_DOXYGEN=OFF
