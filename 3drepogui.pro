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

SOURCES += src/main.cpp\
        src/repogui.cpp

HEADERS  += src/repogui.h

FORMS    += src/ui/repogui.ui

# 3D Repo Core
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/submodules/3drepocore/release/ -l3drepocore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/submodules/3drepocore/debug/ -l3drepocore
else:unix: LIBS += -L$$OUT_PWD/submodules/3drepocore/ -l3drepocore

INCLUDEPATH += $$PWD/submodules/3drepocore
DEPENDPATH += $$PWD/submodules/3drepocore

# GLC Lib
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/submodules/GLC_lib/src/release/ -lGLC_lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/submodules/GLC_lib/src/debug/ -lGLC_lib
else:unix: LIBS += -L$$OUT_PWD/submodules/GLC_lib/src/ -lGLC_lib

INCLUDEPATH += $$PWD/submodules/GLC_lib/src
DEPENDPATH += $$PWD/submodules/GLC_lib/src
