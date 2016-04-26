/**
 *  Copyright (C) 2015 3D Repo Ltd
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

#include <QWidget>
#include <QDialog>
#include <QMessageBox>

//------------------------------------------------------------------------------
// GUI
#include "repo_widget_tree_editable.h"
#include "repo_widget_tree_clickable.h"
#include "../dialogs/repo_dialog_connect.h"
#include "../primitives/repo_idbcache.h"
#include "../primitives/repo_standard_item.h"
#include "../../settings/repo_settings_credentials.h"

//------------------------------------------------------------------------------
// CORE
#include <repo/repo_controller.h>

//------------------------------------------------------------------------------

namespace repo {
	namespace gui{
		namespace widget {
			Q_DECLARE_METATYPE(std::string)

			class ConnectionManagerWidget : public EditableTreeWidget
			{
				Q_OBJECT

					static const QString COLUMNS_SETTINGS;

				static const QString SELECTION_SETTINGS;

				//! Tree view columns
				enum class Columns { ALIAS, HOST_PORT, AUTHENTICATION, SSL, SSH };

			public:

				explicit ConnectionManagerWidget(QWidget *parent);

				~ConnectionManagerWidget();

				public slots :

				//! Adds credentials as an item into the table.
				void addItem(const std::string &credentials);

				//! Updates selected item.
				virtual void edit();

				//! Updates item based on model index.
				virtual void edit(const QModelIndex &index);

				//! Returns currectly selected connection.
				std::string getConnection();

				//! Returns connection for given model index.
				std::string getConnection(const QModelIndex &index);

				//! Refreshes the current list
				virtual void refresh();

				//! Copies currectly selected item.
				virtual void copyItem()
				{
					showEditDialog(getConnection(), QModelIndex(), Action::COPY);
				}

				//! Removes item and refreshes the DB if necessary.
				virtual void removeItem();

				//! Shows edit dialog.
				virtual void showEditDialog()
				{
					showEditDialog(
						std::string(), QModelIndex(), Action::ADD);
				}

				//! Shows edit dialog populated with given credentials.
				void showEditDialog(
					const std::string &credentials,
					const QModelIndex &index,
					const Action action);

			public:

				//! Sets DB controller for connectin validation to work.
				void setController(repo::RepoController *controller)
				{
					this->controller = controller;
				}

			private:

				//! Serializes table contents onto the HDD.
				void serialize();

				//! Returns alias tree item from given credentials
				static QStandardItem *makeAliasItem(
					const std::string &credentials,
					const std::string &value);

				//! Returns address tree item from given credentials
				static QStandardItem *makeAddressItem(const std::string &credentials,
					const std::string &value);

				//! Returns authentication tree item from given credentials
				static QStandardItem *makeAuthenticationItem(
					const std::string &credentials,
					const std::string &authDB,
					const std::string &username);

				//! Returns ssl tree item from given credentials
				static QStandardItem *makeSSLItem(const std::string &);

				//! Returns ssh tree item from given credentials
				static QStandardItem *makeSSHItem(const std::string &);

			private:

				//! DB controller.
				repo::RepoController *controller;
			};
		} // widgets
	}
} // repo
