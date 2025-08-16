# Windows Cross-Compilation Build Instructions

## Problem Description

There is a known issue when building Windows dependencies from within the Cursor IDE AppImage environment. The AppImage creates environment conflicts that prevent proper execution of build tools like `gcc`, `tar`, and `gzip`.

## Solution

We have implemented two workarounds:

### 1. Python-based tar extraction (Already Applied)
- Modified `depends/funcs.mk` to use Python's tarfile module instead of system tar
- Created `depends/extract_helper.py` to handle archive extraction
- This allows extraction to work even with the environment issues

### 2. Build Script for Clean Environment
Due to compiler issues that cannot be worked around within Cursor, you need to run the build in a clean terminal environment.

## Build Instructions

### Step 1: Open a Terminal Outside of Cursor
Open a new terminal window using one of:
- GNOME Terminal (Ubuntu default)
- Konsole (KDE)
- xterm
- Any terminal emulator NOT launched from within Cursor

### Step 2: Run the Build Script
```bash
cd /home/alexander/Cascoin
./build-windows-deps.sh
```

This script will:
- Clean any previous build attempts
- Configure mingw for POSIX threads (required for C++11)
- Build all Windows dependencies
- Take approximately 20-40 minutes

### Step 3: Build Cascoin
After dependencies are built successfully:

```bash
cd /home/alexander/Cascoin
./autogen.sh
CONFIG_SITE=$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --prefix=/
make
```

## Output

The Windows executables will be located in:
- `src/cascoind.exe` - Daemon
- `src/cascoin-cli.exe` - Command-line interface
- `src/cascoin-tx.exe` - Transaction utility
- `src/qt/cascoin-qt.exe` - GUI wallet (if Qt is enabled)

## Troubleshooting

If the build fails:

1. **"Socket is not connected" errors**: Make sure you're running outside of Cursor
2. **Missing dependencies**: Install required packages:
   ```bash
   sudo apt install build-essential libtool autotools-dev automake \
                    pkg-config bsdmainutils curl git \
                    g++-mingw-w64-x86-64
   ```
3. **Check logs**: Look in `depends/work/build/` for detailed error logs

## Technical Details

The issue is caused by Cursor's AppImage environment which sets problematic environment variables and interferes with `posix_spawn` system calls. This prevents programs from executing child processes correctly.

Modified files:
- `depends/funcs.mk` - Uses Python for tar extraction
- `depends/extract_helper.py` - Python extraction helper
- `depends/packages/qt.mk` - Fixed Qt 6.5.3 configuration

