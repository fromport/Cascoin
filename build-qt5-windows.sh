#!/bin/bash
# Build script for Windows cross-compilation with Qt 5

set -e

echo "============================================"
echo "Cascoin Windows Build with Qt 5 GUI Support"
echo "============================================"
echo ""
echo "This script uses Qt 5 instead of Qt 6 for better"
echo "cross-compilation stability."
echo ""

cd /home/alexander/Cascoin/depends

# Clean any Qt 6 remnants
echo "Cleaning old Qt builds..."
rm -rf work/build/x86_64-w64-mingw32/qt
rm -rf work/staging/x86_64-w64-mingw32/qt
rm -rf work/download/qt-6*
rm -rf sources/qt*-6*
rm -rf x86_64-w64-mingw32

echo "Building Windows dependencies with Qt 5..."
make HOST=x86_64-w64-mingw32 -j$(nproc) 2>&1 | tee build-qt5.log

if [ $? -eq 0 ]; then
    echo ""
    echo "============================================"
    echo "Dependencies built successfully!"
    echo "============================================"
    echo ""
    echo "Now build Cascoin:"
    echo ""
    echo "cd /home/alexander/Cascoin"
    echo "./autogen.sh"
    echo "CONFIG_SITE=\$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --prefix=/"
    echo "make -j$(nproc)"
    echo ""
    echo "This will create:"
    echo "- cascoind.exe (daemon)"
    echo "- cascoin-cli.exe (command-line interface)"
    echo "- cascoin-tx.exe (transaction utility)"
    echo "- qt/cascoin-qt.exe (GUI wallet with Qt 5)"
else
    echo ""
    echo "Build failed! Check build-qt5.log for details."
    echo ""
    echo "Common issues:"
    echo "1. Missing dependencies - install:"
    echo "   sudo apt install g++-mingw-w64-x86-64 python3"
    echo ""
    echo "2. If Qt fails, you can build without GUI:"
    echo "   make HOST=x86_64-w64-mingw32 NO_QT=1"
    exit 1
fi
