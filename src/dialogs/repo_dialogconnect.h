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

#include <QtWidgets/qdialog.h>
#include <QtGui>
#include <QIcon>
#include <QSettings>
#include <QCompleter>
//------------------------------------------------------------------------------
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
    RepoDialogConnect(QWidget *parent = 0, Qt::WindowFlags flags = 0);

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
		
	//! Returns the user specified host value and writes it to the global settings.
	QString getHost();

	//! Returns the user specified port value and writes it to the global settings.
	int getPort();

	//! Returns the user specified username value and writes it to the global settings.
	QString getUsername();

	//! Returns the user specified password value and writes it to the global settings.
	QString getPassword(); 

    //! Returns true if to show at startup, false otherwise.
    bool isShowAtStartup();

    //--------------------------------------------------------------------------
	//
	// Static
	//
    //--------------------------------------------------------------------------

	//! Returns the default blue icon for the connection dialog.
	static QIcon getIcon();
  
public slots:

	/*! 
	 * Shows the dialog as a modal window, blocking until the user closes it. 
	 * If the user selects to connect, the dialog saves all filled-in values
	 * as user settings. Returns a DialogCode result.
	 */
	virtual int exec();

private :

    //! Ui var
    Ui::RepoDialogConnect *ui;

	//! Application settings such as user specified host/port/username/password.
	QSettings settings;

    //! Completer for databases line edit.
    QCompleter *databasesCompleter;

    //! Settings host label.
	static const QString REPO_SETTINGS_CONNECTION_DIALOG_HOST; 

    //! Settings port label.
	static const QString REPO_SETTINGS_CONNECTION_DIALOG_PORT;
	
    //! Settings username label.
	static const QString REPO_SETTINGS_CONNECTION_DIALOG_USERNAME;

    //! Settings password label.
	static const QString REPO_SETTINGS_CONNECTION_DIALOG_PASSWORD; 

    //! Settings show at startup label.
    static const QString REPO_SETTINGS_CONNECTION_DIALOG_STARTUP;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_CONNECT_H
