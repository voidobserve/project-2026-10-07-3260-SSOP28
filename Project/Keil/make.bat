cd /d %~dp0
:: C:\Keil\ARM\ARMCC\bin\fromelf --bin --output .\project.bin  .\project.axf
::C:\Keil\ARM\ARMCC\bin\fromelf --text -a -c --output=.\project.lst  .\project.axf

::copy .\Objects\project.axf project.axf

SET FormerFileHex=Demo.hex
SET FormerFileM51=Demo.m51
if exist %FormerFileHex% (
	del /q /f Demo.hex
)
if exist %FormerFileM51% (
	del /q /f Demo.m51
)

:: ÷ª∂¡ Ù–‘
ATTRIB +R ..\..\.\User\lib\*.*
ATTRIB -R ..\..\.\User\lib\tk_config.h
ATTRIB -R ..\..\.\User\lib\tk_set.c
ATTRIB -R ..\..\.\User\lib\tk_set.h


rem makecode.exe
rem copy app_eflash_nvr.bin project_crc.bin
rem BinScript.exe BinScript_app_hex.BinScript

::crc.exe crc.ini
rem BinScript.exe BinScript_Bin2Hex.BinScript

