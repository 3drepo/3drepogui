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

#include <repo/lib/repo_log.h>
#include <iomanip>
using namespace repo::gui::dialog;
using namespace repo::gui;

UserDialog::UserDialog(
        const repo::RepoController::RepoToken *token,
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
    , ui(new Ui::UserDialog)
{
    ui->setupUi(this);
    setWindowIcon(getIcon());


    //--------------------------------------------------------------------------
    // Connections
    QObject::connect(
        ui->avatarPushButton, &QPushButton::pressed,
        this, &UserDialog::openImageFileDialog);

    ui->rolesUnfilterableTreeWidget->registerTabWidget(ui->tabUsers, (int) Tab::ROLES);
    //ui->apiKeysUnfilterableTreeWidget->registerTabWidget(ui->tabUsers, (int) Tab::API_KEYS);
    ui->licensesUnfilterableTreeWidget->registerTabWidget(ui->tabUsers, (int) Tab::LICENSES);

    //--------------------------------------------------------------------------
    ui->avatarPushButton->setIcon(repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                                       repo::gui::primitive::RepoFontAwesome::fa_user,
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
    repo::gui::primitive::RepoComboBoxEditor::SeparatedEntries dbEntries;
    dbEntries << databases;
    repo::gui::primitive::RepoComboBoxEditor::SeparatedEntries dbRoleEntries;
    dbRoleEntries << customRolesList << controller->getStandardDatabaseRoles(token);

    //------------------------------------------------------------------------
    // Any DB Roles
    QList<repo::gui::primitive::RepoComboBoxEditor::SeparatedEntries> anyDBRolesLists;
    anyDBRolesLists << dbEntries << dbRoleEntries;

    //--------------------------------------------------------------------------
    // Admin DB Roles (any roles + admin only roles)
	dbRoleEntries << controller->getAdminDatabaseRoles(token);
    QList<repo::gui::primitive::RepoComboBoxEditor::SeparatedEntries> adminDBRolesLists;
    adminDBRolesLists << dbEntries << dbRoleEntries;

    //--------------------------------------------------------------------------
    // Populate Delegates

    QHash<QString, repo::gui::primitive::RepoComboBoxDelegate *> rolesDelegates;
    QHash<QString, repo::gui::primitive::RepoComboBoxDelegate *> projectsDelegates;

    for (auto database : databases)
    {
        QString qDatabase = QString::fromStdString(database);

        //----------------------------------------------------------------------
        // Projects delegate
        std::list<std::string> projects = databasesWithProjects.find(database)->second;
        repo::gui::primitive::RepoComboBoxEditor::SeparatedEntries projectsEntries;
        projectsEntries << projects;
        QList<repo::gui::primitive::RepoComboBoxEditor::SeparatedEntries> projectsLists;
        projectsLists << dbEntries << projectsEntries;
        projectsDelegates.insert(qDatabase, new repo::gui::primitive::RepoComboBoxDelegate(projectsLists));

        //----------------------------------------------------------------------
        // Roles delegate
        repo::gui::primitive::RepoComboBoxDelegate *rolesDelegate =
            (controller->getNameOfAdminDatabase(token) == database)
                ? new repo::gui::primitive::RepoComboBoxDelegate(adminDBRolesLists)
                : new repo::gui::primitive::RepoComboBoxDelegate(anyDBRolesLists);
        rolesDelegates.insert(qDatabase, rolesDelegate);
    }

    // Projects
    QStringList defaultRow = {tr("admin"), tr("<empty>")};

    // Roles
    ui->rolesUnfilterableTreeWidget->setHeaders({tr("Database"), tr("Role")});
    ui->rolesUnfilterableTreeWidget->setNewRowText(defaultRow);
    ui->rolesUnfilterableTreeWidget->setDelegates(rolesDelegates);

    // API Keys
    ui->apiKeysUnfilterableTreeWidget->setHeaders({tr("Label"), tr("API Key")});
    setNextAPIKey();

    //Licenses
    ui->licensesUnfilterableTreeWidget->setHeaders({tr("License Type"), tr("Assigned to")});

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
        // Access Rights
        ui->rolesUnfilterableTreeWidget->addRows(user.getRolesList());
        ui->apiKeysUnfilterableTreeWidget->addRows(user.getAPIKeysList());
        updateLicenseWidget();
    }


    QObject::connect(ui->apiKeysUnfilterableTreeWidget,
                     &repo::gui::widget::UnfilterableTreeWidget::rowCountChanged,
                     this, &UserDialog::setNextAPIKey);

    QObject::connect(ui->licensesUnfilterableTreeWidget,
                     &repo::gui::widget::UnfilterableTreeWidget::rowCountChanged,
                     this, &UserDialog::addRemoveLicense);

    //--------------------------------------------------------------------------
    // Regular expression validator for email
    // See http://www.regular-expressions.info/email.html
    QRegExp emailRegularExpression("^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}$", Qt::CaseInsensitive);
    emailValidator = new QRegExpValidator(emailRegularExpression);
    ui->emailLineEdit->setValidator(emailValidator);

}

UserDialog::~UserDialog()
{
    delete ui;
    delete emailValidator;
}

void UserDialog::addRemoveLicense(int oldRowCount, int newRowCount)
{
    int diff = newRowCount - oldRowCount;
    auto modUser = user.cloneAndUpdateLicenseCount(diff);
    if(modUser.isEmpty())
    {
       repoLogError("Failed to add/remove license.");
    }
    else
        user = modUser;
    updateLicenseWidget();
}

QIcon UserDialog::getIcon()
{
   return repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(repo::gui::primitive::RepoFontAwesome::fa_user);
}

std::string UserDialog::getEmail() const
{
    return ui->emailLineEdit->text().toStdString();
}

std::string UserDialog::getFirstName() const
{
    return ui->firstNameLineEdit->text().toStdString();
}

std::list<std::pair<std::string, std::string> > UserDialog::getAPIKeys() const
{
    return ui->apiKeysUnfilterableTreeWidget->getItemsAsListOfPairsOfStrings();
}

std::string UserDialog::getLastName() const
{
    return ui->lastNameLineEdit->text().toStdString();
}

std::string UserDialog::getPassword() const
{
    std::string currentPassword = ui->passwordLineEdit->text().toStdString();
    return currentPassword != user.getPassword() ? currentPassword : "";
}

std::list<std::pair<std::string, std::string> > UserDialog::getRoles() const
{
    return ui->rolesUnfilterableTreeWidget->getItemsAsListOfPairsOfStrings();
}

std::string UserDialog::getUsername() const
{
    return ui->usernameLineEdit->text().toStdString();
}

void UserDialog::openImageFileDialog()
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

void UserDialog::setNextAPIKey()
{
    ui->apiKeysUnfilterableTreeWidget->setNewRowText(
        {tr("label"),QString::fromStdString(UUIDtoString(generateUUID()))});
}

repo::core::model::RepoUser UserDialog::getUpdatedUser() const
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
                getRoles(),
                getAPIKeys(),
                avatar);
}

void UserDialog::setAvatar(const std::vector<char> &image)
{
    if (!image.empty())
    {
		QImage qimage = QImage::fromData((uchar*)&(image.at(0)), image.size());
		setAvatar(qimage);
    }
}

void UserDialog::setAvatar(const QImage &image)
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

void UserDialog::updateLicenseWidget()
{
    ui->licensesUnfilterableTreeWidget->disableSignals();
    ui->licensesUnfilterableTreeWidget->removeAll();
    ui->licensesUnfilterableTreeWidget->addRows(user.getLicenseAssignment(), true, false);
    ui->licensesUnfilterableTreeWidget->enableSignals();

    double quota = user.getQuota();
    const static std::vector<std::string> units = {"B", "KB", "MB", "GB", "TB"};
    int unitSelection = 0;
    while(quota > 1024. && unitSelection < units.size()-1)
    {
        quota /= 1024.;
        ++unitSelection;
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << quota << " " <<  units[unitSelection];

    auto quotaStr = stream.str();

    ui->labelQuota->setText(QString::fromStdString(quotaStr));
    ui->labelCollaborators->setText(QString::fromStdString(std::to_string(user.getNCollaborators())));
}
