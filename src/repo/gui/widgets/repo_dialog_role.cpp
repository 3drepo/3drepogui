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


#include "repo_dialog_role.h"
#include "ui_repo_dialog_role.h"

using namespace repo::widgets;
using namespace repo::gui;

RepoDialogRole::RepoDialogRole(
        const repo::core::model::RepoRole &role,
        const repo::core::model::RepoRoleSettings &settings,
        const QString &currentDatabase,
        const std::map<std::string, std::list<std::string> > &databasesWithProjects,
        const bool isCopy,
        QWidget *parent)
    : QDialog(parent)
    , oldRole(role)
    , settings(settings)
    , databasesWithProjects(databasesWithProjects)
    , ui(new Ui::RepoDialogRole)
    , rwSeparatedEntries(repo::gui::primitive::RepoComboBoxEditor::getSeparatedEntries({
	tr("Read").toStdString(),
	tr("Write").toStdString(),
	tr("ReadWrite").toStdString() }))
{

	
    ui->setupUi(this);



    //--------------------------------------------------------------------------
    // Permissions
    ui->permissionsUnfilterableTree->setHeaders({tr("Project"), tr("Permission")});
    ui->permissionsUnfilterableTree->registerTabWidget(ui->tabWidget, (int)Tab::PERMISSIONS);
    QHash<QString, repo::gui::primitive::RepoComboBoxDelegate *> permissionsDelegates;
    for (std::pair<std::string, std::list<std::string> > pair : databasesWithProjects)
    {
        QList<repo::gui::primitive::RepoComboBoxEditor::SeparatedEntries> rwSEList =
        { repo::gui::primitive::RepoComboBoxEditor::getSeparatedEntries(pair.second),
          rwSeparatedEntries};
        permissionsDelegates.insert(
                    QString::fromStdString(pair.first),
                    new repo::gui::primitive::RepoComboBoxDelegate(rwSEList));
    }
    ui->permissionsUnfilterableTree->setDelegates(permissionsDelegates);
    for (repo::core::model::RepoPermission p : role.getProjectAccessRights())
    {
        ui->permissionsUnfilterableTree->addRow(
                    QString::fromStdString(p.project),
                    accessRightToString(p.permission));
    }
    QObject::connect(ui->databaseComboBox, &QComboBox::currentTextChanged,
                     this, &RepoDialogRole::setDelegate);


    //--------------------------------------------------------------------------
    // Set database combo box
    for (auto entry : databasesWithProjects)
    {
        ui->databaseComboBox->addItem(QString::fromStdString(entry.first));
    }

    //--------------------------------------------------------------------------
    // Set name
    if (!role.isEmpty())
    {
        QString roleName = QString::fromStdString(role.getName());
        if (isCopy)
            roleName += " " + tr("(Copy)");
        ui->nameLineEdit->setText(roleName);
        ui->roleNameGroupBox->setChecked(isCopy && !role.isEmpty());

        // Set database
        ui->databaseComboBox->setCurrentText(
                    role.getDatabase().empty()
                    ? currentDatabase
                    : QString::fromStdString(role.getDatabase()));
    }

    //--------------------------------------------------------------------------
    // Settings
    // Color
    ui->colorLineEdit->addAction(QIcon(), QLineEdit::TrailingPosition);
    QObject::connect(ui->colorLineEdit->actions()[0], &QAction::triggered,
            this, &RepoDialogRole::showColorDialog);
    setColor(QString::fromStdString(settings.getColor()));


    // Description
    ui->descriptionPlainTextEdit->setPlainText(QString::fromStdString(settings.getDescription()));


    //--------------------------------------------------------------------------
    // Privileges
    ui->privilegesUnfilterableTree->setHeaders({
                                                   tr("Database"),
                                                   tr("Collection"),
                                                   tr("Actions")});
    ui->privilegesUnfilterableTree->registerTabWidget(ui->tabWidget, (int)Tab::PRIVILEGES);
    ui->privilegesUnfilterableTree->setButtonsEnabled(false);
    for (repo::core::model::RepoPrivilege p : role.getPrivileges())
    {
        QString qactions;
        for (std::string action : repo::core::model::RepoRole::dbActionsToStrings(p.actions))
        {
            qactions += QString::fromStdString(action) + ", ";
        }
        qactions.remove(qactions.size()-2, 2); // remove last comma

        QStringList qlist = {
            QString::fromStdString(p.database),
            QString::fromStdString(p.collection),
            qactions
        };
        ui->privilegesUnfilterableTree->addRow(qlist, false);
    }

    //--------------------------------------------------------------------------
    // Inherited Roles
    ui->inheritedRolesUnfilterableTree->setHeaders({tr("Database"), tr("Role")});
    ui->inheritedRolesUnfilterableTree->registerTabWidget(ui->tabWidget, (int)Tab::INHERITED_ROLES);
    ui->inheritedRolesUnfilterableTree->setButtonsEnabled(false);
    for (std::pair<std::string, std::string> pair : role.getInheritedRoles())
    {
        ui->inheritedRolesUnfilterableTree->addRow(pair, false);
    }

    //--------------------------------------------------------------------------
    // Modules
    ui->modulesUnfilterableTree->setHeaders({tr("Module")});
    ui->modulesUnfilterableTree->registerTabWidget(ui->tabWidget, (int)Tab::MODULES);
    ui->modulesUnfilterableTree->setNewRowText({tr("<module>")});
    for (std::string module : settings.getModules())
    {
        ui->modulesUnfilterableTree->addRow({QString::fromStdString(module)});
    }

    //--------------------------------------------------------------------------
    // Connect buttons
    QObject::connect(
                ui->colorPickerPushButton, &QPushButton::pressed,
                this, &RepoDialogRole::showColorDialog);

    QObject::connect(
                ui->colorLineEdit, &QLineEdit::textEdited,
                this, &RepoDialogRole::setColor);
}

