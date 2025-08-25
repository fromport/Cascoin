#!/usr/bin/env bash
set -euo pipefail

# macOS build helper for Cascoin with Qt6 GUI and wallet enabled.
# - Installs deps via Homebrew
# - Prefers Homebrew berkeley-db@5; otherwise builds BDB 5.3.28 locally
# - Configures with Qt6 and incompatible BDB enabled

ROOT_DIR="$(cd "$(dirname "$0")"/.. && pwd)"
cd "$ROOT_DIR"

DEPLOY=${DEPLOY:-0}
if [[ ${1:-} == "--deploy" ]]; then
  DEPLOY=1
fi

BREW="$(command -v brew || true)"
if [[ -z "$BREW" ]]; then
  echo "Homebrew not found. Please install from https://brew.sh and re-run." >&2
  exit 1
fi

echo "Updating Homebrew and installing dependencies..."
"$BREW" update
"$BREW" install autoconf automake libtool boost miniupnpc openssl@3 pkg-config protobuf python qt libevent zeromq qrencode || true
if [[ $DEPLOY -eq 1 ]]; then
  "$BREW" install librsvg || true
fi

QT_PREFIX="$($BREW --prefix qt)"
BOOST_PREFIX="$($BREW --prefix boost)"
BREW_PREFIX="$(brew --prefix)"
echo "Qt prefix: $QT_PREFIX"
# Resolve Qt6 host tools location (Qt6 uses libexec for moc/uic/rcc). Homebrew often installs them under share/qt/libexec in the Cellar.
QT_HOST_BINS="$(pkg-config --variable=host_bins Qt6Core 2>/dev/null || true)"
QT_CELLAR="$($BREW --cellar qt 2>/dev/null || true)"
if [[ -z "$QT_HOST_BINS" || ! -x "$QT_HOST_BINS/moc" ]]; then
  for p in \
    "$QT_PREFIX/libexec" \
    "$QT_PREFIX/libexec/Qt6/bin" \
    "$QT_PREFIX/Qt6/bin" \
    "$QT_PREFIX/share/qt/libexec" \
    ${QT_CELLAR:+"$QT_CELLAR"/*/share/qt/libexec}; do
    if [[ -n "${p:-}" && -x "$p/moc" && -x "$p/uic" && -x "$p/rcc" ]]; then
      QT_HOST_BINS="$p"
      break
    fi
  done
fi
if [[ -z "${QT_HOST_BINS:-}" || ! -x "$QT_HOST_BINS/moc" ]]; then
  echo "Qt6 host tools (moc/uic/rcc) not found. Ensure Homebrew qt is installed and provides share/qt/libexec." >&2
  exit 1
fi
export PATH="$QT_HOST_BINS:$PATH"
export PKG_CONFIG_PATH="$QT_PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH:-}"

echo "Ensuring Berkeley DB availability..."
# Try to install berkeley-db@5 via Homebrew first
if ! "$BREW" list --versions berkeley-db@5 >/dev/null 2>&1; then
  "$BREW" install berkeley-db@5 || true
fi
BDB_PREFIX="$($BREW --prefix berkeley-db@5 2>/dev/null || true)"
if [[ -z "$BDB_PREFIX" ]]; then
  echo "berkeley-db@5 not available via Homebrew; building BDB 5.3.28 locally..."
  rm -rf .bdb5 db-5.3.28.NC db-5.3.28.NC.tar.gz || true
  curl -L -o db-5.3.28.NC.tar.gz http://download.oracle.com/berkeley-db/db-5.3.28.NC.tar.gz
  tar -xzf db-5.3.28.NC.tar.gz
  mkdir -p .bdb5
  ( cd db-5.3.28.NC/build_unix \
    && ../dist/configure --prefix="$(pwd)/../../.bdb5" --enable-cxx --disable-shared --with-mutex=POSIX/pthreads --disable-atomicsupport \
    && make -j"$(sysctl -n hw.ncpu)" \
    && make install )
  BDB_PREFIX="$ROOT_DIR/.bdb5"
else
  # Validate the brew prefix actually contains a usable C++ Berkeley DB library
  if [[ ! -e "$BDB_PREFIX/lib/libdb_cxx-5.3.dylib" && ! -e "$BDB_PREFIX/lib/libdb_cxx-5.3.a" ]]; then
    echo "brew berkeley-db@5 found at $BDB_PREFIX, but libdb_cxx-5.3 not present; trying Homebrew 'berkeley-db' (v18) with --with-incompatible-bdb..."
    if ! "$BREW" list --versions berkeley-db >/dev/null 2>&1; then
      "$BREW" install berkeley-db || true
    fi
    ALT_BDB_PREFIX="$($BREW --prefix berkeley-db 2>/dev/null || true)"
    if [[ -n "$ALT_BDB_PREFIX" && ( -e "$ALT_BDB_PREFIX/lib/libdb_cxx-18.dylib" || -e "$ALT_BDB_PREFIX/lib/libdb_cxx-18.a" ) ]]; then
      BDB_PREFIX="$ALT_BDB_PREFIX"
    else
      echo "No usable Homebrew Berkeley DB found; skipping Homebrew and continuing (configure will fail if wallet required)."
      BDB_PREFIX=""
    fi
  fi
fi
echo "Berkeley DB prefix: $BDB_PREFIX"

export PKG_CONFIG_PATH="$($BREW --prefix openssl@3)/lib/pkgconfig:$BREW_PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
export CPPFLAGS="-I$BREW_PREFIX/include ${CPPFLAGS:-}"
export LDFLAGS="-L$BREW_PREFIX/lib ${LDFLAGS:-}"

echo "Running autogen.sh..."
./autogen.sh

echo "Configuring with Qt6 and wallet enabled..."
LIBDB_FLAG="-ldb_cxx-5.3"
if [[ -n "$BDB_PREFIX" ]]; then
  if [[ -e "$BDB_PREFIX/lib/libdb_cxx-18.dylib" || -e "$BDB_PREFIX/lib/libdb_cxx-18.a" ]]; then
    LIBDB_FLAG="-ldb_cxx-18"
  fi
fi

./configure \
  --with-gui=qt6 \
  --enable-wallet \
  --with-qrencode \
  --enable-zmq \
  --with-incompatible-bdb \
  --with-qt-bindir="$QT_HOST_BINS" \
  --with-boost="$BOOST_PREFIX" \
  --with-boost-libdir="$BREW_PREFIX/lib" \
  MOC="$QT_HOST_BINS/moc" \
  UIC="$QT_HOST_BINS/uic" \
  RCC="$QT_HOST_BINS/rcc" \
  LRELEASE="$QT_PREFIX/bin/lrelease" \
  LUPDATE="$QT_PREFIX/bin/lupdate" \
  BDB_CFLAGS="${BDB_PREFIX:+-I$BDB_PREFIX/include}" \
  BDB_LIBS="${BDB_PREFIX:+-L$BDB_PREFIX/lib }$LIBDB_FLAG"

echo "Building..."
make -j"$(sysctl -n hw.ncpu)"

if [[ $DEPLOY -eq 1 ]]; then
  echo "Creating .dmg (make deploy)..."
  make deploy
fi

echo "Done. Binaries (if build succeeded):"
ls -lh src/cascoin-qt src/cascoind 2>/dev/null || true


