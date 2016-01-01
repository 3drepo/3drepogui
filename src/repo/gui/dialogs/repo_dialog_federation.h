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
// Qt
#include <QDialog>
#include <QStandardItem>
#include <QModelIndexList>
#include <QDialogButtonBox>
#include <QPair>
#include <QMenu>
#include <QTreeView>

//------------------------------------------------------------------------------
// GUI
#include "../primitives/repoidbcache.h"
#include "repo_dialog_generic.h"
#include "repo_dialog_transformation.h"
#include "../primitives/repo_standard_item.h"

//------------------------------------------------------------------------------
// Core
#include <repo/core/model/bson/repo_bson_factory.h>
#include <repo/core/model/collection/repo_scene.h>

namespace Ui {
    class repo_dialog_federation;
}

typedef QPair<repo::core::model::TransformationNode, repo::core::model::ReferenceNode> RepoTransRefPair;

Q_DECLARE_METATYPE(RepoTransRefPair)

namespace repo {
namespace gui {

class repo_dialog_federation : public QDialog
{
    Q_OBJECT

    enum Columns { PROJECT = 0, BRANCH = 1, REVISION = 2 };

public:

    explicit repo_dialog_federation(
            RepoIDBCache *dbCache,
            QWidget *parent = 0);

    ~repo_dialog_federation();

public slots :

    void addAvailableProject(const QString &);

    void addProjectsToFederation();

    int exec();

    void refresh();

    void refreshCache() { dbCache->refresh(); }

    void removeProjectsFromFederation();

    void showFederationMenu(const QPoint &);

    void showTransformationDialog();

    //--------------------------------------------------------------------------

    QStandardItem *getCurrentFederatedItem() const;

	repo::core::model::TransformationNode getCurrentFederatedTransformation() const;

    QModelIndexList getAvailableSelection() const;

    QModelIndexList getFederatedSelection() const;

	std::map<repo::core::model::TransformationNode, repo::core::model::ReferenceNode>  getFederation();

protected :

	std::map<repo::core::model::TransformationNode, repo::core::model::ReferenceNode>
        getFederationRecursively(
		QStandardItem *parentItem);

private:

    Ui::repo_dialog_federation *ui;

    RepoIDBCache *dbCache;
};

} // end namespace gui
} // end namespace repo

