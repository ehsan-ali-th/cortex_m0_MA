# LLVM-based C/C++ Compiler to Target ARM Cortex-M0 Microprocessor

**Parent Project**: Innovative Hardware Accelerator Architecture for FPGA-Based General-Purpose RISC Microprocessors (Article)

**Author**: Dr. Ehsan Ali  
**Email**: <ehssan.aali@gmail.com>  
**Last update**: 26th October, 2024.  


## Getting LLVM Source Code

1. Download the LLVM project source code (version = 18.1.8):
```console
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-18.1.8/llvm-project-18.1.8.src.tar.xz  
```

```console
tar xvf llvm-project-18.1.8.src.tar.xz  
```

2. Apply patches:

Copy [./src/patches/CMakePolicy.cmake.patch](./src/patches/CMakePolicy.cmake.patch) into lvm-project-18.1.8.src/patches folder, then:

```console
cd llvm-project-18.1.8.src/  
```

```console
mkdir patches 
```

```console
cp ../src/patches/CMakePolicy.cmake.patch ./patches
```

```console
patch -p1 < ./patches/CMakePolicy.cmake.patch  
```

## LLVM Stage-1 Build: Use host native compiler (e.g., GCC) to build LLVM Clang/Clang++ and LLD
**Clang**: A C language family frontend for LLVM.  
**Clang++**: A C++ language family frontend for LLVM.  
**LLD**: A linker from the LLVM project.  
 
1. Create a build folder:  
```console
cd llvm-project-18.1.8.src/  
```

```console
mkdir build_stage1 && cd build_stage1/  
```

2. Create [./src/stage1/stage1.sh](./src/stage1/stage1.sh) script:
```bash
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
```

3. Run the CMAKE configure script:
```console
chmod +x stage1.sh  
```

```console
./stage1.sh  
```
    
4. Build and install:
```console
ninja  
```

```console
ninja install
```

5. Verify that the stage-1 Clang is installed:
```console
/usr/local/bin/clang –version
```

```text
clang version 18.1.8
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /usr/local/bin
```

```console
/usr/local/bin/clang -print-target-triple
```

```console
x86_64-unknown-linux-gnu
```

```console
/usr/local/bin/clang -print-targets
```

```text
 Registered Targets:
    x86    - 32-bit X86: Pentium-Pro and above
    x86-64 - 64-bit X86: EM64T and AMD64
```

**Note:** According to: https://llvm.org/docs/AdvancedBuilds.html the Clang CMake build system supports bootstrap (aka multi-stage) builds. `-DCLANG_ENABLE_BOOTSTRAP=On` does this bootstrap mode with multiple stages. The `-DCLANG_BOOTSTRAP_PASSTHROUGH` allows passing arguments between stages.

Although using the above two arguments are possble, But we are going to perform the multi-stage build manually for the learning purposes. This allows the reader to see the detailes of every step and reduces confusion.

6. Compile a test C++ program:

We can now compile a C++ program on our local host X86_64 machine using the freshly installed stage-1 LLVM Clang++ compiler (installed at local folder /home/esi/llvm/bin):

```console
sudo ldconfig /home/esi/llvm/lib/x86_64-unknown-linux-gnu/
```

```console
/home/esi/llvm/bin/clang++ main.cpp -o main2 -I/home/esi/llvm/include/c++/v1/ -I/home/esi/llvm/include/x86_64-unknown-linux-gnu/c++/v1/ --target=x86_64-unknown-linux-gnu -fuse-ld=lld -stdlib=libc++ -L/home/esi/llvm/lib/x86_64-unknown-linux-gnu/
```
## LLVM Stage-2 Build: Use the stage-1 Clang/Clang++ compilers to build LLVM Clang/Clang++ and LLD

Now we are going to re-compile the LLVM project, this time with the already built Clang/Clang++ in the stage-1:

1. Create a build folder:  
```console
cd llvm-project-18.1.8.src/  
```

```console
mkdir build_stage2 && cd build_stage2/  
```

2. Create [./src/stage2/stage2.sh](./src/stage2/stage2.sh) script:
```bash
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
```

3. Run the CMAKE configure script:
```console
chmod +x stage2.sh  
```

