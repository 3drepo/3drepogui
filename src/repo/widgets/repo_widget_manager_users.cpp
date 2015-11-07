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

#include "repo_widget_manager_users.h"

using namespace repo::widgets;


RepoWidgetManagerUsers::RepoWidgetManagerUsers(QWidget *parent)
    : RepoWidgetTreeEditable(parent)
    , token(nullptr)
    , controller(nullptr)
{
    QList<QString> headers;
    headers << tr("Username") << tr("First Name") << tr("Last Name");
    headers << tr("Email") << tr("Projects") << tr("Roles");

    RepoWidgetTreeFilterable *filterableTree = getFilterableTree();
    filterableTree->setHeaders(headers);
    filterableTree->setRootIsDecorated(false);

    clear();
}

RepoWidgetManagerUsers::~RepoWidgetManagerUsers() {}

void RepoWidgetManagerUsers::addCustomRoles(const std::list<std::string> &list)
{
    customRolesList.clear();
    customRolesList = list;
    customRolesList.sort();
}

void RepoWidgetManagerUsers::addDatabasesWithProjects(
        const std::map<std::string, std::list<std::string> > &mapping)
{
    databasesWithProjects.clear();
    databasesWithProjects = mapping;
}

void RepoWidgetManagerUsers::addUser(const repo::core::model::RepoUser &user)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // User object itself
    QVariant var;
    var.setValue(user);

    repo::primitives::RepoStandardItem *item =
            new repo::primitives::RepoStandardItem(user.getUserName());
    item->setData(var);
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);
    item->setTristate(false);
    row.append(item);

    // First Name
    row.append(new primitives::RepoStandardItem(user.getFirstName()));

    // Last Name
    row.append(new primitives::RepoStandardItem(user.getLastName()));

    // Email
    row.append(new primitives::RepoStandardItem(user.getEmail()));

    // Projects count
    row.append(new primitives::RepoStandardItem(user.getProjectsList().size()));

    // Roles count
    row.append(new primitives::RepoStandardItem(user.getRolesList().size()));

    //--------------------------------------------------------------------------
    getFilterableTree()->addTopLevelRow(row);
}

void RepoWidgetManagerUsers::edit()
{
    showEditDialog(getUser());
}

void RepoWidgetManagerUsers::edit(const QModelIndex &index)
{
    showEditDialog(getUser(index));
}

repo::core::model::RepoUser RepoWidgetManagerUsers::getUser()
{
    return getUser(getFilterableTree()->getCurrentIndex());
}

repo::core::model::RepoUser RepoWidgetManagerUsers::getUser(
        const QModelIndex &index)
{
    repo::core::model::RepoUser user;
    if (index.isValid())
    {
        QModelIndex userIndex = index.sibling(index.row(), (int) Columns::USERNAME);
        user = userIndex.data(Qt::UserRole + 1).value<repo::core::model::RepoUser>();
    }
    return user;
}

void RepoWidgetManagerUsers::refresh(
        const repo::core::model::RepoUser& user,
        const repo::worker::UsersWorker::Command& command)
{
    if (mutex.tryLock() && cancelAllThreads())
    {
        QProgressBar* progressBar = getFilterableTree()->getProgressBar();

        repo::worker::UsersWorker* worker =
                new repo::worker::UsersWorker(token, controller, database, user, command);
        worker->setAutoDelete(true);

        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
                    this, &RepoWidgetManagerUsers::cancel,
                    worker, &repo::worker::UsersWorker::cancel, Qt::DirectConnection);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::userFetched,
                    this, &RepoWidgetManagerUsers::addUser);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::databasesWithProjectsFetched,
                    this, &RepoWidgetManagerUsers::addDatabasesWithProjects);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::customRolesFetched,
                    this, &RepoWidgetManagerUsers::addCustomRoles);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::finished,
                    progressBar, &QProgressBar::hide);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::finished,
                    this, &RepoWidgetManagerUsers::unlockMutex);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::finished,
                    this, &RepoWidgetManagerUsers::finish);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::progressRangeChanged,
                    progressBar, &QProgressBar::setRange);

        QObject::connect(
                    worker, &repo::worker::UsersWorker::progressValueChanged,
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

void RepoWidgetManagerUsers::removeItem()
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
        //FIXME: get a worker to do the work, then signal finish to refresh like db widget.
        break;
    case 1: // no
        std::cout << "Remove user warning box cancelled by user." << std::endl;
        break;
    }
}

void RepoWidgetManagerUsers::showEditDialog(const repo::core::model::RepoUser &user )
{
    repo::gui::RepoDialogUser userDialog(
                token,
                controller,
                user,
                databasesWithProjects,
                customRolesList,
                this);
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


void  RepoWidgetManagerUsers::setDBConnection(
        repo::RepoController *controller,
        const repo::RepoToken* token,
        const std::string& database)
{
    this->controller = controller;
    this->token = token;
    this->database = database;
}

