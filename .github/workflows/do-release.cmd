@echo off
REM Build some release of 7-Zip ZS

SET COPYCMD=/Y /B
SET COPTS=-m0=lzma -mx9 -ms=on -mf=bcj2
SET VERSION=%SZIP_VERSION%
SET VERSION_CODE=%VERSION:.=%
SET URL=https://www.7-zip.org/a/7z%VERSION_CODE%.exe
SET DARK_URL=https://github.com/ozone10/7zip-Dark7zip/releases/download/v%VERSION%-v%DARK_7ZIP_VERSION%/7z%VERSION%-dark-x64.zip
SET SZIP="C:\Program Files\7-Zip\7z.exe"
SET LURL=https://raw.githubusercontent.com/mcmilk/7-Zip-zstd/master/CPP/7zip/Bundles

SET WD=%cd%
SET SKEL=%WD%\skel
SET DARK=%WD%\dark

REM Download our skeleton files
mkdir %SKEL%
cd %SKEL%
:: Make curl follow redirect
curl %URL% -L --max-redirs 5 --output 7-Zip.exe
%SZIP% x 7-Zip.exe
:: mkdir %WD%\totalcmd

:: Download Dark7zip release
mkdir %DARK%
cd %DARK%
curl %DARK_URL% -L --max-redirs 5 --output dark.zip
%SZIP% x dark.zip

goto start

:doit
echo Doing ARCH=%ARCH% in SOURCE=%BIN%

REM 7-Zip Files
cd %SKEL%
del *.exe *.dll *.sfx

:: Copy dark ini
copy %DARK%\7zDark.ini 7zDark.ini
:: Copy remaining files to free installation release
xcopy . %WD%\..\portable\bin-%ARCH%\ /e

FOR %%f IN (7z.dll 7z.exe 7z.sfx 7za.dll 7za.exe 7zCon.sfx 7zFM.exe 7zG.exe 7-zip.dll 7zxa.dll Uninstall.exe) DO (
  copy %BIN%\%%f %%f
)
IF DEFINED ZIP32 copy %ZIP32% 7-zip32.dll
%SZIP% a ..\%ARCH%.7z %COPTS%
cd %WD%
copy %BIN%\Install.exe + %ARCH%.7z 7z-%VERSION%-dark-%ARCH%.exe
del %ARCH%.7z

:: REM Codec Files
:: mkdir codecs-%ARCH%
:: FOR %%f IN (brotli flzma2 lizard lz4 lz5 zstd) DO (
::   copy %BIN%\%%f.dll codecs-%ARCH%\%%f.dll
:: )
:: cd codecs-%ARCH%
:: curl %LURL%/Codecs/LICENSE --output LICENSE
:: curl %LURL%/Codecs/README.md --output README.md
:: %SZIP% a ..\Codecs-%ARCH%.7z %COPTS%
:: cd %WD% && rd /S /Q Codecs-%ARCH%

:: REM Total Commander DLL
:: cd %WD%\totalcmd
:: copy %BIN%\7zxa.dll %TCDLL%
:: curl %LURL%/TotalCMD/LICENSE --output LICENSE
:: curl %LURL%/TotalCMD/README.md --output README.md
goto done_%ARCH%

REM Currently we build 4 architectures
:start
:: SET ARCH=x32
:: SET ZIP32=
:: SET BIN=%WD%\bin-x86
:: SET TCDLL=tc7z.dll
:: goto doit
:: :done_x32

SET ARCH=x64
:: SET ZIP32=%WD%\bin-x86\7-zip.dll
SET ZIP32=
SET BIN=%WD%\bin-x64
SET TCDLL=tc7z64.dll
goto doit
:done_x64

:: SET ARCH=arm
:: SET ZIP32=
:: SET BIN=%WD%\bin-arm
:: SET TCDLL=tc7zarm.dll
:: goto doit
:: :done_arm

:: SET ARCH=arm64
:: SET ZIP32=%WD%\bin-arm\7-zip.dll
:: SET BIN=%WD%\bin-arm64
:: SET TCDLL=tc7zarm64.dll
:: goto doit
:: :done_arm64

:: cd %WD%\totalcmd
:: %SZIP% a ..\TotalCmd.7z %COPTS%

REM cleanup
cd %WD%
rd /S /Q %SKEL%
:: rd /S /Q %WD%\totalcmd
