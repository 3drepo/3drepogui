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

//------------------------------------------------------------------------------
// GUI
#include "repodialoguser.h"
#include "ui_repodialoguser.h"
#include "../primitives/repo_fontawesome.h"



repo::gui::RepoDialogUser::RepoDialogUser(
        core::RepoUser user,
        const std::list<std::string> &databaseList,
        QWidget *parent)
    : QDialog(parent)
    , user(user)
    , databaseList(databaseList)
    , ui(new Ui::RepoDialogUser)
{
    ui->setupUi(this);

    setWindowIcon(getIcon());

//    ui->addPushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
//                                       RepoFontAwesome::fa_plus,
//                                       QColor(Qt::darkGreen)));
//    ui->deletePushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
//                                       RepoFontAwesome::fa_minus,
//                                       QColor(Qt::darkRed)));
    ui->avatarPushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_user,
                                       QColor(Qt::gray)));

    databasesDelegate = new RepoComboBoxDelegate(databaseList);
    ui->projectsTreeView->setItemDelegateForColumn(RepoProjectsColumns::OWNER, databasesDelegate);




    ui->rolesTreeWidget->setItemDelegateForColumn(RepoRolesColumns::DATABASE, databasesDelegate);
    ui->rolesTreeWidget->setItemDelegateForColumn(RepoRolesColumns::ROLE, databasesDelegate);

    ui->projectsTreeView->setItemDelegateForColumn(RepoProjectsColumns::PROJECT, databasesDelegate);

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
    // DB Roles
    rolesModel = new QStandardItemModel(this);
    rolesModel->setColumnCount(2);
    rolesModel->setHeaderData(
                RepoProjectsColumns::OWNER,
                Qt::Horizontal,
                tr("Database"));
    rolesModel->setHeaderData(
                RepoProjectsColumns::PROJECT,
                Qt::Horizontal,
                tr("Role"));
    //ui->rolesTreeView->setModel(rolesModel);
    //ui->rolesTreeView->sortByColumn(RepoProjectsColumns::OWNER, Qt::SortOrder::AscendingOrder);

    //--------------------------------------------------------------------------
    // Populate user data
    if (!user.isEmpty())
    {
        ui->usernameLineEdit->setText(QString::fromStdString(user.getUsername()));
        ui->usernameLineEdit->setEnabled(false);
        ui->passwordLineEdit->setText(QString::fromStdString(user.getPassword()));
        ui->firstNameLineEdit->setText(QString::fromStdString(user.getFirstName()));
        ui->lastNameLineEdit->setText(QString::fromStdString(user.getLastName()));
        ui->emailLineEdit->setText(QString::fromStdString(user.getEmail()));

        //----------------------------------------------------------------------
        // Projects
        std::vector<std::pair<std::string, std::string> > projects = user.getProjects();
        this->populateModel(projectsModel, projects);

        //----------------------------------------------------------------------
        // DB Roles
        std::vector<std::pair<std::string, std::string> > roles = user.getRoles();
        this->populateModel(rolesModel, roles);

        for (unsigned int i = 0; i < roles.size(); ++i)
        {
            QStringList list;
            list.append(QString::fromStdString(roles[i].first));
            list.append(QString::fromStdString(roles[i].second));

            QTreeWidgetItem *item = new QTreeWidgetItem(ui->rolesTreeWidget, list);
            item->setData(0, Qt::DecorationRole, QString::fromStdString(roles[i].first));
            item->setData(1, Qt::DecorationRole, QString::fromStdString(roles[i].second));
            item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
            ui->rolesTreeWidget->addTopLevelItem(item);
        }


    }
}

repo::gui::RepoDialogUser::~RepoDialogUser()
{
    delete projectsModel;
    delete rolesModel;
    delete databasesDelegate;
    delete ui;
}

QIcon repo::gui::RepoDialogUser::getIcon()
{
   return RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_user);
}

void repo::gui::RepoDialogUser::populateModel(
        QStandardItemModel *model,
        const std::vector<std::pair<std::string, std::string> > &data)
{
    for (unsigned int i = 0; i < data.size(); ++i)
    {
        QList<QStandardItem *> row;
        row.append(new QStandardItem(QString::fromStdString(data[i].first)));

        QStandardItem *item = new QStandardItem(QString::fromStdString(data[i].second));
        item->setData(QString::fromStdString(data[i].second), Qt::DecorationRole);
        row.append(item);
        model->invisibleRootItem()->appendRow(row);
    }
}

