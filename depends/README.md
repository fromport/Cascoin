# Enhanced Dependencies Build System

This directory contains a completely rewritten build system for cross-compiling Cascoin dependencies. The system has been modernized to work with current toolchains and dependency versions.

## Features

- **Modern Dependencies**: Updated to current stable versions (Qt 6.8 LTS, OpenSSL 3.4, Boost 1.86, etc.)
- **Enhanced Cross-Compilation**: Improved support for Windows, macOS, and Linux targets
- **Robust Build Process**: Better error handling and verification
- **Parallel Builds**: Optimized for multi-core systems
- **Comprehensive Logging**: Detailed build logs and progress tracking

## Requirements

### Linux (Host System)

```bash
# Essential tools
sudo apt-get update
sudo apt-get install build-essential libtool autotools-dev automake pkg-config bsdmainutils curl git python3 python3-pip

# For Windows cross-compilation
sudo apt-get install g++-mingw-w64-x86-64 mingw-w64-tools

# Configure MinGW alternatives (select 'posix' option)
sudo update-alternatives --config x86_64-w64-mingw32-g++
```

### macOS (Host System)

```bash
# Install Xcode command line tools
xcode-select --install

# Install dependencies via Homebrew
brew install curl python3 autoconf automake libtool pkg-config
```

## Quick Start

### Windows Cross-Compilation

```bash
# Standard Windows 64-bit build
./build-windows.sh

# Windows 32-bit build
./build-windows.sh -t i686-w64-mingw32

# Clean build with custom job count
./build-windows.sh --clean -j 8

# Build without Qt (command-line only)
./build-windows.sh --no-qt
```

### Manual Build Process

```bash
# For Windows 64-bit
make HOST=x86_64-w64-mingw32 -j$(nproc)

# For Windows 32-bit
make HOST=i686-w64-mingw32 -j$(nproc)

# For Linux
make HOST=x86_64-unknown-linux-gnu -j$(nproc)

# For macOS (from Linux)
make HOST=x86_64-apple-darwin11 -j$(nproc)
```

## Build Options

### Environment Variables

- `HOST`: Target platform (e.g., `x86_64-w64-mingw32`)
- `NO_QT`: Set to `1` to build without Qt
- `NO_WALLET`: Set to `1` to build without wallet support
- `NO_UPNP`: Set to `1` to build without UPnP support
- `DEBUG`: Set to `1` for debug builds
- `JOBS`: Number of parallel build jobs (default: `nproc`)

### Supported Targets

- **Windows**: `x86_64-w64-mingw32`, `i686-w64-mingw32`
- **Linux**: `x86_64-unknown-linux-gnu`, `i686-pc-linux-gnu`
- **macOS**: `x86_64-apple-darwin11`
- **ARM**: `arm-linux-gnueabihf`, `aarch64-linux-gnu`

## Package Versions

The build system includes the following updated packages:

- **Qt**: 6.8.1 LTS (with enhanced cross-compilation support)
- **OpenSSL**: 3.4.1 (latest stable)
- **Boost**: 1.86.0 (latest stable)
- **ZeroMQ**: 4.3.6 (latest stable)
- **libevent**: 2.1.12-stable
- **Berkeley DB**: 4.8.30 (for wallet support)

## Cross-Compilation Improvements

### Windows (MinGW-w64)

- Support for modern MinGW-w64 toolchains
- Improved handling of Windows-specific libraries
- Enhanced compatibility with WSL and native Linux
- Better static linking configuration

### Qt 6 Integration

- Updated configuration for Qt 6.8 LTS
- Cross-compilation patches for Windows builds
- Optimized feature selection for cryptocurrency applications
- Enhanced CMake integration

### Build Reliability

- Robust download and extraction handling
- Comprehensive checksum verification
- Better dependency resolution
- Improved caching mechanisms

## Directory Structure

```
depends/
├── build-windows.sh        # Enhanced Windows build script
├── Makefile               # Main build system
├── funcs.mk              # Build functions and logic
├── packages/             # Package definitions
│   ├── packages.mk       # Package list
│   ├── qt.mk            # Qt 6.8 configuration
│   ├── openssl.mk       # OpenSSL configuration
│   ├── boost.mk         # Boost configuration
│   └── ...
├── hosts/                # Host-specific configurations
│   ├── default.mk        # Default host settings
│   ├── mingw32.mk        # Windows MinGW settings
│   └── ...
├── builders/             # Builder configurations
├── patches/              # Source code patches
│   ├── qt/              # Qt-specific patches
│   └── ...
└── README.md            # This file
```

## Usage with Main Project

After building dependencies, configure the main project:

```bash
# Configure with the built dependencies
CONFIG_SITE=$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --prefix=/

# Build the main project
make -j$(nproc)
```

## Troubleshooting

### Common Issues

1. **Missing cross-compiler**: Install MinGW-w64 and configure alternatives
2. **Download failures**: Check network connectivity and proxy settings
3. **Build failures**: Check `build.log` for detailed error messages
4. **WSL path issues**: Ensure project is in WSL filesystem, not Windows mount

### Debug Mode

Enable verbose output and debug information:

```bash
make HOST=x86_64-w64-mingw32 DEBUG=1 V=1
```

### Clean Build

Remove all build artifacts:

```bash
make clean-all
```

## Performance Tips

1. **Use parallel builds**: Set `JOBS` to your CPU core count
2. **Enable caching**: Built packages are cached automatically
3. **Use SSD storage**: Faster I/O improves build times
4. **Adequate RAM**: Ensure at least 4GB available during build

## Contributing

When adding new packages or updating existing ones:

1. Update package definitions in `packages/`
2. Add necessary patches in `patches/`
3. Test cross-compilation for all supported targets
4. Update documentation

## License

This build system is part of the Cascoin project and follows the same license terms.