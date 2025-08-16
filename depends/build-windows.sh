#!/bin/bash
#
# Enhanced Windows Cross-Compilation Build Script
# Supports modern MinGW-w64 toolchains and updated dependencies
#

set -e

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DEPENDS_DIR="$SCRIPT_DIR"

# Default configuration
JOBS=${JOBS:-$(nproc)}
HOST=${HOST:-x86_64-w64-mingw32}
NO_QT=${NO_QT:-}
NO_WALLET=${NO_WALLET:-}
NO_UPNP=${NO_UPNP:-}
DEBUG=${DEBUG:-}

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
    log_info "Checking system requirements..."
    
    local missing_tools=()
    
    # Check for required tools
    for tool in make curl python3 sha256sum; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_tools+=("$tool")
        fi
    done
    
    # Check for cross-compiler
    if ! command -v "${HOST}-gcc" >/dev/null 2>&1; then
        missing_tools+=("${HOST}-gcc (MinGW-w64 cross-compiler)")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        log_error "Missing required tools:"
        for tool in "${missing_tools[@]}"; do
            echo "  - $tool"
        done
        echo
        echo "Install missing tools with:"
        echo "  sudo apt-get update"
        echo "  sudo apt-get install build-essential curl python3 python3-pip"
        echo "  sudo apt-get install g++-mingw-w64-x86-64 mingw-w64-tools"
        echo
        echo "Configure MinGW alternatives:"
        echo "  sudo update-alternatives --config x86_64-w64-mingw32-g++"
        echo "  (Select the 'posix' option)"
        exit 1
    fi
    
    log_success "All requirements satisfied"
}

# Clean previous builds
clean_build() {
    log_info "Cleaning previous builds..."
    cd "$DEPENDS_DIR"
    make clean 2>/dev/null || true
    log_success "Clean completed"
}

# Build dependencies
build_dependencies() {
    log_info "Building dependencies for Windows ($HOST)..."
    log_info "Using $JOBS parallel jobs"
    
    cd "$DEPENDS_DIR"
    
    # Set build environment
    export HOST="$HOST"
    export NO_QT="$NO_QT"
    export NO_WALLET="$NO_WALLET"
    export NO_UPNP="$NO_UPNP"
    export DEBUG="$DEBUG"
    
    # Clean PATH from Windows-specific entries (for WSL compatibility)
    export PATH=$(echo "$PATH" | sed -e 's/:\/mnt.*//g')
    
    # Run the build
    make -j"$JOBS" HOST="$HOST" V=1 2>&1 | tee build.log
    
    if [ ${PIPESTATUS[0]} -eq 0 ]; then
        log_success "Dependencies built successfully"
    else
        log_error "Dependency build failed. Check build.log for details"
        exit 1
    fi
}

# Verify build output
verify_build() {
    log_info "Verifying build output..."
    
    local prefix_dir="$DEPENDS_DIR/$HOST"
    local config_site="$prefix_dir/share/config.site"
    
    if [ ! -f "$config_site" ]; then
        log_error "Build verification failed: config.site not found"
        exit 1
    fi
    
    log_info "Build output directory: $prefix_dir"
    log_info "Config site: $config_site"
    
    # List built libraries
    if [ -d "$prefix_dir/lib" ]; then
        log_info "Built libraries:"
        find "$prefix_dir/lib" -name "*.a" -o -name "*.la" | head -20 | while read lib; do
            echo "  $(basename "$lib")"
        done
    fi
    
    log_success "Build verification completed"
}

# Print usage
print_usage() {
    cat << EOF
Enhanced Windows Cross-Compilation Build Script

Usage: $0 [OPTIONS]

OPTIONS:
    -h, --help              Show this help message
    -j, --jobs NUM          Number of parallel jobs (default: $(nproc))
    -t, --target TARGET     Target host (default: x86_64-w64-mingw32)
        --no-qt             Build without Qt
        --no-wallet         Build without wallet support
        --no-upnp           Build without UPnP support
        --debug             Build debug version
        --clean             Clean before building

EXAMPLES:
    $0                      # Standard Windows 64-bit build
    $0 --clean -j 8         # Clean build with 8 parallel jobs
    $0 -t i686-w64-mingw32  # Windows 32-bit build
    $0 --no-qt --debug      # Debug build without Qt

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
            --no-qt)
                NO_QT="1"
                shift
                ;;
            --no-wallet)
                NO_WALLET="1"
                shift
                ;;
            --no-upnp)
                NO_UPNP="1"
                shift
                ;;
            --debug)
                DEBUG="1"
                shift
                ;;
            --clean)
                CLEAN="1"
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
    log_info "Enhanced Windows Cross-Compilation Build System"
    log_info "Target: $HOST"
    log_info "Jobs: $JOBS"
    
    parse_args "$@"
    check_requirements
    
    if [ "$CLEAN" = "1" ]; then
        clean_build
    fi
    
    build_dependencies
    verify_build
    
    log_success "Windows cross-compilation build completed successfully!"
    log_info "Dependencies are ready in: $DEPENDS_DIR/$HOST"
    log_info "Use CONFIG_SITE=\$PWD/depends/$HOST/share/config.site when configuring the main project"
}

# Run main function with all arguments
main "$@"