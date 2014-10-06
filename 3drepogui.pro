#  Copyright (C) 2014 3D Repo Ltd
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3drepogui
TEMPLATE = app
VERSION = 0.0.1

#-------------------------------------------------------------------------------
# 3D Repo Core
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/submodules/3drepocore/release/ -l3drepocore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/submodules/3drepocore/debug/ -l3drepocore
else:unix: LIBS += -L$$OUT_PWD/submodules/3drepocore/ -l3drepocore

INCLUDEPATH += $$PWD/submodules/3drepocore
DEPENDPATH += $$PWD/submodules/3drepocore

#-------------------------------------------------------------------------------
# GLC Lib
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/submodules/GLC_lib/src/ -lGLC_lib2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/submodules/GLC_lib/src/ -lGLC_lib2
else:unix: LIBS += -L$$OUT_PWD/submodules/GLC_lib/src/ -lGLC_lib

INCLUDEPATH += $$PWD/submodules/GLC_lib/src
DEPENDPATH += $$PWD/submodules/GLC_lib/src

//------------------------------------------------------------------------------


HEADERS  += src/repogui.h \
            src/primitives/repo_fontawesome.h \
            src/dialogs/repo_dialogconnect.h


SOURCES += src/main.cpp\
           src/repogui.cpp \
           src/primitives/repo_fontawesome.cpp \
           src/dialogs/repo_dialogconnect.cpp

FORMS    += forms/repogui.ui \
            forms/repo_dialogconnect.ui

RESOURCES += submodules/fonts.qrc
