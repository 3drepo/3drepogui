/**
 *  Copyright (C) 2014 3D Repo Ltd
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

#include "repo_dialogconnect.h"

//------------------------------------------------------------------------------
repo::gui::RepoDialogConnect::RepoDialogConnect(
        RepoController *controller,
        const RepoCredentials &credentials,
        QWidget *parent,
        Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , ui(new Ui::RepoDialogConnect)
    , controller(controller)
{
    ui->setupUi(this);
    setWindowIcon(RepoFontAwesome::getConnectIcon());

    ui->aliasLineEdit->setText(QString::fromStdString(credentials.getAlias()));
    ui->hostLineEdit->setText(QString::fromStdString(credentials.getHost()));
    ui->portLineEdit->setText(QString::number(credentials.getPort()));
    ui->usernameLineEdit->setText(QString::fromStdString(credentials.getUsername()));
    ui->authenticationDatabaseLineEdit->setText(
                QString::fromStdString(credentials.getAuthenticationDatabase()));

    // TODO: save encrypted binary version of the password,
    // see http://qt-project.org/wiki/Simple_encryption
    ui->passwordLineEdit->setText(QString::fromStdString(credentials.getPassword()));

    //--------------------------------------------------------------------------

    // TODO: make it remember more databases.
    QStringList wordList;
    wordList << "admin";
    databasesCompleter = new QCompleter(wordList);
    ui->authenticationDatabaseLineEdit->setCompleter(databasesCompleter);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    // TODO: code in support for SSL and SSH
    ui->tabWidget->setTabEnabled(2, false); // SSL tab disabled
    ui->tabWidget->setTabEnabled(3, false); // SSH tab disabled

    ui->validateProgressBar->hide();

    connect(ui->validatePushButton, SIGNAL(pressed()), this, SLOT(validate()));
}

//------------------------------------------------------------------------------
repo::gui::RepoDialogConnect::~RepoDialogConnect() 
{
    delete databasesCompleter;
    delete ui;
}

void repo::gui::RepoDialogConnect::validate()
{
//    ui->validateProgressBar->show();

    // TODO: make asynchronous
    repo::RepoCredentials credentials = getConnectionSettings();
    std::string errMsg;
    repo::RepoToken* connectionToken = 0;
    if (controller)
    {
        connectionToken = controller->authenticateToAdminDatabaseMongo(
        errMsg,
        credentials.getHost(),
        credentials.getPort(),
        credentials.getUsername(),
        credentials.getPassword());
    }

    if (connectionToken)
    {
        std::cout << "Connection established." << std::endl;
//        ui->validateProgressBar->setValue(ui->validateProgressBar->maximum());
    }
    else
    {
        //connection/authentication failed
        std::cerr << "Failed to connect/authenticate user. " << errMsg << std::endl;
    }

//    ui->validateProgressBar->hide();
}

repo::RepoCredentials repo::gui::RepoDialogConnect::getConnectionSettings() const
{
    repo::RepoCredentials credentials(
                ui->aliasLineEdit->text().toStdString(),
                ui->hostLineEdit->text().toStdString(),
                ui->portLineEdit->text().toInt(),
                ui->authenticationDatabaseLineEdit->text().toStdString(),
                ui->usernameLineEdit->text().toStdString(),
                ui->passwordLineEdit->text().toStdString());
    return credentials;
}
