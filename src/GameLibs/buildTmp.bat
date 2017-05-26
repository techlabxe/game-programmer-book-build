call "%VS80COMNTOOLS%\vsvars32.bat"
devenv /Rebuild Debug ./Sound2/GameLib.sln

if ERRORLEVEL 1 exit /b 1
devenv /Rebuild Release ./Sound2/GameLib.sln

if ERRORLEVEL 1 exit /b 1
