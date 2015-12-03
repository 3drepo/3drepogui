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

const RepoComboBoxEditor::SeparatedEntries RepoDialogRole::rwSeparatedEntries =
        RepoComboBoxEditor::getSeparatedEntries({
                                                    tr("Read").toStdString(),
                                                    tr("Write").toStdString(),
                                                    tr("ReadWrite").toStdString()});

RepoDialogRole::RepoDialogRole(
        const repo::core::model::RepoRole &role,
        const repo::core::model::RepoRoleSettings &settings,
        const QString &currentDatabase,
        const std::map<std::string, std::list<std::string> > &databasesWithProjects,
        QWidget *parent)
    : QDialog(parent)
    , role(role)
    , settings(settings)
    , databasesWithProjects(databasesWithProjects)
    , ui(new Ui::RepoDialogRole)
    , rwDelegate(nullptr)
{
    ui->setupUi(this);

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
    ui->nameLineEdit->setText(QString::fromStdString(role.getName()));

    // Set database
    ui->databaseComboBox->setCurrentText(
                role.getDatabase().empty()
                ? currentDatabase
                : QString::fromStdString(role.getDatabase()));

    //--------------------------------------------------------------------------
    // Settings
    // Color
    ui->colorLineEdit->addAction(QIcon(), QLineEdit::LeadingPosition);
    QObject::connect(ui->colorLineEdit->actions()[0], &QAction::triggered,
            this, &RepoDialogRole::showColorDialog);
    setColor(QString::fromStdString(settings.getColor()));


    // Description
    ui->descriptionPlainTextEdit->setPlainText(QString::fromStdString(settings.getDescription()));


    //--------------------------------------------------------------------------
    // Permissions
    for (auto p : role.getProjectAccessRights())
    {
        addPermissionItem(p.project, p.permission);
    }

    //--------------------------------------------------------------------------
    // Privileges
    for (repo::core::model::RepoPrivilege p : role.getPrivileges())
    {
        addPrivilegeItem(
            p.database,
            p.collection,
            repo::core::model::RepoRole::dbActionsToStrings(p.actions));
    }


    //--------------------------------------------------------------------------
    // Modules
    for (auto m : settings.getModules())
    {
        addModuleItem(m);
    }



    //--------------------------------------------------------------------------
    // Connect buttons
    QObject::connect(
                ui->addPermissionPushButton, SIGNAL(pressed()),
                this, SLOT(addPermissionItem()));

    QObject::connect(
                ui->addModulePushButton, SIGNAL(pressed()),
                this, SLOT(addModuleItem()));


    QObject::connect(
                ui->removePermissionPushButton, &QPushButton::pressed,
                this, &RepoDialogRole::removePermissionItem);
    QObject::connect(
                ui->removeModulePushButton, &QPushButton::pressed,
                this, &RepoDialogRole::removeModuleItem);

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

    if (rwDelegate)
    {
        delete rwDelegate;
        rwDelegate = nullptr;
    }
}

QTreeWidgetItem *RepoDialogRole::addPermissionItem()
{
    std::string database = ui->databaseComboBox->currentText().toStdString();
    std::map<std::string, std::list<std::string> >::iterator it =
            databasesWithProjects.find(database);

    std::string project;
    if (it != databasesWithProjects.end())
    {
        std::list<std::string> projects = it->second;
        if (projects.size() > ui->accessRightsTreeWidget->model()->rowCount())
        {
            std::list<std::string>::iterator pit = projects.begin();
            std::advance(pit, ui->accessRightsTreeWidget->model()->rowCount());
            project = (*pit);
        }
    }
    return addPermissionItem(project, repo::core::model::AccessRight::READ);
}

QTreeWidgetItem *RepoDialogRole::addModuleItem()
{
    // TODO: add list of available modules to bouncer or something
    std::string module = tr("module").toStdString();
    return addModuleItem(module);
}


QTreeWidgetItem *RepoDialogRole::addPermissionItem(
        const std::string &project,
        const repo::core::model::AccessRight &rw)
{
    QString qproject = QString::fromStdString(project);
    QString qrw = accessRightToString(rw);
    QStringList qlist = { qproject, qrw };

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->accessRightsTreeWidget, qlist);
    item->setData(0, Qt::DecorationRole, qproject);
    item->setData(1, Qt::DecorationRole, qrw);
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
    ui->accessRightsTreeWidget->addTopLevelItem(item);

    return item;
}


