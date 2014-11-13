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

#ifndef REPO_DIALOG_HISTORY_H
#define REPO_DIALOG_HISTORY_H
//------------------------------------------------------------------------------
// STL
#include <string>
#include <vector>
//------------------------------------------------------------------------------
// Qt
#include <QtWidgets/QDialog>
#include <QtGui>
#include <QList>
#include <QUuid>
#include <QThreadPool>
//------------------------------------------------------------------------------
// Repo Core
#include <RepoWrapperMongo>
#include "ui_repo_dialoghistory.h"
//------------------------------------------------------------------------------

namespace Ui {
    class RepoDialogHistory;
}

namespace repo {
namespace gui {
	
class RepoDialogHistory : public QDialog
{
	Q_OBJECT

	//! Databases table header positions
	enum RepoHistoryColumns { REVISION, BRANCH, MESSAGE, AUTHOR, TIMESTAMP };

	//! Revision table header positions
	enum RepoRevisionColumns { SID, ACTION };
  
public:

	//! Constuctor
	RepoDialogHistory(
        const core::MongoClientWrapper &mongo,
        const QString &database,
        QWidget *parent = 0,
		Qt::WindowFlags flags = 0);

	//! Destructor
	~RepoDialogHistory();

signals :

	//! Emitted whenever running threads are to be cancelled.
	void cancel();

public slots:

	//! Cancels all running threads and waits for their completion.
	bool cancelAllThreads();

	//! Forces refresh.
	int exec();
	
	//! Refreshes the history model
	void refresh();

	//! Adds a revision row to the history model
	void addRevision(
		QVariant uid, 
		QVariant sid, 
		QVariant message, 
		QVariant author, 
		QVariant timestamp);

	//! Clears the history model (does not remove headers)
	void clearHistoryModel();

public :

	//! Returns a list of selected revisions' unique IDs (UIDs), empty list if none selected.
	QList<QUuid> getSelectedRevisions();

    //--------------------------------------------------------------------------
	//
	// Static helpers
	//
    //--------------------------------------------------------------------------

	//! Returns icon specific to the history dialog.
	static QIcon getIcon();

	//! Returns a non-editable item with set tooltip.
    static QStandardItem *createItem(QVariant& data);

private:	

    //! Ui var.
    Ui::RepoDialogHistory *ui;
		
	//! Model of the revision history.
    QStandardItemModel *historyModel;

	//! Proxy model for revision history to enable filtering.
    QSortFilterProxyModel *historyProxy;

	//! Model of a single revision.
    QStandardItemModel *revisionModel;

	//! Client connection.
    core::MongoClientWrapper mongo;

	//! Database to retrieve revision history from.
	QString database;

	//! Threadpool for this object only.
	QThreadPool threadPool;
 
}; // end class

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_HISTORY_H
