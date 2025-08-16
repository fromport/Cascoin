#!/bin/bash
#
# Build script for Cascoin Windows dependencies
# Run this script in a terminal OUTSIDE of Cursor IDE to avoid environment issues
#

set -e

echo "=========================================="
echo "Cascoin Windows Dependencies Build Script"
echo "=========================================="
echo ""
echo "NOTE: This script should be run in a terminal OUTSIDE of Cursor IDE"
echo "      due to AppImage environment conflicts."
echo ""
echo "Press Enter to continue or Ctrl+C to abort..."
read

# Navigate to depends directory
cd /home/alexander/Cascoin/depends

# Clean previous failed builds
echo "Cleaning previous build attempts..."
rm -rf work/build
rm -rf work/staging
rm -rf x86_64-w64-mingw32

# Check if required tools are installed
echo "Checking build requirements..."
command -v x86_64-w64-mingw32-g++ >/dev/null 2>&1 || { 
    echo "Error: mingw-w64 is not installed. Please run:"
    echo "  sudo apt install g++-mingw-w64-x86-64"
    exit 1
}

command -v python3 >/dev/null 2>&1 || { 
    echo "Error: python3 is not installed. Please run:"
    echo "  sudo apt install python3"
    exit 1
}

# Set mingw to use POSIX threads
echo "Configuring mingw for POSIX threads..."
echo "1" | sudo update-alternatives --config x86_64-w64-mingw32-g++ 2>/dev/null || true
echo "1" | sudo update-alternatives --config x86_64-w64-mingw32-gcc 2>/dev/null || true

# Build dependencies
echo ""
echo "Building Windows 64-bit dependencies..."
echo "This will take approximately 20-40 minutes depending on your system."
echo ""

make HOST=x86_64-w64-mingw32 -j$(nproc) || {
    echo ""
    echo "=========================================="
    echo "Build failed!"
    echo "=========================================="
    echo ""
    echo "Common issues and solutions:"
    echo "1. If you see 'Socket is not connected' errors:"
    echo "   - Make sure you're running this script OUTSIDE of Cursor IDE"
    echo "   - Run in a clean terminal: gnome-terminal, konsole, xterm, etc."
    echo ""
    echo "2. If you see compiler errors:"
    echo "   - Make sure all dependencies are installed:"
    echo "     sudo apt install build-essential libtool autotools-dev automake"
    echo "     sudo apt install pkg-config bsdmainutils curl git"
    echo "     sudo apt install g++-mingw-w64-x86-64"
    echo ""
    echo "3. Check the log files in work/build/ for more details"
    exit 1
}

echo ""
echo "=========================================="
echo "Build completed successfully!"
echo "=========================================="
echo ""
echo "Dependencies have been built to: depends/x86_64-w64-mingw32"
echo ""
echo "Next steps to build Cascoin for Windows:"
echo "1. cd /home/alexander/Cascoin"
echo "2. ./autogen.sh"
echo "3. CONFIG_SITE=\$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --prefix=/"
echo "4. make"
echo ""
echo "The Windows executables will be in src/ after building."

