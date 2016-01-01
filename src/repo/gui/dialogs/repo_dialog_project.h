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

//------------------------------------------------------------------------------
// Qt
#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QLocale>

#include <repo/core/model/bson/repo_bson_project_settings.h>

namespace Ui {
class ProjectDialog;
}

namespace repo {
namespace gui{
namespace dialog {


	class ProjectDialog : public QDialog
	{
		Q_OBJECT

		enum Permissions { BITS, OWNER, GROUP, PUBLIC };

	public:

		explicit ProjectDialog(
			const repo::core::model::RepoProjectSettings &projectSettings,
			const bool isCopy = false,
			QWidget *parent = 0);

		~ProjectDialog();

		public slots :

		//! Returns bson.
		repo::core::model::RepoProjectSettings getSettings() const;

	private:

		void explainDefaultValue(QLabel *label, double value);

	private:

		Ui::ProjectDialog *ui;

	}; // end class
}
} // end namespace gui
} // end namespace repo

