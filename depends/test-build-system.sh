#!/bin/bash
#
# Test Script for Enhanced Dependencies Build System
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[TEST-INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[TEST-SUCCESS]${NC} $1"
}

log_error() {
    echo -e "${RED}[TEST-ERROR]${NC} $1"
}

print_banner() {
    echo
    echo "========================================"
    echo "    Enhanced Build System Test Suite   "
    echo "========================================"
    echo
}

# Test 1: Validate build system syntax
test_syntax() {
    log_info "Testing build system syntax..."
    
    # Test make dry-run for basic syntax validation
    if make --dry-run HOST=x86_64-w64-mingw32 2>&1 | grep -q "Extracting"; then
        log_success "Build system syntax is valid"
    else
        log_error "Build system syntax has errors"
        return 1
    fi
}

# Test 2: Test individual package builds
test_packages() {
    log_info "Testing individual package builds..."
    
    local packages=("native_ccache")
    
    for package in "${packages[@]}"; do
        log_info "Testing package: $package"
        
        if make HOST=x86_64-w64-mingw32 "$package" -j2 >/dev/null 2>&1; then
            log_success "Package $package builds successfully"
        else
            log_error "Package $package failed to build"
            return 1
        fi
    done
}

# Test 3: Verify package definitions
test_package_definitions() {
    log_info "Verifying package definitions..."
    
    local required_packages=("boost" "openssl" "qt" "zeromq" "libevent")
    local missing_packages=()
    
    for package in "${required_packages[@]}"; do
        if [ ! -f "packages/${package}.mk" ]; then
            missing_packages+=("$package")
        fi
    done
    
    if [ ${#missing_packages[@]} -eq 0 ]; then
        log_success "All required package definitions found"
    else
        log_error "Missing package definitions: ${missing_packages[*]}"
        return 1
    fi
}

# Test 4: Check for updated versions
test_versions() {
    log_info "Checking package versions..."
    
    # Check Qt version
    local qt_version=$(grep "_version=" packages/qt.mk | cut -d'=' -f2)
    if [[ "$qt_version" == "6.8.1" ]]; then
        log_success "Qt version is current ($qt_version)"
    else
        log_error "Qt version may be outdated ($qt_version)"
    fi
    
    # Check Boost version
    local boost_version=$(grep "_version=" packages/boost.mk | cut -d'=' -f2)
    if [[ "$boost_version" == "1_86_0" ]]; then
        log_success "Boost version is current ($boost_version)"
    else
        log_error "Boost version may be outdated ($boost_version)"
    fi
}

# Test 5: Verify scripts are executable
test_scripts() {
    log_info "Verifying build scripts..."
    
    local scripts=("build-windows.sh" "../build-qt6-windows.sh" "../clean-and-build.sh")
    local missing_scripts=()
    
    for script in "${scripts[@]}"; do
        if [ -f "$script" ] && [ -x "$script" ]; then
            log_success "Script $script is present and executable"
        else
            missing_scripts+=("$script")
        fi
    done
    
    if [ ${#missing_scripts[@]} -gt 0 ]; then
        log_error "Missing or non-executable scripts: ${missing_scripts[*]}"
        return 1
    fi
}

# Main test runner
main() {
    print_banner
    
    log_info "Starting Enhanced Build System Test Suite..."
    echo
    
    local tests=(
        "test_syntax"
        "test_package_definitions" 
        "test_versions"
        "test_scripts"
        "test_packages"
    )
    
    local passed=0
    local total=${#tests[@]}
    
    for test_func in "${tests[@]}"; do
        if $test_func; then
            ((passed++))
        fi
        echo
    done
    
    echo "========================================"
    log_info "Test Results: $passed/$total tests passed"
    
    if [ $passed -eq $total ]; then
        log_success "All tests passed! Build system is ready for use."
        echo
        echo "Usage Examples:"
        echo "  ./build-windows.sh                    # Build dependencies for Windows"
        echo "  ../clean-and-build.sh -p windows      # Complete Windows build"
        echo "  make HOST=x86_64-w64-mingw32 -j\$(nproc)  # Manual build"
        return 0
    else
        log_error "Some tests failed. Please review the output above."
        return 1
    fi
}

# Run tests
main "$@"
