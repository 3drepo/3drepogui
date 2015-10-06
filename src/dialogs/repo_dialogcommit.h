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

#ifndef REPO_DIALOG_COMMIT_H
#define REPO_DIALOG_COMMIT_H

//------------------------------------------------------------------------------
// STL
#include <string>
//------------------------------------------------------------------------------
// Qt
#include <QtWidgets/QDialog>
#include <QtGui>
#include <QStandardItemModel>
#include <QModelIndex>

//------------------------------------------------------------------------------
// Repo GUI
#include "ui_repo_dialogcommit.h"
#include "../widgets/repo_lineedit.h"
#include "../primitives/repoidbcache.h"
#include "repo_transformationdialog.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Repo Core
#include <repo/core/model/collection/repo_scene.h>

namespace Ui {
    class RepoDialogCommit;
}

Q_DECLARE_METATYPE(repo::core::model::RepoNode*);

namespace repo {
namespace gui {

/*!fontawesome-webfont
 * Commit dialog which enables users to confirm those nodes that are to be 
 * committed to the repository. The dialog modifies the "revision" object 
 * accordingly.
 */
class RepoDialogCommit : public QDialog
{
	Q_OBJECT

    enum Columns { NAME, TYPE, STATUS, UID, SID };

public:

    //--------------------------------------------------------------------------
	//
	// Constructor
	//
    //--------------------------------------------------------------------------

	/*!
	 * Constructs the commit dialog with specified destination and a table of
	 * scene graph nodes to be commited. Use Qt::Window for flag to enable
	 * dialog to have minimize/maximize buttons.
	 */
    RepoDialogCommit(
        QWidget* parent = nullptr,
		Qt::WindowFlags flags = nullptr,
		RepoIDBCache *dbCache = nullptr,
		repo::core::model::RepoScene * scene = nullptr);

    //--------------------------------------------------------------------------
	//
	// Destructor
	//
    //--------------------------------------------------------------------------

	//! Destructs the tree model.
	~RepoDialogCommit();

	//! Returns the commit message
	QString getMessage();

	//! Returns the default green icon for the commit dialog.
	static QIcon getIcon();

    QString getCurrentHost() const;

    QString getCurrentDatabase() const;

    QString getCurrentProject() const;

	repo::core::model::RepoScene* getScene() { return scene; }

public slots:

    void editItem(const QModelIndex &);

    /*!
     * Shows the dialog as a modal window, blocking until the user closes it.
     * If the user selects to connect, the dialog saves all filled-in values
     * as user settings. Returns a DialogCode result.
     */
	virtual int exec();

    void updateCountLabel() const;

    void updateHosts();

    void updateDatabases();

    void updateProjects();

    void updateBranches();



private :

    //! Extracts modified objects from the revision and scene to list in change table.
    void setModifiedObjects();

private:

    //! Ui var.
    Ui::RepoDialogCommit *ui;
		   
	//! Scene from which the nodes to be committed come (based on info from revision object).
	repo::core::model::RepoScene *scene;

	//! Data model to list commit table.
	QStandardItemModel *model;

	//! Proxy model to enable table sorting.
	QSortFilterProxyModel *proxyModel;

    RepoIDBCache *dbCache;

    QString projectName;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_COMMIT_H
