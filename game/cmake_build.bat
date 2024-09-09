@echo off
setlocal

:: Set directories
set "SOURCE_DIR=%~dp0Syris\vendor\hwloc\contrib\windows-cmake"
set "BUILD_DIR=%SOURCE_DIR%\build"
set "INSTALL_DIR=%~dp0build\Syris\vendor\hwloc"

:: Create directories if they don't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

:: Navigate to the build directory
cd /d "%BUILD_DIR%"

:: Configure HWLOC with CMake
cmake -G "Visual Studio 17 2022" ^
      -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
      -DCMAKE_BUILD_TYPE=Release ^
      "%SOURCE_DIR%"

:: Build HWLOC
cmake --build . --config Release

:: Install HWLOC
cmake --install . --config Release

:: Output completion message
echo HWLOC build and installation complete at %INSTALL_DIR%.

endlocal
pause
