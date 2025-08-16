#!/bin/bash
#
# Comprehensive Clean and Build Script for Cascoin
# Supports multiple platforms and build configurations
#

set -e

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"

# Default configuration
JOBS=${JOBS:-$(nproc)}
PLATFORM=${PLATFORM:-native}
BUILD_TYPE=${BUILD_TYPE:-release}
CLEAN_ALL=${CLEAN_ALL:-}
GUI=${GUI:-1}

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
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

log_step() {
    echo -e "${PURPLE}[STEP]${NC} $1"
}

# Print banner
print_banner() {
    echo
    echo "========================================"
    echo "    Cascoin Enhanced Build System      "
    echo "========================================"
    echo
}

# Clean build artifacts
clean_build() {
    log_step "Cleaning build artifacts..."
    
    cd "$PROJECT_ROOT"
    
    # Clean main project
    if [ -f "Makefile" ]; then
        make clean 2>/dev/null || true
        make distclean 2>/dev/null || true
    fi
    
    # Remove generated files
    rm -f configure config.log config.status
    rm -rf autom4te.cache/
    rm -f src/config/cascoin-config.h
    rm -f src/config/cascoin-config.h.in
    rm -f src/config/stamp-h1
    
    # Clean dependencies if requested
    if [ "$CLEAN_ALL" = "1" ]; then
        log_info "Performing deep clean of dependencies..."
        cd depends
        make clean-all 2>/dev/null || true
        cd ..
    fi
    
    log_success "Clean completed"
}

# Detect platform
detect_platform() {
    local detected=""
    
    case "$PLATFORM" in
        native|linux)
            detected="x86_64-unknown-linux-gnu"
            ;;
        windows|win64)
            detected="x86_64-w64-mingw32"
            ;;
        win32)
            detected="i686-w64-mingw32"
            ;;
        macos|osx)
            detected="x86_64-apple-darwin11"
            ;;
        *)
            # Assume it's already a full host triple
            detected="$PLATFORM"
            ;;
    esac
    
    echo "$detected"
}

# Check dependencies
check_dependencies() {
    log_step "Checking build dependencies..."
    
    local missing_tools=()
    local platform_host=$(detect_platform)
    
    # Common tools
    for tool in make autoconf automake libtool pkg-config curl python3; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_tools+=("$tool")
        fi
    done
    
    # Platform-specific tools
    case "$platform_host" in
        *mingw*)
            local compiler="${platform_host%-*}-gcc"
            if ! command -v "$compiler" >/dev/null 2>&1; then
                missing_tools+=("$compiler (MinGW-w64)")
            fi
            ;;
        *apple*)
            if [ ! -d "/Applications/Xcode.app" ] && ! command -v "clang" >/dev/null 2>&1; then
                missing_tools+=("Xcode or clang")
            fi
            ;;
    esac
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        log_error "Missing required dependencies:"
        for tool in "${missing_tools[@]}"; do
            echo "  - $tool"
        done
        echo
        echo "Install missing dependencies:"
        echo "  Ubuntu/Debian: sudo apt-get install build-essential libtool autotools-dev automake pkg-config curl python3"
        echo "  For Windows cross-compilation: sudo apt-get install g++-mingw-w64-x86-64 mingw-w64-tools"
        echo "  macOS: xcode-select --install && brew install autoconf automake libtool pkg-config"
        exit 1
    fi
    
    log_success "All dependencies satisfied"
}

# Build dependencies
build_dependencies() {
    log_step "Building dependencies..."
    
    local platform_host=$(detect_platform)
    
    cd "$PROJECT_ROOT/depends"
    
    # Set build options
    local build_args=""
    [ "$BUILD_TYPE" = "debug" ] && build_args="$build_args DEBUG=1"
    [ "$GUI" = "0" ] && build_args="$build_args NO_QT=1"
    
    # Use platform-specific build script if available
    case "$platform_host" in
        *mingw*)
            if [ -f "build-windows.sh" ]; then
                local script_args="-j $JOBS -t $platform_host"
                [ "$BUILD_TYPE" = "debug" ] && script_args="$script_args --debug"
                [ "$GUI" = "0" ] && script_args="$script_args --no-qt"
                
                ./build-windows.sh $script_args
            else
                make HOST="$platform_host" $build_args -j"$JOBS"
            fi
            ;;
        *)
            make HOST="$platform_host" $build_args -j"$JOBS"
            ;;
    esac
    
    cd "$PROJECT_ROOT"
    log_success "Dependencies built successfully"
}

# Configure project
configure_project() {
    log_step "Configuring project..."
    
    cd "$PROJECT_ROOT"
    
    # Generate configure script
    if [ ! -f "configure" ]; then
        log_info "Generating configure script..."
        ./autogen.sh
    fi
    
    local platform_host=$(detect_platform)
    local config_args=""
    local config_site=""
    
    # Set configuration based on platform
    case "$platform_host" in
        *mingw*)
            config_site="$PROJECT_ROOT/depends/$platform_host/share/config.site"
            config_args="--prefix=/ --enable-reduce-exports --disable-bench"
            ;;
        *apple*)
            config_site="$PROJECT_ROOT/depends/$platform_host/share/config.site"
            config_args="--prefix=/ --enable-reduce-exports"
            ;;
        *)
            # Native Linux build
            if [ -f "$PROJECT_ROOT/depends/$platform_host/share/config.site" ]; then
                config_site="$PROJECT_ROOT/depends/$platform_host/share/config.site"
            fi
            config_args="--prefix=/usr/local"
            ;;
    esac
    
    # Add build type specific flags
    if [ "$BUILD_TYPE" = "debug" ]; then
        config_args="$config_args --enable-debug"
    fi
    
    # Configure GUI options
    if [ "$GUI" = "0" ]; then
        config_args="$config_args --without-gui"
    fi
    
    # Run configure
    if [ -n "$config_site" ]; then
        CONFIG_SITE="$config_site" ./configure $config_args
    else
        ./configure $config_args
    fi
    
    log_success "Configuration completed"
}

