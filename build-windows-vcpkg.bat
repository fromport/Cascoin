@echo off
REM Build script for Cascoin Core using vcpkg on Windows
REM Prerequisites: Visual Studio 2022, Git, vcpkg

setlocal enabledelayedexpansion

echo Building Cascoin Core with vcpkg...
echo.

REM Check if vcpkg is installed
if not exist "C:\vcpkg\vcpkg.exe" (
    echo Error: vcpkg not found at C:\vcpkg\
    echo Please install vcpkg first:
    echo   git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
    echo   cd C:\vcpkg
    echo   .\bootstrap-vcpkg.bat
    echo   .\vcpkg integrate install
    pause
    exit /b 1
)

REM Set vcpkg triplet
set VCPKG_DEFAULT_TRIPLET=x64-windows
set VCPKG_ROOT=C:\vcpkg

echo Installing dependencies with vcpkg...
echo.

REM Install required packages
call "%VCPKG_ROOT%\vcpkg.exe" install boost:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install openssl:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install libevent:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install protobuf:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install zeromq:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install qrencode:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install miniupnpc:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install berkeleydb:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install qt6-base:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install qt6-tools:x64-windows --recurse
call "%VCPKG_ROOT%\vcpkg.exe" install qt6-svg:x64-windows --recurse

if errorlevel 1 (
    echo Error: Failed to install dependencies
    pause
    exit /b 1
)

echo.
echo Dependencies installed successfully!
echo.

REM Create build directory
if not exist "build-vcpkg" mkdir build-vcpkg
cd build-vcpkg

echo Configuring with CMake...
echo.

REM Configure with CMake using vcpkg toolchain
cmake .. ^
    -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DENABLE_WALLET=ON ^
    -DENABLE_QT=ON ^
    -DENABLE_ZMQ=ON ^
    -DENABLE_UPNP=ON ^
    -DENABLE_TESTS=OFF ^
    -DENABLE_BENCH=OFF ^
    -G "Visual Studio 17 2022" ^
    -A x64

if errorlevel 1 (
    echo Error: CMake configuration failed
    pause
    exit /b 1
)

echo.
echo Building Cascoin Core...
echo.

REM Build the project
cmake --build . --config Release --parallel

if errorlevel 1 (
    echo Error: Build failed
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo.
echo Executables can be found in:
echo   %cd%\Release\
echo.
echo Available executables:
echo   - cascoind.exe       (Daemon)
echo   - cascoin-cli.exe    (Command line interface)
echo   - cascoin-tx.exe     (Transaction utility)
echo   - cascoin-qt.exe     (GUI wallet)
echo.

pause
