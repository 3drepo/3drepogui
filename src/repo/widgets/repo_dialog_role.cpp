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

RepoDialogRole::RepoDialogRole(
        const repo::core::model::RepoRole &role,
        const QString &database,
        const QStringList &projects,
        QWidget *parent)
    : QDialog(parent)
    , role(role)
    , projects(projects)
    , ui(new Ui::RepoDialogRole)
{
    ui->setupUi(this);
    ui->databaseComboBox->addItem(
                role.getDatabase().empty()
                ? database
                : QString::fromStdString(role.getDatabase()));
    ui->databaseComboBox->setEnabled(false);
    ui->nameLineEdit->setText(QString::fromStdString(role.getName()));


    //--------------------------------------------------------------------------
    // RW delegate
    std::list<std::string> rwList;
    rwList.push_back("Read");
    rwList.push_back("Write");
    rwList.push_back("ReadWrite");
    QList<repo::gui::RepoComboBoxEditor::SeparatedEntries> rwSepEntries;
    rwSepEntries << repo::gui::RepoComboBoxEditor::getSeparatedEntries(projects);
    rwSepEntries << repo::gui::RepoComboBoxEditor::getSeparatedEntries(rwList);
    rwDelegate = new repo::gui::RepoComboBoxDelegate(rwSepEntries);

    if (projects.size() > 0)
        ui->accessRightsTreeWidget->setItemDelegateForColumn(0, rwDelegate);

    ui->accessRightsTreeWidget->setItemDelegateForColumn(1, rwDelegate);

    //--------------------------------------------------------------------------
    // Set permissions
    std::vector<repo::core::model::RepoPermission> permissions = role.getProjectAccessRights();
    for (repo::core::model::RepoPermission p : permissions)
    {
        addItem(p.project, p.permission);
    }

    //--------------------------------------------------------------------------
    // Connect buttons
    QObject::connect(
                ui->addPushButton, SIGNAL(pressed()),
                this, SLOT(addItem()));
    QObject::connect(
        ui->removePushButton, &QPushButton::pressed,
        this, &RepoDialogRole::removeItem);
}

RepoDialogRole::~RepoDialogRole()
{
    delete ui;
    delete rwDelegate;
}

QTreeWidgetItem *RepoDialogRole::addItem()
{
    std::string project = projects.size() > ui->accessRightsTreeWidget->model()->rowCount()
            ? projects[ui->accessRightsTreeWidget->model()->rowCount()].toStdString()
            : "";

    return addItem(project, repo::core::model::AccessRight::READ);
}

QTreeWidgetItem *RepoDialogRole::addItem(
        const std::string &project,
        const repo::core::model::AccessRight &rw)
{
    QString qproject = QString::fromStdString(project);
    QString qrw = accessRightToString(rw);
    QStringList qlist;
    qlist.append(qproject);
    qlist.append(qrw);

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->accessRightsTreeWidget, qlist);
    item->setData(0, Qt::DecorationRole, qproject);
    item->setData(1, Qt::DecorationRole, qrw);
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
    ui->accessRightsTreeWidget->addTopLevelItem(item);

    return item;
}

void RepoDialogRole::removeItem()
{
    QTreeWidgetItem *item = 0;
    item = ui->accessRightsTreeWidget->currentItem();
    if (item)
    {
        delete item;
        item = 0;
    }
}

std::string RepoDialogRole::getName() const
{
    return ui->nameLineEdit->text().toStdString();
}

std::string RepoDialogRole::getDatabase() const
{
    return ui->databaseComboBox->currentText().toStdString();
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
    return getName() != role.getName();
}

repo::core::model::RepoRole RepoDialogRole::getUpdatedRole()
{
    repo::core::model::RepoRole role = repo::core::model::RepoBSONFactory::makeRepoRole(
                getName(), getDatabase(), getPermissions());
    return role;
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
