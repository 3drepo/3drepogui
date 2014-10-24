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


namespace Ui {
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

    //! Shows connection dialog and connects to the specified database
    void connect();

    //! Refreshes currently connected database
    void refresh();

    //! Shows a dialog confirmation and drops a selected database if agreed.
    void dropDatabase();

    //! Displays the popup context menu on the repositories widget databases view.
    void showDatabaseContextMenu(const QPoint &pos);

    //! Displays the popup context menu on the repositories widget collection view.
    void showCollectionContextMenuSlot(const QPoint &pos);

    //! Opens local email client with support email address predefined.
    void openSupportEmail() const;

protected :

    //! Saves window geometry (position) and state.
    void closeEvent(QCloseEvent *event);

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

}; // end class

} // end namespace core
} // end namespace repo


#endif // REPOGUI_H
