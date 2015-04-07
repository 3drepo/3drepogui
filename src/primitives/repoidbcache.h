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

#ifndef REPO_I_DB_CACHE_H
#define REPO_I_DB_CACHE_H

//------------------------------------------------------------------------------
// Core
#include <RepoWrapperMongo>

//------------------------------------------------------------------------------
// Qt
#include <QList>
#include <QString>

namespace repo {
namespace gui {

//! Interface to database cache.
class RepoIDBCache
{

public:

    virtual ~RepoIDBCache() {}

    //! Returns a list of available collections.
    virtual QList<QString> getCollections(
            const QString& host,
            const QString& database) const = 0;

    //! Returns connection corresponding to given host.
    virtual core::MongoClientWrapper getConnection(const QString &host) const = 0;

    //! Returns a list of available databases.
    virtual QList<QString> getDatabases(const QString& host) const = 0;

    //! Returns a list of available hosts.
    virtual QList<QString> getHosts() const = 0;

    //! Refreshes cache.
    virtual void refresh() = 0;

    //! Returns selected connection, needs to be reconnected and reauthenticated.
    virtual core::MongoClientWrapper getSelectedConnection() const = 0;

    //! Returns selected database.
    virtual QString getSelectedDatabase() const = 0;

    //! Returns selected host.
    virtual QString getSelectedHost() const = 0;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_I_DB_CACHE_H
