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


#include <fstream>
#include <iostream>

//------------------------------------------------------------------------------
// Qt
#include <QMap>
#include <QRegExpValidator>
#include <QFileDialog>
#include <QBuffer>
#include <QImageReader>
#include <QMessageBox>

//------------------------------------------------------------------------------
// GUI
#include "repodialoguser.h"
#include "ui_repodialoguser.h"
#include "../primitives/repo_fontawesome.h"

//------------------------------------------------------------------------------
// Core
#include <repo/core/model/bson/repo_bson_factory.h>

//------------------------------------------------------------------------------

repo::gui::RepoDialogUser::RepoDialogUser(
		const repo::RepoToken			  *token,
		repo::RepoController              *controller,
        const repo::core::model::RepoUser &user,
        const std::map<std::string, std::list<std::string> > &databasesWithProjects,
        const std::list<std::string> &customRolesList,
        QWidget *parent)
    : QDialog(parent)
    , controller(controller)
	, token(token)
    , user(user)
    , ui(new Ui::RepoDialogUser)
{
    ui->setupUi(this);
    setWindowIcon(getIcon());

    //--------------------------------------------------------------------------
    ui->avatarPushButton->setIcon(RepoFontAwesome::getInstance().getIcon(
                                       RepoFontAwesome::fa_user,
                                       QColor(Qt::gray)));

    if (!user.isEmpty() && user.isValid())
    {        
		setAvatar(user.getAvatarAsRawData());
    }

    //--------------------------------------------------------------------------
    // Databases
    QMap<std::string, std::list<std::string> > databasesMapping(databasesWithProjects);
    std::list<std::string> databases = databasesMapping.keys().toStdList();
    databases.sort();

    //--------------------------------------------------------------------------
    // DB Roles
    RepoComboBoxEditor::SeparatedEntries dbEntries;
    dbEntries << databases;
    RepoComboBoxEditor::SeparatedEntries dbRoleEntries;
    dbRoleEntries << customRolesList << controller->getStandardDatabaseRoles(token);

    //------------------------------------------------------------------------
    // Any DB Roles
    QList<RepoComboBoxEditor::SeparatedEntries> anyDBRolesLists;
    anyDBRolesLists << dbEntries << dbRoleEntries;

    //--------------------------------------------------------------------------
    // Admin DB Roles (any roles + admin only roles)
	dbRoleEntries << controller->getAdminDatabaseRoles(token);
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
        //----------------------------------------------------------------------
        //
        // TODO: Groups delegate
        //
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        // Roles delegate
		RepoComboBoxDelegate *rolesDelegate =
			(controller->getNameOfAdminDatabase(token) == database) ?
			new RepoComboBoxDelegate(adminDBRolesLists)
             : new RepoComboBoxDelegate(anyDBRolesLists);
        rolesDelegates.insert(qDatabase, rolesDelegate);
    }

    //--------------------------------------------------------------------------
    // Populate user data
    if (!user.isEmpty())
    {
        ui->usernameLineEdit->setText(QString::fromStdString(user.getUserName()));      
        ui->passwordLineEdit->setText(QString::fromStdString(user.getPassword()));
        ui->firstNameLineEdit->setText(QString::fromStdString(user.getFirstName()));
        ui->lastNameLineEdit->setText(QString::fromStdString(user.getLastName()));
        ui->emailLineEdit->setText(QString::fromStdString(user.getEmail()));

        ui->credentialsGroupBox->setChecked(false);

        //----------------------------------------------------------------------
        // Acess Rights
        addItems(Tabs::PROJECTS, user.getProjectsList());
        addItems(Tabs::ROLES, user.getRolesList());
        addItems(Tabs::API_KEYS, user.getAPIKeysList());
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

    QObject::connect(
        ui->avatarPushButton, &QPushButton::pressed,
        this, &RepoDialogUser::openImageFileDialog);

    //--------------------------------------------------------------------------
    // Regular expression validator for email
    // See http://www.regular-expressions.info/email.html
    QRegExp emailRegularExpression("^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}$", Qt::CaseInsensitive);
    emailValidator = new QRegExpValidator(emailRegularExpression);
    ui->emailLineEdit->setValidator(emailValidator);

}

