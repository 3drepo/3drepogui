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
// Qt
#include <QTimer>

//------------------------------------------------------------------------------
// GUI
#include "repodialoguser.h"
#include "ui_repodialoguser.h"
#include "../primitives/repo_fontawesome.h"

#include <RepoWrapperMongo>

repo::gui::RepoDialogUser::RepoDialogUser(
        core::RepoUser user,
        const std::list<std::string> &databaseList,
        const std::list<std::string> &customRolesList,
        QWidget *parent)
    : QDialog(parent)
    , user(user)
    , ui(new Ui::RepoDialogUser)
{
    ui->setupUi(this);
    setWindowIcon(getIcon());
    //--------------------------------------------------------------------------
    ui->avatarPushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_user,
                                       QColor(Qt::gray)));

//    databasesDelegate = new RepoComboBoxDelegate(databaseList);
//    ui->projectsTreeView->setItemDelegateForColumn(RepoProjectsColumns::OWNER, databasesDelegate);
//    ui->projectsTreeView->setItemDelegateForColumn(RepoProjectsColumns::PROJECT, databasesDelegate);

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


//        std::list<std::string> adminRoles = core::MongoClientWrapper::ADMIN_DATABASE_ROLES;
//        adminRoles.insert(adminRoles.end(), core::MongoClientWrapper::DATABASE_ROLES.begin(), core::MongoClientWrapper::DATABASE_ROLES.end());


        RepoComboBoxEditor::SeparatedEntries dbEntries;
        dbEntries << databaseList;


        // TODO: sort alphabetically customRolesList
        RepoComboBoxEditor::SeparatedEntries dbRoleEntries;
        dbRoleEntries << customRolesList << core::MongoClientWrapper::ANY_DATABASE_ROLES;

        //----------------------------------------------------------------------
        // Any DB Roles
        QList<RepoComboBoxEditor::SeparatedEntries> anyDBRolesLists;
        anyDBRolesLists << dbEntries << dbRoleEntries;
        anyDBRolesDelegate = new RepoComboBoxDelegate(anyDBRolesLists);

        //----------------------------------------------------------------------
        // Admin DB Roles (any roles + admin only roles)
        dbRoleEntries << core::MongoClientWrapper::ADMIN_ONLY_DATABASE_ROLES;
        QList<RepoComboBoxEditor::SeparatedEntries> adminDBRolesLists;
        adminDBRolesLists << dbEntries << dbRoleEntries;
        adminDBRolesDelegate = new RepoComboBoxDelegate(adminDBRolesLists);



        //----------------------------------------------------------------------
        // Populate DB Roles
        std::vector<std::pair<std::string, std::string> > roles = user.getRoles();
        for (unsigned int i = 0; i < roles.size(); ++i)
            addRole(roles[i]);

    }

    //--------------------------------------------------------------------------
    // Connections

    QObject::connect(
        ui->rolesTreeWidget, &QTreeWidget::itemChanged,
        this, &RepoDialogUser::rolesItemChanged);

    QObject::connect(
        ui->addPushButton, &QPushButton::pressed,
        this, &RepoDialogUser::addItem);

    QObject::connect(
        ui->removePushButton, &QPushButton::pressed,
        this, &RepoDialogUser::removeItem);

}

repo::gui::RepoDialogUser::~RepoDialogUser()
{
    delete ui;
}

void repo::gui::RepoDialogUser::addItem()
{
   std::string admin = core::MongoClientWrapper::ADMIN_DATABASE;
   QTreeWidgetItem * item = 0;
   switch(ui->accessRightsTabWidget->currentIndex())
   {
        case Tabs::ROLES :
            item = addRole(std::make_pair<std::string, std::string>(""+admin,""));
            ui->rolesTreeWidget->scrollToItem(item);
                    ui->rolesTreeWidget->setCurrentItem(item, RolesColumns::DATABASE);
        break;
   }
}


QTreeWidgetItem * repo::gui::RepoDialogUser::addRole(
        const std::pair<std::string, std::string> &role)
{
    QStringList list;
    list.append(QString::fromStdString(role.first));
    list.append(QString::fromStdString(role.second));

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->rolesTreeWidget, list);
    item->setData(0, Qt::DecorationRole, QString::fromStdString(role.first));
    item->setData(1, Qt::DecorationRole, QString::fromStdString(role.second));
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
    ui->rolesTreeWidget->addTopLevelItem(item);

    RepoComboBoxDelegate *delegate = (core::MongoClientWrapper::ADMIN_DATABASE == role.first)
            ? adminDBRolesDelegate
            : anyDBRolesDelegate;
    ui->rolesTreeWidget->setItemDelegateForRow(ui->rolesTreeWidget->topLevelItemCount() - 1, delegate);

    return item;
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

void repo::gui::RepoDialogUser::removeItem()
{
    QTreeWidgetItem *item = 0;
    switch(ui->accessRightsTabWidget->currentIndex())
    {
         case Tabs::ROLES :
             item = ui->rolesTreeWidget->currentItem();
             if (item)
                delete item;
         break;
    }
}

void repo::gui::RepoDialogUser::rolesItemChanged(QTreeWidgetItem *current, int column)
{
    if (current && RolesColumns::DATABASE == column)
    {
        int row = ui->rolesTreeWidget->indexOfTopLevelItem(current);
        RepoComboBoxDelegate *delegate = (core::MongoClientWrapper::ADMIN_DATABASE == current->data(column, Qt::EditRole).toString().toStdString())
                 ? adminDBRolesDelegate
                 : anyDBRolesDelegate;
        ui->rolesTreeWidget->setItemDelegateForRow(row, delegate);
        ui->rolesTreeWidget->setCurrentItem(current, RolesColumns::ROLE);
    }
}
