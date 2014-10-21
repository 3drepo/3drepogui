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

include(3drepocore.pri)
include(glclib.pri)

#-------------------------------------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3drepogui
TEMPLATE = app
VERSION = 0.0.1

#-------------------------------------------------------------------------------

HEADERS  += src/repogui.h \
            src/primitives/repo_fontawesome.h \
            src/dialogs/repo_dialogconnect.h \
            src/widgets/repo_widgetrepository.h \
            src/workers/repo_workerdatabases.h \
            src/workers/repo_worker_abstract.h \
            src/workers/repo_workercollection.h \
            src/primitives/repo_sortfilterproxymodel.h \

SOURCES += src/main.cpp\
           src/repogui.cpp \
           src/primitives/repo_fontawesome.cpp \
           src/dialogs/repo_dialogconnect.cpp \
           src/widgets/repo_widgetrepository.cpp \
           src/workers/repo_workerdatabases.cpp \
           src/workers/repo_worker_abstract.cpp \
           src/workers/repo_workercollection.cpp \
           src/primitives/repo_sortfilterproxymodel.cpp \

FORMS    += forms/repogui.ui \
            forms/repo_dialogconnect.ui \
            forms/repo_widgetrepository.ui \

RESOURCES += submodules/fonts.qrc

