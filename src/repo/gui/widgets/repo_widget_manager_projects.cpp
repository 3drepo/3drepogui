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
#include "../primitives/repo_standard_item.h"

using namespace repo::gui;
using namespace repo::gui::widget;

const QString ProjectsManagerWidget::COLUMNS_SETTINGS = "ProjectsManagerWidgetColumnsSettings";

ProjectsManagerWidget::ProjectsManagerWidget(QWidget *parent)
    : EditableTreeWidget(parent)
{
    QList<QString> headers = {
        tr("Project"),
        tr("Owner"),
        tr("Type"),
        tr("Description")};

    FilterableTreeWidget *filterableTree = getFilterableTree();
    filterableTree->restoreHeaders(headers, COLUMNS_SETTINGS);
    filterableTree->setRootIsDecorated(false);

    clear();
}

ProjectsManagerWidget::~ProjectsManagerWidget()
{

    // TODO: store column settings
}

void ProjectsManagerWidget::addProjectSettings(
        repo::core::model::RepoProjectSettings projectSettings)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // Project object itself
    QVariant var;
    var.setValue(projectSettings);
    repo::gui::primitive::RepoStandardItem *item =
            new repo::gui::primitive::RepoStandardItem(projectSettings.getProjectName());
    item->setData(var);
//    item->setCheckable(true);
//    item->setCheckState(Qt::Checked);
//    item->setTristate(false);
    row.append(item);

    // Owner
    row.append(new repo::gui::primitive::RepoStandardItem(projectSettings.getOwner()));

    // Type
    row.append(new repo::gui::primitive::RepoStandardItem(projectSettings.getType()));

    // Description
    row.append(new repo::gui::primitive::RepoStandardItem(projectSettings.getDescription()));

    //--------------------------------------------------------------------------
    getFilterableTree()->addTopLevelRow(row);
}

repo::core::model::RepoProjectSettings ProjectsManagerWidget::getProjectSettings()
{
    return getProjectSettings(getFilterableTree()->getCurrentIndex());
}

repo::core::model::RepoProjectSettings
   ProjectsManagerWidget::getProjectSettings(const QModelIndex &index)
{
    repo::core::model::RepoProjectSettings projectSettings;
    if (index.isValid())
    {
        QModelIndex userIndex = index.sibling(index.row(), (int)Columns::PROJECT);
        projectSettings = userIndex.data(Qt::UserRole + 1).value<repo::core::model::RepoProjectSettings>();
    }
    return projectSettings;
}



void ProjectsManagerWidget::refresh(
        const repo::core::model::RepoProjectSettings &settings,
        bool isDelete)
{
    if (isReady())
    {        
        repo::worker::ProjectSettingsWorker* worker =
            new repo::worker::ProjectSettingsWorker(
                    token,
                    controller,
                    database,
                    settings,
                    isDelete);

        QObject::connect(
            worker, &repo::worker::ProjectSettingsWorker::projectSettingsFetched,
            this, &ProjectsManagerWidget::addProjectSettings);

        //----------------------------------------------------------------------
        // Clear any previous entries
        clear();

        //----------------------------------------------------------------------
        connectAndStartWorker(worker, getFilterableTree()->getProgressBar());
    }
}

void ProjectsManagerWidget::removeItem()
{
    repo::core::model::RepoProjectSettings projectSettings = this->getProjectSettings();
    switch(QMessageBox::warning(this,
        tr("Remove project settings?"),
        tr("Are you sure you want to remove '") + QString::fromStdString(projectSettings.getProjectName()) + "'?",
        tr("&Yes"),
        tr("&No"),
        QString::null, 1, 1))
        {
            case 0: // yes
                refresh(projectSettings, true);
                break;
            case 1: // no
                std::cout << tr("Remove project settings warning box cancelled by user.").toStdString() << std::endl;
                break;
        }
}

void ProjectsManagerWidget::showEditDialog(
        const repo::core::model::RepoProjectSettings &projectSettings,
        const Action action)
{
    dialog::ProjectDialog projectDialog(
                projectSettings,
                action == Action::COPY,
                this);
    if (QDialog::Rejected == projectDialog.exec())
    {
        std::cout << tr("Project dialog cancelled by user.").toStdString() << std::endl;
    }
    else // QDialog::Accepted
    {
        // Create or update project
        refresh(projectDialog.getSettings(), false);
    }
}

void ProjectsManagerWidget::setDBConnection(
        repo::RepoController *controller,
        const repo::RepoToken* token,
        const std::string& database)
{
    this->controller = controller;
    this->token = token;
    this->database = database;
}
