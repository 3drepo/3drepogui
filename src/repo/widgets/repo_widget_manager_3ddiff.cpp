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


#include "repo_widget_manager_3ddiff.h"
#include "ui_repo_widget_manager_3ddiff.h"

using namespace repo::widgets;

RepoWidgetManager3DDiff::RepoWidgetManager3DDiff(
        repo::gui::RepoMdiArea *mdiArea,
        QWidget *parent)
    : QWidget(parent)
    , mdiArea(mdiArea)
    , ui(new Ui::RepoWidgetManager3DDiff)
{
    ui->setupUi(this);



    for (repo::gui::RepoMdiSubWindow* subWindow : mdiArea->subWindowList(true))
    {
        subWindows.insert(subWindow->windowTitle(), subWindow);
    }

    ui->modelAComboBox->addItems(subWindows.keys());
    ui->modelBComboBox->addItems(subWindows.keys());
}

RepoWidgetManager3DDiff::~RepoWidgetManager3DDiff()
{
    delete ui;
}