RepoDialogRole::~RepoDialogRole()
{
    delete ui;
}

QTreeWidgetItem *RepoDialogRole::addPermissionItem()
{
    //    std::string database = ui->databaseComboBox->currentText().toStdString();
    //    std::map<std::string, std::list<std::string> >::iterator it =
    //            databasesWithProjects.find(database);

    //    std::string project;
    //    if (it != databasesWithProjects.end())
    //    {
    //        std::list<std::string> projects = it->second;
    //        if (projects.size() > ui->accessRightsTreeWidget->model()->rowCount())
    //        {
    //            std::list<std::string>::iterator pit = projects.begin();
    //            std::advance(pit, ui->accessRightsTreeWidget->model()->rowCount());
    //            project = (*pit);
    //        }
    //    }
    //    return addPermissionItem(project, repo::core::model::AccessRight::READ);
    return 0;
}

std::string RepoDialogRole::getColor() const
{
    return ui->colorLineEdit->text().toStdString();
}

std::string RepoDialogRole::getDescription() const
{
    return ui->descriptionPlainTextEdit->toPlainText().toStdString();
}

std::string RepoDialogRole::getName() const
{
    return ui->nameLineEdit->text().toStdString();
}

std::string RepoDialogRole::getDatabase() const
{
    return ui->databaseComboBox->currentText().toStdString();
}

std::vector<std::string> RepoDialogRole::getModules() const
{
    return ui->modulesUnfilterableTree->getItemsAsVectorOfStrings();
}

std::vector<repo::core::model::RepoPermission> RepoDialogRole::getPermissions() const
{
    std::list<std::pair<std::string, std::string> > items =
            ui->permissionsUnfilterableTree->getItemsAsListOfPairsOfStrings();
    std::vector<repo::core::model::RepoPermission> permissions(items.size());

    int i = 0;
    for (std::pair<std::string, std::string> item : items)
    {
        repo::core::model::RepoPermission permission;
        permission.database = getDatabase();
        permission.project = item.first;
        permission.permission = stringToAccessRight(QString::fromStdString(item.second));
        permissions[i++] = permission;
    }
    return permissions;
}

bool RepoDialogRole::isNewRole() const
{
    return getName() != oldRole.getName() ||
            getDatabase() != oldRole.getDatabase();
}

void RepoDialogRole::showColorDialog()
{
    QColor oldColor(ui->colorLineEdit->text());
    QColor color = QColor(QColorDialog::getColor(oldColor, this));
    if (color.isValid())
    {
        setColor(color.name(QColor::HexRgb));
    }
}

void RepoDialogRole::setColor(const QString &hex)
{
    if (!hex.isEmpty())
    {
        ui->colorLineEdit->setText(hex);
        QPixmap px(16,16);
        px.fill(repo::gui::primitive::RepoColor::fromHex(hex.toStdString()));
        if (ui->colorLineEdit->actions().size() > 0)
            ui->colorLineEdit->actions()[0]->setIcon(QIcon(px));
    }
}

void RepoDialogRole::setDelegate(const QString &database)
{
    ui->permissionsUnfilterableTree->setItemDelegateForColumn(database,0);
    ui->permissionsUnfilterableTree->setItemDelegateForColumn(database,1);
}

repo::core::model::RepoRole RepoDialogRole::getUpdatedRole() const
{
//    repo::core::model::RepoRole oldRole = role.cloneAndUpdatePermissions(getPermissions());
//    repo::core::model::RepoRole newRole = repo::core::model::RepoBSONFactory::makeRepoRole(
//                getName(), getDatabase(), oldRole.getProjectAccessRights(),
//                oldRole.getInheritedRoles());
//    return newRole;

    return repo::core::model::RepoBSONFactory::makeRepoRole(
                getName(), getDatabase(), getPermissions(), oldRole);
}

repo::core::model::RepoRoleSettings RepoDialogRole::getUpdatedRoleSettings() const
{
    repo::core::model::RepoRoleSettings settings =
            repo::core::model::RepoBSONFactory::makeRepoRoleSettings(
                getName(),getColor(), getDescription(), getModules());
    return settings;
}

QString RepoDialogRole::accessRightToString(const repo::core::model::AccessRight &rw)
{
    QString str;
    switch(rw)
    {
    case repo::core::model::AccessRight::READ :
        str = tr("Read");
        break;
    case repo::core::model::AccessRight::READ_WRITE :
        str = tr("ReadWrite");
        break;
    case repo::core::model::AccessRight::WRITE :
        str  = tr("Write");
        break;
    }
    return str;
}

repo::core::model::AccessRight RepoDialogRole::stringToAccessRight(
        const QString &str)
{
    repo::core::model::AccessRight ar;
    if (str == tr("Read"))
        ar = repo::core::model::AccessRight::READ;
    else if (str == tr("ReadWrite"))
        ar = repo::core::model::AccessRight::READ_WRITE;
    else if (str == tr("Write"))
        ar = repo::core::model::AccessRight::WRITE;
    return ar;
}
