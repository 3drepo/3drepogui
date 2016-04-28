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
#include "../primitives/repo_standard_item.h"

using namespace repo::gui;
using namespace repo::gui::widget;

const QString RolesManagerWidget::COLUMNS_SETTINGS = "RolesManagerWidgetColumnsSettings";

RolesManagerWidget::RolesManagerWidget(QWidget *parent)
    : EditableTreeWidget(parent)
{
    QList<QString> headers = {
        tr("Role"),
        tr("Database"),
        tr("Permissions"),
        tr("Privileges"),
        tr("Inherited Roles"),
        tr("Modules")};

    FilterableTreeWidget *filterableTree = getFilterableTree();
    filterableTree->restoreHeaders(headers, COLUMNS_SETTINGS);
    filterableTree->setRootIsDecorated(false);

    clear();
}

RolesManagerWidget::~RolesManagerWidget()
{
     getFilterableTree()->storeHeaders(COLUMNS_SETTINGS);
}

void RolesManagerWidget::addRole(
        const repo::core::model::RepoRole &role,
        const repo::core::model::RepoRoleSettings &settings)
{
    QList<QStandardItem *> row;

    repo::gui::primitive::RepoStandardItem *item = nullptr;

    //--------------------------------------------------------------------------
    // Role with color
    QVariant var;
    var.setValue(role);
    item = new repo::gui::primitive::RepoStandardItem(role.getName());
    item->setData(var);    
    QPixmap px(32,32);
    if (!settings.getColor().empty())
    {
        px.fill(repo::gui::primitive::RepoColor::fromHex(settings.getColor()));
    }
    else
    {
        px.fill(QColor(0,0,0,0)); // transparent
    }
    item->setIcon(QIcon(px));
    row.append(item);


    // Database
    row.append(new repo::gui::primitive::RepoStandardItem(role.getDatabase()));

    // Project access rights
    row.append(new repo::gui::primitive::RepoStandardItem(role.getProjectAccessRights().size()));

    // Privileges
    row.append(new repo::gui::primitive::RepoStandardItem(role.getPrivileges().size()));

    // Inherited roles
    row.append(new repo::gui::primitive::RepoStandardItem(role.getInheritedRoles().size()));

    // Modules
    QVariant qsetting;
    qsetting.setValue(settings);
    item = new repo::gui::primitive::RepoStandardItem(settings.getModules().size());
    item->setData(qsetting);
    row.append(item);

    //--------------------------------------------------------------------------
    getFilterableTree()->addTopLevelRow(row);
}


void RolesManagerWidget::edit()
{
    showEditDialog(
                getRole(),
                getRoleSettings(),
                EditableTreeWidget::Action::EDIT);
}

void RolesManagerWidget::edit(const QModelIndex &index)
{
    showEditDialog(
                getRole(index),
                getRoleSettings(index),
                EditableTreeWidget::Action::EDIT);
}

repo::core::model::RepoRole RolesManagerWidget::getRole() const
{
    return getRole(getFilterableTree()->getCurrentIndex());
}

repo::core::model::RepoRole RolesManagerWidget::getRole(
        const QModelIndex &index) const
{
    repo::core::model::RepoRole role;
    if (index.isValid())
    {
        QModelIndex roleIndex = index.sibling(index.row(), (int) Columns::ROLE);
        role = roleIndex.data(Qt::UserRole + 1).value<repo::core::model::RepoRole>();
    }
    return role;
}

repo::core::model::RepoRoleSettings RolesManagerWidget::getRoleSettings() const
{
    return getRoleSettings(getFilterableTree()->getCurrentIndex());
}

repo::core::model::RepoRoleSettings RolesManagerWidget::getRoleSettings(
        const QModelIndex &index) const
{
    repo::core::model::RepoRoleSettings settings;
    if (index.isValid())
    {
        QModelIndex roleSettingsIndex = index.sibling(index.row(), (int) Columns::MODULES);
        settings = roleSettingsIndex.data(Qt::UserRole + 1).value<repo::core::model::RepoRoleSettings>();
    }
    return settings;
}

void RolesManagerWidget::refresh(
        const repo::core::model::RepoRole &role,
        const repo::core::model::RepoRoleSettings &settings,
        repo::worker::RepoWorkerRoles::Command command)
{
    if (isReady())
    {
        // Clear any previous entries
        clear();

        repo::worker::RepoWorkerRoles* worker =
                new repo::worker::RepoWorkerRoles(
                    token,
                    controller,
                    database,
                    role,
                    settings,
                    command);
        QObject::connect(
                    worker, &repo::worker::RepoWorkerRoles::roleFetched,
                    this, &RolesManagerWidget::addRole);
        connectAndStartWorker(worker, getFilterableTree()->getProgressBar());
    }
}

void RolesManagerWidget::setDBConnection(
        repo::RepoController *controller,
        const repo::RepoController::RepoToken* token,
        const std::string& database)
{
    this->controller = controller;
    this->token = token;
    this->database = database;
}


void RolesManagerWidget::removeItem()
{
    repo::core::model::RepoRole role = this->getRole();
    repo::core::model::RepoRoleSettings settings = this->getRoleSettings();
    switch(QMessageBox::warning(this,
                                tr("Remove role?"),
                                tr("Are you sure you want to remove '") + QString::fromStdString(role.getName()) + "'?",
                                tr("&Yes"),
                                tr("&No"),
                                QString::null, 1, 1))
    {
    case 0: // yes
        refresh(role, settings, repo::worker::RepoWorkerRoles::Command::DROP);
        //FIXME: get a worker to do the work, then signal finish to refresh like db widget.
        break;
    case 1: // no
		repoLog("Remove role warning box cancelled by user.");
        break;
    }
}

void RolesManagerWidget::showEditDialog(
        const repo::core::model::RepoRole &role,
        const repo::core::model::RepoRoleSettings &settings,
        const EditableTreeWidget::Action action)
{
    dialog::RoleDialog roleDialog(
                role,
                settings,
                QString::fromStdString(database),
                databasesWithProjects,
                action == EditableTreeWidget::Action::COPY,
                this);
    if (QDialog::Rejected == roleDialog.exec())
    {
        repoLog("Role dialog cancelled by user.\n");
		repoLog(tr("Role dialog cancelled by user.").toStdString());
    }
    else // QDialog::Accepted
    {
        repoLog("create or update role...\n");
        // Create or update role
        refresh(roleDialog.getUpdatedRole(),
                roleDialog.getUpdatedRoleSettings(),
                action == EditableTreeWidget::Action::ADD ||
                roleDialog.isNewRole()
                ? repo::worker::RepoWorkerRoles::Command::INSERT
                : repo::worker::RepoWorkerRoles::Command::UPDATE);
    }
}

void RolesManagerWidget::setDatabasesWithProjects(
        const std::map<std::string, std::list<std::string> > &rdwp)
{
    this->databasesWithProjects.clear();
    this->databasesWithProjects = rdwp;
}
