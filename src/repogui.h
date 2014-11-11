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
#include "mongoclientwrapper.h"
//------------------------------------------------------------------------------
// GUI
#include "primitives/repo_fontawesome.h"
#include "dialogs/repo_dialogconnect.h"
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

    //! Runs startup procedures. E.g. showing connect window if set via settings
    void startup();

    //! Shows connection dialog and connects to the specified database
    void connect();

    //! Refreshes currently connected database
    void refresh();

    //! Fetches head revision from selected DB and branch
    void fetchHead();

    //! Shows a dialog confirmation and drops a selected database if agreed.
    void dropDatabase();

    //! Displays the popup context menu on the repositories widget databases view
    void showDatabaseContextMenu(const QPoint &pos);

    //! Displays the popup context menu on the repositories widget collection view
    void showCollectionContextMenuSlot(const QPoint &pos);

    //! Opens a file dialog.
    void openFile();

    //! Opens local email client with support email address predefined.
    void openSupportEmail() const;

    //! Opens a GitHub issues webpage in the default web browser.
    void reportIssue() const;

    /*!
     * Toggles fullscreen mode depending on the status of fullscreen menu action.
     * All visual aspects such as menubar, toolbars, dockwidgets etc are hiddden
     * when in a fullscreen mode and shown upon return.
     */
    void toggleFullScreen();

    //! Loads a single 3D file asynchronously given a full file path.
    void loadFile(const QString &filePath);

    //! Loads files from a list of URLs.
    void loadFiles(const QList<QUrl> &urls);

    //! Loads files from a list of full file paths.
    void loadFiles(const QStringList &filePaths);




protected :

    //! Saves window geometry (position) and state.
    void closeEvent(QCloseEvent *event);

    //! Triggered when mouse is being dragged and enters the window.
    void dragEnterEvent(QDragEnterEvent *event);

    //! Triggered when the mouse drops the drag event.
    void dropEvent(QDropEvent *event);

    //! F11 and Esc to return from fullscreen mode.
    void keyPressEvent(QKeyEvent *event);

    //! Stores the current geometry and state of the application.
    void storeSettings();

    //! Restores the previously saved geometry and state of the application.
    void restoreSettings();


private:

    //! UI var
    Ui::RepoGUI *ui;

    //! Settings geometry label.
    static const QString REPO_SETTINGS_GUI_GEOMETRY;

    //! Settings state label.
    static const QString REPO_SETTINGS_GUI_STATE;

    //! Settings link windows label.
    static const QString REPO_SETTINGS_LINK_WINDOWS;

}; // end class

} // end namespace core
} // end namespace repo


#endif // REPOGUI_H
