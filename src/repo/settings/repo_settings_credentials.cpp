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

//------------------------------------------------------------------------------
using namespace repo::settings;
//------------------------------------------------------------------------------

const QString RepoSettingsCredentials::CREDENTIALS = "credentials";
const QString RepoSettingsCredentials::CREDENTIALS_ARRAY = "credentials_array";

//------------------------------------------------------------------------------

RepoSettingsCredentials::RepoSettingsCredentials() : QSettings()
{
    qRegisterMetaType<repo::RepoCredentials>();
}

//------------------------------------------------------------------------------

void RepoSettingsCredentials::writeCredentials(QList<repo::RepoCredentials> &credentials)
{
    beginWriteArray(CREDENTIALS_ARRAY);
    for (int i = 0; i < credentials.size(); ++i)
    {
        setArrayIndex(i);
        QVariant var;
        var.setValue(credentials.at(i));
        setValue(CREDENTIALS, var);
    }
    endArray();
}

//------------------------------------------------------------------------------

QList<repo::RepoCredentials> RepoSettingsCredentials::readCredentials()
{    
    int size = beginReadArray(CREDENTIALS_ARRAY);
    QList<repo::RepoCredentials> credentialsList;
    for (int i = 0; i < size; ++i) {
        setArrayIndex(i);
        repo::RepoCredentials credentials = value(CREDENTIALS).value<repo::RepoCredentials>();
        credentialsList.append(credentials);
//        emit credentialsAt(i, credentials);
    }
    endArray();
    return credentialsList;
}
