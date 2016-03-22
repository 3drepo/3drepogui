#  Copyright (C) 2015 3D Repo Ltd
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

#===============================================================================
# Modify contents of the dependencies.pri file to correctly point to your local
# paths should your environmental variables not be set correctly.
#===============================================================================
include(dependencies.pri)

#=========================== QT Configuration ==================================
QT += core gui opengl openglextensions webkitwidgets network svg #gui-private
unix:!macx:QT += x11extras

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

# Qt5.5 support
win32:LIBS += -lopengl32

TARGET = 3drepogui
TEMPLATE = app
CONFIG += ordered warn_off c++11

unix|macx:QMAKE_CXXFLAGS += -fpermissive
unix|macx:DESTDIR = ./build
unix|macx:INCLUDEPATH += ./src
unix|macx:OBJECTS_DIR = ./build
unix|macx:UI_DIR = ./src/ui
unix|macx:MOC_DIR = ./moc/ui

#================================== BOOST ======================================
!isEmpty(BOOSTDIR) {
    BOOST_INC_DIR = $${BOOSTDIR}/
    isEmpty(BOOST_LIB_DIR){
            BOOST_LIB_DIR = $${BOOSTDIR}/include
    }

    win32:CONFIG(release, debug|release):BOOSTLIB = -lboost_system-$$COMPILER-mt-$$BOOST_VERS \
                     -lboost_thread-$$COMPILER-mt-$$BOOST_VERS \
                     -lboost_chrono-$$COMPILER-mt-$$BOOST_VERS \
                     -lboost_log-$$COMPILER-mt-$$BOOST_VERS \
                     -lboost_log_setup-$$COMPILER-mt-$$BOOST_VERS \
                     -lboost_filesystem-$$COMPILER-mt-$$BOOST_VERS

    win32:CONFIG(debug, debug|release):BOOSTLIB = -lboost_system-$$COMPILER-mt-gd-$$BOOST_VERS \
                     -lboost_thread-$$COMPILER-mt-gd-$$BOOST_VERS \
                     -lboost_chrono-$$COMPILER-mt-gd-$$BOOST_VERS \
                     -lboost_log-$$COMPILER-mt-gd-$$BOOST_VERS \
                     -lboost_log_setup-$$COMPILER-mt-gd-$$BOOST_VERS \
                     -lboost_filesystem-$$COMPILER-mt-gd-$$BOOST_VERS

    unix:BOOSTLIB = -lboost_system -lboost_thread -lboost_chrono -lboost_log -lboost_log_setup -lboost_filesystem
    macx:BOOSTLIB = -lboost_thread-mt -lboost_system -lboost_chrono -lboost_log-mt -lboost_log_setup -lboost_filesystem

    macx|unix|win32: LIBS += -L$${BOOST_LIB_DIR} $${BOOSTLIB}

    INCLUDEPATH += $${BOOST_INC_DIR}
    DEPENDPATH += $${BOOST_INC_DIR}

} else {
    error(Cannot find BOOST library. Please ensure the environment variables BOOST_ROOT and BOOST_LIBARYDIR is set.)
}


#============================= 3D Repobouncer ==================================
!isEmpty(BOUNCERDIR) {
    BOUNCER_LIB_DIR = $${BOUNCERDIR}/lib/
    BOUNCER_INC_DIR = $${BOUNCERDIR}/include


    #win32:CONFIG(release, debug|release):BOUNCERLIB = -l3drepobouncer_$${BOUNCER_VERS}
    #else:win32:CONFIG(debug, debug|release):BOUNCERLIB = -l3drepobouncer_$${BOUNCER_VERS}_d

    #The libraries should have the same postfixes regardless of platforms, change if it\'s not the case
    CONFIG(release, debug|release):BOUNCERLIB = -l3drepobouncer_$${BOUNCER_VERS}
    else:CONFIG(debug, debug|release):BOUNCERLIB = -l3drepobouncer_$${BOUNCER_VERS}_d

    macx:BOUNCERLIB = -l3drepobouncer
    LIBS += -L$${BOUNCER_LIB_DIR} $${BOUNCERLIB}

    INCLUDEPATH += $${BOUNCER_INC_DIR}
    DEPENDPATH += $${BOUNCER_INC_DIR}
} else {
    error(Cannot find 3drepobouncer installation. Please ensure the environment variable REPOBOUNCER_ROOT is set)
}


#================================ ASSIMP =======================================
!isEmpty(ASSIMPDIR) {
    ASSIMP_LIB_DIR = $${ASSIMPDIR}/lib/
    ASSIMP_INC_DIR = $${ASSIMPDIR}/include/

    win32:CONFIG(release, debug|release):ASSIMPLIB = -lassimp-$$COMPILER-mt
    else:win32:CONFIG(debug, debug|release):ASSIMPLIB = -lassimp-$$COMPILER-mtd
    else:unix|macx:ASSIMPLIB = -lassimp
    LIBS += -L$${ASSIMP_LIB_DIR} $${ASSIMPLIB}

    INCLUDEPATH += $${ASSIMP_INC_DIR}
    DEPENDPATH += $${ASSIMP_INC_DIR}

} else {
    error(Cannot find Assimp installation. Please ensure the environment variable ASSIMP_ROOT is set)
}


#============================= MONGO CXX DRIVER ================================

!isEmpty(MONGODIR) {
    MONGO_LIB_DIR = $${MONGODIR}/lib
    MONGO_INC_DIR = $${MONGODIR}/include/

    win32:CONFIG(release, debug|release):MONGOLIB = -lmongoclient
    else:win32:CONFIG(debug, debug|release):MONGOLIB = -lmongoclient-gd
    else:unix|macx:MONGOLIB = -lmongoclient
    LIBS += -L$${MONGO_LIB_DIR} $${MONGOLIB}

    INCLUDEPATH += $${MONGO_INC_DIR}
    DEPENDPATH += $${MONGO_INC_DIR}
} else {
    error(Cannot find Mongo installation. Please ensure the environment variable MONGO_ROOT is set)
}


#=============================== GLC LIB =======================================
!isEmpty(GLCLIBDIR) {
    win32:GLC_INC_DIR = $${GLCLIBDIR}/include
    unix|macx:GLC_INC_DIR = $${GLCLIBDIR}/include/GLC_lib-3.0

    win32:CONFIG(release, debug|release):GLC_LIB_DIR = $${GLCLIBDIR}/lib/Release/
    else:win32:CONFIG(debug, debug|release):GLC_LIB_DIR = $${GLCLIBDIR}/lib/Debug/
    else:unix|macx:GLC_LIB_DIR = $${GLCLIBDIR}/lib


    win32:CONFIG(release, debug|release):GLCLIB = -lGLC_lib3
    else:win32:CONFIG(debug, debug|release):GLCLIB = -lGLC_lib3
    else:unix|macx:GLCLIB = -lGLC_lib

    LIBS += -L$${GLC_LIB_DIR} $${GLCLIB}

    INCLUDEPATH += $${GLC_INC_DIR}
    DEPENDPATH += $${GLC_INC_DIR}

} else {
    error(Cannot find GLC library. Please ensure the environment variable GLC_ROOT is set.)
}


#===============================================================================

win32:DEFINES += _WINDOWS UNICODE WIN32 WIN64 WIN32_LEAN_AND_MEAN _SCL_SECURE_NO_WARNINGS

DEFINES += BOOST_LOG_DYN_LINK BOOST_ALL_NO_LIB

include(3drepogui.pri)
