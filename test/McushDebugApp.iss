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
Source: "dist\McushDebugApp\*"; DestDir: "{app}"; Excludes: "dist\McushDebugApp\McushDebugApp.exe.log"; Flags: recursesubdirs

[Icons]
Name: "{group}\McushDebug"; Filename: "{app}\McushDebugApp.exe"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"
