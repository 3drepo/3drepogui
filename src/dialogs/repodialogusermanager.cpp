/**
 *  Copyright (C) 2014 3D Repo Ltd
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


#include "repodialogusermanager.h"
#include "ui_repoabstractmanagerdialog.h"


#if defined(_WIN32) || defined(_WIN64)
#define strcasecmp _stricmp
#endif
#include <repo/core/model/repo_model_global.h>
#include <repo/repo_bouncer_global.h>

repo::gui::RepoDialogUserManager::RepoDialogUserManager(
        repo::RepoController *controller,
        const RepoIDBCache *dbCache,
        QWidget *parent)
    : controller(controller),
      RepoAbstractManagerDialog(dbCache, parent)
{
    setWindowTitle("User Manager");

    ui->databaseComboBox->setCurrentText(QString::fromStdString(
                                             controller->getNameOfAdminDatabase(dbCache->getSelectedConnection())));

    //--------------------------------------------------------------------------
    // Users
    model->setColumnCount(8);
    model->setHeaderData(
                (int) Columns::ACTIVE,
                Qt::Horizontal,
                tr("Active"));
    model->setHeaderData(
                (int) Columns::USERNAME,
                Qt::Horizontal,
                tr("Username"));
    model->setHeaderData(
                (int) Columns::FIRST_NAME,
                Qt::Horizontal,
                tr("First Name"));
    model->setHeaderData(
                (int) Columns::LAST_NAME,
                Qt::Horizontal,
                tr("Last Name"));
    model->setHeaderData(
                (int) Columns::EMAIL,
                Qt::Horizontal,
                tr("Email"));
    model->setHeaderData(
                (int) Columns::PROJECTS,
                Qt::Horizontal,
                tr("Projects"));
    model->setHeaderData(
                (int) Columns::GROUPS,
                Qt::Horizontal,
                tr("Groups"));
    model->setHeaderData(
                (int) Columns::ROLES,
                Qt::Horizontal,
                tr("Roles"));

    ui->treeView->sortByColumn((int) Columns::USERNAME, Qt::SortOrder::AscendingOrder);
    clear();
}

repo::gui::RepoDialogUserManager::~RepoDialogUserManager() {}

void repo::gui::RepoDialogUserManager::addCustomRoles(const std::list<std::string> &list)
{
    customRolesList.clear();
    customRolesList = list;
    customRolesList.sort();
}

void repo::gui::RepoDialogUserManager::addDatabasesWithProjects(
        const std::map<std::string, std::list<std::string> > &mapping)
{
    databasesWithProjects.clear();
    databasesWithProjects = mapping;
}

void repo::gui::RepoDialogUserManager::addUser(const repo::core::model::RepoUser &user)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // User object itself
    QVariant var;
    var.setValue(user);

    QStandardItem *item = new QStandardItem();
    item->setData(var);
    item->setEnabled(true);
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);
    item->setTristate(false);
    item->setEditable(false);
    row.append(item);


    // Username
    row.append(createItem(QString::fromStdString(user.getUserName())));

    // First Name
    row.append(createItem(QString::fromStdString(user.getFirstName())));

    // Last Name
    row.append(createItem(QString::fromStdString(user.getLastName())));

    // Email
    row.append(createItem(QString::fromStdString(user.getEmail())));

    // Projects count
    row.append(createItem((qulonglong)(user.getProjectsList().size())));

    // Groups count
    row.append(createItem((qulonglong)(user.getGroupsList().size())));

    // Roles count
    row.append(createItem((qulonglong)(user.getRolesList().size())));

    //--------------------------------------------------------------------------
    model->invisibleRootItem()->appendRow(row);
}

void repo::gui::RepoDialogUserManager::edit()
{
    showEditDialog(getUser());
}

void repo::gui::RepoDialogUserManager::edit(const QModelIndex &index)
{
    showEditDialog(getUser(index));
}

repo::core::model::RepoUser repo::gui::RepoDialogUserManager::getUser()
{
    return getUser(ui->treeView->selectionModel()->currentIndex());
}

repo::core::model::RepoUser repo::gui::RepoDialogUserManager::getUser(const QModelIndex &index)
{
    repo::core::model::RepoUser user;
    if (index.isValid())
    {
        QModelIndex userIndex = index.sibling(index.row(), (int) Columns::ACTIVE);
        user = userIndex.data(Qt::UserRole + 1).value<repo::core::model::RepoUser>();
    }
    return user;
}

void repo::gui::RepoDialogUserManager::refresh(
        const repo::core::model::RepoUser &user,
        const repo::worker::UsersWorker::Command &command)
{
    if (mutex.tryLock() && cancelAllThreads())
    {
        const repo::RepoToken *token = dbCache->getConnection(ui->hostComboBox->currentText());
        std::string database = ui->databaseComboBox->currentText().toStdString();

        repo::worker::UsersWorker* worker =
                new repo::worker::UsersWorker(token, controller, database, user, command);
        worker->setAutoDelete(true);

        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
                    this, &RepoDialogUserManager::cancel,
                    worker, &repo::worker::UsersWorker::cancel, Qt::DirectConnection);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::userFetched,
                    this, &RepoDialogUserManager::addUser);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::databasesWithProjectsFetched,
                    this, &RepoDialogUserManager::addDatabasesWithProjects);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::customRolesFetched,
                    this, &RepoDialogUserManager::addCustomRoles);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::finished,
                    ui->progressBar, &QProgressBar::hide);


        QObject::connect(
                    worker, &repo::worker::UsersWorker::finished,
                    this, &RepoDialogUserManager::unlockMutex);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::finished,
                    this, &RepoDialogUserManager::finish);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::progressRangeChanged,
                    ui->progressBar, &QProgressBar::setRange);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::progressValueChanged,
                    ui->progressBar, &QProgressBar::setValue);
        threadPool.start(worker);

        //----------------------------------------------------------------------
        // Clear any previous entries
        clear();

        //----------------------------------------------------------------------
        ui->progressBar->show();
        ui->hostComboBox->setEnabled(false);
        ui->databaseComboBox->setEnabled(false);

    }
}

void repo::gui::RepoDialogUserManager::removeItem()
{
    repo::core::model::RepoUser user = this->getUser();
    switch(QMessageBox::warning(this,
                                tr("Remove user?"),
                                tr("Are you sure you want to remove '") + QString::fromStdString(user.getUserName()) + "'?",
                                tr("&Yes"),
                                tr("&No"),
                                QString::null, 1, 1))
    {
    case 0: // yes
        refresh(user, repo::worker::UsersWorker::Command::DROP);
        //FIXME get a worker to do the work, then signal finish to refresh like db widget.
        break;
    case 1: // no
        std::cout << "Remove user warning box cancelled by user." << std::endl;
        break;
    }
}

void repo::gui::RepoDialogUserManager::showEditDialog(const repo::core::model::RepoUser &user )
{
    RepoDialogUser userDialog(dbCache->getConnection(ui->hostComboBox->currentText()),
                              controller, user, databasesWithProjects, customRolesList, this);
    if (QDialog::Rejected == userDialog.exec())
    {
        repoLog("User profile dialog cancelled by user.\n");
        std::cout << tr("User profile dialog cancelled by user.").toStdString() << std::endl;
    }
    else // QDialog::Accepted
    {
        repoLog("create or update user...\n");
        // Create or update user
        refresh(userDialog.getUpdatedUser(),
                userDialog.isNewUser()
                ? repo::worker::UsersWorker::Command::INSERT
                : repo::worker::UsersWorker::Command::UPDATE);
    }
}
