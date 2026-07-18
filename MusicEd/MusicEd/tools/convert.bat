@echo off
set "ASF_FOLDER=asf"
set "WAV_FOLDER=wav"
set "SX_PATH=sx.exe"

if not exist "%WAV_FOLDER%" mkdir "%WAV_FOLDER%"

for %%F in ("%ASF_FOLDER%\*.asf") do (
    "%SX_PATH%" -wave "%%F" -="%WAV_FOLDER%\%%~nF.wav"
)

echo Conversion complete.
