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

#ifndef REPO_WIDGET_REPOSITORY_H
#define REPO_WIDGET_REPOSITORY_H

//------------------------------------------------------------------------------
// Qt
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QCompleter>

//------------------------------------------------------------------------------
// Repo Core
#include <repo/repo_controller.h>

//------------------------------------------------------------------------------
// Repo GUI
#include "ui_repo_widgetrepository.h"
#include "../primitives/repo_fontawesome.h"
#include "../primitives/repoidbcache.h"
#include "../primitives/repo_sortfilterproxymodel.h"
#include "repo_lineedit.h"

namespace Ui {
    class RepoWidgetRepository;
}

namespace repo {
namespace gui {

class RepoWidgetRepository : public QWidget, public RepoIDBCache
{
	Q_OBJECT
		
	//! Databases header positions
    enum RepoDatabasesColumns { NAME = 0, COUNT = 1, SIZE = 2, ALLOCATED = 3 };

	//! Collection header positions
	enum RepoCollectionColumns { DOCUMENT = 0, VALUE = 1, TYPE = 2 };

public :

	//! Constructor
    explicit RepoWidgetRepository(QWidget* parent = 0);

	//! Desctructor, removes the custom title bar widget.
	~RepoWidgetRepository();
	
signals :

	/*! Signal emitted to cancel all running threads. 
     *	Use waitForDone() to make sure all have finished.
     */
	void cancel();




    //--------------------------------------------------------------------------
    //
    // RepoIDBCache
    //
    //--------------------------------------------------------------------------

public slots :

    virtual QList<QString> getCollections(const QString &host, const QString &database) const
    { return QList<QString>(); }

    ////! Returns connection corresponding to given host.
    //virtual core::MongoClientWrapper getConnection(const QString &host) const;

    //! Returns a list of available hosts.
    virtual QList<QString> getHosts() const;

    //! Returns a list of available projects in a given database (a subset of all collections).
    virtual QList<QString> getProjects(const QString &host, const QString &database) const;

    /*!
     * Returns a copy of a selected connection. It is necessary to reconnect
     * and reauthenticate.
     */
    //core::MongoClientWrapper getSelectedConnection() const { return mongo; }

    //! Returns a list of available databases.
    QList<QString> getDatabases(const QString& host) const;

    //! Returns selected host, empty string if none selected.
    QString getSelectedHost() const;

    //! Returns selected database, empty string if none selected.
    QString getSelectedDatabase() const;

    //--------------------------------------------------------------------------

public slots :

    //! Refreshes all connected databases. Only one at the moment.
    void refresh();
		
	//! Removes all threads from the thread pool and returns true if successful.
	bool cancelAllThreads();

	//! Fetches databases from the server.
	void fetchDatabases(repo::RepoController *controller, repo::RepoToken * token);

	//! Fetches currently selected collection (if any) from the server.
	//void fetchCollection();

	////! Fetches collection.
	//void fetchCollection(
	//	const repo::core::MongoClientWrapper& /* clientConnection */, 
	//	const QString& /* database */, 
	//	const QString& /* collection */);

	void addHost(QString name);

	void addDatabase(QString name);

    void addCollection(const repo::core::model::bson::CollectionStats &stats);

	void addKeyValuePair(
		QVariant /* key */, 
		QVariant /* value */, 
		QVariant /* type */,
		unsigned int depth = 0);

    //--------------------------------------------------------------------------
	//
	// Data management
	//
    //--------------------------------------------------------------------------

	//! Removes all items from the databases model.
	void clearDatabaseModel();

	//! Removes all items from the collection model.
	void clearCollectionModel();

    //! Changes the tab based on an index value.
	void changeTab(int index);

    //! Copies selected collection cell to clipboard.
	void copySelectedCollectionCellToClipboard();

    //! Expands all collection records.
    inline void expandAllCollectionRecords() { ui->collectionTreeView->expandAll(); }

    //! Increments the current database row.
    inline void incrementDatabaseRow() { databaseRowCounter++; }

public :

    //--------------------------------------------------------------------------
    //
    // Getters
    //
    //--------------------------------------------------------------------------



	//! Returns selected collection, empty string if none selected.
	QString getSelectedCollection() const;

    //! Returns selected project, empty string if none selected.
    QString getSelectedProject() const;

    //! Returns the databases tree view.
    QWidget *getDatabasesTreeView() const { return ui->databasesTreeView; }

    //! Returns the collection tree view.
    QWidget *getCollectionTreeView() const { return ui->collectionTreeView; }

    const QPoint &mapToGlobalDatabasesTreeView(const QPoint &pos)
        { return ui->databasesTreeView->viewport()->mapToGlobal(pos); }

    const QPoint &mapToGlobalCollectionTreeView(const QPoint &pos)
        { return ui->collectionTreeView->viewport()->mapToGlobal(pos); }

private :

	//! Returns a selected databases model corresponding to the NAME column.
	QModelIndex getSelectedDatabasesTreeViewIndex() const;

    QModelIndex getHostModelIndex(const QString& host) const;

    //--------------------------------------------------------------------------
	//
	// Static helpers
	//
    //--------------------------------------------------------------------------
	
	/*! Returns a hierarchy depth for given model index. 
	    Root is depth 0, top level items are 1, their children are 2, etc.
	*/
	static unsigned int getHierarchyDepth(const QModelIndex&); 

	static QStandardItem* getHierarchyDepth(
		const QStandardItemModel* /* model */, 
		unsigned int /* depth */);

	/*! Sets the appropriate flags for case insensitive filtering for all 
	 * columns of a model.
	 */
	static void enableFiltering(
		QAbstractItemView*, 
		QStandardItemModel*, 
		QSortFilterProxyModel*,
		QLineEdit*);

    //--------------------------------------------------------------------------
	//! Returns a non-editable item with set properties as given.
	static QStandardItem* createItem(const QString&, const QVariant&, Qt::Alignment = Qt::AlignLeft, bool enabled = true);

	static QStandardItem* createItem(const QVariant&, Qt::Alignment = Qt::AlignLeft);
	
	static void setItem(QStandardItem*, const QString&, const QVariant&);

	static void setItemSize(QStandardItem*, unsigned long long);

	static void setItemCount(QStandardItem*, unsigned long long);

	/*! Returns icon if collection contains recognized string such as "scene" or "history",
     *	empty icon otherwise.
     */
	QIcon getIcon(const QString& collection) const;

    //! Returns a human readable string of kilobytes, megabytes etc.
    static QString toFileSize(unsigned long long int bytes);

    //! Returns the current locale string representation.
    template <class T>
    static QString toLocaleString(const T & value)
    {
        QLocale locale;
        return locale.toString(value);
    }

    //--------------------------------------------------------------------------
	//
	// Private variables
	//
    //--------------------------------------------------------------------------

private :

    Ui::RepoWidgetRepository *ui;

	//! Default model for the databases.
    QStandardItemModel *databasesModel;

	//! Sorting model proxy for the databases.
    QSortFilterProxyModel *databasesProxyModel;

	//! Default model for the collection.
    QStandardItemModel *collectionModel;

	//! Sorting model proxy for the collection.
    QSortFilterProxyModel *collectionProxyModel;

	//! Private thread pool local to this object only.
	QThreadPool threadPool;

    repo::RepoController *controller;
	repo::RepoToken      *token;

    int databaseRowCounter;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_WIDGET_REPOSITORY_H

