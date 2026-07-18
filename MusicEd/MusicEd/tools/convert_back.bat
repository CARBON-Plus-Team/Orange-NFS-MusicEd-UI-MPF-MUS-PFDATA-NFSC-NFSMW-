@echo off
set "WAV_FOLDER=wav"
set "ASF_FOLDER=asf"
set "SX_PATH=sx.exe"

if not exist "%ASF_FOLDER%" mkdir "%ASF_FOLDER%"

for %%F in ("%WAV_FOLDER%\*.wav") do (
    "%SX_PATH%" -pcstream -eaxa_blk "%%F" -="%ASF_FOLDER%\%%~nF.asf"
)

echo Conversion complete.
