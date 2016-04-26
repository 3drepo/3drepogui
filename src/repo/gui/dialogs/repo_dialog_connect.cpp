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

using namespace repo::gui::dialog;

//------------------------------------------------------------------------------
ConnectDialog::ConnectDialog(
	repo::RepoController *controller,
	const std::string &credentials,
	const bool isCopy,
	QWidget *parent,
	Qt::WindowFlags flags)
	: QDialog(parent, flags)
	, ui(new Ui::ConnectDialog)
	, credentials(credentials)
	, controller(controller)
{
	ui->setupUi(this);
	setWindowIcon(repo::gui::primitive::RepoFontAwesome::getConnectIcon());

	std::string aliasStr, host, authDB, username;
	uint32_t port = 27017;

	repo::RepoController::RepoToken *token = nullptr;
	if (!credentials.empty())
	{
		token = controller->createTokenFromSerialised(credentials);
		controller->getInfoFromToken(token, aliasStr, host, port, username, authDB);
	}

	// Alias
	QString alias = QString::fromStdString(aliasStr);
	if (isCopy)
	{
		alias += " " + tr("(Copy)");
	}
	ui->aliasLineEdit->setText(alias);

	ui->hostLineEdit->setText(QString::fromStdString(host));
	ui->portLineEdit->setText(QString::number(port));
	ui->usernameLineEdit->setText(QString::fromStdString(username));
	ui->authenticationDatabaseLineEdit->setText(
		QString::fromStdString(authDB));

	// TODO: save encrypted binary version of the password,
	// see http://qt-project.org/wiki/Simple_encryption

	if (token)
		delete token;

	//--------------------------------------------------------------------------

	// TODO: make it remember more databases.
	QStringList wordList;
	wordList << "admin";
	databasesCompleter = new QCompleter(wordList);
	ui->authenticationDatabaseLineEdit->setCompleter(databasesCompleter);

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	// TODO: code in support for SSL and SSH
	ui->tabWidget->setTabEnabled((int)Tab::SSL, false); // SSL tab disabled
	ui->tabWidget->setTabEnabled((int)Tab::SSH, false); // SSH tab disabled

	ui->validateProgressBar->hide();

	connect(ui->validatePushButton, SIGNAL(pressed()), this, SLOT(validate()));
}

//------------------------------------------------------------------------------
ConnectDialog::~ConnectDialog()
{
	delete databasesCompleter;
	delete ui;
}

void ConnectDialog::validate()
{
	//    ui->validateProgressBar->show();

	// TODO: make asynchronous
	repo::RepoController::RepoToken *token = getConnectionSettings();

	controller->testConnection(token);

	//    ui->validateProgressBar->hide();
}

repo::RepoController::RepoToken* ConnectDialog::getConnectionSettings() const
{
	const auto oldToken = controller->createTokenFromSerialised(credentials);
	repo::RepoController::RepoToken *res;
	if (!oldToken ||
		!ui->passwordLineEdit->text().toStdString().empty() ||
		(ui->passwordLineEdit->text().toStdString().empty() && ui->usernameLineEdit->text().toStdString().empty()))
	{
		//password unchanged
		res = controller->createToken(
			ui->aliasLineEdit->text().toStdString(),
			ui->hostLineEdit->text().toStdString(),
			ui->portLineEdit->text().toInt(),
			ui->authenticationDatabaseLineEdit->text().toStdString(),
			ui->usernameLineEdit->text().toStdString(),
			ui->passwordLineEdit->text().toStdString());
	}
	else
	{
		res = controller->createToken(
			ui->aliasLineEdit->text().toStdString(),
			ui->hostLineEdit->text().toStdString(),
			ui->portLineEdit->text().toInt(),
			ui->authenticationDatabaseLineEdit->text().toStdString(),
			oldToken);
	}

	if (oldToken) delete oldToken;
	return res;
}