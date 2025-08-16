# Building Cascoin Core for Windows (Without Depends System)

This guide provides alternative methods for building Cascoin Core on Windows without using the traditional `depends` system.

## Method 1: MSYS2 Native Build

MSYS2 provides a modern package management system for Windows with pre-built libraries.

### Prerequisites

1. Download and install [MSYS2](https://www.msys2.org/)
2. Open MSYS2 UCRT64 terminal

### Install Dependencies

```bash
# Update package database
pacman -Syu

# Install build tools
pacman -S mingw-w64-ucrt-x86_64-toolchain
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-ninja
pacman -S mingw-w64-ucrt-x86_64-pkg-config
pacman -S git

# Install Cascoin dependencies
pacman -S mingw-w64-ucrt-x86_64-qt6-base
pacman -S mingw-w64-ucrt-x86_64-qt6-tools
pacman -S mingw-w64-ucrt-x86_64-qt6-svg
pacman -S mingw-w64-ucrt-x86_64-boost
pacman -S mingw-w64-ucrt-x86_64-openssl
pacman -S mingw-w64-ucrt-x86_64-libevent
pacman -S mingw-w64-ucrt-x86_64-protobuf
pacman -S mingw-w64-ucrt-x86_64-zeromq
pacman -S mingw-w64-ucrt-x86_64-qrencode
pacman -S mingw-w64-ucrt-x86_64-miniupnpc
pacman -S mingw-w64-ucrt-x86_64-db
```

### Build Process

```bash
# Clone and prepare
cd /c/dev  # or your preferred directory
git clone https://github.com/cascoin-project/cascoin.git
cd cascoin

# Generate build system
./autogen.sh

# Configure with system libraries
./configure \
    --prefix=/ucrt64 \
    --with-gui=qt6 \
    --enable-wallet \
    --with-qrencode \
    --enable-zmq \
    --with-miniupnpc \
    CPPFLAGS="-I/ucrt64/include" \
    LDFLAGS="-L/ucrt64/lib"

# Build
make -j$(nproc)
```

## Method 2: vcpkg + Visual Studio

vcpkg is Microsoft's C++ package manager that works well with Visual Studio.

### Prerequisites

1. Install [Visual Studio 2022](https://visualstudio.microsoft.com/) with C++ development tools
2. Install [Git](https://git-scm.com/)

### Setup vcpkg

```cmd
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Integrate with Visual Studio
.\vcpkg integrate install
```

### Install Dependencies

```cmd
# Install required packages
.\vcpkg install boost:x64-windows
.\vcpkg install openssl:x64-windows
.\vcpkg install libevent:x64-windows
.\vcpkg install protobuf:x64-windows
.\vcpkg install zeromq:x64-windows
.\vcpkg install qrencode:x64-windows
.\vcpkg install miniupnpc:x64-windows
.\vcpkg install berkeleydb:x64-windows
.\vcpkg install qt6-base:x64-windows
.\vcpkg install qt6-tools:x64-windows
.\vcpkg install qt6-svg:x64-windows
```

### Build with CMake

Create a `CMakeLists.txt` file in the project root:

```cmake
cmake_minimum_required(VERSION 3.20)
project(CascoinCore VERSION 2.0.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Svg)
find_package(Boost REQUIRED COMPONENTS system filesystem program_options thread chrono)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBEVENT REQUIRED libevent)
pkg_check_modules(ZMQ REQUIRED libzmq)

# Add executable and source files
# (You'll need to specify all source files here)
```

### Build Process

```cmd
# Configure with vcpkg toolchain
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

# Build
cmake --build build --config Release
```

## Method 3: Conan Package Manager

Conan is another modern C++ package manager.

### Prerequisites

1. Install [Python](https://www.python.org/)
2. Install Conan: `pip install conan`

### Setup

```cmd
# Create Conan profile
conan profile detect --force

# Add required remotes
conan remote add conancenter https://center.conan.io
```

### Create conanfile.txt

```ini
[requires]
boost/1.82.0
openssl/3.1.3
qt/6.5.2
protobuf/3.21.12
libevent/2.1.12
zeromq/4.3.4
libqrencode/4.1.1

[generators]
CMakeDeps
CMakeToolchain

[options]
qt/*:shared=True
qt/*:qtsvg=True
qt/*:qtnetwork=True
```

### Build Process

```cmd
# Install dependencies
conan install . --output-folder=build --build=missing -s build_type=Release

# Configure and build
cmake --preset conan-default
cmake --build --preset conan-release
```

## Method 4: Direct System Installation (Advanced)

For experienced users who prefer manual dependency management.

### Prerequisites

1. Install build tools (Visual Studio Build Tools or full Visual Studio)
2. Manually download and install each dependency

### Required Libraries

- **Qt6**: Download from Qt website or use Qt Online Installer
- **Boost**: Download precompiled binaries or build from source
- **OpenSSL**: Use pre-built binaries from OpenSSL website
- **Berkeley DB**: Download and build from Oracle
- **libevent**: Build from source
- **Protocol Buffers**: Download from Google
- **ZeroMQ**: Download from ZeroMQ website
- **QR Encode**: Build from source
- **MiniUPnP**: Build from source

### Configure and Build

```cmd
# Set environment variables for library paths
set QTDIR=C:\Qt\6.5.0\msvc2022_64
set BOOST_ROOT=C:\boost_1_82_0
set OPENSSL_ROOT_DIR=C:\OpenSSL-Win64

# Configure with autotools or CMake
# (Specific commands depend on your installation paths)
```

## Troubleshooting

### Common Issues

1. **Missing Qt tools**: Ensure Qt's bin directory is in your PATH
2. **Library linking errors**: Check that all libraries are compiled with the same MSVC version
3. **Berkeley DB compatibility**: Use version 4.8.x for wallet compatibility

### Environment Variables

```cmd
# Add to system PATH
set PATH=%PATH%;C:\Qt\6.5.0\msvc2022_64\bin
set PATH=%PATH%;C:\vcpkg\installed\x64-windows\bin
```

## Comparison of Methods

| Method | Ease of Use | Maintenance | Performance | Dependencies |
|--------|-------------|-------------|-------------|--------------|
| MSYS2 | High | Medium | Good | Automatic |
| vcpkg | Medium | Low | Excellent | Semi-automatic |
| Conan | Medium | Low | Excellent | Semi-automatic |
| Manual | Low | High | Excellent | Manual |

## Recommendations

- **For beginners**: Use MSYS2 method
- **For Visual Studio users**: Use vcpkg method
- **For modern C++ projects**: Use Conan method
- **For production builds**: Consider manual method with specific versions

## Notes

- These methods bypass the project's depends system entirely
- Some methods may require creating additional build configuration files
- Always verify that all dependencies are compatible versions
- Consider using the same compiler toolchain for all dependencies
