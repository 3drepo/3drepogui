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


#ifndef REPO_FEDERATION_DIALOG_H
#define REPO_FEDERATION_DIALOG_H

//------------------------------------------------------------------------------
// Qt
#include <QDialog>
#include <QStandardItem>
#include <QModelIndexList>
#include <QDialogButtonBox>
#include <QPair>

//------------------------------------------------------------------------------
// GUI
#include "../primitives/repoidbcache.h"
#include "repo_genericdialog.h"
#include "../widgets/repo_transformationwidget.h"

//------------------------------------------------------------------------------
// Core
#include <RepoGraphScene>
#include <RepoNodeAbstract>
#include <RepoNodeTransformation>
#include <RepoNodeReference>

namespace Ui {
    class RepoFederationDialog;
}

namespace repo {
namespace gui {

typedef QPair<core::RepoNodeTransformation*, core::RepoNodeReference* > RepoTransRefPair;

Q_DECLARE_METATYPE(RepoTransRefPair)

class RepoFederationDialog : public QDialog
{
    Q_OBJECT

    enum Columns { PROJECT = 0, BRANCH = 1, REVISION = 2 };

public:

    explicit RepoFederationDialog(
            RepoIDBCache *dbCache,
            QWidget *parent = 0);

    ~RepoFederationDialog();

public slots :

    void addAvailableProject(const QString &);

    void addProjectsToFederation();

    int exec();

    void refresh();

    void refreshCache() { dbCache->refresh(); }

    void removeProjectsFromFederation();

    void showFederationMenu(const QPoint &);

    //--------------------------------------------------------------------------

    QStandardItem *getCurrentFederatedItem() const;

    QModelIndexList getAvailableSelection() const;

    QModelIndexList getFederatedSelection() const;

    core::RepoGraphScene *getFederation();

protected :

    void getFederationRecursively(
            QStandardItem *parentItem,
            core::RepoNodeAbstract *parentNode);

private:

    Ui::RepoFederationDialog *ui;

    RepoIDBCache *dbCache;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_FEDERATION_DIALOG_H