repo::gui::RepoDialogUser::~RepoDialogUser()
{
    delete ui;
    delete emailValidator;

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

QTreeWidgetItem * repo::gui::RepoDialogUser::addAPIKey(
        const std::pair<std::string, std::string> &apiKey)
{
    QTreeWidgetItem *item = addItem(
                apiKey,
                ui->apiKeysTreeWidget);
    ui->apiKeysTreeWidget->scrollToItem(item);
	ui->apiKeysTreeWidget->setCurrentItem(item, (int)Columns::DATABASE);
    return item;
}

QTreeWidgetItem * repo::gui::RepoDialogUser::addItem()
{
    return addItem(static_cast<Tabs>(ui->accessRightsTabWidget->currentIndex()));
}

QTreeWidgetItem* repo::gui::RepoDialogUser::addItem(
        Tabs tab,
        const std::pair<std::string, std::string> &pair)
{
    const static std::string admin = controller->getNameOfAdminDatabase(token);
    QTreeWidgetItem* item = 0;
    switch(tab)
    {
    case Tabs::PROJECTS :
        if (pair.first.empty() && pair.second.empty())
            item = addProject(std::make_pair(""+admin,""));
        else
            item = addProject(pair);
        break;
    case Tabs::ROLES :
        if (pair.first.empty() && pair.second.empty())
            item = addRole(std::make_pair(""+admin,""));
        else
            item = addRole(pair);
        break;
    case Tabs::API_KEYS :
        if (pair.first.empty() && pair.second.empty())
            item = addAPIKey(std::make_pair("label",
			UUIDtoString(generateUUID())));
        else
            item = addAPIKey(pair);
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

void repo::gui::RepoDialogUser::addItems(
        Tabs tab,
        const std::list<std::pair<std::string, std::string> > &list)
{
    std::list<std::pair<std::string, std::string> >::const_iterator i;
    for (i = list.begin(); i != list.end(); ++i)
        addItem(tab, *i);
}

QTreeWidgetItem* repo::gui::RepoDialogUser::addProject(
        const std::pair<std::string, std::string> &project)
{
    QTreeWidgetItem *item = addItem(
                project,
                ui->projectsTreeWidget,
                projectsDelegates);
    ui->projectsTreeWidget->scrollToItem(item);
	ui->projectsTreeWidget->setCurrentItem(item, (int)Columns::DATABASE);
    return item;
}

QTreeWidgetItem* repo::gui::RepoDialogUser::addRole(
        const std::pair<std::string, std::string> &role)
{    
    QTreeWidgetItem * item = addItem(
                role,
                ui->rolesTreeWidget,
                rolesDelegates);
    ui->rolesTreeWidget->scrollToItem(item);
	ui->rolesTreeWidget->setCurrentItem(item, (int)Columns::DATABASE);
    return item;
}

QIcon repo::gui::RepoDialogUser::getIcon()
{
   return RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_user);
}

void repo::gui::RepoDialogUser::removeItem()
{
    QTreeWidgetItem *item = 0;
    switch(static_cast<Tabs>(ui->accessRightsTabWidget->currentIndex()))
    {
    case Tabs::PROJECTS :
        item = ui->projectsTreeWidget->currentItem();
        break;
    case Tabs::ROLES :
        item = ui->rolesTreeWidget->currentItem();
        break;
    case Tabs::API_KEYS :
        item = ui->apiKeysTreeWidget->currentItem();
        break;
    }
    if (item)
        delete item;
}

std::string repo::gui::RepoDialogUser::getEmail() const
{
    return ui->emailLineEdit->text().toStdString();
}

std::string repo::gui::RepoDialogUser::getFirstName() const
{
    return ui->firstNameLineEdit->text().toStdString();
}

std::list<std::pair<std::string, std::string> > repo::gui::RepoDialogUser::getAPIKeys() const
{
    return getItems(ui->apiKeysTreeWidget);
}

std::list<std::pair<std::string, std::string> > repo::gui::RepoDialogUser::getItems(QTreeWidget *widget) const
{
    std::list<std::pair<std::string, std::string> > list;
    for (int i = 0; i < widget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = widget->topLevelItem(i);
        std::string database = item->data((int)Columns::DATABASE, Qt::EditRole).toString().toStdString();
		std::string value = item->data((int)Columns::VALUE, Qt::EditRole).toString().toStdString();
        list.push_back(std::make_pair(database, value));
    }
    return list;
}

std::string repo::gui::RepoDialogUser::getLastName() const
{
    return ui->lastNameLineEdit->text().toStdString();
}

std::string repo::gui::RepoDialogUser::getPassword() const
{
    std::string currentPassword = ui->passwordLineEdit->text().toStdString();
    return currentPassword != user.getPassword() ? currentPassword : "";
}

std::list<std::pair<std::string, std::string> > repo::gui::RepoDialogUser::getProjects() const
{
    return getItems(ui->projectsTreeWidget);
}

std::list<std::pair<std::string, std::string> > repo::gui::RepoDialogUser::getRoles() const
{
    return getItems(ui->rolesTreeWidget);
}

std::string repo::gui::RepoDialogUser::getUsername() const
{
    return ui->usernameLineEdit->text().toStdString();
}

void repo::gui::RepoDialogUser::updateProjectsDelegate(
        QTreeWidgetItem *current, int column)
{
	if (current && (int)Columns::DATABASE == column)
    {
        int row = ui->projectsTreeWidget->indexOfTopLevelItem(current);
        QString database = current->data(column, Qt::EditRole).toString();
        if (projectsDelegates.contains(database))
        {
            RepoComboBoxDelegate* delegate = projectsDelegates.value(database);
            ui->projectsTreeWidget->setItemDelegateForRow(row, delegate);
        }
		ui->projectsTreeWidget->setCurrentItem(current, (int)Columns::VALUE);
    }
}

void repo::gui::RepoDialogUser::updateRolesDelegate(
        QTreeWidgetItem *current, int column)
{
	if (current && (int)Columns::DATABASE == column)
    {
        int row = ui->rolesTreeWidget->indexOfTopLevelItem(current);        
        QString database = current->data(column, Qt::EditRole).toString();
        if (rolesDelegates.contains(database))
        {
            RepoComboBoxDelegate* delegate = rolesDelegates.value(database);
            ui->rolesTreeWidget->setItemDelegateForRow(row, delegate);
        }
		ui->rolesTreeWidget->setCurrentItem(current, (int)Columns::VALUE);
    }
}

void repo::gui::RepoDialogUser::openImageFileDialog()
{
    QImageReader::supportedImageFormats();

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Image"),
        QString::null,
        tr("Image Files (*.jpg *.png)"));

    if (!filePath.isEmpty())
    {
        QImage image(filePath); //icon.pixmap(100,100).toImage());
        if (image.isNull())
        {
            QMessageBox::information(this,
                                     tr("Avatar"),
                                     tr("Cannot load %1.").arg(filePath));
        }
        else
        {
            QImage scaledImage = image.scaled(QSize(300,300),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation);
            setAvatar(scaledImage);
        }
    }
}

repo::core::model::RepoUser repo::gui::RepoDialogUser::getUpdatedUser() const
{
    // TODO: validate fields are set correctly including
    // non-empty selections in projects, groups and roles

    // TODO: make sure the password has changed since the last edit.
	return  repo::core::model::RepoBSONFactory::makeRepoUser(
                getUsername(),
                getPassword(),
                getFirstName(),
                getLastName(),
                getEmail(),
                getProjects(),
                getRoles(),
                std::list<std::pair<std::string, std::string> >(),
                getAPIKeys(),
                avatar);
}

bool repo::gui::RepoDialogUser::isNewUser() const
{
	return getUsername() != user.getUserName();
}

void repo::gui::RepoDialogUser::setAvatar(const std::vector<char> &image)
{
    if (!image.empty())
    {
		QImage qimage = QImage::fromData((uchar*)&(image.at(0)), image.size());
		setAvatar(qimage);
    }
}

void repo::gui::RepoDialogUser::setAvatar(const QImage &image)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG"); // writes image in PNG format
    std::vector<char> imageBytes((unsigned char*) byteArray.constData(), 
		(unsigned char*) byteArray.constData() + byteArray.size());
    //this->avatar = core::RepoImage(imageBytes, image.width(), image.height(), REPO_MEDIA_TYPE_JPG);

   /* this->avatar = core::RepoImage((unsigned char*) byteArray.constData(),
                                   byteArray.size(),
                                   image.width(),
                                   image.height(),
                                   REPO_MEDIA_TYPE_JPG);*/

	this->avatar = imageBytes;

    ui->avatarPushButton->setIcon(QIcon(QPixmap::fromImage(image)));
}
