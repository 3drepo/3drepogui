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

#include "repo_settings_credentials.h"
#include <repo/lib/repo_log.h>
#include <sstream>

//------------------------------------------------------------------------------
using namespace repo::settings;

const QString RepoSettingsCredentials::CREDENTIALS = "credentials";
const QString RepoSettingsCredentials::CREDENTIALS_ARRAY = "credentials_array";

//------------------------------------------------------------------------------

RepoSettingsCredentials::RepoSettingsCredentials() : QSettings()
{
	qRegisterMetaType<std::vector<char>>();
	//qRegisterMetaTypeStreamOperators<std::vector<char>>();
}

//------------------------------------------------------------------------------

void RepoSettingsCredentials::writeCredentials(QList<std::vector<char>> &serialisedTokens)
{
	beginWriteArray(CREDENTIALS_ARRAY);
	for (int i = 0; i < serialisedTokens.size(); ++i)
	{
		const auto serialisedToken = serialisedTokens.at(i);
		std::stringstream ss;
		for (const auto &c : serialisedToken)
		{
			ss << c;
		}
		repoLog("writing token of size: " + std::to_string(serialisedToken.size()));
		auto tokenStr = QString::fromStdString(ss.str());
		setArrayIndex(i);
		QVariant var;
		var.setValue(tokenStr);
		setValue(CREDENTIALS, var);
	}
	endArray();
}

//------------------------------------------------------------------------------

QList<std::vector<char>> RepoSettingsCredentials::readCredentials()
{
	int size = beginReadArray(CREDENTIALS_ARRAY);
	QList<std::vector<char>> credentialsList;
	for (int i = 0; i < size; ++i) {
		setArrayIndex(i);
		QString strToken = value(CREDENTIALS).value<QString>();
		std::string stdStringToken = strToken.toStdString();
		std::vector<char> token;
		token.resize(stdStringToken.size());
		memcpy(token.data(), stdStringToken.data(), stdStringToken.size());
		repoLog("reading token of size: " + std::to_string(token.size()));
		credentialsList.append(token);
		emit credentialsAt(i, token);
	}
	endArray();
	return credentialsList;
}