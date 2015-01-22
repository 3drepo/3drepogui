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
#include <QMap>

//------------------------------------------------------------------------------
// GUI
#include "repodialoguser.h"
#include "ui_repodialoguser.h"
#include "../primitives/repo_fontawesome.h"

#include <RepoWrapperMongo>

repo::gui::RepoDialogUser::RepoDialogUser(
        core::RepoUser user,
        const std::map<std::string, std::list<std::string> > &databasesWithProjects,
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
    // Databases
    QMap<std::string, std::list<std::string> > databasesMapping(databasesWithProjects);
    std::list<std::string> databases = databasesMapping.keys().toStdList();
    databases.sort(core::MongoClientWrapper::caseInsensitiveStringCompare);

    //--------------------------------------------------------------------------
    // DB Roles
    RepoComboBoxEditor::SeparatedEntries dbEntries;
    dbEntries << databases;
    RepoComboBoxEditor::SeparatedEntries dbRoleEntries;
    dbRoleEntries << customRolesList << core::MongoClientWrapper::ANY_DATABASE_ROLES;

    //--------------------------------------------------------------------------
    // Any DB Roles
    QList<RepoComboBoxEditor::SeparatedEntries> anyDBRolesLists;
    anyDBRolesLists << dbEntries << dbRoleEntries;

    //--------------------------------------------------------------------------
    // Admin DB Roles (any roles + admin only roles)
    dbRoleEntries << core::MongoClientWrapper::ADMIN_ONLY_DATABASE_ROLES;
    QList<RepoComboBoxEditor::SeparatedEntries> adminDBRolesLists;
    adminDBRolesLists << dbEntries << dbRoleEntries;

    //--------------------------------------------------------------------------
    // Populate Delegates
    for (std::list<std::string>::const_iterator it = databases.begin();
         it != databases.end(); ++it)
    {
        std::string database = *it;
        QString qDatabase = QString::fromStdString(database);

        //----------------------------------------------------------------------
        // Projects delegate
        std::list<std::string> projects = databasesWithProjects.find(database)->second;
        RepoComboBoxEditor::SeparatedEntries projectsEntries;
        projectsEntries << projects;
        QList<RepoComboBoxEditor::SeparatedEntries> projectsLists;
        projectsLists << dbEntries << projectsEntries;
        projectsDelegates.insert(qDatabase, new RepoComboBoxDelegate(projectsLists));

        //----------------------------------------------------------------------
        // Roles delegate
        RepoComboBoxDelegate *rolesDelegate =
            (core::MongoClientWrapper::ADMIN_DATABASE == database)
             ? new RepoComboBoxDelegate(adminDBRolesLists)
             : new RepoComboBoxDelegate(anyDBRolesLists);
        rolesDelegates.insert(qDatabase, rolesDelegate);
    }

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
        ui->projectsTreeWidget, &QTreeWidget::itemChanged,
        this, &RepoDialogUser::updateProjectsDelegate);

    QObject::connect(
        ui->rolesTreeWidget, &QTreeWidget::itemChanged,
        this, &RepoDialogUser::updateRolesDelegate);

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
    // Dealocate pointers
    QListIterator<QString> i(rolesDelegates.keys());
    while (i.hasNext())
    {
        QString database = i.next();
        delete projectsDelegates.take(database);
        delete rolesDelegates.take(database);

    }
}

QTreeWidgetItem * repo::gui::RepoDialogUser::addGroup(
        const std::pair<std::string, std::string> &group)
{
    QTreeWidgetItem *item = addItem(
                group,
                ui->groupsTreeWidget,
                groupsDelegates);
    ui->groupsTreeWidget->scrollToItem(item);
    ui->groupsTreeWidget->setCurrentItem(item, Columns::DATABASE);
    return item;
}


QTreeWidgetItem * repo::gui::RepoDialogUser::addItem()
{
    std::string admin = core::MongoClientWrapper::ADMIN_DATABASE;
    QTreeWidgetItem * item = 0;
    std::pair<std::string, std::string> emptyPair = std::make_pair(""+admin,"");
    switch(ui->accessRightsTabWidget->currentIndex())
    {
    case Tabs::PROJECTS :
       item = addProject(emptyPair);
       break;
    case Tabs::GROUPS :
        item = addGroup(emptyPair);
        break;
    case Tabs::ROLES :
        item = addRole(emptyPair);
        break;
    }
    return item;
}

QTreeWidgetItem * repo::gui::RepoDialogUser::addItem(
    const std::pair<std::string, std::string> &pair,
    QTreeWidget *parent,
    const QHash<QString, RepoComboBoxDelegate* > &delegates)
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
        RepoComboBoxDelegate* delegate = delegates.value(database);
        parent->setItemDelegateForRow(parent->topLevelItemCount() - 1, delegate );
    }
    return item;
}

QTreeWidgetItem * repo::gui::RepoDialogUser::addProject(
        const std::pair<std::string, std::string> &project)
{
    QTreeWidgetItem *item = addItem(
                project,
                ui->projectsTreeWidget,
                projectsDelegates);
    ui->projectsTreeWidget->scrollToItem(item);
    ui->projectsTreeWidget->setCurrentItem(item, Columns::DATABASE);
    return item;
}

QTreeWidgetItem * repo::gui::RepoDialogUser::addRole(
        const std::pair<std::string, std::string> &role)
{    
    QTreeWidgetItem * item = addItem(
                role,
                ui->rolesTreeWidget,
                rolesDelegates);
    ui->rolesTreeWidget->scrollToItem(item);
    ui->rolesTreeWidget->setCurrentItem(item, Columns::DATABASE);
    return item;
}

QIcon repo::gui::RepoDialogUser::getIcon()
{
   return RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_user);
}

void repo::gui::RepoDialogUser::removeItem()
{
    QTreeWidgetItem *item = 0;
    switch(ui->accessRightsTabWidget->currentIndex())
    {
    case Tabs::PROJECTS :
        item = ui->projectsTreeWidget->currentItem();
        break;
    case Tabs::GROUPS :
        item = ui->groupsTreeWidget->currentItem();
        break;
    case Tabs::ROLES :
        item = ui->rolesTreeWidget->currentItem();
        break;
    }
    if (item)
       delete item;
}

void repo::gui::RepoDialogUser::updateProjectsDelegate(QTreeWidgetItem *current, int column)
{
    if (current && Columns::DATABASE == column)
    {
        int row = ui->projectsTreeWidget->indexOfTopLevelItem(current);
        QString database = current->data(column, Qt::EditRole).toString();
        if (projectsDelegates.contains(database))
        {
            RepoComboBoxDelegate* delegate = projectsDelegates.value(database);
            ui->projectsTreeWidget->setItemDelegateForRow(row, delegate);
        }
        ui->projectsTreeWidget->setCurrentItem(current, Columns::VALUE);
    }
}

void repo::gui::RepoDialogUser::updateRolesDelegate(QTreeWidgetItem *current, int column)
{
    if (current && Columns::DATABASE == column)
    {
        int row = ui->rolesTreeWidget->indexOfTopLevelItem(current);        
        QString database = current->data(column, Qt::EditRole).toString();
        if (rolesDelegates.contains(database))
        {
            RepoComboBoxDelegate* delegate = rolesDelegates.value(database);
            ui->rolesTreeWidget->setItemDelegateForRow(row, delegate);
        }
        ui->rolesTreeWidget->setCurrentItem(current, Columns::VALUE);
    }
}
