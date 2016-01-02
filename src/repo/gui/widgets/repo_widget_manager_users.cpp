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

using namespace repo::gui::widget;
using namespace repo::gui;

const QString UsersManagerWidget::COLUMNS_SETTINGS = "UsersManagerWidgetHeaders";

UsersManagerWidget::UsersManagerWidget(QWidget *parent)
    : EditableTreeWidget(parent)
    , token(nullptr)
    , controller(nullptr)
{
    QList<QString> headers = {
        tr("Username"),
        tr("First Name"),
        tr("Last Name"),
        tr("Email"),
        tr("Roles"),
        tr("API Keys")};

    FilterableTreeWidget *filterableTree = getFilterableTree();
    filterableTree->restoreHeaders(headers, COLUMNS_SETTINGS);
    filterableTree->setRootIsDecorated(false);

    clear();
}

UsersManagerWidget::~UsersManagerWidget()
{
    getFilterableTree()->storeHeaders(COLUMNS_SETTINGS);
}

void UsersManagerWidget::addCustomRoles(const std::list<std::string> &list)
{
    customRolesList.clear();
    customRolesList = list;
    customRolesList.sort();
}

void UsersManagerWidget::addUser(const repo::core::model::RepoUser &user)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // User object itself
    QVariant var;
    var.setValue(user);

    repo::gui::primitive::RepoStandardItem *item =
            new repo::gui::primitive::RepoStandardItem(user.getUserName());

    item->setData(var);
//    item->setCheckable(true);
//    item->setCheckState(Qt::Checked);
//    item->setTristate(false);

    // Add avatar image if present
    const std::vector<char> image = user.getAvatarAsRawData();
    if (image.size())
    {
        QImage qimage = QImage::fromData((uchar*)&(image.at(0)), image.size());
        item->setIcon(QIcon(QPixmap::fromImage(qimage.scaled(QSize(32,32), Qt::IgnoreAspectRatio))));
    }
    else
    {
        QPixmap px(32,32);
        px.fill(QColor(0,0,0,0));
        item->setIcon(QIcon(px));
    }
    row.append(item);

    // First Name
    row.append(new repo::gui::primitive::RepoStandardItem(user.getFirstName()));

    // Last Name
    row.append(new repo::gui::primitive::RepoStandardItem(user.getLastName()));

    // Email
    row.append(new repo::gui::primitive::RepoStandardItem(user.getEmail()));

    // Roles count
    row.append(new repo::gui::primitive::RepoStandardItem(user.getRolesList().size()));

    // API Keys count
    row.append(new repo::gui::primitive::RepoStandardItem(user.getAPIKeysList().size()));

    //--------------------------------------------------------------------------
    getFilterableTree()->addTopLevelRow(row);
}

void UsersManagerWidget::edit()
{
    showEditDialog(getUser(), EditableTreeWidget::Action::EDIT);
}

void UsersManagerWidget::edit(const QModelIndex &index)
{
    showEditDialog(getUser(index), EditableTreeWidget::Action::EDIT);
}

repo::core::model::RepoUser UsersManagerWidget::getUser() const
{
    return getUser(getFilterableTree()->getCurrentIndex());
}

repo::core::model::RepoUser UsersManagerWidget::getUser(
        const QModelIndex &index) const
{
    repo::core::model::RepoUser user;
    if (index.isValid())
    {
        QModelIndex userIndex = index.sibling(index.row(), (int) Columns::USERNAME);
        user = userIndex.data(Qt::UserRole + 1).value<repo::core::model::RepoUser>();
    }
    return user;
}

void UsersManagerWidget::refresh(
        const repo::core::model::RepoUser& user,
        const repo::worker::UsersWorker::Command& command)
{
    if (isReady())
    {
        // Clear any previous entries
        clear();

        repo::worker::UsersWorker* worker = new repo::worker::UsersWorker(
                    token,
                    controller,
                    database,
                    user,
                    command);
        QObject::connect(
                    worker, &repo::worker::UsersWorker::userFetched,
                    this, &UsersManagerWidget::addUser);
        QObject::connect(
                    worker, &repo::worker::UsersWorker::customRolesFetched,
                    this, &UsersManagerWidget::addCustomRoles);
        connectAndStartWorker(worker, getFilterableTree()->getProgressBar());
    }
}

void UsersManagerWidget::removeItem()
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

void UsersManagerWidget::showEditDialog(
        const repo::core::model::RepoUser &user,
        Action action)
{
    dialog::UserDialog userDialog(
                token,
                controller,
                user,
                databasesWithProjects,
                customRolesList,
                action == EditableTreeWidget::Action::COPY,
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
                action == EditableTreeWidget::Action::ADD ||
                action == EditableTreeWidget::Action::COPY
                ? repo::worker::UsersWorker::Command::INSERT
                : repo::worker::UsersWorker::Command::UPDATE);
    }
}

void UsersManagerWidget::setDatabasesWithProjects(
        const std::map<std::string, std::list<std::string> > &rdwp)
{
    this->databasesWithProjects.clear();
    this->databasesWithProjects = rdwp;
}


void  UsersManagerWidget::setDBConnection(
        repo::RepoController *controller,
        const repo::RepoToken* token,
        const std::string& database)
{
    this->controller = controller;
    this->token = token;
    this->database = database;
}

