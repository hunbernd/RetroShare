@echo off

setlocal

:: Initialize environment
call "%~dp0..\env.bat"
if errorlevel 1 goto error_env
call "%EnvPath%\env.bat"
if errorlevel 1 goto error_env
call "%EnvPath%\env.bat"
if errorlevel 1 goto error_env
call "%EnvPath%\env-msys2.bat"
if errorlevel 1 goto error_env

:: Initialize base environment
call "%~dp0env-base.bat" %*
if errorlevel 2 exit /B 2
if errorlevel 1 goto error_env

:: Get Qt version
%EnvMSYS2Cmd% "pacman -Si mingw-w64-%RsMSYS2Architecture%-qt5" > "%~dp0..\qt.cachekey"
endlocal && set pkgcachefolder=%EnvMSYS2BasePath%\var\cache\pacman\pkg\
exit /B 0

:error
popd

title %COMSPEC%

if errorlevel 1 echo.& echo Build failed& echo.
exit /B %ERRORLEVEL%

:error_env
echo Failed to initialize environment.
endlocal
exit /B 1
