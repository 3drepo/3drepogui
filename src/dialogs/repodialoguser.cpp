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
        // DB Roles
        RepoComboBoxEditor::SeparatedEntries dbEntries;
        dbEntries << databaseList;

        RepoComboBoxEditor::SeparatedEntries dbRoleEntries;
        dbRoleEntries << customRolesList << core::MongoClientWrapper::ANY_DATABASE_ROLES;

        //----------------------------------------------------------------------
        // Any DB Roles
        QList<RepoComboBoxEditor::SeparatedEntries> anyDBRolesLists;
        anyDBRolesLists << dbEntries << dbRoleEntries;
        RepoComboBoxDelegate *anyDBRolesDelegate = new RepoComboBoxDelegate(anyDBRolesLists);
        //----------------------------------------------------------------------
        // Admin DB Roles (any roles + admin only roles)
        dbRoleEntries << core::MongoClientWrapper::ADMIN_ONLY_DATABASE_ROLES;
        QList<RepoComboBoxEditor::SeparatedEntries> adminDBRolesLists;
        adminDBRolesLists << dbEntries << dbRoleEntries;
        RepoComboBoxDelegate *adminDBRolesDelegate = new RepoComboBoxDelegate(adminDBRolesLists);


        //----------------------------------------------------------------------
        // Populate Delegates
        for (std::list<std::string>::const_iterator it = databaseList.begin();
             it != databaseList.end(); ++it)
        {
            std::string database = *it;
            std::cerr <<database << std::endl;
            RepoComboBoxDelegate *delegate =
                (core::MongoClientWrapper::ADMIN_DATABASE == database)
                 ? adminDBRolesDelegate
                 : anyDBRolesDelegate;
            rolesDelegates.insert(QString::fromStdString(database), &delegate);
        }

        //----------------------------------------------------------------------
        // Projects
        std::vector<std::pair<std::string, std::string> > projects = user.getProjects();
        for (unsigned int i = 0; i < projects.size(); ++i)
            addProject(projects[i]);

        //----------------------------------------------------------------------
        // Populate Roles
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
        ui->addPushButton, SIGNAL(pressed()),
        this, SLOT(addItem()));

    QObject::connect(
        ui->removePushButton, &QPushButton::pressed,
        this, &RepoDialogUser::removeItem);

}

repo::gui::RepoDialogUser::~RepoDialogUser()
{
    delete ui;

    //--------------------------------------------------------------------------
    // Dealocate shared pointers
//    projectsDelegates.clear();
//    rolesDelegates.clear();
}

QTreeWidgetItem * repo::gui::RepoDialogUser::addItem()
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

   return item;
}

QTreeWidgetItem * repo::gui::RepoDialogUser::addItem(
    const std::pair<std::string, std::string> &pair,
    QTreeWidget *parent,
    const QHash<QString, RepoComboBoxDelegate** > &delegates)
{
    QStringList list;
    list.append(QString::fromStdString(pair.first));
    list.append(QString::fromStdString(pair.second));

    QTreeWidgetItem *item = new QTreeWidgetItem(parent, list);
    item->setData(0, Qt::DecorationRole, QString::fromStdString(pair.first));
    item->setData(1, Qt::DecorationRole, QString::fromStdString(pair.second));
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
    parent->addTopLevelItem(item);

    QString database = QString::fromStdString(pair.first);
    if (delegates.contains(database))
    {
        RepoComboBoxDelegate** delegate = delegates.value(database);
        parent->setItemDelegateForRow(parent->topLevelItemCount() - 1, *delegate );
    }
    return item;
}



QTreeWidgetItem * repo::gui::RepoDialogUser::addProject(
        const std::pair<std::string, std::string> &project)
{
    return addItem(
                project,
                ui->projectsTreeWidget,
                projectsDelegates);
}

QTreeWidgetItem * repo::gui::RepoDialogUser::addRole(
        const std::pair<std::string, std::string> &role)
{    
    return addItem(
                role,
                ui->rolesTreeWidget,
                rolesDelegates);
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
        QString database = current->data(column, Qt::EditRole).toString();
        if (rolesDelegates.contains(database))
        {
            RepoComboBoxDelegate** delegate = rolesDelegates.value(database);
            ui->rolesTreeWidget->setItemDelegateForRow(row, *delegate);
        }
        ui->rolesTreeWidget->setCurrentItem(current, RolesColumns::ROLE);
    }
}
