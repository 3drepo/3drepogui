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
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_HOST		= "RepoDialogConnect/host";
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_PORT		= "RepoDialogConnect/port";
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_USERNAME	= "RepoDialogConnect/username";
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_PASSWORD	= "RepoDialogConnect/password";
const QString repo::gui::RepoDialogConnect::REPO_SETTINGS_CONNECTION_DIALOG_STARTUP     = "RepoDialogConnect/startup";

//------------------------------------------------------------------------------
repo::gui::RepoDialogConnect::RepoDialogConnect(
        QWidget *parent,
        Qt::WindowFlags flags)
	: QDialog(parent, flags)
    , ui(new Ui::RepoDialogConnect)
{
    ui->setupUi(this);
	this->setWindowIcon(getIcon());

    ui->hostLineEdit->setText(
                settings.value(REPO_SETTINGS_CONNECTION_DIALOG_HOST,
                               "localhost").toString());
    ui->portLineEdit->setText(
                settings.value(REPO_SETTINGS_CONNECTION_DIALOG_PORT,
                               27017).toString());
    ui->usernameLineEdit->setText(
                settings.value(REPO_SETTINGS_CONNECTION_DIALOG_USERNAME,
                               "anonymous").toString());

    // TODO: save encrypted binary version of the password,
    // see http://qt-project.org/wiki/Simple_encryption
    ui->passwordLineEdit->setText(
                settings.value(REPO_SETTINGS_CONNECTION_DIALOG_PASSWORD,
                               "").toString());
    ui->passwordLineEdit->setFocus();


    //--------------------------------------------------------------------------
	QStringList wordList;
	wordList << "admin";
	databasesCompleter = new QCompleter(wordList);
    ui->databaseLineEdit->setCompleter(databasesCompleter);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
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

bool repo::gui::RepoDialogConnect::isShowAtStartup()
{
    return settings.value(REPO_SETTINGS_CONNECTION_DIALOG_STARTUP).toBool();
}

//------------------------------------------------------------------------------
QIcon repo::gui::RepoDialogConnect::getIcon()
{
    return RepoFontAwesome::getInstance().getIcon(
        RepoFontAwesome::fa_sign_in, QColor(0, 122, 204)); // blue
}

//------------------------------------------------------------------------------
int repo::gui::RepoDialogConnect::exec()
{
	int result;
    //--------------------------------------------------------------------------
    // If the user confirms the selection and does not click "Cancel", save
    // values.
	if (result = QDialog::exec())
	{
        settings.setValue(REPO_SETTINGS_CONNECTION_DIALOG_HOST,
                          ui->hostLineEdit->text());
        settings.setValue(REPO_SETTINGS_CONNECTION_DIALOG_PORT,
                          ui->portLineEdit->text().toInt());
        settings.setValue(REPO_SETTINGS_CONNECTION_DIALOG_USERNAME,
                          ui->usernameLineEdit->text());
        settings.setValue(REPO_SETTINGS_CONNECTION_DIALOG_PASSWORD,
                          ui->passwordLineEdit->text());
	}
	return result;
}

