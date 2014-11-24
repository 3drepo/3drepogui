3D Repo GUI
=========

Cross-platform C++ GUI written in Qt.

## Dependencies

- [3D Repo Core master branch](https://github.com/3drepo/3drepocore)
- [GLC Lib v2.5.1](https://github.com/laumaya/GLC_lib/tree/Version_2_5_1)
- [Font Awesome v4.2.0](https://github.com/FortAwesome/Font-Awesome/tree/v4.2.0)
- [Qt v5.3](https://qt.gitorious.org/qt)
- [Oculus SDK 0.4.3](https://developer.oculus.com/downloads/)

## Compiling on Windows

### Qt

Download and install Qt with an explicit OpenGL support.
For Windows, download also [WinDbg](http://msdn.microsoft.com/en-us/windows/hardware/hh852365), see [http://qt-project.org/doc/qtcreator-2.6/creator-debugger-engines.html](http://qt-project.org/doc/qtcreator-2.6/creator-debugger-engines.html).

### ICU

Download pre-compiled ICU libraries for MSVC 2010, 2012 and 2013 directly from Qt: [http://download.qt-project.org/development_releases/prebuilt/icu/prebuilt/](http://download.qt-project.org/development_releases/prebuilt/icu/prebuilt/)
See also [http://qt-project.org/wiki/Compiling-ICU-with-MSVC](http://qt-project.org/wiki/Compiling-ICU-with-MSVC)

### Oculus

Download and instal Oculus Runtime for Windows. Download Oculus SDK and copy precompiled Lib folder under submodules/LibOVR. Modify oculus.pri in the main folder to point to the library matching your compiler.
