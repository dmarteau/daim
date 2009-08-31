echo off

set GDAL_COMMAND=%1
set GDAL_BUILDTYPE=%2
set GDAL_SRC_DIR=%3
set GDAL_INSTALLDIR=%4

rem CLEAR MAKEFLAGS, otherwise it passes invalid options to nmake when called
rem from Gnu make
set MAKEFLAGS=

if DEFINED WIN64 (
if "%WIN64%"=="0" set WIN64=
)



echo command=%GDAL_COMMAND% mode=%GDAL_BUILDTYPE% srcdir=%GDAL_SRC_DIR% installdir=%GDAL_INSTALLDIR%

cd %GDAL_SRC_DIR%

if "%GDAL_BUILDTYPE%"=="debug" (
 set DEBUG_BUILD=DEBUG_BUILD=1
) else (
 set DEBUG_BUILD=
 echo building in release mode
)


echo on

if "%GDAL_COMMAND%"=="configure" goto end
if "%GDAL_COMMAND%"=="clean"     goto do_clean
if "%GDAL_COMMAND%"=="build"     goto do_build
if "%GDAL_COMMAND%"=="install"   goto do_install
if "%GDAL_COMMAND%"=="all"       goto do_all

goto end

:do_all
nmake -f makefile.vc MSVC_VER=1400 GDAL_HOME=%GDAL_INSTALLDIR% clean
nmake -f makefile.vc MSVC_VER=1400 GDAL_HOME=%GDAL_INSTALLDIR% %DEBUG_BUILD%
nmake -f makefile.vc MSVC_VER=1400 GDAL_HOME=%GDAL_INSTALLDIR% devinstall
goto end

:do_build
nmake -f makefile.vc MSVC_VER=1400 GDAL_HOME=%GDAL_INSTALLDIR% %DEBUG_BUILD%
goto end

:do_install
nmake -f makefile.vc MSVC_VER=1400 GDAL_HOME=%GDAL_INSTALLDIR% devinstall
goto end

:do_clean
nmake -f makefile.vc MSVC_VER=1400 GDAL_HOME=%GDAL_INSTALLDIR% clean
goto end

:end

cd ..
