#!/bin/bash
#
# Enhanced Windows Build Script for Cascoin with Qt6
# Cross-compilation support for modern Windows targets
#

set -e

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"

# Default configuration
JOBS=${JOBS:-$(nproc)}
HOST=${HOST:-x86_64-w64-mingw32}
DEBUG=${DEBUG:-}
CLEAN=${CLEAN:-}

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check system requirements
check_requirements() {
    log_info "Checking build requirements..."
    
    local missing_tools=()
    
    # Check for required tools
    for tool in make autoconf automake libtool pkg-config curl python3; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_tools+=("$tool")
        fi
    done
    
    # Check for cross-compiler
    if ! command -v "${HOST}-gcc" >/dev/null 2>&1; then
        missing_tools+=("${HOST}-gcc")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        log_error "Missing required tools:"
        for tool in "${missing_tools[@]}"; do
            echo "  - $tool"
        done
        echo
        echo "Install missing tools with:"
        echo "  sudo apt-get install build-essential libtool autotools-dev automake pkg-config curl python3"
        echo "  sudo apt-get install g++-mingw-w64-x86-64 mingw-w64-tools"
        exit 1
    fi
    
    log_success "All requirements satisfied"
}

# Build dependencies
build_dependencies() {
    log_info "Building dependencies..."
    
    cd "$PROJECT_ROOT/depends"
    
    # Use the enhanced build script if available
    if [ -f "build-windows.sh" ]; then
        local args=""
        [ "$DEBUG" = "1" ] && args="$args --debug"
        [ "$CLEAN" = "1" ] && args="$args --clean"
        
        ./build-windows.sh -j "$JOBS" -t "$HOST" $args
    else
        # Fallback to make
        [ "$CLEAN" = "1" ] && make clean
        make HOST="$HOST" -j"$JOBS"
    fi
    
    log_success "Dependencies built successfully"
}

# Configure the project
configure_project() {
    log_info "Configuring Cascoin for Windows..."
    
    cd "$PROJECT_ROOT"
    
    # Generate configure script if needed
    if [ ! -f "configure" ]; then
        log_info "Generating configure script..."
        ./autogen.sh
    fi
    
    # Set up configuration
    local config_site="$PROJECT_ROOT/depends/$HOST/share/config.site"
    local configure_args=""
    
    # Add debug flags if requested
    if [ "$DEBUG" = "1" ]; then
        configure_args="$configure_args --enable-debug"
    fi
    
    # Configure with dependencies
    CONFIG_SITE="$config_site" ./configure \
        --prefix=/ \
        --enable-reduce-exports \
        --disable-bench \
        --disable-gui-tests \
        $configure_args
    
    log_success "Configuration completed"
}

# Build the project
build_project() {
    log_info "Building Cascoin..."
    
    cd "$PROJECT_ROOT"
    
    make -j"$JOBS"
    
    log_success "Build completed successfully"
}

# Verify build output
verify_build() {
    log_info "Verifying build output..."
    
    local binaries=("cascoind" "cascoin-cli" "cascoin-tx")
    local missing_binaries=()
    
    for binary in "${binaries[@]}"; do
        if [ ! -f "src/${binary}.exe" ]; then
            missing_binaries+=("${binary}.exe")
        fi
    done
    
    if [ ${#missing_binaries[@]} -ne 0 ]; then
        log_error "Missing binaries:"
        for binary in "${missing_binaries[@]}"; do
            echo "  - $binary"
        done
        exit 1
    fi
    
    # Check if Qt GUI was built
    if [ -f "src/qt/cascoin-qt.exe" ]; then
        log_success "Qt GUI binary built: src/qt/cascoin-qt.exe"
    else
        log_warning "Qt GUI binary not found (this is normal if building without GUI)"
    fi
    
    log_success "All expected binaries found"
    
    # Display binary information
    log_info "Built binaries:"
    for binary in src/*.exe src/qt/*.exe; do
        if [ -f "$binary" ]; then
            local size=$(du -h "$binary" | cut -f1)
            echo "  $(basename "$binary") ($size)"
        fi
    done
}

# Package binaries
package_binaries() {
    log_info "Packaging Windows binaries..."
    
    local package_dir="cascoin-windows-$HOST"
    local archive_name="${package_dir}.zip"
    
    # Create package directory
    rm -rf "$package_dir"
    mkdir -p "$package_dir"
    
    # Copy binaries
    cp src/cascoind.exe "$package_dir/"
    cp src/cascoin-cli.exe "$package_dir/"
    cp src/cascoin-tx.exe "$package_dir/"
    
    if [ -f "src/qt/cascoin-qt.exe" ]; then
        cp src/qt/cascoin-qt.exe "$package_dir/"
    fi
    
    # Copy documentation
    cp README.md "$package_dir/" 2>/dev/null || true
    cp COPYING "$package_dir/" 2>/dev/null || true
    
    # Create archive
    if command -v zip >/dev/null 2>&1; then
        zip -r "$archive_name" "$package_dir"
        log_success "Package created: $archive_name"
    else
        log_warning "zip not available, package directory created: $package_dir"
    fi
}

# Print usage
print_usage() {
    cat << EOF
Enhanced Windows Build Script for Cascoin

Usage: $0 [OPTIONS]

OPTIONS:
    -h, --help              Show this help message
    -j, --jobs NUM          Number of parallel jobs (default: $(nproc))
    -t, --target TARGET     Target host (default: x86_64-w64-mingw32)
        --debug             Build debug version
        --clean             Clean before building
        --deps-only         Build dependencies only
        --no-package        Skip packaging step

EXAMPLES:
    $0                      # Standard Windows 64-bit build
    $0 --clean -j 8         # Clean build with 8 parallel jobs
    $0 -t i686-w64-mingw32  # Windows 32-bit build
    $0 --debug              # Debug build
    $0 --deps-only          # Build dependencies only

TARGETS:
    x86_64-w64-mingw32      Windows 64-bit (default)
    i686-w64-mingw32        Windows 32-bit

EOF
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                print_usage
                exit 0
                ;;
            -j|--jobs)
                JOBS="$2"
                shift 2
                ;;
            -t|--target)
                HOST="$2"
                shift 2
                ;;
            --debug)
                DEBUG="1"
                shift
                ;;
            --clean)
                CLEAN="1"
                shift
                ;;
            --deps-only)
                DEPS_ONLY="1"
                shift
                ;;
            --no-package)
                NO_PACKAGE="1"
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done
}

# Main function
main() {
    log_info "Enhanced Cascoin Windows Build System"
    log_info "Target: $HOST"
    log_info "Jobs: $JOBS"
    
    parse_args "$@"
    check_requirements
    build_dependencies
    
    if [ "$DEPS_ONLY" != "1" ]; then
        configure_project
        build_project
        verify_build
        
        if [ "$NO_PACKAGE" != "1" ]; then
            package_binaries
        fi
    fi
    
    log_success "Windows build completed successfully!"
    
    if [ "$DEPS_ONLY" = "1" ]; then
        log_info "Dependencies are ready in: depends/$HOST"
    else
        log_info "Binaries are available in: src/"
        log_info "To run on Windows, copy the binaries and any required DLLs"
    fi
}

# Run main function with all arguments
main "$@"