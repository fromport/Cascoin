# Enhanced MinGW32 cross-compilation support for Windows

mingw32_CFLAGS=-pipe -O2
mingw32_CXXFLAGS=$(mingw32_CFLAGS)

mingw32_release_CFLAGS=-O2 -fomit-frame-pointer
mingw32_release_CXXFLAGS=$(mingw32_release_CFLAGS)

mingw32_debug_CFLAGS=-O1 -g
mingw32_debug_CXXFLAGS=$(mingw32_debug_CFLAGS)

mingw32_debug_CPPFLAGS=-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC

# Enhanced cross-compilation flags for modern Windows targets
mingw32_CPPFLAGS=-DWIN32 -D_WIN32_WINNT=0x0601 -D_WIN32_IE=0x0501
mingw32_LDFLAGS=-static-libgcc -static-libstdc++

# Architecture-specific settings
x86_64_mingw32_CFLAGS=-m64 -mno-ms-bitfields
x86_64_mingw32_CXXFLAGS=$(x86_64_mingw32_CFLAGS)
x86_64_mingw32_LDFLAGS=-m64

i686_mingw32_CFLAGS=-m32 -mno-ms-bitfields  
i686_mingw32_CXXFLAGS=$(i686_mingw32_CFLAGS)
i686_mingw32_LDFLAGS=-m32

# Windows-specific libraries and features
mingw32_cmake_system=Windows
mingw32_qt_config_opts_base=QMAKE_LFLAGS_WINDOWS=-static