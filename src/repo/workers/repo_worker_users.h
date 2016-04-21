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

#pragma once

//------------------------------------------------------------------------------
// Repo Core
#include <repo/repo_controller.h>

//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"
#include "../logger/repo_logger.h"

namespace repo {
namespace worker {

//! Worker class that fetches individual users from given Mongo client.
class UsersWorker : public RepoAbstractWorker
{
    Q_OBJECT

public:

    enum class Command { DROP, INSERT, UPDATE };

    /**
            * Default worker constructor.
            * @param token repo token for the connection
            * @param controller controller to talk to the core library
            * @param database name of the database where users are stored
            * @param user user to update/delete (empty if none)
            * @param isUpdate true - update/insert user, false - delete user
            */
    UsersWorker(
            const repo::RepoController::RepoToken *token,
            repo::RepoController  *controller,
            const std::string &database,
            const repo::core::model::RepoUser &user = repo::core::model::RepoUser(),
            const Command            &op = Command::INSERT);

    //! Default empty destructor.
    ~UsersWorker();

signals:

    //! Emitted when user is fetched.
    void userFetched(const repo::core::model::RepoUser &);

    //! Emitted when custom roles are fetched.
    void customRolesFetched(const std::list<std::string> &);

public slots :

    void run();

private:

    //! Database connector.
    const repo::RepoController::RepoToken *token;
    repo::RepoController *controller;

    //! Database name.
    const std::string database;

    //! User to be create or updated or deleted in the database.
    const repo::core::model::RepoUser user;

    //! Command to execute on user.
    const Command command;

}; // end class

} // end namespace worker
} // end namespace repo


