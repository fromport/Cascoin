#!/bin/bash

set -e

echo "=== Cascoin Core Linux Build Script ==="

# Check if this is a full build
if [ -f ./.fullbuild ]; then
    echo "Running full Linux build with depends system"
    make clean
    ./autogen.sh
    CONFIG_SITE=$PWD/depends/x86_64-pc-linux-gnu/share/config.site ./configure --prefix=/
else
    echo "Running system libraries build"
    ./autogen.sh
    ./configure \
        --with-gui=qt6 \
        --enable-wallet \
        --with-qrencode \
        --enable-zmq \
        --with-incompatible-bdb \
        --prefix=/usr \
        MOC=/usr/lib/qt6/libexec/moc \
        UIC=/usr/lib/qt6/libexec/uic \
        RCC=/usr/lib/qt6/libexec/rcc \
        LRELEASE=/usr/bin/lrelease \
        LUPDATE=/usr/bin/lupdate
fi

# Build with optimal number of cores
CORES=$(nproc)
echo "Building with $CORES cores..."
make -j$CORES

# Strip binaries to reduce size
echo "Stripping binaries..."
strip src/qt/cascoin-qt src/cascoind src/cascoin-cli src/cascoin-tx

echo "=== Build completed successfully ==="
echo "Binaries:"
ls -la src/qt/cascoin-qt src/cascoind src/cascoin-cli src/cascoin-tx
