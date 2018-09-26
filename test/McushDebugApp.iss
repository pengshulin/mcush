; Mcush Debug

[Setup]
AppName=McushDebug
AppVersion=1.0
DefaultDirName={pf}\McushDebug
DefaultGroupName=McushDebug
DisableProgramGroupPage=yes
Compression=lzma2
SolidCompression=yes
UninstallDisplayIcon={app}\img\logo_inv.png
OutputDir=app_dist
OutputBaseFilename=mcush_debug_setup_v1.0

[Files]
Source: "img\*.ico"; DestDir: "{app}\img"
Source: "img\*.png"; DestDir: "{app}\img"
;Source: "fonts\*.ttf"; DestDir: "{app}\fonts"
Source: "dist-mcush-debug\*"; DestDir: "{app}"; Excludes: "dist-mcush-debug\McushDebugApp.exe.log"
;Source: "C:\Python27\Lib\site-packages\matplotlib\mpl-data\*"; DestDir: "{app}\mpl-data"; Flags: recursesubdirs

[Icons]
Name: "{group}\McushDebug"; Filename: "{app}\McushDebugApp.exe"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"
