# Windows Qt Platform Plugin Fix

## Problem
When cross-compiling for Windows, the application fails to start with the error:
```
This application failed to start because no Qt platform plugin could be initialized. 
Reinstalling the application may fix this problem. Available platform plugins are: windows.
```

## Root Cause
The issue was in the Qt6 static plugin configuration in the build system. For Qt6 builds, the platform-specific plugin imports were not being configured properly:

1. **Qt6 Configuration Gap**: The Qt6 branch in `build-aux/m4/bitcoin_qt.m4` was only defining `QT_STATICPLUGIN` but not setting up platform-specific macros like `QT_QPA_PLATFORM_WINDOWS`.

2. **Inconsistent Plugin Imports**: In `src/qt/bitcoin.cpp`, Qt6 was unconditionally importing `QWindowsIntegrationPlugin`, while Qt5 correctly used conditional imports based on platform macros.

## Solution Applied

### 1. Fixed Qt6 Static Plugin Configuration (`build-aux/m4/bitcoin_qt.m4`)
Added proper platform-specific plugin detection and linking for Qt6 static builds:

```m4
if test "x$bitcoin_qt_got_major_vers" = x6; then
  dnl Qt6: detect static Qt and define QT_STATICPLUGIN so Q_IMPORT_PLUGIN works
  _BITCOIN_QT_IS_STATIC
  if test "x$bitcoin_cv_static_qt" = xyes; then
    AC_DEFINE(QT_STATICPLUGIN, 1, [Define this symbol if qt plugins are static])
    _BITCOIN_QT_CHECK_STATIC_PLUGINS([Q_IMPORT_PLUGIN(QMinimalIntegrationPlugin)],[-lqminimal])
    AC_DEFINE(QT_QPA_PLATFORM_MINIMAL, 1, [Define this symbol if the minimal qt platform exists])
    if test "x$TARGET_OS" = xwindows; then
      _BITCOIN_QT_CHECK_STATIC_PLUGINS([Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)],[-lqwindows])
      AC_DEFINE(QT_QPA_PLATFORM_WINDOWS, 1, [Define this symbol if the qt platform is windows])
    elif test "x$TARGET_OS" = xlinux; then
      _BITCOIN_QT_CHECK_STATIC_PLUGINS([Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)],[-lqxcb -lxcb-static])
      AC_DEFINE(QT_QPA_PLATFORM_XCB, 1, [Define this symbol if the qt platform is xcb])
    elif test "x$TARGET_OS" = xdarwin; then
      AX_CHECK_LINK_FLAG([[-framework IOKit]],[QT_LIBS="$QT_LIBS -framework IOKit"],[AC_MSG_ERROR(could not iokit framework)])
      _BITCOIN_QT_CHECK_STATIC_PLUGINS([Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)],[-lqcocoa])
      AC_DEFINE(QT_QPA_PLATFORM_COCOA, 1, [Define this symbol if the qt platform is cocoa])
    fi
  fi
```

### 2. Fixed Qt6 Plugin Imports (`src/qt/bitcoin.cpp`)
Made Qt6 plugin imports conditional like Qt5, based on platform detection:

```cpp
#if defined(QT_STATICPLUGIN)
#include <QtPlugin>
#if QT_VERSION >= 0x060000
// Qt6 static plugins
#if defined(QT_QPA_PLATFORM_MINIMAL)
Q_IMPORT_PLUGIN(QMinimalIntegrationPlugin);
#endif
#if defined(QT_QPA_PLATFORM_XCB)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin);
#elif defined(QT_QPA_PLATFORM_WINDOWS)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#elif defined(QT_QPA_PLATFORM_COCOA)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);
#endif
#else
// Qt5 static plugins (unchanged)
// ...
#endif
#endif
```

## What This Fixes

1. **Proper Platform Detection**: The build system now correctly detects the target platform (Windows) and defines `QT_QPA_PLATFORM_WINDOWS` for Qt6 builds.

2. **Correct Static Plugin Linking**: The Windows platform plugin (`-lqwindows`) is now properly linked into the executable during the static build process.

3. **Conditional Plugin Imports**: The application now only imports the platform plugin that matches the target platform, preventing conflicts.

## How to Apply the Fix

1. **Regenerate Build Configuration**:
   ```bash
   cd /path/to/cascoin
   ./autogen.sh
   ```

2. **Reconfigure for Windows Cross-Compilation**:
   ```bash
   cd depends
   make HOST=x86_64-w64-mingw32
   cd ..
   CONFIG_SITE=$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --prefix=/
   ```

3. **Rebuild**:
   ```bash
   make clean
   make
   ```

## Verification

After applying this fix, the Windows executable should:
- Have the Qt Windows platform plugin statically linked
- Start successfully without the "no Qt platform plugin" error
- Not require any external Qt DLL files or plugin directories

## Technical Details

The error occurred because:
- Qt6 static builds weren't getting the proper `QT_QPA_PLATFORM_WINDOWS` macro defined
- Without this macro, the conditional plugin import in `bitcoin.cpp` wasn't working
- The application had `QT_STATICPLUGIN` defined but wasn't importing the correct platform plugin
- Qt runtime couldn't find an available platform plugin to initialize the GUI

This fix ensures that for Qt6 Windows cross-compilation:
1. `QT_STATICPLUGIN` is defined ✓
2. `QT_QPA_PLATFORM_WINDOWS` is defined ✓  
3. `QWindowsIntegrationPlugin` is imported ✓
4. `-lqwindows` is linked ✓

The static plugin is now properly embedded in the executable, eliminating the need for external plugin files.