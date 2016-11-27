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

//------------------------------------------------------------------------------
// Core
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson_collection_stats.h>
#include <repo/core/model/bson/repo_bson_database_stats.h>

//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"
#include "../gui/primitives/repo_standard_item_factory.h"

namespace repo {
    namespace worker {

        /*!
        * Worker class to that fetches individual databases from given Mongo client.
        */
        class DatabasesWorker : public RepoAbstractWorker {

            Q_OBJECT

            //! Databases header positions
            enum RepoDatabasesColumns { NAME = 0, COUNT = 1, SIZE = 2, ALLOCATED = 3 };

        public:

            //! Default worker constructor.
            DatabasesWorker(repo::RepoController *controller,
                            repo::RepoController::RepoToken *token,
                            const gui::primitive::RepoStandardItemRow &hostRow);

            //! Default empty destructor.
            ~DatabasesWorker();

        signals:

            void databaseFetched(const gui::primitive::RepoStandardItemRow &hostRow,
                                 const gui::primitive::RepoStandardItemRow &databaseRow);

            void databaseStatsFetched(
                    const gui::primitive::RepoStandardItemRow &hostRow,
                    const gui::primitive::RepoStandardItemRow &databaseRow,
                    const core::model::DatabaseStats &databaseStats);

            void collectionFetched(repo::core::model::CollectionStats stats);

        public slots :

            /*!
            * Loads individual databases and for each propagates their collections to
            * the given tree model.
            */
            void run();

        signals:

            //! Emitted when a single database processing is finished
            void databaseFinished(const QString &dbName);

        private:
            repo::RepoController *controller;
            repo::RepoController::RepoToken *token;
            repo::gui::primitive::RepoStandardItemRow hostRow;

        }; // end class

    } // end namespace worker
} // end namespace repo
