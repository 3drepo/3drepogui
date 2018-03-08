/**
*  Copyright (C) 2016 3D Repo Ltd
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
// Core
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson_collection_stats.h>
#include <repo/core/model/bson/repo_bson_database_stats.h>


//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"
#include "../logger/repo_logger.h"
#include "../gui/primitives/repo_standard_item_factory.h"

namespace repo {
namespace worker {

class RepositoriesWorker : public RepoAbstractWorker
{

    Q_OBJECT

public:

    RepositoriesWorker(
            repo::RepoController *controller,
            const repo::RepoController::RepoToken *token,
            const gui::primitive::RepoStandardItemRow &databaseRow);

    //! Default empty destructor.
    ~RepositoriesWorker();

signals:

    //! Emitted when all databases with associated projects are fetched.
    void projectFetched(const gui::primitive::RepoStandardItemRow &databseRow,
                        const gui::primitive::RepoStandardItemRow &projectRow);

public slots :

    void run();

private:

    //! Database connector.
    const repo::RepoController::RepoToken *token;

    //! Controller
    repo::RepoController *controller;

    gui::primitive::RepoStandardItemRow databaseRow;


}; // end class

}
}
