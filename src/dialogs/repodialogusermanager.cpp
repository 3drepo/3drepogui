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
    //--------------------------------------------------------------------------
    // Icons
    this->setWindowIcon(getIcon());
    ui->addUserPushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_plus,
                                       QColor(Qt::darkGreen)));
    ui->deleteUserPushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_minus,
                                       QColor(Qt::darkRed)));
    ui->addPushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_plus,
                                       QColor(Qt::darkGreen)));
    ui->deletePushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_minus,
                                       QColor(Qt::darkRed)));
    ui->picturePushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_user,
                                       QColor(Qt::gray)));

    //--------------------------------------------------------------------------
    // Users
    usersModel = new QStandardItemModel(this);
    usersModel->setColumnCount(6);
    usersModel->setHeaderData(
                RepoUsersColumns::ACTIVE,
                Qt::Horizontal,
                tr("Active"));
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

    usersProxy = new QSortFilterProxyModel(this);
    usersProxy->setFilterKeyColumn(-1); // filter all columns
    usersProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    usersProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    usersProxy->setSourceModel(usersModel);
    ui->usersTreeView->setModel(usersProxy);
    ui->usersTreeView->sortByColumn(RepoUsersColumns::USERNAME, Qt::SortOrder::AscendingOrder);
    clearUsersModel();

    //--------------------------------------------------------------------------
    // Projects
    projectsModel = new QStandardItemModel(this);
    projectsModel->setColumnCount(2);
    projectsModel->setHeaderData(
                RepoProjectsColumns::OWNER,
                Qt::Horizontal,
                tr("Owner"));
    projectsModel->setHeaderData(
                RepoProjectsColumns::PROJECT,
                Qt::Horizontal,
                tr("Project"));
    ui->projectsTreeView->setModel(projectsModel);
    ui->projectsTreeView->sortByColumn(RepoProjectsColumns::OWNER, Qt::SortOrder::AscendingOrder);

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
}

repo::gui::RepoDialogUserManager::~RepoDialogUserManager()
{
    cancelAllThreads();

    delete usersModel;
    delete usersProxy;

    delete projectsModel;

    delete ui;
}

void repo::gui::RepoDialogUserManager::addUser(const core::RepoUser &user)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // User object itself
    QVariant var;
//    core::RepoUser userCopy(user.copy());
    var.setValue(user.copy());

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
    QString passwordAsBullets;
    for (int i = 0; i < user.getPassword().size(); ++i)
        passwordAsBullets += QChar(0x2022);
    item = createItem(passwordAsBullets);
    item->setData(QVariant(QString::fromStdString(user.getPassword())));
    row.append(item);

    // First Name
    row.append(createItem(QString::fromStdString(user.getFirstName())));

    // Last Name
    row.append(createItem(QString::fromStdString(user.getLastName())));

    // Email
    row.append(createItem(QString::fromStdString(user.getEmail())));
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
    int result = QDialog::exec();
    return result;
}

void repo::gui::RepoDialogUserManager::clearUsersModel()
{
    usersModel->removeRows(0, usersModel->rowCount());
    //--------------------------------------------------------------------------
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::ACTIVE);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::USERNAME);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::PASSWORD);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::FIRST_NAME);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::LAST_NAME);
    ui->usersTreeView->resizeColumnToContents(RepoUsersColumns::EMAIL);
    //--------------------------------------------------------------------------
    ui->filterLineEdit->clear();
}

void repo::gui::RepoDialogUserManager::clearProjectsModel()
{
    projectsModel->removeRows(0, projectsModel->rowCount());
    ui->emailLabel->setText("");
    ui->nameLabel->setText("");
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
        // Clear any previous entries
        clearUsersModel();
        clearProjectsModel();

        //----------------------------------------------------------------------
        threadPool.start(worker);

        ui->picturePushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                           RepoFontAwesome::fa_user,
                                           QColor(Qt::gray)));

    }
}

//! Selects the data from the given item.
void  repo::gui::RepoDialogUserManager::select(
        const QItemSelection &selected,
        const QItemSelection &)
{
    clearProjectsModel();
    QModelIndexList list = selected.indexes();
    for (int i = 0; i < list.size(); ++i)
    {
        QModelIndex selectedIndex = list[i];
        QString text = selectedIndex.data().toString();
        switch (selectedIndex.column())
        {
            case RepoUsersColumns::ACTIVE :
                {
                    core::RepoUser user = selectedIndex.data(Qt::UserRole+1).value<core::RepoUser>();

                    std::vector<std::pair<std::string, std::string> > projects = user.getProjects();
                    for (unsigned int i = 0; i < projects.size(); ++i)
                    {
                        QList<QStandardItem *> row;
                        row.append(createItem(QString::fromStdString(projects[i].first)));
                        row.append(createItem(QString::fromStdString(projects[i].second)));
                        projectsModel->invisibleRootItem()->appendRow(row);
                    }
                }
                break;
        case RepoUsersColumns::USERNAME :
        case RepoUsersColumns::FIRST_NAME :
            if (!text.isEmpty())
                ui->nameLabel->setText(text);
            break;
        case RepoUsersColumns::LAST_NAME :
            if (!text.isEmpty())
                ui->nameLabel->setText(ui->nameLabel->text() + " " + text);
          break;
        case RepoUsersColumns::EMAIL :
            if (!text.isEmpty())
                ui->emailLabel->setText(text);
            break;
        }


    }
}

void repo::gui::RepoDialogUserManager::updateUsersCount() const
{
    ui->usersCountLabel->setText(tr("Showing %1 of %2").arg(usersProxy->rowCount()).arg(usersModel->rowCount()));
}

QStandardItem *repo::gui::RepoDialogUserManager::createItem(const QString &data)
{
    QStandardItem* item = new QStandardItem(data);
    item->setEditable(true);
    item->setToolTip(data);
    return item;
}
