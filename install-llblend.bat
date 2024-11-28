@echo off

@echo Copy Release to d:\opt\bin
copy llblend-ms\x64\Release\llblend.exe d:\opt\bin\llblend.exe


@echo
@echo Compare md5 hash
cmp -h llblend-ms\x64\Release\llblend.exe d:\opt\bin\llblend.exe
ld -a d:\opt\bin\llblend.exe

@echo
@echo List all llblend.exe
ld -r -F=llblend.exe bin d:\opt\bin
