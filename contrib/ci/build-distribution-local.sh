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

# Function to copy libraries recursively
copy_libs_for_binary() {
    local binary="$1"
    local lib_dir="$2"
    
    echo "Analyzing dependencies for: $(basename "$binary")"
    
    ldd "$binary" 2>/dev/null | grep "=>" | awk '{print $3}' | while read lib; do
        if [[ -f "$lib" && "$lib" =~ ^/usr/lib|^/lib ]]; then
            lib_name=$(basename "$lib")
            if [[ ! -f "$lib_dir/$lib_name" ]]; then
                echo "  Copying: $lib_name"
                cp "$lib" "$lib_dir/" 2>/dev/null || true
            fi
        fi
    done
}

# Copy libraries for each binary
LIB_DIR="$DIST_DIR/usr/lib/x86_64-linux-gnu"
for binary in "$DIST_DIR/usr/bin"/*; do
    copy_libs_for_binary "$binary" "$LIB_DIR"
done

# Copy Qt6 libraries and plugins
echo "Copying Qt6 libraries..."
find /usr/lib/x86_64-linux-gnu -name "libQt6*.so*" -exec cp {} "$LIB_DIR/" \; 2>/dev/null || true

if [[ -d /usr/lib/x86_64-linux-gnu/qt6/plugins ]]; then
    echo "Copying Qt6 plugins..."
    cp -r /usr/lib/x86_64-linux-gnu/qt6/plugins/* "$DIST_DIR/usr/lib/qt6/plugins/" 2>/dev/null || true
fi

# Create wrapper scripts
echo "Creating wrapper scripts..."
cat > "$DIST_DIR/usr/bin/cascoin-qt-wrapper" << 'EOF'
#!/bin/bash
export LD_LIBRARY_PATH="/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="/usr/lib/qt6/plugins:$QT_PLUGIN_PATH"
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
