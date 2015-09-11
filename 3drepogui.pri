#THIS IS AN AUTOMATICALLY GENERATED FILE - DO NOT OVERWRITE THE CONTENT!
#If you need to update the sources/headers/forms, run updateSources.py at project root level
#If you need to import an extra library or something clever, do it on the 3drepogui.pron
#If you really need to overwrite this file, be aware that it will be overwritten if updateSources.py is executed.


HEADERS +=  \
	src/repogui.h \
	src/reposettings.h \
	src/primitives/repo_fontawesome.h \
	src/primitives/repo_sortfilterproxymodel.h \
	src/primitives/repo_color.h \
	src/primitives/repo_glccamera.h \
	src/primitives/repocomboboxeditor.h \
	src/primitives/repocomboboxdelegate.h \
	src/primitives/repoidbcache.h \
	src/repo/logger/repo_subscriber_abstract.h \
	src/repo/logger/repo_logger.h \
	src/repo/logger/repo_stream_redirect.h \
	src/repo/workers/repo_worker_users.h \
	src/repo/workers/repo_worker_scene_graph.h \
	src/repo/workers/repo_worker_abstract.h \
	src/repo/workers/repo_worker_commit.h \
	src/repo/workers/repo_worker_database.h \
	src/repo/workers/repo_worker_collection.h \
	src/repo/workers/repo_worker_project_settings.h \
	src/repo/workers/repo_worker_glc_export.h \
	src/repo/workers/repo_worker_file_export.h \
	src/repo/workers/repo_worker_file_import.h \
	src/repo/workers/repo_worker_history.h \
	src/widgets/repo_mdiarea.h \
	src/widgets/repo_lineedit.h \
	src/widgets/repo_textbrowser.h \
	src/widgets/repowidgetassimpflags.h \
	src/widgets/repo_clickabletreeview.h \
	src/widgets/reposelectiontreedockwidget.h \
	src/widgets/repofilterabletreewidget.h \
	src/widgets/repo_mdisubwindow.h \
	src/widgets/repo_widgetrepository.h \
	src/renderers/repo_3ddiffrenderer.h \
	src/renderers/repo_fpscounter.h \
	src/renderers/repo_webview.h \
	src/renderers/repo_glcwidget.h \
	src/dialogs/repoprojectsettingsdialog.h \
	src/dialogs/repodialoguser.h \
	src/dialogs/repo_transformationdialog.h \
	src/dialogs/repo_dialogcommit.h \
	src/dialogs/repodialogusermanager.h \
	src/dialogs/repodialogabout.h \
	src/dialogs/repo_maptilesdialog.h \
	src/dialogs/repofederationdialog.h \
	src/dialogs/repoabstractmanagerdialog.h \
	src/dialogs/repo_dialogconnect.h \
	src/dialogs/repo_dialoghistory.h \
	src/dialogs/repoprojectmanagerdialog.h \
	src/dialogs/repo_genericdialog.h \
	src/dialogs/repodialogsettings.h

SOURCES +=  \
	src/main.cpp \
	src/repogui.cpp \
	src/reposettings.cpp \
	src/primitives/repo_fontawesome.cpp \
	src/primitives/repoidbcache.cpp \
	src/primitives/repocomboboxdelegate.cpp \
	src/primitives/repocomboboxeditor.cpp \
	src/primitives/repo_glccamera.cpp \
	src/primitives/repo_color.cpp \
	src/primitives/repo_sortfilterproxymodel.cpp \
	src/repo/logger/repo_subscriber_abstract.cpp \
	src/repo/logger/repo_logger.cpp \
	src/repo/logger/repo_stream_redirect.cpp \
	src/repo/workers/repo_worker_users.cpp \
	src/repo/workers/repo_worker_history.cpp \
	src/repo/workers/repo_worker_abstract.cpp \
	src/repo/workers/repo_worker_scene_graph.cpp \
	src/repo/workers/repo_worker_database.cpp \
	src/repo/workers/repo_worker_commit.cpp \
	src/repo/workers/repo_worker_collection.cpp \
	src/repo/workers/repo_worker_file_export.cpp \
	src/repo/workers/repo_worker_glc_export.cpp \
	src/repo/workers/repo_worker_project_settings.cpp \
	src/repo/workers/repo_worker_file_import.cpp \
	src/widgets/repo_clickabletreeview.cpp \
	src/widgets/repo_widgetrepository.cpp \
	src/widgets/reposelectiontreedockwidget.cpp \
	src/widgets/repo_lineedit.cpp \
	src/widgets/repofilterabletreewidget.cpp \
	src/widgets/repowidgetassimpflags.cpp \
	src/widgets/repo_mdiarea.cpp \
	src/widgets/repo_mdisubwindow.cpp \
	src/widgets/repo_textbrowser.cpp \
	src/renderers/repo_glcwidget.cpp \
	src/renderers/repo_webview.cpp \
	src/renderers/repo_fpscounter.cpp \
	src/renderers/repo_3ddiffrenderer.cpp \
	src/dialogs/repo_dialoghistory.cpp \
	src/dialogs/repoprojectmanagerdialog.cpp \
	src/dialogs/repodialoguser.cpp \
	src/dialogs/repo_transformationdialog.cpp \
	src/dialogs/repo_maptilesdialog.cpp \
	src/dialogs/repo_dialogcommit.cpp \
	src/dialogs/repodialogsettings.cpp \
	src/dialogs/repoabstractmanagerdialog.cpp \
	src/dialogs/repo_genericdialog.cpp \
	src/dialogs/repodialogusermanager.cpp \
	src/dialogs/repoprojectsettingsdialog.cpp \
	src/dialogs/repo_dialogconnect.cpp \
	src/dialogs/repofederationdialog.cpp \
	src/dialogs/repodialogabout.cpp

FORMS +=  \
	forms/repo_genericdialog.ui \
	forms/repofilterabletreewidget.ui \
	forms/repo_dialoghistory.ui \
	forms/repogui.ui \
	forms/repo_transformationdialog.ui \
	forms/repo_dialogconnect.ui \
	forms/repo_maptilesdialog.ui \
	forms/repodialoguser.ui \
	forms/repo_widgetrepository.ui \
	forms/repofederationdialog.ui \
	forms/repodialogsettings.ui \
	forms/repodialogabout.ui \
	forms/reposelectiontreedockwidget.ui \
	forms/repoprojectsettingsdialog.ui \
	forms/repo_dialogcommit.ui \
	forms/repoabstractmanagerdialog.ui \
	forms/repo_webview.ui \
	forms/repowidgetassimpflags.ui

RESOURCES +=  \
	./resources.qrc \
	./submodules/fonts.qrc