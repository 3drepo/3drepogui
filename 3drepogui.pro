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


#========================== QT Configuration==================================
QT += core gui opengl openglextensions webkitwidgets network svg #gui-private
unix:!macx:QT += x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3drepogui
TEMPLATE = app
CONFIG += ordered warn_off

#================================ BOOST =====================================
BOOSTDIR = $$(BOOST_ROOT) #change this if you don't want to use envar
!isEmpty(BOOSTDIR) {
        BOOST_INC_DIR = $${BOOSTDIR}/
        BOOST_LIB_DIR = $${BOOST_LIBRARYDIR}
	isEmpty(BOOST_LIB_DIR){
                BOOST_LIB_DIR = $${BOOSTDIR}/include
	}
} else {
	error(Cannot find BOOST library. Please ensure the environment variables BOOST_ROOT and BOOST_LIBARYDIR is set.)
}

BOOSTLIB = -lboost_system -lboost_thread -lboost_chrono -lboost_log -lboost_log_setup -lboost_filesystem

#========================== 3D Repobouncer ==================================
BOUNCERDIR = $$(REPOBOUNCER_ROOT) #change this if you don't want to use envar
!isEmpty(BOUNCERDIR) {
        BOUNCER_LIB_DIR = $${BOUNCERDIR}/lib/
        BOUNCER_INC_DIR = $${BOUNCERDIR}/include
} else {
	error(Cannot find 3drepobouncer installation. Please ensure the environment variable REPOBOUNCER_ROOT is set)
}

BOUNCERLIB = -l3drepobouncer

ASSIMPDIR = $$(ASSIMP_ROOT) #change this if not using envar
!isEmpty(ASSIMPDIR) {
        ASSIMP_LIB_DIR = $${ASSIMPDIR}/lib/
        ASSIMP_INC_DIR = $${ASSIMPDIR}/include/
} else {
	error(Cannot find Assimp installation. Please ensure the environment variable ASSIMP_ROOT is set)

}

ASSIMPLIB = -lassimp

MONGODIR = $$(MONGO_ROOT) #change this if not using envar
!isEmpty(MONGODIR) {
        MONGO_LIB_DIR = $${MONGODIR}/lib
        MONGO_INC_DIR = $${MONGODIR}/include/
} else {
	error(Cannot find Mongo installation. Please ensure the environment variable MONGO_ROOT is set)
}

MONGOLIB = -lmongoclient


#============================== GLC LIB =======================================
GLCLIBDIR = $$(GLC_ROOT) #change this if you don't want ot use envar
!isEmpty(GLCLIBDIR) {
        GLC_INC_DIR = $${GLCLIBDIR}/include
        GLC_LIB_DIR = $${GLCLIBDIR}/lib
} else {
	error(Cannot find GLC library. Please ensure the environment variable GLC_ROOT is set.)
}

GLCLIB = -lGLC_lib

INCLUDEPATH += $${BOOST_INC_DIR} $${BOUNCER_INC_DIR} $${ASSIMP_INC_DIR} $${MONGO_INC_DIR} $${GLC_INC_DIR}
LIBS += -L$${BOOST_LIB_DIR} -L$${BOUNCER_LIB_DIR} -L$${ASSIMP_LIB_DIR} -L$${MONGO_LIB_DIR} -L$${GLC_LIB_DIR}
LIBS += $${BOOSTLIB} $${BOUNCERLIB} $${ASSIMPLIB} $${MONGOLIB} $${GLCLIB}

include(3drepogui.pri)
