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


#include <QMessageBox>
#include "repodialogusermanager.h"
#include "ui_repodialogusermanager.h"
#include "../primitives/repo_fontawesome.h"
#include "../workers/repo_workerusers.h"

repo::gui::RepoDialogUserManager::RepoDialogUserManager(
        const core::MongoClientWrapper& mongo,
        const std::string &database,
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RepoDialogUserManager)
    , mongo(mongo)
    , database(database)
{
    ui->setupUi(this);
    setWindowIcon(getIcon());

    //--------------------------------------------------------------------------
    // Add DB connections to selector
    ui->hostComboBox->addItem(
                RepoFontAwesome::getHostIcon(),
                QString::fromStdString(mongo.getHostAndPort()));

    ui->databaseComboBox->addItem(
                RepoFontAwesome::getDatabaseIcon(),
                "admin");


    //--------------------------------------------------------------------------
    // Users
    usersModel = new QStandardItemModel(this);
    usersModel->setColumnCount(8);
    usersModel->setHeaderData(
                Columns::ACTIVE,
                Qt::Horizontal,
                tr("Active"));
    usersModel->setHeaderData(
                Columns::USERNAME,
                Qt::Horizontal,
                tr("Username"));
    usersModel->setHeaderData(
                Columns::FIRST_NAME,
                Qt::Horizontal,
                tr("First Name"));
    usersModel->setHeaderData(
                Columns::LAST_NAME,
                Qt::Horizontal,
                tr("Last Name"));
    usersModel->setHeaderData(
                Columns::EMAIL,
                Qt::Horizontal,
                tr("Email"));
    usersModel->setHeaderData(
                Columns::PROJECTS,
                Qt::Horizontal,
                tr("Projects"));
    usersModel->setHeaderData(
                Columns::GROUPS,
                Qt::Horizontal,
                tr("Groups"));
    usersModel->setHeaderData(
                Columns::ROLES,
                Qt::Horizontal,
                tr("Roles"));

    usersProxy = new QSortFilterProxyModel(this);
    usersProxy->setFilterKeyColumn(-1); // filter all columns
    usersProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    usersProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    usersProxy->setSourceModel(usersModel);
    ui->treeView->setModel(usersProxy);
    ui->treeView->sortByColumn(Columns::USERNAME, Qt::SortOrder::AscendingOrder);
    clearUsersModel();

    //--------------------------------------------------------------------------
    // Connect filtering text input to the filtering proxy model
    QObject::connect(
        ui->filterLineEdit, &QLineEdit::textChanged,
        usersProxy, &QSortFilterProxyModel::setFilterFixedString);

    QObject::connect(
        ui->refreshPushButton, SIGNAL(pressed()),
        this, SLOT(refresh()));

    QObject::connect(
        usersProxy, &QSortFilterProxyModel::rowsInserted,
        this, &RepoDialogUserManager::updateCountLabel);

    QObject::connect(
        usersProxy, &QSortFilterProxyModel::rowsRemoved,
        this, &RepoDialogUserManager::updateCountLabel);

    QObject::connect(
        ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &RepoDialogUserManager::select);

    QObject::connect(ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)),
                     this, SLOT(editUser(const QModelIndex &)));

    QObject::connect(
        ui->addPushButton, SIGNAL(pressed()),
                this, SLOT(showEditDialog()));

    QObject::connect(
        ui->removePushButton, SIGNAL(pressed()),
                this, SLOT(removeItem()));

    QObject::connect(ui->editPushButton, SIGNAL(pressed()),
                     this, SLOT(editUser()));
}

repo::gui::RepoDialogUserManager::~RepoDialogUserManager()
{
    cancelAllThreads();
    delete usersModel;
    delete usersProxy;
    delete ui;
}

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
    usersModel->invisibleRootItem()->appendRow(row);
}

void repo::gui::RepoDialogUserManager::finish()
{
    ui->hostComboBox->setEnabled(true);
    ui->databaseComboBox->setEnabled(true);
}


bool repo::gui::RepoDialogUserManager::cancelAllThreads()
{
    emit cancel();
    return threadPool.waitForDone(); // msecs
}

int repo::gui::RepoDialogUserManager::exec()
{
    refresh();
    return QDialog::exec();
}

void repo::gui::RepoDialogUserManager::editUser()
{
    showEditDialog(getUser());
}

void repo::gui::RepoDialogUserManager::editUser(const QModelIndex &index)
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
    usersModel->removeRows(0, usersModel->rowCount());
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

    usersProxy->clear();
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

//! Selects the data from the given item.
void repo::gui::RepoDialogUserManager::select(
        const QItemSelection &,
        const QItemSelection &)
{
    ui->removePushButton->setEnabled(true);
    ui->editPushButton->setEnabled(true);
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

void repo::gui::RepoDialogUserManager::updateCountLabel() const
{
    ui->countLabel->setText(tr("Showing %1 of %2").arg(usersProxy->rowCount()).arg(usersModel->rowCount()));
}

QStandardItem *repo::gui::RepoDialogUserManager::createItem(const QString &data)
{
    QStandardItem *item = new QStandardItem(data);
    item->setEditable(false);
    item->setToolTip(data);
    return item;
}

QStandardItem *repo::gui::RepoDialogUserManager::createItem(
    const QVariant& data)
{
    QStandardItem* item = new QStandardItem();
    item->setEditable(false);
    item->setTextAlignment(Qt::AlignRight);
    item->setText(data.toString());
    item->setToolTip(data.toString());
    item->setData(data);
    return item;
}

