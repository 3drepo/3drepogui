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

#include "repo_widget_manager_roles.h"

using namespace repo::widgets;

const QString RepoWidgetManagerRoles::COLUMNS_SETTINGS = "RepoWidgetManagerRolesColumnsSettings";

RepoWidgetManagerRoles::RepoWidgetManagerRoles(QWidget *parent)
    : RepoWidgetTreeEditable(parent)
{
    QList<QString> headers;
    headers << tr("Role") << tr("Database") << tr("Collection");
    headers << tr("Read") << tr("Write");

    RepoWidgetTreeFilterable *filterableTree = getFilterableTree();
    filterableTree->restoreHeaders(headers, COLUMNS_SETTINGS);
    filterableTree->setRootIsDecorated(false);

    clear();
}

RepoWidgetManagerRoles::~RepoWidgetManagerRoles()
{
    // FIXME: put back in once the column headers are agreed upon
//     getFilterableTree()->storeHeaders(COLUMNS_SETTINGS);
}

void RepoWidgetManagerRoles::refresh()
{

}
