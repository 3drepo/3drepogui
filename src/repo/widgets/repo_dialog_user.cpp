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
#include "repo_dialog_user.h"
#include "ui_repo_dialog_user.h"

using namespace repo::widgets;
using namespace repo::gui;

RepoDialogUser::RepoDialogUser(
        const repo::RepoToken *token,
        repo::RepoController *controller,
        const repo::core::model::RepoUser &user,
        const std::map<std::string, std::list<std::string> > &databasesWithProjects,
        const std::list<std::string> &customRolesList,
        const bool isCopy,
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
    // Connections
    QObject::connect(
        ui->avatarPushButton, &QPushButton::pressed,
        this, &RepoDialogUser::openImageFileDialog);

    QObject::connect(
                ui->projectsUnfilterableTreeWidget,
                &RepoWidgetTreeUnfilterable::rowCountChanged,
                this,
                &RepoDialogUser::updateProjectsTabCount);

    QObject::connect(
                ui->rolesUnfilterableTreeWidget,
                &RepoWidgetTreeUnfilterable::rowCountChanged,
                this,
                &RepoDialogUser::updateRolesTabCount);

    QObject::connect(
                ui->apiKeysUnfilterableTreeWidget,
                &RepoWidgetTreeUnfilterable::rowCountChanged,
                this,
                &RepoDialogUser::updateApiKeysTabCount);

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

    QHash<QString, repo::gui::RepoComboBoxDelegate *> rolesDelegates;
    QHash<QString, repo::gui::RepoComboBoxDelegate *> projectsDelegates;

    for (auto database : databases)
    {
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
            (controller->getNameOfAdminDatabase(token) == database)
                ? new RepoComboBoxDelegate(adminDBRolesLists)
                : new RepoComboBoxDelegate(anyDBRolesLists);
        rolesDelegates.insert(qDatabase, rolesDelegate);
    }

    // Projects
    QStringList defaultRow = {tr("admin"), tr("<empty>")};
    ui->projectsUnfilterableTreeWidget->setHeaders({tr("Database"), tr("Project")});
    ui->projectsUnfilterableTreeWidget->setNewRowText(defaultRow);
    ui->projectsUnfilterableTreeWidget->setDelegates(projectsDelegates);

    // Roles
    ui->rolesUnfilterableTreeWidget->setHeaders({tr("Database"), tr("Role")});
    ui->rolesUnfilterableTreeWidget->setNewRowText(defaultRow);
    ui->rolesUnfilterableTreeWidget->setDelegates(rolesDelegates);

    // API Keys
    ui->apiKeysUnfilterableTreeWidget->setHeaders({tr("Label"), tr("API Key")});
    ui->apiKeysUnfilterableTreeWidget->setNewRowText(
        {tr("label"),QString::fromStdString(UUIDtoString(generateUUID()))});

    //--------------------------------------------------------------------------
    // Populate user data
    if (!user.isEmpty())
    {
        QString username = QString::fromStdString(user.getUserName());
        if (isCopy)
        {
            username += " " + tr("(Copy)");
        }
        else
            ui->passwordLineEdit->setText(QString::fromStdString(user.getPassword()));

        ui->usernameLineEdit->setText(username);

        ui->firstNameLineEdit->setText(QString::fromStdString(user.getFirstName()));
        ui->lastNameLineEdit->setText(QString::fromStdString(user.getLastName()));
        ui->emailLineEdit->setText(QString::fromStdString(user.getEmail()));

        ui->credentialsGroupBox->setChecked(isCopy && !user.isEmpty());

        //----------------------------------------------------------------------
        // Acess Rights
        ui->projectsUnfilterableTreeWidget->addRows(user.getProjectsList());
        ui->rolesUnfilterableTreeWidget->addRows(user.getRolesList());
        ui->apiKeysUnfilterableTreeWidget->addRows(user.getAPIKeysList());
    }




    //--------------------------------------------------------------------------
    // Regular expression validator for email
    // See http://www.regular-expressions.info/email.html
    QRegExp emailRegularExpression("^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}$", Qt::CaseInsensitive);
    emailValidator = new QRegExpValidator(emailRegularExpression);
    ui->emailLineEdit->setValidator(emailValidator);

}

RepoDialogUser::~RepoDialogUser()
{
    delete ui;
    delete emailValidator;
}

QIcon RepoDialogUser::getIcon()
{
   return RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_user);
}

std::string RepoDialogUser::getEmail() const
{
    return ui->emailLineEdit->text().toStdString();
}

std::string RepoDialogUser::getFirstName() const
{
    return ui->firstNameLineEdit->text().toStdString();
}

std::list<std::pair<std::string, std::string> > RepoDialogUser::getAPIKeys() const
{
    return ui->apiKeysUnfilterableTreeWidget->getItems();
}

std::string RepoDialogUser::getLastName() const
{
    return ui->lastNameLineEdit->text().toStdString();
}

std::string RepoDialogUser::getPassword() const
{
    std::string currentPassword = ui->passwordLineEdit->text().toStdString();
    return currentPassword != user.getPassword() ? currentPassword : "";
}

std::list<std::pair<std::string, std::string> > RepoDialogUser::getProjects() const
{
    return ui->projectsUnfilterableTreeWidget->getItems();
}

std::list<std::pair<std::string, std::string> > RepoDialogUser::getRoles() const
{
    return ui->rolesUnfilterableTreeWidget->getItems();
}

std::string RepoDialogUser::getUsername() const
{
    return ui->usernameLineEdit->text().toStdString();
}

void RepoDialogUser::openImageFileDialog()
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

void RepoDialogUser::updateProjectsTabCount(int oldRowCount, int newRowCount)
{
    ui->tabWidget->setTabText((int)Tab::PROJECTS,
                              RepoWidgetTreeUnfilterable::updateCountString(
                                  ui->tabWidget->tabText((int)Tab::PROJECTS),
                                  oldRowCount,
                                  newRowCount));
}

void RepoDialogUser::updateRolesTabCount(int oldRowCount, int newRowCount)
{
    ui->tabWidget->setTabText((int)Tab::ROLES,
                              RepoWidgetTreeUnfilterable::updateCountString(
                                  ui->tabWidget->tabText((int)Tab::ROLES),
                                  oldRowCount,
                                  newRowCount));
}

void RepoDialogUser::updateApiKeysTabCount(int oldRowCount, int newRowCount)
{
    ui->tabWidget->setTabText((int)Tab::API_KEYS,
                              RepoWidgetTreeUnfilterable::updateCountString(
                                  ui->tabWidget->tabText((int)Tab::API_KEYS),
                                  oldRowCount,
                                  newRowCount));
    ui->apiKeysUnfilterableTreeWidget->setNewRowText(
        {tr("label"),QString::fromStdString(UUIDtoString(generateUUID()))});
}

repo::core::model::RepoUser RepoDialogUser::getUpdatedUser() const
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

void RepoDialogUser::setAvatar(const std::vector<char> &image)
{
    if (!image.empty())
    {
		QImage qimage = QImage::fromData((uchar*)&(image.at(0)), image.size());
		setAvatar(qimage);
    }
}

void RepoDialogUser::setAvatar(const QImage &image)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG"); // writes image in JPG format
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
