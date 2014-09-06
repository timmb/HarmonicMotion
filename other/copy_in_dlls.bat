@rem This script is called by Visual Studio 

@rem replace forward slash with back slash
set QT5_PATH=%QT5_PATH:/=\%
set BIN_DLLS=%BIN_DLLS:/=\%
set PLUGIN_DLLS=%PLUGIN_DLLS:/=\%
set EXE_PATH=%EXE_PATH:/=\%

for %%I in (%BIN_DLLS%) do copy "%QT5_PATH%\bin\%%I" "%EXE_PATH%\"
for %%I in (%PLUGIN_DLLS%) do copy "%QT5_PATH%\plugins\%%I" "%EXE_PATH%\"