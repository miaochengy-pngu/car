@echo off
setlocal

set "UV4=C:\Keil_v5\UV4\UV4.exe"

if not exist "%UV4%" (
  echo [ERROR] Keil uVision not found at:
  echo %UV4%
  echo.
  echo Open car.uvproj manually, or edit UV4 in this file.
  pause
  exit /b 1
)

"%UV4%" -b car.uvproj -j0
exit /b %ERRORLEVEL%
