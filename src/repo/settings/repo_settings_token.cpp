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

#include "repo_settings_token.h"

//------------------------------------------------------------------------------
using namespace repo::settings;
//------------------------------------------------------------------------------

std::string TOKEN = "token";
std::string TOKENS_ARRAY = "tokens";

//------------------------------------------------------------------------------

RepoSettingsToken::RepoSettingsToken() : QSettings()
{
    qRegisterMetaType<repo::RepoToken>();
}

//------------------------------------------------------------------------------

void RepoSettingsToken::writeTokens(QList<repo::RepoToken> &tokens)
{
    beginWriteArray(TOKENS_ARRAY);
    for (int i = 0; i < logins.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue(TOKEN, tokens.at(i));
    }
    settings.endArray();
}

//------------------------------------------------------------------------------

void RepoSettingsToken::readTokens() const
{
    int size = beginReadArray(TOKENS_ARRAY);
    for (int i = 0; i < size; ++i) {
        setArrayIndex(i);
        repo::RepoToken token = settings.value(TOKEN).value<repo::RepoToken>();
        emit tokenReadAt(token);
    }
    endArray();
}
