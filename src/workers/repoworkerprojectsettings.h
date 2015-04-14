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


#ifndef REPO_WORKER_PROJECT_SETTINGS_H
#define REPO_WORKER_PROJECT_SETTINGS_H

//-----------------------------------------------------------------------------
// Repo Core
#include <RepoWrapperMongo>
#include <RepoProjectSettings>

//-----------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"

namespace repo {
namespace gui {

class RepoWorkerProjectSettings : public RepoWorkerAbstract
{
    Q_OBJECT

public:
    RepoWorkerProjectSettings(
            const core::MongoClientWrapper &mongo,
            const std::string &database,
            const core::RepoBSON &command);

    ~RepoWorkerProjectSettings();

signals :

    void projectSettingsFetched(core::RepoProjectSettings);

public slots :

    void run();

private :

    core::MongoClientWrapper mongo;

    std::string database;

    core::RepoBSON command;

}; // end class

} // end namespace core
} // end namespace repo

#endif // REPO_WORKER_PROJECT_SETTINGS_H
