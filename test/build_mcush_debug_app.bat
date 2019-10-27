pushd h:\test\

pyinstaller --clean -y --noconsole -i img\logo_inv.ico McushDebugApp.py
"c:\Program Files\Inno Setup 5\Compil32.exe" /cc McushDebugApp.iss

pause
