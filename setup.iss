; BB60C Spectrum Analyzer Kurulum Betiği
#define MyAppName "BB60C Spectrum Analyzer"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "BB60C"
#define MyAppURL "https://github.com/username/BB60C-Analyzer"
#define MyAppExeName "BB60C_Analyzer.exe"

[Setup]
AppId={{BB60C-ANALYZER-GUID}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=LICENSE
OutputDir=installer
OutputBaseFilename=BB60C_Analyzer_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "build\release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "include\bb_api\bb_api.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "Qt6PrintSupport.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs
Source: "styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs
Source: "imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\*.*"
Type: dirifempty; Name: "{app}" 