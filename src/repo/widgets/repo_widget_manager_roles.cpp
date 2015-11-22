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
    QList<QString> headers;
    headers << tr("Role") << tr("Database") << tr("Collection");
    headers << tr("Privileges") << tr("Access Rights") << tr("Inherited Roles");

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



void RepoWidgetManagerRoles::addRole(const repo::core::model::RepoRole &role)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // User object itself
    QVariant var;
    var.setValue(role);

    repo::primitives::RepoStandardItem *item =
            new repo::primitives::RepoStandardItem(role.getName());
    item->setData(var);
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);
    item->setTristate(false);
    row.append(item);

    // Database
    row.append(new primitives::RepoStandardItem(role.getDatabase()));

    // Collection
    row.append(new primitives::RepoStandardItem(std::string()));

    // Privileges
    row.append(new primitives::RepoStandardItem(role.getPrivileges().size()));

    // Project access rights
    row.append(new primitives::RepoStandardItem(role.getProjectAccessRights().size()));

    // Inherited roles
    row.append(new primitives::RepoStandardItem(role.getInheritedRoles().size()));

    //--------------------------------------------------------------------------
    getFilterableTree()->addTopLevelRow(row);
}


void RepoWidgetManagerRoles::edit()
{
    showEditDialog(getRole());
}

void RepoWidgetManagerRoles::edit(const QModelIndex &index)
{
    showEditDialog(getRole(index));
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



void RepoWidgetManagerRoles::refresh()
{
    if (mutex.tryLock() && cancelAllThreads())
    {
        QProgressBar* progressBar = getFilterableTree()->getProgressBar();

        repo::worker::RepoWorkerRoles* worker =
                new repo::worker::RepoWorkerRoles(token, controller, database);
        worker->setAutoDelete(true);

        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
                    this, &RepoWidgetManagerRoles::cancel,
                    worker, &repo::worker::RepoWorkerRoles::cancel, Qt::DirectConnection);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerRoles::roleFetched,
                    this, &RepoWidgetManagerRoles::addRole);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerRoles::finished,
                    progressBar, &QProgressBar::hide);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerRoles::finished,
                    this, &RepoWidgetManagerRoles::unlockMutex);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerRoles::finished,
                    this, &RepoWidgetManagerRoles::finish);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerRoles::progressRangeChanged,
                    progressBar, &QProgressBar::setRange);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerRoles::progressValueChanged,
                    progressBar, &QProgressBar::setValue);
        threadPool.start(worker);

        //----------------------------------------------------------------------
        // Clear any previous entries
        clear();

        //----------------------------------------------------------------------
        progressBar->show();
        //        ui->hostComboBox->setEnabled(false);
        //        ui->databaseComboBox->setEnabled(false);

    }
}

void RepoWidgetManagerRoles::setDBConnection(
        repo::RepoController *controller,
        const repo::RepoToken* token,
        const std::string& database,
        const QStringList &projects)
{
    this->controller = controller;
    this->token = token;
    this->database = database;
    this->projects = projects;
}

void RepoWidgetManagerRoles::showEditDialog(const repo::core::model::RepoRole &role)
{
    repo::widgets::RepoDialogRole roleDialog(role, projects, this);
    if (QDialog::Rejected == roleDialog.exec())
    {
        repoLog("Role dialog cancelled by user.\n");
        std::cout << tr("Role dialog cancelled by user.").toStdString() << std::endl;
    }
    else // QDialog::Accepted
    {
        repoLog("create or update role...\n");
        // Create or update user
//        refresh(userDialog.getUpdatedUser(),
//                userDialog.isNewUser()
//                ? repo::worker::UsersWorker::Command::INSERT
//                : repo::worker::UsersWorker::Command::UPDATE);
    }


}
