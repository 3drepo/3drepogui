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

#include "../primitives/repo_fontawesome.h"
#include "../workers/repo_workerusers.h"
#include "ui_repoabstractmanagerdialog.h"

repo::gui::RepoDialogUserManager::RepoDialogUserManager(
        const core::MongoClientWrapper& mongo,
        const std::string &database,
        QWidget *parent)
    : RepoAbstractManagerDialog(mongo, database, parent)
{
    setWindowIcon(getIcon());

    //--------------------------------------------------------------------------
    // Users
    model->setColumnCount(8);
    model->setHeaderData(
                Columns::ACTIVE,
                Qt::Horizontal,
                tr("Active"));
    model->setHeaderData(
                Columns::USERNAME,
                Qt::Horizontal,
                tr("Username"));
    model->setHeaderData(
                Columns::FIRST_NAME,
                Qt::Horizontal,
                tr("First Name"));
    model->setHeaderData(
                Columns::LAST_NAME,
                Qt::Horizontal,
                tr("Last Name"));
    model->setHeaderData(
                Columns::EMAIL,
                Qt::Horizontal,
                tr("Email"));
    model->setHeaderData(
                Columns::PROJECTS,
                Qt::Horizontal,
                tr("Projects"));
    model->setHeaderData(
                Columns::GROUPS,
                Qt::Horizontal,
                tr("Groups"));
    model->setHeaderData(
                Columns::ROLES,
                Qt::Horizontal,
                tr("Roles"));

    ui->treeView->sortByColumn(Columns::USERNAME, Qt::SortOrder::AscendingOrder);
    clearUsersModel();
}

repo::gui::RepoDialogUserManager::~RepoDialogUserManager() {}

void repo::gui::RepoDialogUserManager::addCustomRoles(const std::list<std::string> &list)
{
    customRolesList.clear();
    customRolesList = list;
    customRolesList.sort(core::MongoClientWrapper::caseInsensitiveStringCompare);
}

void repo::gui::RepoDialogUserManager::addDatabasesWithProjects(
        const std::map<std::string, std::list<std::string> > &mapping)
{
    databasesWithProjects.clear();
    databasesWithProjects = mapping;
}

void repo::gui::RepoDialogUserManager::addUser(const core::RepoUser &user)
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
    row.append(createItem(QString::fromStdString(user.getUsername())));

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

repo::core::RepoUser repo::gui::RepoDialogUserManager::getUser()
{
    return getUser(ui->treeView->selectionModel()->currentIndex());
}

repo::core::RepoUser repo::gui::RepoDialogUserManager::getUser(const QModelIndex &index)
{
    core::RepoUser user;
    if (index.isValid())
    {
        QModelIndex userIndex = index.sibling(index.row(), Columns::ACTIVE);
        user = userIndex.data(Qt::UserRole+1).value<core::RepoUser>();
    }
    return user;
}

void repo::gui::RepoDialogUserManager::clearUsersModel()
{
    model->removeRows(0, model->rowCount());
    //--------------------------------------------------------------------------
    ui->treeView->resizeColumnToContents(Columns::ACTIVE);
    ui->treeView->resizeColumnToContents(Columns::USERNAME);
    ui->treeView->resizeColumnToContents(Columns::FIRST_NAME);
    ui->treeView->resizeColumnToContents(Columns::LAST_NAME);
    ui->treeView->resizeColumnToContents(Columns::EMAIL);
    ui->treeView->resizeColumnToContents(Columns::PROJECTS);
    ui->treeView->resizeColumnToContents(Columns::GROUPS);
    ui->treeView->resizeColumnToContents(Columns::ROLES);
    //--------------------------------------------------------------------------
    ui->filterLineEdit->clear();
    ui->removePushButton->setEnabled(false);
    ui->editPushButton->setEnabled(false);

    proxy->clear();
    updateCountLabel();
}

QIcon repo::gui::RepoDialogUserManager::getIcon()
{
   return RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_users);
}

void repo::gui::RepoDialogUserManager::refresh(const core::RepoBSON &command)
{
    if (cancelAllThreads())
    {
        RepoWorkerUsers* worker = new RepoWorkerUsers(mongo, database, command);
        worker->setAutoDelete(true);

        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
            this, &RepoDialogUserManager::cancel,
            worker, &RepoWorkerUsers::cancel, Qt::DirectConnection);

        QObject::connect(
            worker, &RepoWorkerUsers::userFetched,
            this, &RepoDialogUserManager::addUser);

        QObject::connect(
            worker, &RepoWorkerUsers::databasesWithProjectsFetched,
            this, &RepoDialogUserManager::addDatabasesWithProjects);

        QObject::connect(
            worker, &RepoWorkerUsers::customRolesFetched,
            this, &RepoDialogUserManager::addCustomRoles);

        QObject::connect(
            worker, &RepoWorkerUsers::finished,
            ui->progressBar, &QProgressBar::hide);

        QObject::connect(
            worker, &RepoWorkerUsers::finished,
            this, &RepoDialogUserManager::finish);

        QObject::connect(
            worker, &RepoWorkerUsers::progressRangeChanged,
            ui->progressBar, &QProgressBar::setRange);

        QObject::connect(
            worker, &RepoWorkerUsers::progressValueChanged,
            ui->progressBar, &QProgressBar::setValue);

        //----------------------------------------------------------------------
        // Clear any previous entries
        clearUsersModel();

        //----------------------------------------------------------------------
        ui->progressBar->show();
        ui->hostComboBox->setEnabled(false);
        ui->databaseComboBox->setEnabled(false);
        threadPool.start(worker);
    }
}

void repo::gui::RepoDialogUserManager::removeItem()
{
    core::RepoUser user = this->getUser();
    switch(QMessageBox::warning(this,
        tr("Remove user?"),
        tr("Are you sure you want to remove '") + QString::fromStdString(user.getUsername())  + tr("' user?"),
        tr("&Yes"),
        tr("&No"),
        QString::null, 1, 1))
        {
            case 0: // yes
                refresh(user.dropUser());
                break;
            case 1: // no
                std::cout << tr("Remove user warning box cancelled by user.").toStdString() << std::endl;
                break;
        }
}

void repo::gui::RepoDialogUserManager::showEditDialog(const core::RepoUser &user)
{
    RepoDialogUser userDialog(user, databasesWithProjects, customRolesList, this);
    if (QDialog::Rejected == userDialog.exec())
    {
        std::cout << tr("User profile dialog cancelled by user.").toStdString() << std::endl;
    }
    else // QDialog::Accepted
    {
        // Create or update user
        refresh(userDialog.getCommand());
    }
}
