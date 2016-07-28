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

#include <fstream>
#include <iostream>

//------------------------------------------------------------------------------
// Qt
#include <QDialog>
#include <QStandardItemModel>
#include <QComboBox>
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <QStandardItemEditorCreator>
#include <QTreeWidgetItem>
#include <QUuid>
#include <QMap>
#include <QRegExpValidator>
#include <QFileDialog>
#include <QBuffer>
#include <QImageReader>
#include <QMessageBox>

//------------------------------------------------------------------------------
// GUI
#include "../primitives/repo_combo_box_editor.h"
#include "../primitives/repo_combo_box_delegate.h"
#include "../primitives/repo_fontawesome.h"

//------------------------------------------------------------------------------
// Core
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson_user.h>
#include <repo/core/model/bson/repo_bson_factory.h>

namespace Ui {
class UserDialog;
}

namespace repo {
namespace gui{
namespace dialog {

    class UserDialog : public QDialog
	{
		Q_OBJECT

        enum class Tab { ROLES, API_KEYS, LICENSES };

	public:

		//! Explicit constructor.
		explicit UserDialog(
			const repo::RepoController::RepoToken *token,
			repo::RepoController *controller,
			const repo::core::model::RepoUser &user,
            const std::map<std::string, std::list<std::string> > &databasesWithProjects,
			const std::list<std::string> &customRolesList,
			const bool isCopy = false,
			QWidget *parent = 0);

		//! Destructor.
		~UserDialog();

		public slots:

		//! Opens up file dialog to load an image.
		void openImageFileDialog();

		void setNextAPIKey();

	public:

		//--------------------------------------------------------------------------
		//
		// Getters
		//
		//--------------------------------------------------------------------------

		//! Returns a list of api keys as label, key pairs.
		std::list<std::pair<std::string, std::string> > getAPIKeys() const;

		//! Returns the email currently set in the dialog if any.
		std::string getEmail() const;

		//! Returns the first name currently set in the dialog if any.
		std::string getFirstName() const;

		//! Returns a list of items as db, value pairs from given tree widget.
		std::list<std::pair<std::string, std::string> > getItems(QTreeWidget *) const;

		//! Returns the last name currently set in the dialog if any.
		std::string getLastName() const;

		/*!
			* Returns password currently set in the dialog if it has changed. Returns
			* empty string otherwise.
			*/
		std::string getPassword() const;

		//! Returns a list of roles as db, role pairs.
		std::list<std::pair<std::string, std::string> > getRoles() const;

		//! Returns username currently set in the dialog.
		std::string getUsername() const;

		//! Returns the user action command to be used with db.runCommand().
		repo::core::model::RepoUser getUpdatedUser() const;

		//! Returns the icon for this dialog.
		static QIcon getIcon();

		//--------------------------------------------------------------------------
		//
		// Setters
		//
		//--------------------------------------------------------------------------

		//! Sets the avatar icon with the give image.
		void setAvatar(const std::vector<char> &image);

		//! Sets the avatar icon with the given image.
		void setAvatar(const QImage &image);

	private:
        void addRemoveLicense(int oldRowCount, int newRowCount);
        //Update contents of the license widget with the user info
        void updateLicenseWidget();

		//! Email regular expression validator.
		QRegExpValidator *emailValidator;

		//! User to be created or modified.
		repo::core::model::RepoUser user;

		//! User avatar (profile picture).
		std::vector<char> avatar;

		//! Ui var.
		Ui::UserDialog *ui;

		//! Database token.
		const repo::RepoController::RepoToken *token;

		//! Database connection controller.
		repo::RepoController *controller;
	};
}
} // end namespace widget
} // end namespace repo
