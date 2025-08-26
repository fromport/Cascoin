#!/bin/bash

set -e

echo "=== Cascoin Core Local Distribution Builder ==="

# Check dependencies
echo "Checking build dependencies..."
MISSING_DEPS=()

check_command() {
    if ! command -v "$1" &> /dev/null; then
        MISSING_DEPS+=("$1")
    fi
}

check_package() {
    if ! dpkg -l "$1" &> /dev/null; then
        MISSING_DEPS+=("$1")
    fi
}

# Check essential tools
check_command "ldd"
check_command "patchelf"
check_command "chrpath"

# Check development packages
check_package "qt6-base-dev"
check_package "libboost-all-dev"
check_package "libssl-dev"

# Check for ccache (optional but recommended)
if ! command -v ccache &> /dev/null; then
    echo "ccache not found - builds will be slower (install with: sudo apt-get install ccache)"
fi

if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
    echo "Missing dependencies: ${MISSING_DEPS[*]}"
    echo "Install with: sudo apt-get install ${MISSING_DEPS[*]}"
    exit 1
fi

# Setup ccache for faster builds
if command -v ccache &> /dev/null; then
    echo "Setting up ccache..."
    export CCACHE_DIR=$HOME/.ccache
    export PATH="/usr/lib/ccache:$PATH"
    ccache --max-size=2G
    ccache --show-stats
fi

# Build the project
echo "Building Cascoin Core..."
./contrib/ci/build-linux.sh

# Show cache stats if available
if command -v ccache &> /dev/null; then
    echo "Final ccache stats:"
    ccache --show-stats
fi

# Create distribution directory
DIST_DIR="cascoin-linux-distribution"
echo "Creating distribution in $DIST_DIR..."

rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR/usr/bin"
mkdir -p "$DIST_DIR/usr/lib/x86_64-linux-gnu"
mkdir -p "$DIST_DIR/usr/share/applications"
mkdir -p "$DIST_DIR/usr/share/pixmaps"
mkdir -p "$DIST_DIR/usr/lib/qt6/plugins"

# Copy binaries
echo "Copying binaries..."
cp src/qt/cascoin-qt "$DIST_DIR/usr/bin/"
cp src/cascoind "$DIST_DIR/usr/bin/"
cp src/cascoin-cli "$DIST_DIR/usr/bin/"
cp src/cascoin-tx "$DIST_DIR/usr/bin/"

# Copy ONLY the exact libraries that Cascoin needs (no automatic discovery)
echo "Copying ONLY required Cascoin libraries..."
LIB_DIR="$DIST_DIR/usr/lib/x86_64-linux-gnu"

# Copy exact Boost libraries needed by Cascoin
echo "  Collecting required Boost libraries..."
boost_libs=(
  "libboost_system.so*"
  "libboost_filesystem.so*" 
  "libboost_chrono.so*"
  "libboost_thread.so*"
  "libboost_program_options.so*"
)

for boost_lib in "${boost_libs[@]}"; do
  find /usr/lib/x86_64-linux-gnu -name "$boost_lib" 2>/dev/null | while read lib; do
    if [[ -f "$lib" ]]; then
      lib_name=$(basename "$lib")
      if [[ ! -f "$LIB_DIR/$lib_name" ]]; then
        echo "    Copying: $lib_name"
        cp "$lib" "$LIB_DIR/" 2>/dev/null || true
      fi
    fi
  done
done

# Copy Berkeley DB libraries (C++ interface for wallet)
echo "  Collecting Berkeley DB libraries..."
find /usr/lib/x86_64-linux-gnu -name "libdb_cxx-*.so*" -o -name "libdb-*.so*" 2>/dev/null | while read lib; do
  if [[ -f "$lib" ]]; then
    lib_name=$(basename "$lib")
    if [[ ! -f "$LIB_DIR/$lib_name" ]]; then
      echo "    Copying: $lib_name"
      cp "$lib" "$LIB_DIR/" 2>/dev/null || true
    fi
  fi
done

# Copy libcrypto (OpenSSL crypto only, not SSL)
echo "  Collecting libcrypto..."
find /usr/lib/x86_64-linux-gnu -name "libcrypto.so*" 2>/dev/null | while read lib; do
  if [[ -f "$lib" ]]; then
    lib_name=$(basename "$lib")
    if [[ ! -f "$LIB_DIR/$lib_name" ]]; then
      echo "    Copying: $lib_name"
      cp "$lib" "$LIB_DIR/" 2>/dev/null || true
    fi
  fi
done

# Copy exact libevent libraries needed
echo "  Collecting libevent libraries..."
libevent_libs=(
  "libevent-*.so*"
  "libevent_pthreads-*.so*"
)

for event_lib in "${libevent_libs[@]}"; do
  find /usr/lib/x86_64-linux-gnu -name "$event_lib" 2>/dev/null | while read lib; do
    if [[ -f "$lib" ]]; then
      lib_name=$(basename "$lib")
      if [[ ! -f "$LIB_DIR/$lib_name" ]]; then
        echo "    Copying: $lib_name"
        cp "$lib" "$LIB_DIR/" 2>/dev/null || true
      fi
    fi
  done
done

