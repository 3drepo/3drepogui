3D Repo GUI
=========

Cross-platform C++ GUI written in Qt.

## License
This project is Copyright of [3D Repo Ltd](http://3drepo.org) and is released under the open source [GNU Affero General Public License v3](http://www.gnu.org/licenses/agpl-3.0.en.html). Should you require a commercial license, please contact [support@3drepo.org](mailto:support@3drepo.org)

## Dependencies

- [3D Repo Core master branch](https://github.com/3drepo/3drepocore)
- [GLC Lib v2.5.1](https://github.com/laumaya/GLC_lib/tree/Version_2_5_1)
- [Font Awesome v4.3.0](https://github.com/FortAwesome/Font-Awesome/tree/v4.3.0)
- [Qt v5.4.1 OpenGL](https://www.qt.io/download-open-source/#section-2)
- [Oculus SDK 0.4.3](https://developer.oculus.com/downloads/)
- [UnrealEngine 4.7.2](https://github.com/3drepo/UnrealEngine)

## Compiling on Windows

### Qt

Download and install Qt with an explicit OpenGL support.
For Windows, download also [WinDbg](http://msdn.microsoft.com/en-us/windows/hardware/hh852365), see [http://qt-project.org/doc/qtcreator-2.6/creator-debugger-engines.html](http://qt-project.org/doc/qtcreator-2.6/creator-debugger-engines.html).


QtSSL
Download the openSSL libraries for the integrated web browser
http://gnuwin32.sourceforge.net/packages/openssl.htm


### ICU

Download pre-compiled ICU libraries for MSVC 2010, 2012 and 2013 directly from Qt: [http://download.qt-project.org/development_releases/prebuilt/icu/prebuilt/](http://download.qt-project.org/development_releases/prebuilt/icu/prebuilt/)
See also [http://qt-project.org/wiki/Compiling-ICU-with-MSVC](http://qt-project.org/wiki/Compiling-ICU-with-MSVC)

### Oculus

Download and install Oculus Runtime for Windows. Download Oculus SDK and copy precompiled Lib folder under submodules/LibOVR. Modify oculus.pri in the main folder to point to the library matching your compiler.

### OpenSSL

Download pre-compiled OpenSSL binaries from [https://www.openssl.org/related/binaries.html](https://www.openssl.org/related/binaries.html). Either add them to the Path variable or copy the DLLs next to the 3D Repo executable.

### UnrealEngine

Unsupported yet: Register for an Unreal account and follow these instructions to link the account with Epic Games' GitHub private repository: [https://www.unrealengine.com/ue4-on-github](https://www.unrealengine.com/ue4-on-github)
Once forked, follow the installation instructions (the page is inaccessible unless you're registered and linked): [https://github.com/EpicGames/UnrealEngine](https://github.com/EpicGames/UnrealEngine) 

## Compiling on Mac

These instructions are for OSX Yosemite.

### Qt

Download and install Qt with an explicit OpenGL support.

Navigate to where you installed Qt using finder.
Go to the subdirectory 5.3/clang_64/mkspecs directory.
Open the file called qdevice.pri with a text editor
Change the line !host_build:QMAKE_MAC_SDK = macosx10.8 to !host_build:QMAKE_MAC_SDK = macosx10.9
Save the file and restart Qt Creator.

### GLC Lib

On Mac you will need to compile GLC Lib separately. To do so, navigate to the submodule directly, and run the following command:

```
qmake -recursive
make -j8
```

### Oculus

Download and install Oculus Runtime for Mac. Download Oculus SDK and copy precompiled Lib folder under submodules/LibOVR. Modify oculus.pri in the main folder to point to the library matching your compiler.
