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
#include "ui_repodialogusermanager.h"
#include "../primitives/repo_fontawesome.h"
#include "../workers/repo_workerusers.h"

repo::gui::RepoDialogUserManager::RepoDialogUserManager(
        const core::MongoClientWrapper& mongo,
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RepoDialogUserManager)
    , mongo(mongo)
{
    ui->setupUi(this);
    setWindowIcon(getIcon());

    //--------------------------------------------------------------------------
    // Add DB connections to selector
    ui->comboBox->addItem(
                RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_database),
                QString::fromStdString(mongo.getHostAndPort()));

    //--------------------------------------------------------------------------
    // Users
    usersModel = new QStandardItemModel(this);
    usersModel->setColumnCount(7);
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
                Columns::ROLES,
                Qt::Horizontal,
                tr("Roles"));

    usersProxy = new QSortFilterProxyModel(this);
    usersProxy->setFilterKeyColumn(-1); // filter all columns
    usersProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    usersProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    usersProxy->setSourceModel(usersModel);
    ui->usersTreeView->setModel(usersProxy);
    ui->usersTreeView->sortByColumn(Columns::USERNAME, Qt::SortOrder::AscendingOrder);
    clearUsersModel();

    //--------------------------------------------------------------------------
    // Connect filtering text input to the filtering proxy model
    QObject::connect(
        ui->filterLineEdit, &QLineEdit::textChanged,
        usersProxy, &QSortFilterProxyModel::setFilterFixedString);

    QObject::connect(
        ui->refreshPushButton, &QPushButton::pressed,
        this, &RepoDialogUserManager::refresh);

    QObject::connect(
        usersProxy, &QSortFilterProxyModel::rowsInserted,
        this, &RepoDialogUserManager::updateUsersCount);

    QObject::connect(
        usersProxy, &QSortFilterProxyModel::rowsRemoved,
        this, &RepoDialogUserManager::updateUsersCount);

    QObject::connect(
        ui->usersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &RepoDialogUserManager::select);

    QObject::connect(ui->usersTreeView, SIGNAL(doubleClicked(const QModelIndex &)),
                     this, SLOT(editUser(const QModelIndex &)));

    QObject::connect(
        ui->addUserPushButton, SIGNAL(pressed()),
                this, SLOT(showUserDialog()));

    QObject::connect(ui->editUserPushButton, SIGNAL(pressed()),
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

    // Password
//    QString passwordAsBullets;
//    for (int i = 0; i < user.getPassword().size(); ++i)
//        passwordAsBullets += QChar(0x2022);
    //item = createItem(passwordAsBullets);
    //item->setData(QVariant(QString::fromStdString(user.getPassword())));
//    item = createItem(QString::fromStdString(user.getPassword()));
//    row.append(item);

    // First Name
    row.append(createItem(QString::fromStdString(user.getFirstName())));

    // Last Name
    row.append(createItem(QString::fromStdString(user.getLastName())));

    // Email
    row.append(createItem(QString::fromStdString(user.getEmail())));

    // Projects count
    row.append(createItem(user.getProjects().size()));

    // Roles count
    row.append(createItem(user.getRoles().size()));

    //--------------------------------------------------------------------------
    usersModel->invisibleRootItem()->appendRow(row);
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
    editUser(ui->usersTreeView->selectionModel()->currentIndex());
}

void repo::gui::RepoDialogUserManager::editUser(const QModelIndex &index)
{
    if (index.isValid())
    {
        QModelIndex userIndex = index.sibling(index.row(),Columns::ACTIVE);
        core::RepoUser user = userIndex.data(Qt::UserRole+1).value<core::RepoUser>();
        showUserDialog(user);
    }
}

void repo::gui::RepoDialogUserManager::clearUsersModel()
{
    usersModel->removeRows(0, usersModel->rowCount());
    //--------------------------------------------------------------------------
    ui->usersTreeView->resizeColumnToContents(Columns::ACTIVE);
    ui->usersTreeView->resizeColumnToContents(Columns::USERNAME);
    ui->usersTreeView->resizeColumnToContents(Columns::FIRST_NAME);
    ui->usersTreeView->resizeColumnToContents(Columns::LAST_NAME);
    ui->usersTreeView->resizeColumnToContents(Columns::EMAIL);
    ui->usersTreeView->resizeColumnToContents(Columns::PROJECTS);
    ui->usersTreeView->resizeColumnToContents(Columns::ROLES);
    //--------------------------------------------------------------------------
    ui->filterLineEdit->clear();
    ui->deleteUserPushButton->setEnabled(false);
    ui->editUserPushButton->setEnabled(false);
}

QIcon repo::gui::RepoDialogUserManager::getIcon()
{
   return RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_users);
}

void repo::gui::RepoDialogUserManager::refresh()
{
    if (cancelAllThreads())
    {
        RepoWorkerUsers* worker = new RepoWorkerUsers(mongo);
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

        //----------------------------------------------------------------------
        // Clear any previous entries
        clearUsersModel();        

        //----------------------------------------------------------------------
        threadPool.start(worker);
    }
}

//! Selects the data from the given item.
void repo::gui::RepoDialogUserManager::select(
        const QItemSelection &,
        const QItemSelection &)
{
    ui->deleteUserPushButton->setEnabled(true);
    ui->editUserPushButton->setEnabled(true);
}

void repo::gui::RepoDialogUserManager::showUserDialog(const core::RepoUser &user)
{
    RepoDialogUser userDialog(user, databasesWithProjects, customRolesList, this);
    if (QDialog::Rejected == userDialog.exec())
    {
        std::cout << tr("User profile dialog cancelled by user.").toStdString() << std::endl;
    }
    else // QDialog::Accepted
    {
        // Update existing user
        refresh();
    }
}

void repo::gui::RepoDialogUserManager::updateUsersCount() const
{
    ui->usersCountLabel->setText(tr("Showing %1 of %2").arg(usersProxy->rowCount()).arg(usersModel->rowCount()));
}

QStandardItem *repo::gui::RepoDialogUserManager::createItem(const QString &data)
{
    QStandardItem* item = new QStandardItem(data);
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

