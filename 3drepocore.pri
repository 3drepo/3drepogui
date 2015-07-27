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

#include(submodules/3drepocore/header.pri)
#include(submodules/3drepocore/boost.pri)
#include(submodules/3drepocore/assimp.pri)
#include(submodules/3drepocore/mongo.pri)

#-------------------------------------------------------------------------------
# 3D Repo Core

win32:CONFIG(release, debug|release):
{
LIBS += -L"C:/Users/Carmen/3D Repo/Repo/3drepobouncer/x64/Debug" -l3drepobouncer
INCLUDEPATH += "C:/Users/Carmen/3D Repo/Repo/3drepobouncer/src"

LIBS += -L"C:/local/boost_1_58_0/lib64-msvc-12.0" -l3drepobouncer
INCLUDEPATH += C:/local/boost_1_58_0"
win32:DEPENDPATH += C:/local/boost_1_56_0/

}
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Users/Carmen/3D Repo/Repo/3drepobouncer/x64/Debug" -l3drepobouncer
else:unix {
LIBS += -L$$OUT_PWD/submodules/3drepocore/ -l3drepocore -lboost_system -lmongoclient -lGLC_lib
INCLUDEPATH += $$PWD/src/widgets
}
INCLUDEPATH += "C:/Users/Carmen/3D Repo/Repo/3drepobouncer/src"



