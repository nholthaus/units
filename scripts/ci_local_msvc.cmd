@echo off
REM Mirror the msvc-2022 CI leg locally so local is never more permissive than CI. MSVC differs from gcc/clang on
REM overload resolution, concept evaluation, and template instantiation order, and it has no __int128 -- a change that
REM builds clean on both Linux compilers can still fail here, so run this before pushing anything that touches
REM operators, constraints, or traits.
REM
REM Warm by design: configures build-msvc once, then Ninja rebuilds only what changed.
REM
REM Usage:  scripts\ci_local_msvc.cmd            build everything and run ctest
REM         scripts\ci_local_msvc.cmd harness    run the errorMessages diagnostic harness under cl (~160s)
REM         scripts\ci_local_msvc.cmd <target>   build one target only

setlocal
set VSDIR=C:\Program Files\Microsoft Visual Studio\2022\Professional
if not exist "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" (
	echo ERROR: VS2022 not found at "%VSDIR%".
	exit /b 1
)
call "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" >nul
set "PATH=%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"

cd /d %~dp0..

REM The MSVC CI leg grades the diagnostic messages too, and its wording differs from gcc/clang (expect-match-msvc), so
REM the harness needs its own MSVC run.
if /i "%~1"=="harness" (
	python test/errorMessages/run.py --cc cl --std c++23 --include include --jobs 4
	exit /b %ERRORLEVEL%
)

if not exist build-msvc\build.ninja (
	cmake -B build-msvc -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl || exit /b 1
)

REM Two-stage build, as CI does: the heavy test target alone first, then everything else. ctest covers the examples and
REM the lean (iostream/format/string-disabled) configurations too, so all targets must exist or those cases report
REM "Not Run" -- a red that says nothing about the code.
if "%~1"=="" (
	cmake --build build-msvc --parallel --target unitLibTest || exit /b 1
	cmake --build build-msvc --parallel || exit /b 1
	pushd build-msvc
	ctest --output-on-failure --parallel 4
	set RC=%ERRORLEVEL%
	popd
	exit /b %RC%
)

cmake --build build-msvc --parallel --target %~1
