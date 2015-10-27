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
#include <QDataStream>
#include <QMetaType>

//------------------------------------------------------------------------------
// CORE
#include <repo/repo_credentials.h>

#include <iostream>

namespace repo {
namespace settings {


//! Wrapper class to enable serialization of RepoCredentials via Qt.
class RepoCredentialsStreamable
{

public :
    RepoCredentialsStreamable(
            const repo::RepoCredentials &credentials = repo::RepoCredentials())
        : credentials(credentials)
    {}

    friend QDataStream &operator<<(QDataStream &out, const RepoCredentialsStreamable &cs)
    {
        out << QString::fromStdString(cs.credentials.getAlias())
        << QString::fromStdString(cs.credentials.getAuthenticationDatabase())
        << QString::fromStdString(cs.credentials.getHost())
        << QString::fromStdString(cs.credentials.getPassword())
        << QString::number(cs.credentials.getPort())
        << QString::fromStdString(cs.credentials.getUsername());
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, RepoCredentialsStreamable &cs)
    {
       QString alias;
       in >>  alias;
       QString authenticationDatabase;
       in >> authenticationDatabase;
       QString host;
       in >> host;
       QString password;
       in >> password;
       QString port;
       in >> port;
       QString username;
       in >> username;

       cs = RepoCredentialsStreamable(
                   repo::RepoCredentials(alias.toStdString(),
                                 host.toStdString(),
                                 port.toInt(),
                                 authenticationDatabase.toStdString(),
                                 username.toStdString(),
                                 password.toStdString()));

       return in;
    }

    repo::RepoCredentials credentials;
};

} // end namespace settings
} // end namespace repo

Q_DECLARE_METATYPE(repo::settings::RepoCredentialsStreamable)

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
    void writeCredentials(QList<repo::RepoCredentials> &credentialsList);

    //! Emits a singal upon each stored credentials and returns them in a list.
    QList<RepoCredentials> readCredentials();
};


} // end namespace settings
} // end namespace repo