```console
./stage2.sh  
```
    
4. Build and install:

```console
ninja  
```

```console
ninja install
```

Now we have a fully functional LLVM Clang/Clang++ compilers and LLD linker with compiler builtins (Compiler-rt) and C++ standard libraries (libcxx).

## Built LLVM Clang/Clang++ Compilers for ARM Architecture

1. Create a build folder:
```console
cd llvm-project-18.1.8.src
```

```console
mkdir build_ARM && cd build_ARM
```

2. Create [./src/stage_ARM/stage_ARM.sh](./src/stage_ARM/stage_ARM.sh) script:

```bash
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
    -DCMAKE_INSTALL_PREFIX=$arminstallprefix \
    -DLLVM_TARGETS_TO_BUILD="ARM" \
    -DLLVM_TARGET_ARCH="ARM" \
    -DLLVM_DEFAULT_TARGET_TRIPLE="armv6m-unknown-eabi" \
    -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld;lldb" \
    -DCLANG_ENABLE_BOOTSTRAP=OFF \
    -DLLVM_BUILD_DOCS=OFF \
    -DLLVM_ENABLE_DOXYGEN=OFF
```
3. Run the CMAKE configure script:
```console
chmod +x stage_ARM.sh  
```

```console
./stage_ARM.sh  
```
    
4. Build and install:
```console
ninja  
```        

```console
ninja install
```

5. Verify that the ARM Clang is installed:
```console
/home/esi/arm-none-eabi/bin/clang++ -print-targets
```

```text
  Registered Targets:
    arm     - ARM
    armeb   - ARM (big endian)
    thumb   - Thumb
    thumbeb - Thumb (big endian)
```
## Compile LLVM Compiler-RT Project for Baremetal ARM Cortex-M0 (Triple: armv6m-unknown-eabi)


To cross-compile compiler-rt for ARM architectur eon an x86_64 host machine we need the following items:
- An x84_64 build of LLVM/clang for the llvm-tools and llvm-config. We built this in our stage-2 built [See Stage-2](#llvm-stage-2-build-use-the-stage-1-clangclang-compilers-to-build-llvm-clangclang-and-lld).

- A clang executable with support for the ARM target. We built this 
- compiler-rt sources
- The qemu-arm user mode emulator
- An arm-linux-gnueabihf sysroot

The LLVM compiler-rt project consists of:
- **Builtins**: a simple library that provides an implementation of the low-level target-specific hooks required by code generation and other runtime components.
- **Sanitizer runtimes**: runtime libraries that are required to run the code with sanitizer instrumentation.
- **Profile**: library which is used to collect coverage information.
- **BlocksRuntime**: a target-independent implementation of Apple "Blocks" runtime interfaces.

We need to compile the compiler-rt project for ARM Cortext-M0 Microprocessor. Before that one more concept should be explained: *Triple*.

### LLVM Target Triple

It is a dash seperated string (text) that specifies the architectrue which the generated machine code is compiled for. The word *triple* because it originally had exactly three parts (today it can have between two and five parts). The triple format is shown below:

\<arch>\<sub_arch>-\<vendor>-\<sys>-\<env>

Therefore, if we would like to form the target triple text for ARM Cortex-M0 microprocessor we should have:
- The ARM architecture (`arm`)
- Cortex-M0 processor is ARMSubArch_v6m so sub-architecture is (`v6m`)
- From any vendor (`unknown`)
- Environment type is embedded application binary interface (`eabi`)

Combining all the above and put them together yields the correct target triple: `armv6m-unknown-eabi`

**Note**: For the Cortex-M0 core, instruction memory and private peripheral bus (PPB) accesses are always little-endian. For the data memory, it is dependent on the implementation. So if we face an option that allows us to set the endianness we will opt for little-endian.

1. Create a build folder:  
```console
cd llvm-project-18.1.8.src/  
```

```console
mkdir build_compiler_rt && cd build_compiler_rt  
```

2. Create [./src/stage_ARM_crt/stage_AMR_compiler-rt.sh](./src/stage_ARM_crt/stage_AMR_compiler-rt.sh) script:
```bash
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
```
 

