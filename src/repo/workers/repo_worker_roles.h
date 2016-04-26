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

//-----------------------------------------------------------------------------
// Repo Core
#include <repo/repo_controller.h>

//-----------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"
#include "../logger/repo_logger.h"

namespace repo {
namespace worker {

//! Worker class that fetches individual roles from given Mongo client.
class RepoWorkerRoles : public RepoAbstractWorker
{

    Q_OBJECT

public:

    enum class Command { DROP, INSERT, UPDATE };

    RepoWorkerRoles(
            const repo::RepoController::RepoToken *token,
            repo::RepoController *controller,
            const std::string &database,
            const repo::core::model::RepoRole &role = repo::core::model::RepoRole(),
            const repo::core::model::RepoRoleSettings &settings = repo::core::model::RepoRoleSettings(),
            Command command = Command::INSERT);

    //! Default empty destructor.
    ~RepoWorkerRoles();

signals:

    //! Emitted when a single role is fetched.
    void roleFetched(const repo::core::model::RepoRole &,
                     const repo::core::model::RepoRoleSettings &);

public slots :

    void run();

private:

    //! Database connector.
    const repo::RepoController::RepoToken *token;

    //! Connection controller.
    repo::RepoController *controller;

    //! Database name.
    const std::string database;

    repo::core::model::RepoRole role;

    repo::core::model::RepoRoleSettings settings;

    Command command;

}; // end class

} // end namespace worker
} // end namespace repo



