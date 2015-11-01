#THIS IS AN AUTOMATICALLY GENERATED FILE - DO NOT OVERWRITE THE CONTENT!
#If you need to update the sources/headers/forms, run updateSources.py at project root level
#If you need to import an extra library or something clever, do it on the 3drepogui.pron
#If you really need to overwrite this file, be aware that it will be overwritten if updateSources.py is executed.


HEADERS +=  \
	src/repogui.h \
	src/reposettings.h \
	src/dialogs/repoabstractmanagerdialog.h \
	src/dialogs/repodialogabout.h \
	src/dialogs/repodialogsettings.h \
	src/dialogs/repodialoguser.h \
	src/dialogs/repodialogusermanager.h \
	src/dialogs/repofederationdialog.h \
	src/dialogs/repoprojectmanagerdialog.h \
	src/dialogs/repoprojectsettingsdialog.h \
	src/dialogs/repo_dialogcommit.h \
	src/dialogs/repo_dialogconnect.h \
	src/dialogs/repo_dialoghistory.h \
	src/dialogs/repo_dialog_manager_connect.h \
	src/dialogs/repo_genericdialog.h \
	src/dialogs/repo_maptilesdialog.h \
	src/dialogs/repo_transformationdialog.h \
	src/primitives/repocomboboxdelegate.h \
	src/primitives/repocomboboxeditor.h \
	src/primitives/repoidbcache.h \
	src/primitives/repo_color.h \
	src/primitives/repo_fontawesome.h \
	src/primitives/repo_glccamera.h \
	src/primitives/repo_sortfilterproxymodel.h \
	src/renderers/repo_3ddiffrenderer.h \
	src/renderers/repo_fpscounter.h \
	src/renderers/repo_glcwidget.h \
	src/renderers/repo_webview.h \
	src/repo/logger/repo_logger.h \
	src/repo/logger/repo_stream_redirect.h \
	src/repo/logger/repo_subscriber_abstract.h \
	src/repo/settings/repo_settings_credentials.h \
	src/repo/workers/repo_worker_abstract.h \
	src/repo/workers/repo_worker_collection.h \
	src/repo/workers/repo_worker_commit.h \
	src/repo/workers/repo_worker_database.h \
	src/repo/workers/repo_worker_file_export.h \
	src/repo/workers/repo_worker_file_import.h \
	src/repo/workers/repo_worker_glc_export.h \
	src/repo/workers/repo_worker_history.h \
	src/repo/workers/repo_worker_modified_nodes.h \
	src/repo/workers/repo_worker_project_settings.h \
	src/repo/workers/repo_worker_scene_graph.h \
	src/repo/workers/repo_worker_users.h \
	src/widgets/repofilterabletreewidget.h \
	src/widgets/reposelectiontreedockwidget.h \
	src/widgets/repowidgetassimpflags.h \
	src/widgets/repo_clickabletreeview.h \
	src/widgets/repo_lineedit.h \
	src/widgets/repo_mdiarea.h \
	src/widgets/repo_mdisubwindow.h \
	src/widgets/repo_textbrowser.h \
	src/widgets/repo_widgetrepository.h

SOURCES +=  \
	src/main.cpp \
	src/repogui.cpp \
	src/reposettings.cpp \
	src/dialogs/repoabstractmanagerdialog.cpp \
	src/dialogs/repodialogabout.cpp \
	src/dialogs/repodialogsettings.cpp \
	src/dialogs/repodialoguser.cpp \
	src/dialogs/repodialogusermanager.cpp \
	src/dialogs/repofederationdialog.cpp \
	src/dialogs/repoprojectmanagerdialog.cpp \
	src/dialogs/repoprojectsettingsdialog.cpp \
	src/dialogs/repo_dialogcommit.cpp \
	src/dialogs/repo_dialogconnect.cpp \
	src/dialogs/repo_dialoghistory.cpp \
	src/dialogs/repo_dialog_manager_connect.cpp \
	src/dialogs/repo_genericdialog.cpp \
	src/dialogs/repo_maptilesdialog.cpp \
	src/dialogs/repo_transformationdialog.cpp \
	src/primitives/repocomboboxdelegate.cpp \
	src/primitives/repocomboboxeditor.cpp \
	src/primitives/repoidbcache.cpp \
	src/primitives/repo_color.cpp \
	src/primitives/repo_fontawesome.cpp \
	src/primitives/repo_glccamera.cpp \
	src/primitives/repo_sortfilterproxymodel.cpp \
	src/renderers/repo_3ddiffrenderer.cpp \
	src/renderers/repo_fpscounter.cpp \
	src/renderers/repo_glcwidget.cpp \
	src/renderers/repo_webview.cpp \
	src/repo/logger/repo_logger.cpp \
	src/repo/logger/repo_stream_redirect.cpp \
	src/repo/logger/repo_subscriber_abstract.cpp \
	src/repo/settings/repo_settings_credentials.cpp \
	src/repo/workers/repo_worker_abstract.cpp \
	src/repo/workers/repo_worker_collection.cpp \
	src/repo/workers/repo_worker_commit.cpp \
	src/repo/workers/repo_worker_database.cpp \
	src/repo/workers/repo_worker_file_export.cpp \
	src/repo/workers/repo_worker_file_import.cpp \
	src/repo/workers/repo_worker_glc_export.cpp \
	src/repo/workers/repo_worker_history.cpp \
	src/repo/workers/repo_worker_modified_nodes.cpp \
	src/repo/workers/repo_worker_project_settings.cpp \
	src/repo/workers/repo_worker_scene_graph.cpp \
	src/repo/workers/repo_worker_users.cpp \
	src/widgets/repofilterabletreewidget.cpp \
	src/widgets/reposelectiontreedockwidget.cpp \
	src/widgets/repowidgetassimpflags.cpp \
	src/widgets/repo_clickabletreeview.cpp \
	src/widgets/repo_lineedit.cpp \
	src/widgets/repo_mdiarea.cpp \
	src/widgets/repo_mdisubwindow.cpp \
	src/widgets/repo_textbrowser.cpp \
	src/widgets/repo_widgetrepository.cpp

FORMS +=  \
	forms/repoabstractmanagerdialog.ui \
	forms/repodialogabout.ui \
	forms/repodialogsettings.ui \
	forms/repodialoguser.ui \
	forms/repofederationdialog.ui \
	forms/repofilterabletreewidget.ui \
	forms/repogui.ui \
	forms/repoprojectsettingsdialog.ui \
	forms/reposelectiontreedockwidget.ui \
	forms/repowidgetassimpflags.ui \
	forms/repo_dialogcommit.ui \
	forms/repo_dialogconnect.ui \
	forms/repo_dialoghistory.ui \
	forms/repo_genericdialog.ui \
	forms/repo_maptilesdialog.ui \
	forms/repo_transformationdialog.ui \
	forms/repo_webview.ui \
	forms/repo_widgetrepository.ui

RESOURCES +=  \
	./resources.qrc \
	./submodules/fonts.qrc