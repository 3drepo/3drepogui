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
        const RepoCredentials &credentials,
        QWidget *parent,
        Qt::WindowFlags flags)
	: QDialog(parent, flags)
    , ui(new Ui::RepoDialogConnect)
{
    ui->setupUi(this);
    setWindowIcon(RepoFontAwesome::getConnectIcon());

    ui->aliasLineEdit->setText(QString::fromStdString(credentials.getAlias()));
    ui->hostLineEdit->setText(QString::fromStdString(credentials.getHost()));
    ui->portLineEdit->setText(QString::number(credentials.getPort()));
    ui->usernameLineEdit->setText(QString::fromStdString(credentials.getUsername()));

    // TODO: save encrypted binary version of the password,
    // see http://qt-project.org/wiki/Simple_encryption
//    ui->passwordLineEdit->setText(QString::fromStdString(credentials.getPassword()).toString());

    //--------------------------------------------------------------------------

    // TODO: make it remember more databases.
	QStringList wordList;
	wordList << "admin";
	databasesCompleter = new QCompleter(wordList);
    ui->authenticationDatabaseLineEdit->setCompleter(databasesCompleter);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

//------------------------------------------------------------------------------
repo::gui::RepoDialogConnect::~RepoDialogConnect() 
{
	delete databasesCompleter; 
    delete ui;
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
