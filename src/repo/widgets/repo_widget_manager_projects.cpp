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

#include "repo_widget_manager_projects.h"

using namespace repo::widgets;

const QString RepoWidgetManagerProjects::COLUMNS_SETTINGS = "RepoWidgetManagerProjectsColumnsSettings";

RepoWidgetManagerProjects::RepoWidgetManagerProjects(QWidget *parent)
    : RepoWidgetTreeEditable(parent)
{
    QList<QString> headers;
    headers << tr("Project") << tr("Description") << tr("Owner");
    headers << tr("Permissions") << tr("Type") << tr("Users");

    RepoWidgetTreeFilterable *filterableTree = getFilterableTree();
    filterableTree->restoreHeaders(headers, COLUMNS_SETTINGS);
    filterableTree->setRootIsDecorated(false);

    clear();
}

RepoWidgetManagerProjects::~RepoWidgetManagerProjects()
{}

