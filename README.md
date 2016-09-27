3D Repo GUI [![Build Status](https://travis-ci.org/3drepo/3drepogui.svg?branch=master)](https://travis-ci.org/3drepo/3drepogui)
=========

Cross-platform C++ GUI written in Qt. 

![screenshot](https://cloud.githubusercontent.com/assets/11945337/14990176/3d866e22-1153-11e6-9128-575bfd89ef8a.png)

### Latest Releases
We always recommend using the [Latest stable release](https://github.com/3drepo/3drepogui/releases). However, to access cutting-edge development versions, check out the [tags](https://github.com/3drepo/3drepogui/tags).

## Licenses
This project is Copyright of [3D Repo Ltd](http://3drepo.org), a company registered in England and Wales No. 09014101, and is released under the open source [GNU Affero General Public License v3](http://www.gnu.org/licenses/agpl-3.0.en.html). Should you require a commercial license, please contact [support@3drepo.org](mailto:support@3drepo.org). All contributors are required to sign either the [3D Repo Individual](https://gist.github.com/jozefdobos/e177af804c9bcd217b73) or the [3D Repo Entity](https://gist.github.com/jozefdobos/c7c4c1c18cfb211c45d2) [Contributor License Agreement (CLA)](https://en.wikipedia.org/wiki/Contributor_License_Agreement).

### Contributing
We very much encourage contributions to the 3D Repo project. Firstly, fork the desired repository and commit your modifications there. Once happy with the changes, you can generate a [pull request](https://help.github.com/articles/using-pull-requests/) and our team will integrate it upstream after a review.

Your pull requests should:

1. Follow the style of the existing code
2. One commit should just do one thing, and one thing only
3. Work in a branch assigned to a specific issue number, e.g. branch called "ISSUE_138"
4. Each commit message should be prefixed with the issue number, e.g. "#138 Fixing bug xyz..."
5. Rebase your branch against [upstream's master](https://help.github.com/articles/merging-an-upstream-repository-into-your-fork/) so that we don't pull redundant commits
6. Sign our [3D Repo Individual CLA](https://gist.github.com/jozefdobos/e177af804c9bcd217b73) or if you are representing a legal entity, sign the [3D Repo Entity CLA](https://gist.github.com/jozefdobos/c7c4c1c18cfb211c45d2)

## Dependencies

- [3D Repo Bouncer master branch](https://github.com/3drepo/3drepobouncer)
- [GLC Lib v3.0](https://github.com/3drepo/GLC_lib/tree/Branch_Version_3_0_0) See our [compilation instructions](https://github.com/3drepo/3drepogui/wiki/Compiling-GLC-Lib)
- [Font Awesome v4.3.0](https://github.com/FortAwesome/Font-Awesome/tree/v4.3.0)
- [Qt v5.5 OpenGL](https://www.qt.io/download/)
- [Python v2.x] (https://www.python.org/)
- [Boost v1.58] (http://www.boost.org/)

## General Compilation Instruction

This project uses qmake as the compilation procedure. The easiest way to build this will be to open the qt project using QT Creator and operate on the GUI interface. For more information about qmake, see [http://doc.qt.io/qt-4.8/qmake-using.html]

External libraries are referenced via environment variables. Please ensure the followings are set to the correct directories:
 - ASSIMP_ROOT
 - BOOST_ROOT
 - REPOBOUNCER_ROOT
 - GLC_ROOT
 - MONGO_ROOT

Libraries are referenced within 3drepogui.pro, you may wish to view and alter this where appropriate.

Before compilation, make sure 3drepogui.pri file is up to date. This can be done by running the provided python script `python updateSources.py`

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

### OpenSSL

Download pre-compiled OpenSSL binaries from [https://www.openssl.org/related/binaries.html](https://www.openssl.org/related/binaries.html). Either add them to the Path variable or copy the DLLs next to the 3D Repo executable.

## Compiling on Mac

These instructions are for OSX Yosemite.

### Qt

Download and install Qt with an explicit OpenGL support.

Navigate to where you installed Qt using finder.
Go to the subdirectory 5.5/clang_64/mkspecs directory.
Open the file called qdevice.pri with a text editor
Change the line !host_build:QMAKE_MAC_SDK = macosx10.8 to !host_build:QMAKE_MAC_SDK = macosx10.9
Save the file and restart Qt Creator.

### GLC Lib

On Mac you will need to compile GLC Lib separately. To do so, navigate to the submodule directly, and run the following command:

```
qmake -recursive
make -j8
```
