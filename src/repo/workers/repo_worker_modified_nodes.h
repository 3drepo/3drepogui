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
#include <repo/core/model/collection/repo_scene.h>
//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"

namespace repo {
namespace worker {

/*!
* Worker class to that fetches individual databases from given Mongo client.
*/
class RepoWorkerModifiedNodes : public RepoAbstractWorker {

    Q_OBJECT

public:

    //! Maximum number of nodes to load in one go.
    static const int DEFAULT_LIMIT;

public:

    //! Default worker constructor.
    RepoWorkerModifiedNodes(repo::core::model::RepoScene *scene,
                            int skip = 0,
                            int limit = DEFAULT_LIMIT);

    //! Default empty destructor.
    ~RepoWorkerModifiedNodes();

public slots :

    /*!
    * Loads modified scene graph nodes and iterates through them one by one.
    */
    void run();

signals :

    //! Emitted whenever modified node is being iterated.
    void modifiedNode(repo::core::model::RepoNode *node, const QString &status);

private:

    //! Repo scene.
    repo::core::model::RepoScene *scene;

    //! Number of nodes to skip when retrieveing.
    int skip;

    //! Max number of nodes to load.
    int limit;

}; // end class

} // end namespace worker
} // end namespace repo
