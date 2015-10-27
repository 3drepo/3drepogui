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

#ifndef REPO_DIALOG_CONNECT_H
#define REPO_DIALOG_CONNECT_H

//------------------------------------------------------------------------------
// Qt
#include <QtWidgets/qdialog.h>
#include <QtGui>
#include <QIcon>
#include <QSettings>
#include <QCompleter>

//------------------------------------------------------------------------------
// CORE
#include <repo/repo_controller.h>
#include <repo/repo_credentials.h>

//------------------------------------------------------------------------------
// GUI
#include "ui_repo_dialogconnect.h"
#include "../primitives/repo_fontawesome.h"

namespace Ui {
    class RepoDialogConnect;
}

namespace repo {
namespace gui {

/*!
 * Connection dialog that saves host, port, username and password as user
 * settings.
 */
class RepoDialogConnect : public QDialog
{
	Q_OBJECT
 
public:

    //--------------------------------------------------------------------------
	//
	// Constructor
	//
    //--------------------------------------------------------------------------
	
	//! Creates a connection dialog. To show, run exec().
    RepoDialogConnect(const repo::RepoCredentials &credentials,
                      QWidget *parent = 0,
                      Qt::WindowFlags flags = 0);

    //--------------------------------------------------------------------------
	//
	// Desctructor
	//
    //--------------------------------------------------------------------------

	//! Syncs the global settings.
	~RepoDialogConnect();

    //--------------------------------------------------------------------------
	//
	// Getters
	//
    //--------------------------------------------------------------------------

    //! Returns current connection settings
    repo::RepoCredentials getConnectionSettings() const;

private :

    //! Ui var
    Ui::RepoDialogConnect *ui;

    //! Completer for databases line edit.
    QCompleter *databasesCompleter;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_CONNECT_H
