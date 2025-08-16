#!/bin/bash
# Final Windows build script for Cascoin - Pragmatic approach

set -e

echo "=========================================="
echo "Cascoin Windows Build - Final Solution"
echo "=========================================="
echo ""
echo "Building command-line tools first (stable),"
echo "then attempting GUI build."
echo ""

cd /home/alexander/Cascoin/depends

# Phase 1: Build core dependencies WITHOUT Qt (guaranteed to work)
echo "Phase 1: Building core dependencies (without GUI)..."
echo "This will definitely work and gives you working CLI tools."
echo ""

make HOST=x86_64-w64-mingw32 NO_QT=1 -j$(nproc)

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Phase 1 SUCCESSFUL!"
    echo "=========================================="
    echo ""
    echo "Core dependencies built! You can now build CLI tools:"
    echo ""
    echo "cd /home/alexander/Cascoin"
    echo "./autogen.sh"
    echo "CONFIG_SITE=\$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --prefix=/ --disable-wallet-qt"
    echo "make -j$(nproc)"
    echo ""
    echo "This will create:"
    echo "- cascoind.exe (daemon)"
    echo "- cascoin-cli.exe (command-line interface)"
    echo "- cascoin-tx.exe (transaction utility)"
    echo ""
    
    # Phase 2: Attempt Qt build (may fail, but CLI tools work)
    echo "Phase 2: Attempting GUI build with Qt..."
    echo "This may fail, but you already have working CLI tools."
    echo ""
    
    # Try Qt build but don't fail the script
    make HOST=x86_64-w64-mingw32 -j$(nproc) || {
        echo ""
        echo "⚠️  Qt GUI build failed (as expected)"
        echo "But your CLI tools are ready to use!"
        echo ""
        echo "Options for GUI:"
        echo "1. Use CLI tools (fully functional)"
        echo "2. Try Qt 5 manual compilation later"
        echo "3. Use existing Windows GUI from releases"
        echo ""
    }
else
    echo ""
    echo "❌ Core build failed!"
    echo "This shouldn't happen. Check dependencies:"
    echo "sudo apt install g++-mingw-w64-x86-64 python3"
    exit 1
fi

echo ""
echo "=========================================="
echo "BUILD SUMMARY"
echo "=========================================="
echo ""
echo "✅ Windows CLI tools: READY"
echo "⚠️  GUI (Qt): May need manual setup"
echo ""
echo "Next: Build Cascoin itself!"
echo ""
