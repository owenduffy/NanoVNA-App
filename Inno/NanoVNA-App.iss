#define MyAppName "NanoVNA-App"

;#pragma option -v+
;#pragma verboselevel 9
;#define Debug

#define icap(str S) (copy(S,1,1)+lowercase(copy(S,2)))
#define MyAppName2 icap(MyAppName)
#define MyAppName3 lowercase(MyAppName)
#define Major 0
#define Minor 0
#define Rev 0
#define Build 0
#define MyAppExeName MyAppName+".exe"
#define MyAppVersion GetVersionComponents("..\"+MyAppExeName,Major,Minor,Rev,Build)
#define MyAppVersion str(Major)+"."+str(Minor)+"."+str(Rev)

[Setup]
;SetupLogging=yes
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{7F33A37F-47CC-4A0E-9E7B-04C97265A2B9}
AppName={#MyAppName}
AppMutex={#MyAppName}
AppVersion={#MyAppVersion}
DefaultDirName=c:\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=licence.txt
OutputDir=inno
;OutputBaseFilename={#MyAppName2}Setup
OutputBaseFilename={#MyAppName}-Setup
;SetupIconFile={#MyAppName}.ico
Compression=lzma2/max
SolidCompression=yes

SourceDir=..
DisableWelcomePage=no
DisableDirPage=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[InstallDelete]
Type: files; Name: "{app}\Uninstall.exe" 

[Files]
Source: "Win32\Release\{#MyAppName}.exe"; DestDir: "{app}"; Flags: ignoreversion;
Source: "licence.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "mem_0.s2p"; DestDir: "{app}"; Flags: ignoreversion

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

;{userappdata}

[Registry]
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: string; ValueName: "DisplayName"; ValueData: {#setupsetting("AppName")}; Flags: deletekey uninsdeletekey
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}"; ValueType: string; ValueName: "DisplayVersion"; ValueData: {#setupsetting("AppVersion")}; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}"; ValueType: string; ValueName: "Install_Dir"; ValueData: {app}; Flags: uninsdeletekeyifempty

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
 