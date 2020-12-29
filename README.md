# QSetupCreator
A gui for qt installation framework to create installer for your qt projects on windows

## Download
<a href="https://github.com/mguludag/QSetupCreator/releases/latest/download/setup_vc17x86.7z"><img alt="GitHub Releases" src="https://img.shields.io/github/downloads/mguludag/QSetupCreator/latest/total?label=Download%20(vc17_x86)&style=for-the-badge">

![](https://raw.githubusercontent.com/mguludag/QSetupCreator/master/qst1.png)
![](https://raw.githubusercontent.com/mguludag/QSetupCreator/master/qst2.png)
![](https://raw.githubusercontent.com/mguludag/QSetupCreator/master/qst3.png)


## Usage
- First you have to download and install Qt Installer Framework or if you have this goto step 2
- Select Qt Installer framework path *(example: C:/Qt/Tools/QtInstallerFramework/4.0/bin)*
- Prepare your deploy folder
  - Run `windeployqt` for copy neccessary dlls and some other libraries into deploy folder.
  - Copy icons and any assets into deploy folder if you want
- Set title for installation window
- Set app version
- Set publisher name
- Set installer name
- If you prepared banner image for setup window, enter banner image filename
- If you prepared setup icon, enter filename
- Choose setup style (for use banner image select Modern style)
- Set shortcut name
- Set target executable name for shortcut
- Set icon filename or target exe filename for icon
- If you want you can add any redist packages and silent switches into Redist Setup Packages section
  - Write each package per line and seperate package name and arguments by comma 
  > **Example:**
  > ```
  > vc_redist.x86.exe,/install,/quiet
  > vc_redist.x64.exe,/install,/quiet
  > ```
- Hit create button then select output folder and wait for finished.
