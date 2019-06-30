@echo off
copy dumpmbr.exe %systemroot%\system32\.
regedit /s add.reg
echo Native Example Installed
pause