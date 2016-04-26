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

//------------------------------------------------------------------------------
// Repo Core
#include <repo/repo_controller.h>

//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"
#include "../logger/repo_logger.h"

namespace repo {
namespace worker {

class RepoWorkerProjects : public RepoAbstractWorker
{

    Q_OBJECT

public:

    RepoWorkerProjects(
            repo::RepoController *controller,
            const repo::RepoController::RepoToken *token);

    //! Default empty destructor.
    ~RepoWorkerProjects();

signals:

    //! Emitted when databases are fetched.
    void databasesFetched(const std::list<std::string> &);

    //! Emitted when all databases with associated projects are fetched.
    void databasesWithProjectsFetched(const std::map<std::string, std::list<std::string> >&);

public slots :

    void run();

private:

    //! Database connector.
    const repo::RepoController::RepoToken *token;

    //! Controller
    repo::RepoController *controller;


}; // end class

} // end namespace worker
} // end namespace repo


