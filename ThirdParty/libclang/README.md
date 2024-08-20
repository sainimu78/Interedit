## github

https://github.com/llvm/llvm-project.git

```
git clone git@github.com:llvm/llvm-project.git
```

白天拉可能容易失败, 继续重试即可

## Branch

```
git checkout llvmorg-17.0.6
```

### SHA-1

6009708b4367171ccdbf4b5905cb6a803753fe18

## Prebuild

[clang+llvm-17.0.6-x86_64-linux-gnu-ubuntu-22.04.tar.xz](https://github.com/llvm/llvm-project/releases/download/llvmorg-17.0.6/clang+llvm-17.0.6-x86_64-linux-gnu-ubuntu-22.04.tar.xz)

The release page is https://releases.llvm.org/

## Build libclang from source on Ubuntu

### GNU Make

```
mkdir build
cd build
cmake -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_LLD=ON -G "Unix Makefiles" ../llvm
make c-index-test -j8
```

The generated `libclang.so` is located in the `build/lib` directory. If you want to move `libclang.so` to another location, you must also copy the linked files, as `libclang.so` depends on them.

```
libclang.so
libclang.so.20.0.0git
libclang.so.20.0git
```

#### Note that

This generates just clang compiler executable only, libclang.so will not be generated

```
make clang -j8
```

### Ninja

已测, 生成的libclang.so很小, 同时依赖很多其它so, 和Windows中生成的不同

```bash
cmake -S llvm -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS="clang"

ninja -C build libclang
```

## Test

```
#include <stdio.h>
#include "clang-c/Index.h"

int main()
{
    auto Idx = clang_createIndex(/* excludeDeclsFromPCH */
        0,
        /* displayDiagnostics=*/1);
    clang_disposeIndex(Idx);
    printf("nihao, %p\n", Idx);
    return 0;
}
```

### g++ command

```
g++ main.cpp -I/media/sainimu78/b7d5a972-427d-49db-abe6-de129d4c9ca7/llvm-project/clang/include -L/media/sainimu78/b7d5a972-427d-49db-abe6-de129d4c9ca7/llvm-project/build/lib -lclang

export LD_LIBRARY_PATH=/media/sainimu78/b7d5a972-427d-49db-abe6-de129d4c9ca7/llvm-project/build/lib:$LD_LIBRARY_PATH
```

When encountering issues with missing dependencies, you can use `ldd` to check. eg.

```
sainimu78@sainimu78-VirtualBox:/media/sainimu78/b7d5a972-427d-49db-abe6-de129d4c9ca7/llvm-project/build/lib$ ldd ./a.out
	linux-vdso.so.1 (0x00007fff17bdf000)
	libclang.so.20.0git => not found
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8ede96f000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f8edeb77000)

```

### cmake script

Put this CMakeLists.txt in folder `build/lib`

```
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(MyMain)

# 设置 C++ 标准
#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 libclang 的头文件
#find_path(LLVM_INCLUDE_DIR clang/Index.h PATHS "/usr/include" "/usr/local/include")
set(LLVM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../clang/include)

# 添加 libclang 的共享库
add_library(libclang2 SHARED IMPORTED)

# 指定 libclang.so 的路径
set_target_properties(libclang2 PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libclang.so")

# 添加可执行文件
add_executable(MyMain main.cpp)

# 添加链接库
target_link_libraries(MyMain libclang2)

# 包含目录
if(LLVM_INCLUDE_DIR)
    target_include_directories(MyMain PRIVATE ${LLVM_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find libclang include directory")
endif()
```

and build

```
mkdir aaaaaa
cd aaaaaa
cmake ..
make
./MyMain
```

## Prepare dependencies mannully

Copy

```
libclang.so
libclang.so.20.0.0git
libclang.so.20.0git
```

to path

```
Interedit/Build/NiflectGenTool/Linux/Linux_NiflectGNiflectGenTool_x64/Debug/NiflectGenTool
```

### Maybe helpful references for issues of building clang

https://discourse.llvm.org/t/unable-to-build-on-ubuntu-or-fedora/1083/2

> ld terminated with signal 9 [Killed]

This is the typical symptoms of the linker being killed by the kernel when it runs out of memory.
You can:

1. Use lld (or gold) as a linker instead of the gnu linker (`-DLLVM_ENABLE_LLD=ON`)
2. build without debug info: this is very memory intensive for the linker
3. reduce the parallelism of the build. If you switch to `ninja` instead of `make` you can pass `-D LLVM_PARALLEL_LINK_JOBS=1` to limit only the number of linker jobs and not constraint the compile jobs.