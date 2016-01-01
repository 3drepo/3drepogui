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
// STL
#include <string>
//------------------------------------------------------------------------------
// Qt
#include <QtWidgets/QDialog>
#include <QtGui>
#include <QStandardItemModel>
#include <QThreadPool>
#include <QModelIndex>
#include <QTreeView>
#include <QScrollBar>
#include <QMutex>

//------------------------------------------------------------------------------
// Repo GUI
#include "ui_repo_dialog_commit.h"
#include "repo_dialog_transformation.h"
#include "../widgets/repo_lineedit.h"
#include "../primitives/repoidbcache.h"
#include "../../workers/repo_worker_modified_nodes.h"

//------------------------------------------------------------------------------
// Repo Core
#include <repo/core/model/collection/repo_scene.h>

namespace Ui {
class CommitDialog;
}

Q_DECLARE_METATYPE(repo::core::model::RepoNode*)

namespace repo {
namespace gui {
	namespace dialog{

		/*!fontawesome-webfont
		 * Commit dialog which enables users to confirm those nodes that are to be
		 * committed to the repository. The dialog modifies the "revision" object
		 * accordingly.
		 */
		class CommitDialog : public QDialog
		{
			Q_OBJECT

				Q_CLASSINFO("Since", "1.0.0")

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
			CommitDialog(
				QWidget* parent = nullptr,
				Qt::WindowFlags flags = nullptr,
				RepoIDBCache *dbCache = nullptr,
				repo::core::model::RepoScene * scene = nullptr);

			//! Destructs the tree model.
			~CommitDialog();

			public slots:

			//! Cancels all ongoing threads, if any.
			bool cancelAllThreads();

			void infiniteScroll(int sliderPosition);

		signals:

			//! Emitted whenever running threads are to be cancelled.
			void cancel();

		public:

			//! Returns the commit message
			QString getMessage();

			QString getCurrentHost() const;

			QString getCurrentDatabase() const;

			QString getCurrentProject() const;

			repo::core::model::RepoScene* getScene() { return scene; }

			public slots:

			void addNode(repo::core::model::RepoNode *node, const QString &status);

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

			//! Unlocks mutex when modified nodes loading has finished.
			void unlockMutex() { mutex.unlock(); }

		private:

			//! Extracts modified objects from the revision and scene to list in change table.
			void loadModifiedObjects();

		private:

			//! Ui var.
			Ui::CommitDialog *ui;

			//! Scene from which the nodes to be committed come (based on info from revision object).
			repo::core::model::RepoScene *scene;

			//! Data model to list commit table.
			QStandardItemModel *model;

			//! Proxy model to enable table sorting.
			QSortFilterProxyModel *proxyModel;

			//! Database cache.
			RepoIDBCache *dbCache;

			//! Selected project name.
			QString projectName;

			//! Threadpool for this object only.
			QThreadPool threadPool;

			//! Number of scene graph nodes to be skipped when loading modified objects.
			int skip;

			//! Number of modified nodes.
			int modifiedNodesCount;

			//! Mutex lock for loading modified objects.
			QMutex mutex;
		};

}
} // end namespace gui
} // end namespace repo

