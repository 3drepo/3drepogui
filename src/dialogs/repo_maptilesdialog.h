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


#ifndef REPO_MAP_TILES_DIALOG_H
#define REPO_MAP_TILES_DIALOG_H

// Qt
#include <QDialog>

// Core
#include <RepoMap>

//------------------------------------------------------------------------------

namespace Ui {
class RepoMapTilesDialog;
}

namespace repo {
namespace gui {

class RepoMapTilesDialog : public QDialog
{
    Q_OBJECT

public:

    explicit RepoMapTilesDialog(
            QWidget *parent = 0,
            const core::RepoMap &map = core::RepoMap());

    ~RepoMapTilesDialog();

    core::RepoMap getMap();

private:

    Ui::RepoMapTilesDialog *ui;

    core::RepoMap map;

}; // end class

} // end namespace gui
} // end namespace repo

#endif // REPO_MAP_TILES_DIALOG_H
