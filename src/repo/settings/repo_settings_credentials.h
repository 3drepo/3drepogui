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

#pragma once
#include <QSettings>

//------------------------------------------------------------------------------
// CORE
#include <repo/repo_credentials.h>

Q_DECLARE_METATYPE(repo::RepoCredentials)

namespace repo {
namespace settings {

class RepoSettingsCredentials : public QSettings
{

    static const QString CREDENTIALS;
    static const QString CREDENTIALS_ARRAY;

public:

    RepoSettingsCredentials();

    ~RepoSettingsCredentials() {}

signals :

    void credentialsAt(int i, repo::RepoCredentials &credentials);

public :

    //! Stores given credentials in settings
    void writeCredentials(QList<repo::RepoCredentials> &credentials);

    //! Emits a singal upon each stored credentials and returns them in a list.
    QList<RepoCredentials> readCredentials();
};


} // end namespace settings
} // end namespace repo
