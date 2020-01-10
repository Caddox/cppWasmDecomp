@echo off

echo Running tests. . .

REM remove the old log file
del test.log

REM first, assign the filepath for the executable
set exec = '..\Debug\wasmDecomp.exe'
set myPath = %%~dp0
echo %exec%

for %%f in (.\Tests\*.wasm) do (
    echo %%f_out.txt
    echo %myPath
    ..\Debug\wasmDecomp.exe %%f > %%f_out.txt
)

REM move all the output files back up a level
for %%f in (.\Tests\*.txt) do (
    echo Moving %%f. . .
    move %%f .\
)

REM merge all the files into one
type *.txt > test.log

REM remove all the text files
del *.txt
