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
#include "../primitives/repocomboboxeditor.h"

//------------------------------------------------------------------------------
// Qt
#include <QComboBox>
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <QStandardItemEditorCreator>

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







    // Enable drop down selectors for editing delegate
    // See http://doc.qt.io/qt-5/qtwidgets-itemviews-coloreditorfactory-example.html
    QItemEditorFactory *factory = new QItemEditorFactory();

//    QItemEditorCreatorBase *itemListCreator =
//        new QStandardItemEditorCreator<RepoComboBoxEditor>();

    QItemEditorCreatorBase * myCombo = new RepoComboBoxEditor(databaseList);
    factory->registerEditor(QVariant::Color, myCombo);
    //QItemEditorFactory::setDefaultFactory(factory);

    QItemDelegate *delegate = new QItemDelegate();
    delegate->setItemEditorFactory(factory);
    ui->projectsTreeView->setItemDelegateForColumn(RepoProjectsColumns::OWNER, delegate);


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




        QList<QTreeWidgetItem *> items;
        for (unsigned int i = 0; i < roles.size(); ++i)
        {


            QStringList list;
            list.append(QString::fromStdString(roles[i].first));
            list.append(QString::fromStdString(roles[i].second));

            QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, list);
            ui->rolesTreeWidget->insertTopLevelItem(0, item);





            QComboBox *databaseComboBox = new QComboBox();
            databaseComboBox->addItem(QString::fromStdString(roles[i].first));
            databaseComboBox->addItem("admin");
            databaseComboBox->addItem("test");
            ui->rolesTreeWidget->setItemWidget(item, 0, databaseComboBox);


            QComboBox *roleComboBox = new QComboBox();
            roleComboBox->addItem(QString::fromStdString(roles[i].second));
            roleComboBox->addItem("admin");
            roleComboBox->addItem("test");
            ui->rolesTreeWidget->setItemWidget(item, 1, roleComboBox);



        }
    }
}

repo::gui::RepoDialogUser::~RepoDialogUser()
{
    delete projectsModel;
    delete rolesModel;
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

        QStandardItem *item = new QStandardItem();//QString::fromStdString(data[i].second));
        item->setData(QColor("springgreen"), Qt::DisplayRole);
        row.append(item);
        model->invisibleRootItem()->appendRow(row);

       // model->setData(model->index(i, 0), new QComboBox());
    }
}

