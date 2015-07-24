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

#-------------------------------------------------------------------------------
# Oculus Rift LibOVR
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/LibOVR/Lib/x64/VS2013/ -lLibOVR
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/LibOVR/Lib/x64/VS2013/ -llibovr64d
else:unix:!macx: LIBS += -L$$PWD/submodules/LibOVR/Lib -lXrandr -lovr
else:macx:CONFIG(release, debug|release) LIBS += -L$$PWD/submodules/LibOVR/Lib/Mac/Release -lovr -framework IOKit -framework Cocoa
else:macx:CONFIG(debug, debug|release) LIBS += -L$$PWD/submodules/LibOVR/Lib/Mac/Debug -lovr -framework IOKit -framework Cocoa

INCLUDEPATH += $$PWD/submodules/LibOVR/Src \
               $$PWD/submodules/LibOVR/Include

DEPENDPATH += $$PWD/submodules/LibOVR/Src \
              $$PWD/submodules/LibOVR/Include

#-------------------------------------------------------------------------------
# Oculus Windows Dependencies
win32:LIBS += -lwinmm \
              -lshell32

win32:DEFINES += OVR_OS_WIN32



