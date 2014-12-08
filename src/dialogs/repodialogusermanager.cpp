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
    this->setWindowIcon(getIcon());

    ui->addUserToolButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_plus,
                                       QColor(Qt::darkGreen)));

    ui->deleteUserToolButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_minus,
                                       QColor(Qt::darkRed)));

    //--------------------------------------------------------------------------
    usersModel = new QStandardItemModel(this);
    usersModel->setColumnCount(5);
    usersModel->setHeaderData(
                RepoUsersColumns::USERNAME,
                Qt::Horizontal,
                tr("Username"));
    usersModel->setHeaderData(
                RepoUsersColumns::PASSWORD,
                Qt::Horizontal,
                tr("Password"));
    usersModel->setHeaderData(
                RepoUsersColumns::FIRST_NAME,
                Qt::Horizontal,
                tr("First Name"));
    usersModel->setHeaderData(
                RepoUsersColumns::LAST_NAME,
                Qt::Horizontal,
                tr("Last Name"));
    usersModel->setHeaderData(
                RepoUsersColumns::EMAIL,
                Qt::Horizontal,
                tr("Email"));

    //--------------------------------------------------------------------------
    usersProxy = new QSortFilterProxyModel(this);
    usersProxy->setFilterKeyColumn(-1); // filter all columns
    usersProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    usersProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    usersProxy->setSourceModel(usersModel);
    //--------------------------------------------------------------------------
    ui->usersTreeView->setModel(usersProxy);
    ui->usersTreeView->sortByColumn(RepoUsersColumns::USERNAME, Qt::SortOrder::AscendingOrder);
    //--------------------------------------------------------------------------
    clearUsersModel();

    //--------------------------------------------------------------------------
    // Connect filtering text input to the filtering proxy model
    QObject::connect(
        ui->filterLineEdit, &QLineEdit::textChanged,
        usersProxy, &QSortFilterProxyModel::setFilterFixedString);
    //--------------------------------------------------------------------------
    QObject::connect(
        ui->refreshPushButton, &QPushButton::pressed,
        this, &RepoDialogUserManager::refresh);

    QObject::connect(
        usersProxy, &QSortFilterProxyModel::rowsInserted,
        this, &RepoDialogUserManager::updateUsersCount);

    QObject::connect(
        usersProxy, &QSortFilterProxyModel::rowsRemoved,
        this, &RepoDialogUserManager::updateUsersCount);
}

repo::gui::RepoDialogUserManager::~RepoDialogUserManager()
{
    cancelAllThreads();

    delete usersModel;
    delete usersProxy;

    delete ui;
}

void repo::gui::RepoDialogUserManager::addUser(
        QVariant username,
        QVariant password,
        QVariant firstName,
        QVariant lastName,
        QVariant email)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    row.append(createItem(username));
    row.append(createItem(password));
    row.append(createItem(firstName));
    row.append(createItem(lastName));
    row.append(createItem(email));
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

void repo::gui::RepoDialogUserManager::clearUsersModel()
{
    usersModel->removeRows(0, usersModel->rowCount());
    //--------------------------------------------------------------------------
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::USERNAME);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::PASSWORD);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::FIRST_NAME);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::LAST_NAME);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::EMAIL);
    //--------------------------------------------------------------------------
    ui->filterLineEdit->clear();

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

        //----------------------------------------------------------------------
        // Clear any previous entries : the collection model
        clearUsersModel();

        //----------------------------------------------------------------------
        threadPool.start(worker);
    }
}

void repo::gui::RepoDialogUserManager::updateUsersCount() const
{
    ui->usersCountLabel->setText(tr("Showing %1 of %2").arg(usersProxy->rowCount()).arg(usersModel->rowCount()));
}

QStandardItem *repo::gui::RepoDialogUserManager::createItem(QVariant& data)
{
    QStandardItem* item = new QStandardItem(data.toString());
    item->setEditable(false);
    item->setToolTip(data.toString());
    item->setData(data);

    return item;
}
