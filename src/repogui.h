/**
 *  Copyright (C) 2014 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REPOGUI_H
#define REPOGUI_H

#include <QMainWindow>
#include <QGLFormat>
//------------------------------------------------------------------------------
// Core
#include <RepoWrapperMongo>
#include <RepoGraphScene>
//------------------------------------------------------------------------------
// GUI

#include "renderers/repo_glcwidget.h"
//------------------------------------------------------------------------------


namespace Ui
{
    class RepoGUI;
}

namespace repo {
namespace gui {


//! Main application window.
class RepoGUI : public QMainWindow
{
    Q_OBJECT

public:

    //! Explicit default constructor
    explicit RepoGUI(QWidget *parent = 0);

    //! Destructor
    ~RepoGUI();

public slots:

    //! Adds a selection tree for a currently active widget.
    void addSelectionTree()
    {  addSelectionTree(getActiveWidget()); }

    //! Adds a selection tree for a given widget.
    void addSelectionTree(
            RepoGLCWidget* widget,
            Qt::DockWidgetArea area = Qt::RightDockWidgetArea);


    //! Shows a commit dialog based on currently active 3D window.
    void commit();

    //! Shows a connection dialog and connects to the specified database.
    void connect();

    QMenu* createPanelsMenu();

    //! Shows a dialog confirmation and drops a selected database if agreed.
    void drop();

    //! Fetches head revision from selected DB and branch.
    void fetchHead();

    //! Returns active 3D window, returns null if none and writes to cout.
    RepoGLCWidget *getActiveWidget();

    //! Returns the scene graph of the active 3D window if any.
    const core::RepoGraphScene *getActiveScene();

    //! Shows a history dialog for selected repository.
    void history();

    //! Loads a single 3D file asynchronously given a full file path.
    void loadFile(const QString &filePath);

    //! Loads files from a list of URLs.
    void loadFiles(const QList<QUrl> &urls);

    //! Loads files from a list of full file paths.
    void loadFiles(const QStringList &filePaths);

    //! Opens an oculus rift rendering window.
    void oculus();

    //! Opens a 3D Diff dialog.
    void open3DDiff();

    //! Opens a file dialog.
    void openFile();

    //! Opens metadata manager.
    void openMetadataManager();

    //! Opens settings dialog.
    void openSettings() const;

    //! Opens local email client with support email address predefined.
    void openSupportEmail() const;

    //! Opens user management dialog box.
    void openUserManager() const;

    void optimizeGraph();

    //! Refreshes currently connected database.
    void refresh();

    //! Opens a GitHub issues webpage in the default web browser.
    void reportIssue() const;

    //! Saves open 3D window as a file.
    void saveAs();

    //! Saves open 3D window as an image file (screenshot).
    void saveScreenshot();

    //! Displays the popup context menu on the repositories widget collection view.
    void showCollectionContextMenuSlot(const QPoint &pos);

    //! Displays the popup context menu on the repositories widget databases view.
    void showDatabaseContextMenu(const QPoint &pos);

    //! Runs startup procedures. E.g. showing connect window if set via settings.
    void startup();

    /*!
     * Toggles fullscreen mode depending on the status of fullscreen menu action.
     * All visual aspects such as menubar, toolbars, dockwidgets etc are hiddden
     * when in a fullscreen mode and shown upon return.
     */
    void toggleFullScreen();

protected :

    //! Saves window geometry (position) and state.
    void closeEvent(QCloseEvent *event);

    //! Triggered when mouse is being dragged and enters the window.
    void dragEnterEvent(QDragEnterEvent *event);

    //! Triggered when the mouse drops the drag event.
    void dropEvent(QDropEvent *event);

    //! F11 and Esc to return from fullscreen mode.
    void keyPressEvent(QKeyEvent *event);

    //! Restores the previously saved geometry and state of the application.
    void restoreSettings();

    //! Stores the current geometry and state of the application.
    void storeSettings();

private:

    //! UI var.
    Ui::RepoGUI *ui;

    //! Panels menu (dockable widgets and toolbars)
    QMenu *panelsMenu;

    //! Settings geometry label.
    static const QString REPO_SETTINGS_GUI_GEOMETRY;

    //! Settings state label.
    static const QString REPO_SETTINGS_GUI_STATE;

    //! Settings link windows label.
    static const QString REPO_SETTINGS_LINK_WINDOWS;

}; // end class

} // end namespace gui
} // end namespace repo


#endif // REPOGUI_H
