#!/bin/bash

homedir=/home/durian
arminstallprefix=$homedir/arm-none-eabi
LLVMinstallprefix=$homedir/llvm

LD_FOR_TARGET=$LLVMinstallprefix/bin/ld.lld  cmake -G Ninja ../llvm \
	-DLLVM_INSTALL_UTILS=ON \
	-DLLVM_ENABLE_BINDINGS=OFF \
	-DLLVM_ENABLE_PIC=ON \
	-DLLVM_USE_LINKER="lld" \
	-DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
	-DCMAKE_C_COMPILER=$LLVMinstallprefix/bin/clang \
	-DCMAKE_CXX_COMPILER=$LLVMinstallprefix/bin/clang++ \
	-DCMAKE_C_FLAGS="-I$homedir/esi/llvm/include/c++/v1/ -I$homedir/llvm/include/x86_64-unknown-linux-gnu/c++/v1/" \
	-DCMAKE_CXX_FLAGS="-I$homedir/esi/llvm/include/c++/v1/ -I$homedir/llvm/include/x86_64-unknown-linux-gnu/c++/v1/ -stdlib=libc++" \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$LLVMinstallprefix \
	-DLLVM_TARGETS_TO_BUILD="Native" \
	-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld;lldb;compiler-rt" \
	-DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
	-DCLANG_ENABLE_BOOTSTRAP=OFF \
	-DLLVM_BUILD_DOCS=OFF \
	-DLLVM_ENABLE_DOXYGEN=OFF
