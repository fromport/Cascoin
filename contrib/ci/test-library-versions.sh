#!/bin/bash

set -e

echo "=== Cascoin Library Version Test ==="

# Test welche spezifischen versionierten Bibliotheken auf dem System verfügbar sind

echo ""
echo "1. Boost Libraries (spezifische Module):"
echo "----------------------------------------"
find /usr/lib/x86_64-linux-gnu -name "libboost_system.so*" -o \
                                -name "libboost_filesystem.so*" -o \
                                -name "libboost_chrono.so*" -o \
                                -name "libboost_thread.so*" -o \
                                -name "libboost_program_options.so*" \
                                2>/dev/null | sort | while read lib; do
    if [[ -f "$lib" ]]; then
        echo "  $(basename "$lib") -> $(readlink -f "$lib")"
    fi
done

echo ""
echo "2. Berkeley DB Libraries:"
echo "------------------------"
find /usr/lib/x86_64-linux-gnu -name "libdb-*.so*" -o \
                                -name "libdb++-*.so*" -o \
                                -name "libdb.so*" -o \
                                -name "libdb++.so*" \
                                2>/dev/null | sort | while read lib; do
    if [[ -f "$lib" ]]; then
        echo "  $(basename "$lib") -> $(readlink -f "$lib")"
    fi
done

echo ""
echo "3. OpenSSL Libraries:"
echo "--------------------"
find /usr/lib/x86_64-linux-gnu -name "libssl.so.*" -o \
                                -name "libcrypto.so.*" \
                                2>/dev/null | sort | while read lib; do
    if [[ -f "$lib" ]]; then
        echo "  $(basename "$lib") -> $(readlink -f "$lib")"
    fi
done

echo ""
echo "4. Qt6 Libraries (Core):"
echo "------------------------"
find /usr/lib/x86_64-linux-gnu -name "libQt6Core.so*" -o \
                                -name "libQt6Widgets.so*" -o \
                                -name "libQt6Gui.so*" -o \
                                -name "libQt6Network.so*" \
                                2>/dev/null | sort | while read lib; do
    if [[ -f "$lib" ]]; then
        echo "  $(basename "$lib") -> $(readlink -f "$lib")"
    fi
done

echo ""
echo "5. Networking/Protocol Libraries:"
echo "---------------------------------"
cascoin_libs=(
    "libevent-2.*.so*" "libevent_core-2.*.so*" "libevent_extra-2.*.so*"
    "libprotobuf.so.*" "libprotobuf-lite.so.*"
    "libminiupnpc.so.*"
    "libzmq.so.*"
    "libqrencode.so.*"
)

for pattern in "${cascoin_libs[@]}"; do
    find /usr/lib/x86_64-linux-gnu -name "$pattern" 2>/dev/null | sort | while read lib; do
        if [[ -f "$lib" ]]; then
            echo "  $(basename "$lib") -> $(readlink -f "$lib")"
        fi
    done
done

echo ""
echo "6. Testing binary dependencies:"
echo "-------------------------------"
if [[ -f src/qt/cascoin-qt ]]; then
    echo "Cascoin-Qt dependencies:"
    ldd src/qt/cascoin-qt | grep -E "(boost|db|ssl|crypto|event|protobuf|miniupnpc|zmq|qrencode|Qt6)" | while read line; do
        echo "  $line"
    done
else
    echo "  cascoin-qt not found (not built yet)"
fi

if [[ -f src/cascoind ]]; then
    echo ""
    echo "Cascoind dependencies:"
    ldd src/cascoind | grep -E "(boost|db|ssl|crypto|event|protobuf|miniupnpc|zmq)" | while read line; do
        echo "  $line"
    done
else
    echo "  cascoind not found (not built yet)"
fi

echo ""
echo "=== Library Version Test Complete ==="