# Copy remaining required libraries
echo "  Collecting other required libraries..."
other_required_libs=(
  "libminiupnpc.so*"
  "libprotobuf.so*"
  "libqrencode.so*"
  "libzmq.so*"
)

for req_lib in "${other_required_libs[@]}"; do
  find /usr/lib/x86_64-linux-gnu -name "$req_lib" 2>/dev/null | while read lib; do
    if [[ -f "$lib" ]]; then
      lib_name=$(basename "$lib")
      if [[ ! -f "$LIB_DIR/$lib_name" ]]; then
        echo "    Copying: $lib_name"
        cp "$lib" "$LIB_DIR/" 2>/dev/null || true
      fi
    fi
  done
done

# Note: Qt6 libraries are NOT copied - they should be installed on target system
echo "Skipping Qt6 libraries - they should be installed on target system via package manager"

# Create wrapper scripts
echo "Creating wrapper scripts..."
cat > "$DIST_DIR/usr/bin/cascoin-qt-wrapper" << 'EOF'
#!/bin/bash
export LD_LIBRARY_PATH="/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH"
exec /usr/bin/cascoin-qt "$@"
EOF

cat > "$DIST_DIR/usr/bin/cascoind-wrapper" << 'EOF'
#!/bin/bash
export LD_LIBRARY_PATH="/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH"
exec /usr/bin/cascoind "$@"
EOF

chmod +x "$DIST_DIR/usr/bin"/*wrapper

# Create desktop entry
cat > "$DIST_DIR/usr/share/applications/cascoin-qt.desktop" << 'EOF'
[Desktop Entry]
Name=Cascoin Core
Comment=Cascoin cryptocurrency wallet
Icon=cascoin
Exec=/usr/bin/cascoin-qt-wrapper
Terminal=false
Type=Application
Categories=Office;Finance;
StartupWMClass=Cascoin-qt
EOF

# Copy icon if available
if [[ -f share/pixmaps/bitcoin128.png ]]; then
    cp share/pixmaps/bitcoin128.png "$DIST_DIR/usr/share/pixmaps/cascoin.png"
fi

# Create installation scripts
cat > "$DIST_DIR/install.sh" << 'EOF'
#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root (use sudo)"
   exit 1
fi

echo "Installing Cascoin Core Linux Distribution..."

# Copy all files to system directories
cp -r usr/* /usr/

# Update library cache
echo "/usr/lib/x86_64-linux-gnu" > /etc/ld.so.conf.d/cascoin.conf
ldconfig

# Update desktop database
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database
fi

echo "Installation complete!"
echo ""
echo "You can now run:"
echo "  cascoin-qt-wrapper    # GUI wallet"
echo "  cascoind-wrapper      # Daemon"
echo "  cascoin-cli           # CLI tool"
echo "  cascoin-tx            # Transaction tool"
EOF

chmod +x "$DIST_DIR/install.sh"

# Create README
cat > "$DIST_DIR/README.md" << 'EOF'
# Cascoin Core Linux Distribution Package

This package contains Cascoin Core binaries with essential libraries.
Qt6 must be installed separately via your distribution's package manager.

## Prerequisites

Install Qt6 before running the GUI:

```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-base-dev-tools

# Fedora/RHEL
sudo dnf install qt6-qtbase-devel

# Arch Linux
sudo pacman -S qt6-base
```

## Installation

```bash
sudo ./install.sh
```

## Usage

After installation:

```bash
cascoin-qt-wrapper    # GUI wallet (requires Qt6)
cascoind-wrapper      # Daemon
cascoin-cli           # CLI tool
cascoin-tx            # Transaction tool
```

## System Requirements

- Linux x86_64
- GLIBC 2.31+ (Ubuntu 20.04+)
- Qt6 6.0+ (for GUI)
- X11 or Wayland display server (for GUI)

          ## Included Libraries (ONLY required ones)
          
          - Boost (system, filesystem, chrono, thread, program_options)
          - Berkeley DB (C++ interface for wallet)
          - libcrypto (OpenSSL crypto functions)
          - libevent + libevent_pthreads (networking)
          - libminiupnpc (UPnP support)
          - libprotobuf (serialization)
          - libqrencode (QR code generation)
          - libzmq (ZeroMQ messaging)
          
          Qt6 libraries are NOT included - install via package manager.
EOF

# Create package
VERSION=$(git describe --tags --always --dirty 2>/dev/null || echo "unknown")
PACKAGE_NAME="cascoin-linux-x86_64-$VERSION"

echo "Creating package: $PACKAGE_NAME.tar.gz"
tar -czf "$PACKAGE_NAME.tar.gz" "$DIST_DIR/"

# Show results
echo ""
echo "=== Distribution Package Created ==="
echo "Package: $PACKAGE_NAME.tar.gz"
echo "Size: $(du -h "$PACKAGE_NAME.tar.gz" | cut -f1)"
echo ""
echo "Contents:"
find "$DIST_DIR" -type f | head -20
echo "... ($(find "$DIST_DIR" -type f | wc -l) total files)"
echo ""
echo "To test installation:"
echo "  tar -xzf $PACKAGE_NAME.tar.gz"
echo "  cd $DIST_DIR"
echo "  sudo ./install.sh"
