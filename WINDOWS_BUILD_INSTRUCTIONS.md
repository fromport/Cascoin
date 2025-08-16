# Enhanced Windows Build Instructions for Cascoin

This document provides comprehensive instructions for cross-compiling Cascoin for Windows using the enhanced dependency build system.

## Table of Contents

- [Quick Start](#quick-start)
- [Prerequisites](#prerequisites)
- [Build Methods](#build-methods)
- [Configuration Options](#configuration-options)
- [Troubleshooting](#troubleshooting)
- [Advanced Usage](#advanced-usage)

## Quick Start

For experienced developers, here's the fastest way to build Cascoin for Windows:

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install build-essential libtool autotools-dev automake pkg-config curl python3 g++-mingw-w64-x86-64 mingw-w64-tools

# Configure MinGW alternatives
sudo update-alternatives --config x86_64-w64-mingw32-g++  # Select 'posix'

# Build (choose one method)
./clean-and-build.sh -p windows    # Comprehensive build
./build-qt6-windows.sh              # Qt6-focused build
cd depends && ./build-windows.sh    # Dependencies only
```

## Prerequisites

### System Requirements

- **Host OS**: Linux (Ubuntu 18.04+ recommended) or WSL2
- **RAM**: 4GB minimum, 8GB recommended
- **Storage**: 10GB free space for build artifacts
- **CPU**: Multi-core recommended for parallel builds

### Required Packages

#### Ubuntu/Debian

```bash
# Essential build tools
sudo apt-get install build-essential libtool autotools-dev automake pkg-config bsdmainutils curl git python3 python3-pip

# MinGW-w64 cross-compiler
sudo apt-get install g++-mingw-w64-x86-64 g++-mingw-w64-i686 mingw-w64-tools

# Optional tools
sudo apt-get install zip ccache
```

#### Fedora/CentOS

```bash
# Essential build tools
sudo dnf install gcc-c++ libtool autoconf automake pkgconfig curl git python3

# MinGW-w64 cross-compiler
sudo dnf install mingw64-gcc-c++ mingw32-gcc-c++
```

### Configure MinGW-w64

**Important**: Configure MinGW-w64 to use POSIX threads:

```bash
# For 64-bit targets
sudo update-alternatives --config x86_64-w64-mingw32-g++
# Select option with 'posix' in the name

# For 32-bit targets (if needed)
sudo update-alternatives --config i686-w64-mingw32-g++
# Select option with 'posix' in the name
```

## Build Methods

### Method 1: Comprehensive Build Script (Recommended)

The `clean-and-build.sh` script provides the most comprehensive build experience:

```bash
# Standard Windows 64-bit build
./clean-and-build.sh -p windows

# Windows 32-bit build
./clean-and-build.sh -p win32

# Debug build with GUI disabled
./clean-and-build.sh -p windows -t debug --no-gui

# Clean everything and rebuild
./clean-and-build.sh -p windows --clean-all

# Build with custom job count
./clean-and-build.sh -p windows -j 4
```

### Method 2: Qt6-Focused Build Script

The `build-qt6-windows.sh` script is optimized for Qt6 GUI builds:

```bash
# Standard Qt6 Windows build
./build-qt6-windows.sh

# 32-bit build
./build-qt6-windows.sh -t i686-w64-mingw32

# Debug build
./build-qt6-windows.sh --debug

# Dependencies only
./build-qt6-windows.sh --deps-only
```

### Method 3: Manual Build Process

For maximum control over the build process:

```bash
# 1. Build dependencies
cd depends
make HOST=x86_64-w64-mingw32 -j$(nproc)
cd ..

# 2. Generate configure script
./autogen.sh

# 3. Configure project
CONFIG_SITE=$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --prefix=/

# 4. Build
make -j$(nproc)
```

### Method 4: Dependencies-Only Build

If you only need to build dependencies:

```bash
cd depends
./build-windows.sh                    # Enhanced script
# OR
make HOST=x86_64-w64-mingw32 -j$(nproc)  # Direct make
```

## Configuration Options

### Build Targets

| Target | Description |
|--------|-------------|
| `x86_64-w64-mingw32` | Windows 64-bit (default) |
| `i686-w64-mingw32` | Windows 32-bit |

### Build Types

| Type | Description | Flags |
|------|-------------|-------|
| `release` | Optimized build (default) | `-O2 -DNDEBUG` |
| `debug` | Debug build | `-O1 -g` |

### Feature Options

| Option | Description | Default |
|--------|-------------|---------|
| GUI | Build Qt GUI (cascoin-qt) | Enabled |
| Wallet | Build wallet functionality | Enabled |
| UPnP | Build UPnP support | Enabled |

### Environment Variables

```bash
# Build configuration
export HOST=x86_64-w64-mingw32    # Target platform
export JOBS=8                     # Parallel jobs
export DEBUG=1                    # Debug build
export NO_QT=1                    # Disable Qt GUI
export NO_WALLET=1               # Disable wallet
export NO_UPNP=1                 # Disable UPnP

# Performance tuning
export CCACHE_DIR=/path/to/ccache # Enable ccache
export MAKEFLAGS=-j8              # Default make flags
```

## Build Output

### Expected Binaries

After a successful build, you'll find these executables in the `src/` directory:

| Binary | Description |
|--------|-------------|
| `cascoind.exe` | Core daemon |
| `cascoin-cli.exe` | Command-line interface |
| `cascoin-tx.exe` | Transaction utility |
| `cascoin-qt.exe` | GUI application (if built) |

### Deployment

To run on Windows, you may need additional DLLs. The build system attempts to link statically, but some dependencies might require runtime libraries.

## Troubleshooting

### Common Issues

#### 1. MinGW-w64 Not Found

```
Error: x86_64-w64-mingw32-gcc: command not found
```

**Solution**:
```bash
sudo apt-get install g++-mingw-w64-x86-64
```

#### 2. POSIX Threading Issues

```
Error: This application has requested the Runtime to terminate it in an unusual way.
```

**Solution**: Configure MinGW-w64 alternatives:
```bash
sudo update-alternatives --config x86_64-w64-mingw32-g++
# Select the 'posix' option
```

#### 3. Download Failures

```
Error: Failed to download package
```

**Solutions**:
- Check internet connectivity
- Configure proxy if needed: `export http_proxy=http://proxy:port`
- Use alternative download sources

#### 4. WSL Path Issues

```
Error: /mnt/c/path/to/project: Invalid cross-device link
```

**Solution**: Move project to WSL filesystem:
```bash
# Copy project to WSL filesystem
cp -r /mnt/c/cascoin ~/cascoin
cd ~/cascoin
```

#### 5. Insufficient Memory

```
Error: virtual memory exhausted
```

**Solutions**:
- Reduce parallel jobs: `-j2` instead of `-j$(nproc)`
- Add swap space
- Close other applications

### Debug Mode

Enable verbose output for debugging:

```bash
# Verbose make output
make HOST=x86_64-w64-mingw32 V=1

# Debug dependency builds
cd depends
make HOST=x86_64-w64-mingw32 DEBUG=1 V=1
```

### Log Files

Build scripts create log files for debugging:

```bash
# Dependency build log
depends/build.log

# Configure output
config.log

# Make output (if redirected)
build.log
```

## Advanced Usage

### Custom Toolchain

To use a custom MinGW-w64 toolchain:

```bash
export PATH=/path/to/custom/mingw/bin:$PATH
export HOST=x86_64-w64-mingw32
export CC=x86_64-w64-mingw32-gcc
export CXX=x86_64-w64-mingw32-g++
```

### Cross-Compilation with Docker

Build in a containerized environment:

```bash
# Create Dockerfile
cat > Dockerfile << 'EOF'
FROM ubuntu:20.04
RUN apt-get update && apt-get install -y \
    build-essential libtool autotools-dev automake pkg-config \
    curl python3 g++-mingw-w64-x86-64 mingw-w64-tools
WORKDIR /src
EOF

# Build container
docker build -t cascoin-windows-builder .

# Run build
docker run -v $(pwd):/src cascoin-windows-builder ./clean-and-build.sh -p windows
```

### Performance Optimization

#### Using ccache

```bash
# Install ccache
sudo apt-get install ccache

# Configure
export CCACHE_DIR=$HOME/.ccache
export PATH=/usr/lib/ccache:$PATH

# Build with cache
./clean-and-build.sh -p windows
```

#### Parallel Dependencies

Build dependencies in parallel:

```bash
cd depends
make -j$(nproc) HOST=x86_64-w64-mingw32 download  # Download all first
make -j$(nproc) HOST=x86_64-w64-mingw32           # Then build
```

### Custom Package Versions

Edit `depends/packages/*.mk` files to use custom versions:

```makefile
# In depends/packages/boost.mk
$(package)_version=1_85_0
$(package)_download_path=https://archives.boost.io/release/1.85.0/source/
$(package)_sha256_hash=new_hash_here
```

## Windows-Specific Notes

### Antivirus Software

Some antivirus software may flag the executables as suspicious. This is common with cryptocurrency software. Consider:

- Adding build directory to antivirus exclusions
- Building with debug symbols for better analysis
- Using official release signatures when available

### Runtime Dependencies

The build system attempts to create statically-linked binaries, but you may need:

- Visual C++ Redistributable (for some system libraries)
- Windows 7 SP1 or later (due to API requirements)

### GUI Dependencies

Qt6 applications may require:

- Windows 10 version 1809 or later (for best compatibility)
- Graphics drivers supporting OpenGL or Direct3D

## Support and Contributing

### Getting Help

1. Check this documentation
2. Review GitHub issues
3. Join community forums
4. Create detailed bug reports

### Contributing

When contributing build system improvements:

1. Test on multiple platforms
2. Update documentation
3. Maintain backward compatibility
4. Follow coding standards

### Reporting Issues

Include in bug reports:

- Host OS and version
- Target platform
- Build command used
- Complete error output
- System specifications

## References

- [MinGW-w64 Project](https://mingw-w64.org/)
- [Qt6 Cross-Compilation](https://doc.qt.io/qt-6/cross-compiling.html)
- [Bitcoin Core Dependencies](https://github.com/bitcoin/bitcoin/tree/master/depends)
- [WSL Documentation](https://docs.microsoft.com/en-us/windows/wsl/)

---

For the most up-to-date information, check the project repository and release notes.