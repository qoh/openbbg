@echo off
if "%~1" == "" (
	set gen="Visual Studio 14 2015 Win64"
) else (
	set gen="%~1"
)
mkdir build
pushd build
cmake -Wno-dev -G %gen% ..