QTreeWidgetItem *RepoDialogRole::addPrivilegeItem(
        const std::string &database,
        const std::string &collection,
        const std::vector<std::string> &actions)
{
    QString qactions;
    for (std::string action : actions)
    {
        qactions += QString::fromStdString(action) + ", ";
    }
    qactions.remove(qactions.size()-2, 2);

    QStringList qlist = {
            QString::fromStdString(database),
            QString::fromStdString(collection),
            qactions
    };
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->privilegeTreeWidget, qlist);
    item->setDisabled(true);
    ui->privilegeTreeWidget->addTopLevelItem(item);
    return item;
}

QTreeWidgetItem *RepoDialogRole::addModuleItem(const std::string &module)
{
    QStringList qlist = { QString::fromStdString(module) };
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->modulesTreeWidget, qlist);
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
    ui->modulesTreeWidget->addTopLevelItem(item);
    return item;
}

void RepoDialogRole::removePermissionItem()
{
    QTreeWidgetItem *item = nullptr;
    item = ui->accessRightsTreeWidget->currentItem();
    if (item)
    {
        delete item;
        item = nullptr;
    }
}

void RepoDialogRole::removeModuleItem()
{
    QTreeWidgetItem *item = nullptr;
    item = ui->modulesTreeWidget->currentItem();
    if (item)
    {
        delete item;
        item = nullptr;
    }
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
    std::vector<std::string> modules;
    for (int i = 0; i < ui->modulesTreeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = ui->modulesTreeWidget->topLevelItem(i);
        if (item)
        {
            modules.push_back(item->data(0, Qt::EditRole).toString().toStdString());
        }
    }
    return modules;
}

std::vector<repo::core::model::RepoPermission> RepoDialogRole::getPermissions() const
{
    std::vector<repo::core::model::RepoPermission> permissions;
    for (int i = 0; i < ui->accessRightsTreeWidget->topLevelItemCount(); ++i)
    {
        repo::core::model::RepoPermission permission;
        QTreeWidgetItem *item = ui->accessRightsTreeWidget->topLevelItem(i);

        if (item)
        {
            permission.database = getDatabase();
            permission.project = item->data(0, Qt::EditRole).toString().toStdString();
            permission.permission = stringToAccessRight(item->data(1, Qt::EditRole).toString());
            permissions.push_back(permission);
        }
    }
    return permissions;
}

bool RepoDialogRole::isNewRole() const
{
    return getName() != role.getName() ||
            getDatabase() != role.getDatabase();
}

void RepoDialogRole::showColorDialog()
{
    QColor oldColor(ui->colorLineEdit->text());
    QColor color = QColor(
                QColorDialog::getColor(
                    oldColor, this));

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
        px.fill(repo::gui::RepoColor::fromHex(hex.toStdString()));
        if (ui->colorLineEdit->actions().size() > 0)
            ui->colorLineEdit->actions()[0]->setIcon(QIcon(px));
    }
}

void RepoDialogRole::setDelegate(const QString &database)
{
    std::map<std::string, std::list<std::string> >::iterator it =
            databasesWithProjects.find(database.toStdString());

    if (it != databasesWithProjects.end())
    {
        QList<repo::gui::RepoComboBoxEditor::SeparatedEntries> rwSEList;
        rwSEList << repo::gui::RepoComboBoxEditor::getSeparatedEntries(it->second);
        rwSEList << rwSeparatedEntries;

        if (rwDelegate)
        {
            delete rwDelegate;
            rwDelegate = nullptr;
        }
        rwDelegate = new repo::gui::RepoComboBoxDelegate(rwSEList);
        ui->accessRightsTreeWidget->setItemDelegateForColumn(0, rwDelegate);
        ui->accessRightsTreeWidget->setItemDelegateForColumn(1, rwDelegate);
    }
}

repo::core::model::RepoRole RepoDialogRole::getUpdatedRole() const
{
    repo::core::model::RepoRole role = repo::core::model::RepoBSONFactory::makeRepoRole(
                getName(), getDatabase(), getPermissions());
    return role;
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
