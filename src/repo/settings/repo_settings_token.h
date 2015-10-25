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
#include <repo/repo_controller.h>

Q_DECLARE_METATYPE(repo::RepoToken)

namespace repo {
namespace settings {

class RepoSettingsToken : public QSettings
{

    static const std::string TOKEN;
    static const std::string TOKENS_ARRAY;

public:

    RepoSettingsToken();

    ~RepoSettingsToken() {}

signals :

    void tokenReadAt(repo::RepoToken &token);

public :

    void writeTokens(QList<repo::RepoToken> &tokens);

    void readTokens() const;
};


} // end namespace settings
} // end namespace repo
