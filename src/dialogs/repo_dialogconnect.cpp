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
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_HOST		= "RepoConnectionDialog/host";
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_PORT		= "RepoConnectionDialog/port";
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_USERNAME	= "RepoConnectionDialog/username";
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_PASSWORD	= "RepoConnectionDialog/password";

//------------------------------------------------------------------------------
repo::gui::RepoDialogConnect::RepoDialogConnect(
        QWidget *parent,
        Qt::WindowFlags flags)
	: QDialog(parent, flags)
    , ui(new Ui::RepoDialogConnect)
{
    ui->setupUi(this);
	this->setWindowIcon(getIcon());

    ui->lineEditHost->setText(
                settings.value(REPO_SETTINGS_CONNECTION_DIALOG_HOST,
                               "localhost").toString());
    ui->lineEditPort->setText(
                settings.value(REPO_SETTINGS_CONNECTION_DIALOG_PORT,
                               27017).toString());
    ui->lineEditUserName->setText(
                settings.value(REPO_SETTINGS_CONNECTION_DIALOG_USERNAME,
                               "anonymous").toString());

	// TODO: save encrypted binary version of the password, see http://qt-project.org/wiki/Simple_encryption
    ui->lineEditPassword->setText(
                settings.value(REPO_SETTINGS_CONNECTION_DIALOG_PASSWORD,
                               "").toString());
    ui->lineEditPassword->setFocus();

    //--------------------------------------------------------------------------
	QStringList wordList;
	wordList << "admin";
	databasesCompleter = new QCompleter(wordList);
    ui->lineEditDatabase->setCompleter(databasesCompleter);
}

//------------------------------------------------------------------------------
repo::gui::RepoDialogConnect::~RepoDialogConnect() 
{
	settings.sync();
	delete databasesCompleter; 
    delete ui;
}

//------------------------------------------------------------------------------
QString repo::gui::RepoDialogConnect::getHost()
{
	return settings.value(REPO_SETTINGS_CONNECTION_DIALOG_HOST).toString();
}

//------------------------------------------------------------------------------
int repo::gui::RepoDialogConnect::getPort()
{
	return settings.value(REPO_SETTINGS_CONNECTION_DIALOG_PORT).toInt();
}

//------------------------------------------------------------------------------
QString repo::gui::RepoDialogConnect::getUsername()
{
	return settings.value(REPO_SETTINGS_CONNECTION_DIALOG_USERNAME).toString();
}

//------------------------------------------------------------------------------
QString repo::gui::RepoDialogConnect::getPassword()
{
	return settings.value(REPO_SETTINGS_CONNECTION_DIALOG_PASSWORD).toString();
}

//------------------------------------------------------------------------------
QIcon repo::gui::RepoDialogConnect::getIcon()
{
    return RepoFontAwesome::getInstance().getIcon(
        RepoFontAwesome::fa_sign_in, QColor(0,122,204));
}

//------------------------------------------------------------------------------
int repo::gui::RepoDialogConnect::exec()
{
	int result;
    //--------------------------------------------------------------------------
	// If the user confirms the selection and does not click "Cancel", save values.
	if (result = QDialog::exec())
	{
        settings.setValue(REPO_SETTINGS_CONNECTION_DIALOG_HOST,
                          ui->lineEditHost->text());
        settings.setValue(REPO_SETTINGS_CONNECTION_DIALOG_PORT,
                          ui->lineEditPort->text().toInt());
        settings.setValue(REPO_SETTINGS_CONNECTION_DIALOG_USERNAME,
                          ui->lineEditUserName->text());
        settings.setValue(REPO_SETTINGS_CONNECTION_DIALOG_PASSWORD,
                          ui->lineEditPassword->text());
	}
	return result;
}

