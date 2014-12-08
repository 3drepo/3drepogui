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

repo::gui::RepoDialogUserManager::RepoDialogUserManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoDialogUserManager)
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
    ui->usersTreeView->setModel(usersModel);
    ui->usersTreeView->sortByColumn(RepoUsersColumns::USERNAME);
    //--------------------------------------------------------------------------
    clearUsersModel();
}

repo::gui::RepoDialogUserManager::~RepoDialogUserManager()
{
    cancelAllThreads();

    delete usersModel;
    delete usersProxy;

    delete ui;
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
    ui->usersCountLabel->setText(tr("Showing %1 of %2").arg(0).arg(0));

}

QIcon repo::gui::RepoDialogUserManager::getIcon()
{
   return RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_users);
}

void repo::gui::RepoDialogUserManager::refresh()
{
//	if (!database.isEmpty() && cancelAllThreads())
//	{
//		RepoWorkerHistory* worker = new RepoWorkerHistory(mongo, database);
//		worker->setAutoDelete(true);

//		// Direct connection ensures cancel signal is processed ASAP
//		QObject::connect(
//			this, &RepoDialogHistory::cancel,
//			worker, &RepoWorkerHistory::cancel, Qt::DirectConnection);

//		QObject::connect(
//			worker, &RepoWorkerHistory::revisionFetched,
//			this, &RepoDialogHistory::addRevision);//, Qt::BlockingQueuedConnection);

//        //----------------------------------------------------------------------
//		// Clear any previous entries : the collection model
//		clearHistoryModel();

//        //----------------------------------------------------------------------
//		threadPool.start(worker);
//	}
}
