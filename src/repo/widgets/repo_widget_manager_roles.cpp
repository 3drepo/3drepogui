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

using namespace repo::widgets;

const QString RepoWidgetManagerRoles::COLUMNS_SETTINGS = "RepoWidgetManagerRolesColumnsSettings";

RepoWidgetManagerRoles::RepoWidgetManagerRoles(QWidget *parent)
    : RepoWidgetTreeEditable(parent)
{
    QList<QString> headers = {
        tr("Role"),
        tr("Color"),
        tr("Database"),
        tr("Access Rights"),
        tr("Privileges"),
        tr("Inherited Roles")};

    RepoWidgetTreeFilterable *filterableTree = getFilterableTree();
    filterableTree->restoreHeaders(headers, COLUMNS_SETTINGS);
    filterableTree->setRootIsDecorated(false);

    clear();
}

RepoWidgetManagerRoles::~RepoWidgetManagerRoles()
{
     getFilterableTree()->storeHeaders(COLUMNS_SETTINGS);
}

void RepoWidgetManagerRoles::addRole(
        const repo::core::model::RepoRole &role,
        const repo::core::model::RepoRoleSettings &settings)
{
    QList<QStandardItem *> row;

    repo::primitives::RepoStandardItem *item = nullptr;

    // Role
    QVariant var;
    var.setValue(role);
    item = new repo::primitives::RepoStandardItem(role.getName());
    item->setData(var);
    row.append(item);


    // Color
    QVariant qsetting;
    qsetting.setValue(settings);
    item = new repo::primitives::RepoStandardItem(settings.getColor());
    item->setData(qsetting);

    if (!settings.getColor().empty())
    {
        repo::gui::RepoColor color = repo::gui::RepoColor::fromHex(settings.getColor());
        item->setData(color, Qt::ForegroundRole);
        item->setData(color, Qt::BackgroundRole);
    }
    row.append(item);

    // Database
    row.append(new primitives::RepoStandardItem(role.getDatabase()));

    // Project access rights
    row.append(new primitives::RepoStandardItem(role.getProjectAccessRights().size()));

    // Privileges
    row.append(new primitives::RepoStandardItem(role.getPrivileges().size()));

    // Inherited roles
    row.append(new primitives::RepoStandardItem(role.getInheritedRoles().size()));

    //--------------------------------------------------------------------------
    getFilterableTree()->addTopLevelRow(row);
}


void RepoWidgetManagerRoles::edit()
{
    showEditDialog(getRole(), getRoleSettings());
}

void RepoWidgetManagerRoles::edit(const QModelIndex &index)
{
    showEditDialog(getRole(index), getRoleSettings(index));
}

repo::core::model::RepoRole RepoWidgetManagerRoles::getRole() const
{
    return getRole(getFilterableTree()->getCurrentIndex());
}

repo::core::model::RepoRole RepoWidgetManagerRoles::getRole(
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

repo::core::model::RepoRoleSettings RepoWidgetManagerRoles::getRoleSettings() const
{
    return getRoleSettings(getFilterableTree()->getCurrentIndex());
}

repo::core::model::RepoRoleSettings RepoWidgetManagerRoles::getRoleSettings(
        const QModelIndex &index) const
{
    repo::core::model::RepoRoleSettings settings;
    if (index.isValid())
    {
        QModelIndex roleSettingsIndex = index.sibling(index.row(), (int) Columns::COLOR);
        settings = roleSettingsIndex.data(Qt::UserRole + 1).value<repo::core::model::RepoRoleSettings>();
    }
    return settings;
}

void RepoWidgetManagerRoles::refresh(
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
                    this, &RepoWidgetManagerRoles::addRole);
        connectAndStartWorker(worker, getFilterableTree()->getProgressBar());
    }
}

void RepoWidgetManagerRoles::setDBConnection(
        repo::RepoController *controller,
        const repo::RepoToken* token,
        const std::string& database)
{
    this->controller = controller;
    this->token = token;
    this->database = database;
}


void RepoWidgetManagerRoles::removeItem()
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
        std::cout << "Remove role warning box cancelled by user." << std::endl;
        break;
    }
}

void RepoWidgetManagerRoles::showEditDialog(
        const repo::core::model::RepoRole &role,
        const repo::core::model::RepoRoleSettings &settings)
{
    repo::widgets::RepoDialogRole roleDialog(
                role,
                settings,
                QString::fromStdString(database),
                databasesWithProjects,
                this);
    if (QDialog::Rejected == roleDialog.exec())
    {
        repoLog("Role dialog cancelled by user.\n");
        std::cout << tr("Role dialog cancelled by user.").toStdString() << std::endl;
    }
    else // QDialog::Accepted
    {
        repoLog("create or update role...\n");
        // Create or update role
        refresh(roleDialog.getUpdatedRole(),
                roleDialog.getUpdatedRoleSettings(),
                roleDialog.isNewRole()
                ? repo::worker::RepoWorkerRoles::Command::INSERT
                : repo::worker::RepoWorkerRoles::Command::UPDATE);
    }
}

void RepoWidgetManagerRoles::setDatabasesWithProjects(
        const std::map<std::string, std::list<std::string> > &rdwp)
{
    this->databasesWithProjects.clear();
    this->databasesWithProjects = rdwp;
}
