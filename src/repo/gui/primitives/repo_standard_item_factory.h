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

#include "repo_standard_item.h"
#include "repo_fontawesome.h"

// CORE
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson_database_stats.h>

namespace repo {
namespace gui{
namespace primitive {

struct RepoStandardItemRow : QList<RepoStandardItem *>
{
    const QList<QStandardItem *> toQList() const
    { return reinterpret_cast<const QList<QStandardItem *> &>(*this); }
};

//! Databases header positions
enum RepoDatabasesColumns { NAME = 0, COUNT = 1, SIZE = 2, ALLOCATED = 3 };

class RepoStandardItemFactory
{

public:

    static RepoStandardItemRow makeHostRow(
            RepoController *controller,
            RepoController::RepoToken *token);

    static RepoStandardItemRow makeDatabaseRow(const std::string &database);
};

}
}
}
