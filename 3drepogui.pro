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
include(oculus.pri)

#-------------------------------------------------------------------------------

QT += core gui opengl openglextensions webkitwidgets network svg #gui-private
unix:!macx:QT += x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3drepogui
TEMPLATE = app

INCLUDEPATH += src

#-------------------------------------------------------------------------------

HEADERS  += src/repogui.h \
            src/primitives/repo_fontawesome.h \
            src/dialogs/repo_dialogconnect.h \
            src/dialogs/repo_dialogcommit.h \
            src/dialogs/repo_dialoghistory.h \
            src/widgets/repo_widgetrepository.h \
            src/widgets/repo_lineedit.h \
            src/widgets/repo_textbrowser.h \
            src/widgets/repo_mdiarea.h \
            src/widgets/repo_mdisubwindow.h \
            src/renderers/repo_glcwidget.h \
            src/workers/repo_workerdatabases.h \
            src/workers/repo_worker_abstract.h \
            src/workers/repo_workercollection.h \
            src/workers/repo_worker_assimp.h \
            src/workers/repo_workercommit.h \
            src/workers/repo_workerfetchrevision.h \
            src/workers/repo_workerhistory.h \
            src/workers/repo_workerusers.h \
            src/primitives/repo_sortfilterproxymodel.h \
            src/primitives/repo_glccamera.h \
            src/conversion/repo_transcoder_assimp.h \
            src/conversion/repo_transcoder_graph_scene.h \
            src/conversion/repo_transcoder_helpers.h \
            src/renderers/repo_oculus.h \
            src/dialogs/repodialogoculus.h \
            src/dialogs/repodialogusermanager.h \
            src/dialogs/repodialoguser.h \
            src/primitives/repocomboboxeditor.h \
            src/primitives/repocomboboxdelegate.h \
            src/dialogs/repodialogsettings.h \
            src/widgets/repowidgetassimpflags.h \
            src/reposettings.h \
            src/widgets/repofilterabletreewidget.h \
            src/widgets/reposelectiontreedockwidget.h \
            src/primitives/repo_color.h \
            src/renderers/repooculustexturerenderer.h \
            src/renderers/repo_webview.h \
            src/dialogs/repodialogabout.h \
            src/dialogs/repoabstractmanagerdialog.h \
            src/dialogs/repoprojectmanagerdialog.h

SOURCES += src/main.cpp\
           src/repogui.cpp \
           src/primitives/repo_fontawesome.cpp \
           src/dialogs/repo_dialogconnect.cpp \
           src/dialogs/repo_dialogcommit.cpp \
           src/dialogs/repo_dialoghistory.cpp \
           src/widgets/repo_widgetrepository.cpp \
           src/widgets/repo_lineedit.cpp \
           src/widgets/repo_textbrowser.cpp \
           src/widgets/repo_mdiarea.cpp \
           src/widgets/repo_mdisubwindow.cpp \
           src/renderers/repo_glcwidget.cpp \
           src/workers/repo_workerdatabases.cpp \
           src/workers/repo_worker_abstract.cpp \
           src/workers/repo_workercommit.cpp \
           src/workers/repo_workercollection.cpp \
           src/workers/repo_worker_assimp.cpp \
           src/workers/repo_workerfetchrevision.cpp \
           src/workers/repo_workerhistory.cpp \
           src/workers/repo_workerusers.cpp \
           src/primitives/repo_sortfilterproxymodel.cpp \
           src/primitives/repo_glccamera.cpp \
           src/conversion/repo_transcoder_assimp.cpp \
           src/conversion/repo_transcoder_graph_scene.cpp \
           src/conversion/repo_transcoder_helpers.cpp \
           src/renderers/repo_oculus.cpp \
           src/dialogs/repodialogoculus.cpp \
           src/dialogs/repodialogusermanager.cpp \
           src/dialogs/repodialoguser.cpp \
           src/primitives/repocomboboxeditor.cpp \
           src/primitives/repocomboboxdelegate.cpp \
           src/dialogs/repodialogsettings.cpp \
           src/widgets/repowidgetassimpflags.cpp \
           src/reposettings.cpp \
           src/widgets/repofilterabletreewidget.cpp \
           src/widgets/reposelectiontreedockwidget.cpp \
            src/primitives/repo_color.cpp \
            src/renderers/repooculustexturerenderer.cpp \
            src/renderers/repo_webview.cpp \
            src/dialogs/repodialogabout.cpp \
            src/dialogs/repoabstractmanagerdialog.cpp \
            src/dialogs/repoprojectmanagerdialog.cpp

FORMS    += forms/repogui.ui \
            forms/repo_dialogconnect.ui \
            forms/repo_dialogcommit.ui \
            forms/repo_dialoghistory.ui \
            forms/repo_widgetrepository.ui \
            forms/repodialogoculus.ui \
            forms/repodialoguser.ui \
            forms/repodialogsettings.ui \
            forms/repowidgetassimpflags.ui \
            forms/repofilterabletreewidget.ui \
            forms/reposelectiontreedockwidget.ui \
            forms/repo_webview.ui \
            forms/repodialogabout.ui \
            forms/repoabstractmanagerdialog.ui


# http://qt-project.org/doc/qt-5/resources.html
RESOURCES += resources.qrc \
             submodules/fonts.qrc \


