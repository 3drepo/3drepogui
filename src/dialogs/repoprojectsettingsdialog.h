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


#ifndef REPO_PROJECT_DIALOG_H
#define REPO_PROJECT_DIALOG_H

//------------------------------------------------------------------------------
// Qt
#include <QDialog>
#include <QCheckBox>

#include <repo/core/model/bson/repo_bson_project_settings.h>

namespace Ui {
class RepoProjectSettingsDialog;
}

namespace repo {
namespace gui {


class RepoProjectSettingsDialog : public QDialog
{
    Q_OBJECT

    enum Permissions { BITS, OWNER, GROUP, PUBLIC };

public:

    explicit RepoProjectSettingsDialog(
		repo::core::model::RepoProjectSettings projectSettings,
            QWidget *parent = 0);

    ~RepoProjectSettingsDialog();

public :

    //! Returns bson.
    repo::core::model::RepoProjectSettings getSettings() const;

private:

    Ui::RepoProjectSettingsDialog *ui;

}; // end class

} // end namespace gui
} // end namespace repo

#endif // REPO_PROJECT_DIALOG_H
