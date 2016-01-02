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

#pragma once

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
#include "ui_repo_dialog_connect.h"
#include "../primitives/repo_fontawesome.h"

namespace Ui {
class ConnectDialog;
}

namespace repo{
namespace gui {
namespace dialog {

	/*!
		* Connection dialog that saves alias, host, port, authentication database,
		* username and password as RepoCredentials.
		*/
	class ConnectDialog : public QDialog
	{
		Q_OBJECT

		enum class Tab { ADDRESS, AUTHENTICATION, SSL, SSH };

	public:

		//! Creates a connection dialog. To show, run exec().
		ConnectDialog(repo::RepoController *controller,
			const repo::RepoCredentials &credentials,
			const bool isCopy = false,
			QWidget *parent = 0,
			Qt::WindowFlags flags = 0);

		//! Syncs the global settings.
		~ConnectDialog();

		public slots:

		void validate();

	public:

		//! Returns current connection settings
		repo::RepoCredentials getConnectionSettings() const;

	private:

		//! Ui var
		Ui::ConnectDialog *ui;

		//! Completer for databases line edit.
		QCompleter *databasesCompleter;

		repo::RepoController *controller;

	};
}
} // end namespace widget
} // end namespace repo
