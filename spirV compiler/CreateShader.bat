@echo off
:START
echo Enter command for the spir-v validator, example: "-V cube.vert" compiles cube.vert into cube.spv
set /p command=""
CALL glslangValidator %command%
GOTO START
PAUSE
exit