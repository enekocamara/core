@echo off
cargo build --release
SET path="%CD%\target\release"
echo add this path to u environment variables: %path%
pause
