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

#===========================================================================
#CHANGE THESE TO YOUR LIBRARY DIRECTORES IF YOU ARE NOT USING ENV. VARIABLES
ASSIMPDIR = $$(ASSIMP_ROOT)
BOOSTDIR = $$(BOOST_ROOT)
BOUNCERDIR = $$(REPOBOUNCER_ROOT)
GLCLIBDIR = $$(GLC_ROOT)
MONGODIR = $$(MONGO_ROOT)

#EDIT THESE IF YOU ARE A WINDOWS USER
BOOST_VERS = 1_58
COMPILER = vc120

#========================== QT Configuration==================================
QT += core gui opengl openglextensions webkitwidgets network svg #gui-private
unix:!macx:QT += x11extras

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = 3drepogui
TEMPLATE = app
CONFIG += ordered warn_off c++11

#================================ BOOST =====================================
!isEmpty(BOOSTDIR) {
        BOOST_INC_DIR = $${BOOSTDIR}/
        BOOST_LIB_DIR = $$(BOOST_LIBRARYDIR)
	isEmpty(BOOST_LIB_DIR){
                BOOST_LIB_DIR = $${BOOSTDIR}/include
	}
} else {
	error(Cannot find BOOST library. Please ensure the environment variables BOOST_ROOT and BOOST_LIBARYDIR is set.)
}

INCLUDEPATH += $${BOOST_INC_DIR}
DEPENDPATH += $${BOOST_INC_DIR}

win32:BOOSTLIB = -lboost_system-$$COMPILER-mt-$$BOOST_VERS -lboost_thread-$$COMPILER-mt-$$BOOST_VERS -lboost_chrono-$$COMPILER-mt-$$BOOST_VERS -lboost_log-$$COMPILER-mt-$$BOOST_VERS -lboost_log_setup-$$COMPILER-mt-$$BOOST_VERS -lboost_filesystem-$$COMPILER-mt-$$BOOST_VERS

unix|macx:BOOSTLIB = -lboost_system -lboost_thread -lboost_chrono -lboost_log -lboost_log_setup -lboost_filesystem


unix|win32: LIBS +=  -L$${BOOST_LIB_DIR}   $${BOOSTLIB}



#========================== 3D Repobouncer ==================================
!isEmpty(BOUNCERDIR) {
        BOUNCER_LIB_DIR = $${BOUNCERDIR}/lib/
        BOUNCER_INC_DIR = $${BOUNCERDIR}/src
} else {
	error(Cannot find 3drepobouncer installation. Please ensure the environment variable REPOBOUNCER_ROOT is set)
}

BOUNCERLIB = -l3drepobouncer

INCLUDEPATH += $${BOUNCER_INC_DIR}

LIBS += -L$${BOUNCER_LIB_DIR} $${BOUNCERLIB}

!isEmpty(ASSIMPDIR) {
        ASSIMP_LIB_DIR = $${ASSIMPDIR}/lib/
        ASSIMP_INC_DIR = $${ASSIMPDIR}/include/
} else {
	error(Cannot find Assimp installation. Please ensure the environment variable ASSIMP_ROOT is set)

}

ASSIMPLIB = -lassimp

INCLUDEPATH += $${ASSIMP_INC_DIR}

LIBS += -L$${ASSIMP_LIB_DIR} $${ASSIMPLIB}

!isEmpty(MONGODIR) {
        MONGO_LIB_DIR = $${MONGODIR}/lib
        MONGO_INC_DIR = $${MONGODIR}/include/
} else {
	error(Cannot find Mongo installation. Please ensure the environment variable MONGO_ROOT is set)
}

MONGOLIB = -lmongoclient

INCLUDEPATH += $${MONGO_INC_DIR}

LIBS += -L$${MONGO_LIB_DIR} $${MONGOLIB}



#============================== GLC LIB =======================================
!isEmpty(GLCLIBDIR) {
        GLC_INC_DIR = $${GLCLIBDIR}/include
        GLC_LIB_DIR = $${GLCLIBDIR}/lib
} else {
	error(Cannot find GLC library. Please ensure the environment variable GLC_ROOT is set.)
}

#GLCLIB = -lGLC_lib


INCLUDEPATH += $${GLC_INC_DIR}

LIBS += -L$${GLC_LIB_DIR} $${GLCLIB}



win32:DEFINES += _WINDOWS UNICODE WIN32 WIN64 WIN32_LEAN_AND_MEAN _SCL_SECURE_NO_WARNINGS

DEFINES += BOOST_LOG_DYN_LINK BOOST_ALL_NO_LIB


unix|win32: LIBS += -L$$PWD/../../../../../../local/3drepobouncer/lib/ -l3drepobouncer

INCLUDEPATH += $$PWD/../../../../../../local/3drepobouncer/include
DEPENDPATH += $$PWD/../../../../../../local/3drepobouncer/include

unix|win32: LIBS += -L$$PWD/../../../../../../local/libGLC/lib/ -lGLC_lib2

INCLUDEPATH += $$PWD/../../../../../../local/libGLC/include
DEPENDPATH += $$PWD/../../../../../../local/libGLC/include


unix|win32: LIBS += -L$$PWD/../../../../../../local/mongo-cxx-driver/lib/ -lmongoclient

INCLUDEPATH += $$PWD/../../../../../../local/mongo-cxx-driver/include
DEPENDPATH += $$PWD/../../../../../../local/mongo-cxx-driver/include


unix|win32: LIBS += -L$$PWD/../../../../../../local/assimp_head/lib/ -lassimp-vc120-mtd

INCLUDEPATH += $$PWD/../../../../../../local/assimp_head/include
DEPENDPATH += $$PWD/../../../../../../local/assimp_head/include

include(3drepogui.pri)