# Build project
build_project() {
    log_step "Building Cascoin..."
    
    cd "$PROJECT_ROOT"
    
    make -j"$JOBS"
    
    log_success "Build completed successfully"
}

# Run tests
run_tests() {
    log_step "Running tests..."
    
    cd "$PROJECT_ROOT"
    
    # Run unit tests
    if [ -f "src/test/test_cascoin" ]; then
        log_info "Running unit tests..."
        src/test/test_cascoin
    else
        log_warning "Unit tests not built"
    fi
    
    # Run functional tests for native builds
    local platform_host=$(detect_platform)
    if [[ "$platform_host" != *mingw* ]] && [[ "$platform_host" != *apple* ]]; then
        if [ -d "test/functional" ] && command -v python3 >/dev/null 2>&1; then
            log_info "Running functional tests..."
            cd test/functional
            python3 test_runner.py --combinedlogslen=0 --quiet
            cd "$PROJECT_ROOT"
        fi
    fi
    
    log_success "Tests completed"
}

# Verify build
verify_build() {
    log_step "Verifying build output..."
    
    local platform_host=$(detect_platform)
    local exe_suffix=""
    
    [[ "$platform_host" == *mingw* ]] && exe_suffix=".exe"
    
    local expected_binaries=("cascoind" "cascoin-cli" "cascoin-tx")
    local missing_binaries=()
    
    for binary in "${expected_binaries[@]}"; do
        if [ ! -f "src/${binary}${exe_suffix}" ]; then
            missing_binaries+=("${binary}${exe_suffix}")
        fi
    done
    
    # Check GUI binary if enabled
    if [ "$GUI" = "1" ] && [ ! -f "src/qt/cascoin-qt${exe_suffix}" ]; then
        log_warning "GUI binary not found (cascoin-qt${exe_suffix})"
    fi
    
    if [ ${#missing_binaries[@]} -ne 0 ]; then
        log_error "Missing binaries:"
        for binary in "${missing_binaries[@]}"; do
            echo "  - $binary"
        done
        exit 1
    fi
    
    log_success "All expected binaries built successfully"
    
    # Display build summary
    log_info "Build summary:"
    echo "  Platform: $platform_host"
    echo "  Build type: $BUILD_TYPE"
    echo "  GUI enabled: $([[ $GUI == "1" ]] && echo "Yes" || echo "No")"
    echo "  Binaries:"
    for binary in src/*${exe_suffix} src/qt/*${exe_suffix}; do
        if [ -f "$binary" ]; then
            local size=$(du -h "$binary" | cut -f1)
            echo "    $(basename "$binary") ($size)"
        fi
    done
}

# Print usage
print_usage() {
    cat << EOF
Comprehensive Clean and Build Script for Cascoin

Usage: $0 [OPTIONS]

OPTIONS:
    -h, --help              Show this help message
    -j, --jobs NUM          Number of parallel jobs (default: $(nproc))
    -p, --platform PLATFORM Target platform (default: native)
    -t, --type TYPE         Build type: release or debug (default: release)
        --no-gui            Build without GUI
        --clean-all         Clean dependencies too
        --deps-only         Build dependencies only
        --no-tests          Skip running tests
        --no-verify         Skip build verification

PLATFORMS:
    native, linux           Native Linux build
    windows, win64          Windows 64-bit cross-compile
    win32                   Windows 32-bit cross-compile
    macos, osx              macOS cross-compile
    <host-triple>           Custom host triple

EXAMPLES:
    $0                      # Native Linux build
    $0 -p windows -j 8      # Windows 64-bit with 8 jobs
    $0 -p win32 -t debug    # Windows 32-bit debug build
    $0 --clean-all          # Clean everything and rebuild
    $0 --no-gui             # Build without Qt GUI

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
            -p|--platform)
                PLATFORM="$2"
                shift 2
                ;;
            -t|--type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            --no-gui)
                GUI="0"
                shift
                ;;
            --clean-all)
                CLEAN_ALL="1"
                shift
                ;;
            --deps-only)
                DEPS_ONLY="1"
                shift
                ;;
            --no-tests)
                NO_TESTS="1"
                shift
                ;;
            --no-verify)
                NO_VERIFY="1"
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
    print_banner
    
    parse_args "$@"
    
    local platform_host=$(detect_platform)
    
    log_info "Starting build process..."
    log_info "Platform: $platform_host"
    log_info "Build type: $BUILD_TYPE"
    log_info "Jobs: $JOBS"
    log_info "GUI: $([[ $GUI == "1" ]] && echo "Enabled" || echo "Disabled")"
    echo
    
    check_dependencies
    clean_build
    build_dependencies
    
    if [ "$DEPS_ONLY" != "1" ]; then
        configure_project
        build_project
        
        if [ "$NO_VERIFY" != "1" ]; then
            verify_build
        fi
        
        if [ "$NO_TESTS" != "1" ]; then
            run_tests
        fi
    fi
    
    echo
    log_success "Build process completed successfully!"
    
    if [ "$DEPS_ONLY" = "1" ]; then
        log_info "Dependencies are ready in: depends/$platform_host"
    else
        log_info "Binaries are available in: src/ and src/qt/"
    fi
}

# Run main function with all arguments
main "$@"