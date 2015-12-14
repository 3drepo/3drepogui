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

#include "repo_dialog_connect.h"

using namespace repo::widgets;

//------------------------------------------------------------------------------
RepoDialogConnect::RepoDialogConnect(
        repo::RepoController *controller,
        const repo::RepoCredentials &credentials,
        const bool isCopy,
        QWidget *parent,
        Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , ui(new Ui::RepoDialogConnect)
    , controller(controller)
{
    ui->setupUi(this);
    setWindowIcon(repo::gui::RepoFontAwesome::getConnectIcon());

    // Alias
    QString alias = QString::fromStdString(credentials.getAlias());
    if (isCopy)
    {
        alias += " " + tr("(Copy)");
    }
    ui->aliasLineEdit->setText(alias);

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
    ui->tabWidget->setTabEnabled((int) Tab::SSL, false); // SSL tab disabled
    ui->tabWidget->setTabEnabled((int) Tab::SSH, false); // SSH tab disabled

    ui->validateProgressBar->hide();

    connect(ui->validatePushButton, SIGNAL(pressed()), this, SLOT(validate()));
}

//------------------------------------------------------------------------------
RepoDialogConnect::~RepoDialogConnect()
{
    delete databasesCompleter;
    delete ui;
}

void RepoDialogConnect::validate()
{
//    ui->validateProgressBar->show();

    // TODO: make asynchronous
    repo::RepoCredentials credentials = getConnectionSettings();

    controller->testConnection(credentials);

//    ui->validateProgressBar->hide();
}

repo::RepoCredentials RepoDialogConnect::getConnectionSettings() const
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
