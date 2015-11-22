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
        const QStringList &projects,
        QWidget *parent)
    : QDialog(parent)
    , role(role)
    , projects(projects)
    , ui(new Ui::RepoDialogRole)
{
    ui->setupUi(this);

    ui->databaseComboBox->addItem(QString::fromStdString(role.getDatabase()));
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
