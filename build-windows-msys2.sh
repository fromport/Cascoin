#!/bin/bash
# Build script for Cascoin Core using MSYS2 on Windows
# Run this script in MSYS2 UCRT64 terminal

set -e

echo "Building Cascoin Core with MSYS2..."
echo

# Check if we're in MSYS2
if [[ ! "$MSYSTEM" =~ ^(UCRT64|MINGW64)$ ]]; then
    echo "Error: This script must be run in MSYS2 UCRT64 or MINGW64 terminal"
    echo "Please open MSYS2 UCRT64 and run this script again"
    exit 1
fi

echo "Detected MSYS2 environment: $MSYSTEM"
echo

# Update package database
echo "Updating package database..."
pacman -Sy --noconfirm

# Install build tools if not already installed
echo "Installing build tools..."
pacman -S --needed --noconfirm \
    mingw-w64-ucrt-x86_64-toolchain \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-ninja \
    mingw-w64-ucrt-x86_64-pkg-config \
    autoconf \
    automake \
    libtool \
    make \
    git

# Install Cascoin dependencies
echo "Installing Cascoin dependencies..."
pacman -S --needed --noconfirm \
    mingw-w64-ucrt-x86_64-qt6-base \
    mingw-w64-ucrt-x86_64-qt6-tools \
    mingw-w64-ucrt-x86_64-qt6-svg \
    mingw-w64-ucrt-x86_64-boost \
    mingw-w64-ucrt-x86_64-openssl \
    mingw-w64-ucrt-x86_64-libevent \
    mingw-w64-ucrt-x86_64-protobuf \
    mingw-w64-ucrt-x86_64-zeromq \
    mingw-w64-ucrt-x86_64-qrencode \
    mingw-w64-ucrt-x86_64-miniupnpc \
    mingw-w64-ucrt-x86_64-db

echo
echo "Dependencies installed successfully!"
echo

# Check if CMakeLists.txt exists
if [[ ! -f "CMakeLists.txt" ]]; then
    echo "Error: CMakeLists.txt not found in current directory"
    echo "Please run this script from the Cascoin source root directory"
    exit 1
fi

# Create build directory
BUILD_DIR="build-msys2"
if [[ -d "$BUILD_DIR" ]]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring with CMake..."
echo

# Configure with CMake
cmake .. \
    -G "Ninja" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=/ucrt64 \
    -DENABLE_WALLET=ON \
    -DENABLE_QT=ON \
    -DENABLE_ZMQ=ON \
    -DENABLE_UPNP=ON \
    -DENABLE_TESTS=OFF \
    -DENABLE_BENCH=OFF \
    -DCMAKE_INSTALL_PREFIX=/ucrt64

echo
echo "Building Cascoin Core..."
echo

# Build the project
cmake --build . --parallel $(nproc)

echo
echo "Build completed successfully!"
echo

# Show where executables are located
echo "Executables can be found in:"
echo "  $(pwd)/"
echo
echo "Available executables:"
if [[ -f "cascoind.exe" ]]; then
    echo "  - cascoind.exe       (Daemon)"
fi
if [[ -f "cascoin-cli.exe" ]]; then
    echo "  - cascoin-cli.exe    (Command line interface)"
fi
if [[ -f "cascoin-tx.exe" ]]; then
    echo "  - cascoin-tx.exe     (Transaction utility)"
fi
if [[ -f "cascoin-qt.exe" ]]; then
    echo "  - cascoin-qt.exe     (GUI wallet)"
fi
echo

# Optional: Install to system
read -p "Do you want to install to system directories? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Installing to system..."
    cmake --install .
    echo "Installation completed!"
fi

echo
echo "Build process finished!"

# Show testing instructions
echo
echo "To test the build:"
echo "  ./cascoind.exe --version"
echo "  ./cascoin-cli.exe --version"
if [[ -f "cascoin-qt.exe" ]]; then
    echo "  ./cascoin-qt.exe --version"
fi
echo
