@echo off
if "%1"=="" goto error

ftp -s:put.txt %1

goto end

:error
echo This batch file requires a machine name as a parameter!!! (i.e. put.bat bart)

:end
