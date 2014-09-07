@rem This script is called by Visual Studio 

@rem replace forward slash with back slash
set QT5_PATH=%QT5_PATH:/=\%
set BIN_DLLS=%BIN_DLLS:/=\%
set PLATFORM_DLLS=%PLATFORM_DLLS:/=\%
set IMAGEFORMAT_DLLS=%IMAGEFORMAT_DLLS:/=\%
set EXE_PATH=%EXE_PATH:/=\%

echo EXE_PATH: %EXE_PATH%

for %%I in (%BIN_DLLS%) do copy "%QT5_PATH%\bin\%%I" "%EXE_PATH%\"
for %%I in (%PLATFORM_DLLS%) do (if not exist "%EXE_PATH%\platforms" mkdir "%EXE_PATH%\platforms") & copy "%QT5_PATH%\plugins\platforms\%%I" "%EXE_PATH%\platforms\"
for %%I in (%IMAGEFORMAT_DLLS%) do (if not exist "%EXE_PATH%\imageformats" mkdir "%EXE_PATH%\imageformats") & copy "%QT5_PATH%\plugins\imageformats\%%I" "%EXE_PATH%\imageformats\"