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

#include "repo_widgetrepository.h"

//------------------------------------------------------------------------------
repo::gui::RepoWidgetRepository::RepoWidgetRepository(QWidget* parent)
	: QWidget(parent)
{
	setupUi(this);
    //--------------------------------------------------------------------------
	// Set database model and create headers
	databasesModel = new QStandardItemModel(this); 
	databasesModel->setColumnCount(3);
	databasesModel->setHeaderData(RepoDatabasesColumns::NAME, Qt::Horizontal, QObject::tr("Name"));
	databasesModel->setHeaderData(RepoDatabasesColumns::COUNT, Qt::Horizontal, QObject::tr("Count"));
	databasesModel->setHeaderData(RepoDatabasesColumns::SIZE, Qt::Horizontal, QObject::tr("Size"));

	databasesProxyModel = new RepoSortFilterProxyModel(this, false);
	enableFiltering(
		databasesTreeView, 
		databasesModel, 
		databasesProxyModel, 
		databasesFilterLineEdit);
	databasesTreeView->sortByColumn(RepoDatabasesColumns::NAME, Qt::SortOrder::AscendingOrder);

    //--------------------------------------------------------------------------
	// Set collection model and create headers
	collectionModel = new QStandardItemModel(this); 
	collectionModel->setColumnCount(3);
	collectionModel->setHeaderData(RepoCollectionColumns::DOCUMENT, Qt::Horizontal, QObject::tr("Document"));
	collectionModel->setHeaderData(RepoCollectionColumns::VALUE, Qt::Horizontal, QObject::tr("Value"));
	collectionModel->setHeaderData(RepoCollectionColumns::TYPE, Qt::Horizontal, QObject::tr("Type"));

	collectionProxyModel = new RepoSortFilterProxyModel(this, true);
	enableFiltering(
		collectionTreeView, 
		collectionModel, 
		collectionProxyModel, 
		collectionFilterLineEdit);
	
	collectionTreeView->sortByColumn(RepoDatabasesColumns::NAME, Qt::SortOrder::AscendingOrder);
    //--------------------------------------------------------------------------
	QObject::connect(
		tabWidget, &QTabWidget::currentChanged,
		this, &RepoWidgetRepository::changeTab);

	databasesProgressBar->hide();
	collectionProgressBar->hide();
}

//------------------------------------------------------------------------------

repo::gui::RepoWidgetRepository::~RepoWidgetRepository()
{
	cancelAllThreads();
    //--------------------------------------------------------------------------
	delete databasesProxyModel;
	delete databasesModel;
	delete collectionProxyModel;
	delete collectionModel;
}

//------------------------------------------------------------------------------

bool repo::gui::RepoWidgetRepository::cancelAllThreads()
{
	emit cancel();
	return threadPool.waitForDone(); // msecs
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::fetchDatabases(const repo::core::MongoClientWrapper& mongo)
{
    //--------------------------------------------------------------------------
	// Cancel any previously running threads.
	if (cancelAllThreads())
	{
		this->mongo = mongo;

		RepoLogger::getInstance().log(repo::REPO_INFO, QObject::tr("Fetching databases...").toStdString());
				
        //----------------------------------------------------------------------
		RepoWorkerDatabases * worker = new RepoWorkerDatabases(mongo);	
		worker->setAutoDelete(true);

        //----------------------------------------------------------------------
		// Direct connection ensures cancel signal is processed ASAP
		QObject::connect(
			this, &RepoWidgetRepository::cancel,
			worker, &RepoWorkerDatabases::cancel, Qt::DirectConnection);

		QObject::connect(
			worker, &RepoWorkerDatabases::hostFetched,
			this, &RepoWidgetRepository::addHost);

		QObject::connect(
			worker, &RepoWorkerDatabases::databaseFetched,
			this, &RepoWidgetRepository::addDatabase);

		QObject::connect(
			worker, &RepoWorkerDatabases::collectionFetched,
			this, &RepoWidgetRepository::addCollection);

		QObject::connect(
			worker, &RepoWorkerDatabases::finished,
			databasesProgressBar, &QProgressBar::hide);	
		
		QObject::connect(
			worker, &RepoWorkerDatabases::progressRangeChanged,
			databasesProgressBar, &QProgressBar::setRange);	

		QObject::connect(
			worker, &RepoWorkerDatabases::progressValueChanged,
			databasesProgressBar, &QProgressBar::setValue);			

        //----------------------------------------------------------------------
		// Clear any previous entries in the databases and collection models
		clearDatabaseModel();

        //----------------------------------------------------------------------
		databasesProgressBar->show();
		threadPool.start(worker);	
	}
}

void repo::gui::RepoWidgetRepository::fetchCollection()
{	
	fetchCollection(getSelectedConnection(), getSelectedDatabase(), getSelectedCollection());
}
//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::fetchCollection(
	const repo::core::MongoClientWrapper& mongo, 
	const QString& database, 
	const QString& collection)
{
	if (!database.isEmpty() && !collection.isEmpty()) //&& cancelAllThreads())
	{
		RepoLogger::getInstance().log(repo::REPO_INFO, QObject::tr("Fetching collection...").toStdString());
		RepoWorkerCollection* worker = new RepoWorkerCollection(mongo, database, collection);	
		worker->setAutoDelete(true);

		// Direct connection ensures cancel signal is processed ASAP
		QObject::connect(
			this, &RepoWidgetRepository::cancel,
			worker, &RepoWorkerCollection::cancel, Qt::DirectConnection);

		QObject::connect(
			worker, &RepoWorkerCollection::keyValuePairAdded,
			this, &RepoWidgetRepository::addKeyValuePair);

		QObject::connect(
			worker, &RepoWorkerDatabases::finished,
			collectionProgressBar, &QProgressBar::hide);	
		
		QObject::connect(
			worker, &RepoWorkerDatabases::progressRangeChanged,
			collectionProgressBar, &QProgressBar::setRange);	

		QObject::connect(
			worker, &RepoWorkerDatabases::progressValueChanged,
			collectionProgressBar, &QProgressBar::setValue);		
		
        //----------------------------------------------------------------------
		// Clear any previous entries in the collection model 
		clearCollectionModel();

        //----------------------------------------------------------------------
		collectionProgressBar->show();
		threadPool.start(worker);
	}
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::addHost(QString host)
{	
	QList<QStandardItem *> row;	
	QStandardItem *hostItem = createItem(host, host, Qt::AlignLeft);
	row.append(hostItem);
	row.append(createItem(QString(), 0, Qt::AlignRight));
	row.append(createItem(QString(), 0, Qt::AlignRight));
    //--------------------------------------------------------------------------
	databasesModel->invisibleRootItem()->appendRow(row);
    //--------------------------------------------------------------------------
	// Expand top most host by default
	databasesTreeView->expand(
		databasesProxyModel->mapFromSource(databasesModel->indexFromItem(hostItem)));
	//hostItem->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_hdd_o));
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::addDatabase(QString database)
{
	QList<QStandardItem *> row;	
    //--------------------------------------------------------------------------
	row.append(createItem(database, database, Qt::AlignLeft));
	row.append(createItem(QString(), 0, Qt::AlignRight));
	row.append(createItem(QString(), 0, Qt::AlignRight));	

    //--------------------------------------------------------------------------
	// Append to the bottom most child (host)
	if (QStandardItem * host = databasesModel->invisibleRootItem()->child(
			databasesModel->invisibleRootItem()->rowCount()-1, RepoDatabasesColumns::NAME))
		host->appendRow(row);	
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::addCollection(
	QString collection, 
	unsigned long long count, 
	unsigned long long size)
{
	if (QStandardItem * host = databasesModel->invisibleRootItem()->child(
		databasesModel->invisibleRootItem()->rowCount()-1, RepoDatabasesColumns::NAME))
	{
        //----------------------------------------------------------------------
		// Append to the bottom most database
		QList<QStandardItem *> row;	
		row.append(createItem(collection));
		row.at(0)->setIcon(getIcon(collection));

		row.append(createItem(RepoLogger::toLocaleString(count), count, Qt::AlignRight));
		row.append(createItem(RepoLogger::toFileSize(size), size, Qt::AlignRight));		
		if (QStandardItem * database = host->child(host->rowCount()-1, RepoDatabasesColumns::NAME))
			database->appendRow(row);

        //----------------------------------------------------------------------
		// Increase count and size on the bottom most database
		if (QStandardItem * databaseCount = host->child(host->rowCount()-1, RepoDatabasesColumns::COUNT))
			setItemCount(databaseCount, databaseCount->data().toULongLong() + count);
		if (QStandardItem * databaseSize = host->child(host->rowCount()-1, RepoDatabasesColumns::SIZE))
			setItemSize(databaseSize, databaseSize->data().toULongLong() + size);
		
        //----------------------------------------------------------------------
		// Increase count and size on the bottom most host
		if (QStandardItem * hostCount = databasesModel->invisibleRootItem()->child(
			databasesModel->invisibleRootItem()->rowCount()-1, RepoDatabasesColumns::COUNT))
			setItemCount(hostCount, hostCount->data().toULongLong() + count);	
		if (QStandardItem * hostSize = databasesModel->invisibleRootItem()->child(
			databasesModel->invisibleRootItem()->rowCount()-1, RepoDatabasesColumns::SIZE))
			setItemSize(hostSize, hostSize->data().toULongLong() + size);		
	}
}

void repo::gui::RepoWidgetRepository::addKeyValuePair(
	QVariant document, 
	QVariant value, 
	QVariant type,
	unsigned int depth)
{
	QList<QStandardItem *> row;	
    //--------------------------------------------------------------------------
	if (QVariant::Type::ULongLong == document.type())
		row.append(createItem(RepoLogger::toLocaleString(document.toULongLong()), document, Qt::AlignLeft));
	else
		row.append(createItem(document.toString(), document, Qt::AlignLeft));
    //--------------------------------------------------------------------------
	if (QVariant::Type::ULongLong == value.type())
		row.append(createItem(RepoLogger::toFileSize(value.toULongLong()), value, Qt::AlignRight));	
	else if (QVariant::Type::Int == value.type() || QVariant::Type::LongLong == value.type())
		row.append(createItem(RepoLogger::toLocaleString(value.toLongLong()), value, Qt::AlignLeft));
	else	
		row.append(createItem(value.toString(), value, Qt::AlignLeft));
    //--------------------------------------------------------------------------
	row.append(createItem(type.toString(), type, Qt::AlignLeft));	
    //--------------------------------------------------------------------------
	getHierarchyDepth(collectionModel, depth)->appendRow(row);
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::clearDatabaseModel()
{	
	databasesModel->removeRows(0, databasesModel->rowCount());	
    //--------------------------------------------------------------------------
	databasesTreeView->resizeColumnToContents(RepoDatabasesColumns::COUNT);	
	databasesTreeView->resizeColumnToContents(RepoDatabasesColumns::SIZE);
	databasesFilterLineEdit->clear();
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::clearCollectionModel()
{
	collectionModel->removeRows(0, collectionModel->rowCount());	
    //--------------------------------------------------------------------------
	collectionTreeView->resizeColumnToContents(RepoCollectionColumns::VALUE);	
	collectionTreeView->resizeColumnToContents(RepoCollectionColumns::TYPE);	
	collectionFilterLineEdit->clear();
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::changeTab(int index)
{
	if (0 == index)
	{
		cancelAllThreads();
	}
	else if (1 == index)
	{
		fetchCollection();
	}

}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::copySelectedCollectionCellToClipboard()
{
	const QModelIndex selectedIndex = collectionTreeView->selectionModel()->currentIndex();	
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(selectedIndex.data().toString());
}

//------------------------------------------------------------------------------

QString repo::gui::RepoWidgetRepository::getSelectedHost() const
{
	QVariant host;
	const QModelIndex index = getSelectedDatabasesTreeViewIndex();	
    //--------------------------------------------------------------------------
	switch (getHierarchyDepth(index))
	{
		case 3 : // collection depth
			host = databasesTreeView->model()->data(index.parent().parent());
			break;
		case 2 : // database depth
			host = databasesTreeView->model()->data(index.parent());
			break;
		case 1 : // host depth
			host = databasesTreeView->model()->data(index);
			break;
	}
	return host.toString();
}

//------------------------------------------------------------------------------

QString repo::gui::RepoWidgetRepository::getSelectedDatabase() const
{
	QVariant database;
	const QModelIndex index = getSelectedDatabasesTreeViewIndex();	
    //--------------------------------------------------------------------------
	unsigned int hierarchyDepth = getHierarchyDepth(index);
	if (3 == hierarchyDepth) // collection depth
		database = databasesTreeView->model()->data(index.parent());
	else if (2 == hierarchyDepth) // database depth
		database = databasesTreeView->model()->data(index);
	return database.toString();
}

//------------------------------------------------------------------------------

QString repo::gui::RepoWidgetRepository::getSelectedCollection() const
{
	QVariant collection;
	const QModelIndex index = getSelectedDatabasesTreeViewIndex();	
    //--------------------------------------------------------------------------
	if (3 == getHierarchyDepth(index)) // collection depth
		collection = databasesTreeView->model()->data(index);
	return collection.toString();
}

//------------------------------------------------------------------------------

QModelIndex repo::gui::RepoWidgetRepository::getSelectedDatabasesTreeViewIndex() const
{
    //--------------------------------------------------------------------------
	// Selected index might be on different column, hence create a new index
	// which has the selected row but the desired NAME column.
	const QModelIndex selectedIndex = databasesTreeView->selectionModel()->currentIndex();	
	return databasesTreeView->model()->index(selectedIndex.row(), RepoDatabasesColumns::NAME, selectedIndex.parent());
}

//------------------------------------------------------------------------------

unsigned int repo::gui::RepoWidgetRepository::getHierarchyDepth(const QModelIndex &index)
{
	// TODO: use itemFromIndex 
	// This is inefficient to loop through.
	unsigned int hierarchyDepth = 0; // invisible root item
	if (index.isValid())
	{
		++hierarchyDepth;
		QModelIndex seekRoot = index;	
		while(seekRoot.parent() != QModelIndex())
		{		
			seekRoot = seekRoot.parent();
			++hierarchyDepth;
		}
	}
	return hierarchyDepth;
}

QStandardItem* repo::gui::RepoWidgetRepository::getHierarchyDepth(
	const QStandardItemModel* model, 
	unsigned int depth)
{
	QStandardItem* item = model->invisibleRootItem();
	for (unsigned int i = 0; item && item->hasChildren() && i < depth; ++i)
		item = item->child(item->rowCount() - 1);
	return item;
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::enableFiltering(
	QAbstractItemView* view, 
	QStandardItemModel* model, 
	QSortFilterProxyModel* proxy,
	QLineEdit* lineEdit)
{
	// Sorting is achieved based on values stored in the model via setData(QVariant, Qt::UserRole+1)
	// and proxy model is set to sort as setSortRole(Qt::UserRole+1)
	//model->setSortRole(Qt::UserRole+1);
    //--------------------------------------------------------------------------
	proxy->setSortRole(Qt::UserRole+1);
	proxy->setDynamicSortFilter(true);
	proxy->setFilterKeyColumn(-1); // filter all columns
	proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    //--------------------------------------------------------------------------
	proxy->setSourceModel(model);
	view->setModel(proxy);	
    //--------------------------------------------------------------------------
	QObject::connect(
		lineEdit, &QLineEdit::textChanged, 
		proxy, &QSortFilterProxyModel::setFilterFixedString);	
}

//------------------------------------------------------------------------------

QStandardItem * repo::gui::RepoWidgetRepository::createItem(
	const QString& text, 
	const QVariant& data,
	Qt::Alignment alignment,
	bool enabled)
{
	QStandardItem* item = new QStandardItem();
	item->setEditable(false);
	item->setTextAlignment(alignment);
	item->setEnabled(enabled);
	setItem(item, text, data);
	return item;
}

//------------------------------------------------------------------------------

QStandardItem * repo::gui::RepoWidgetRepository::createItem(
	const QVariant& data, 
	Qt::Alignment alignment)
{
	return createItem(data.toString(), data, alignment);
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::setItem(
	QStandardItem * item,	
	const QString& text, 
	const QVariant& data)
{
	item->setText(text);
	item->setToolTip(text);
	item->setData(data);
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::setItemCount(QStandardItem * item, unsigned long long value)
{
	setItem(item, RepoLogger::toLocaleString(value), value);
}

//------------------------------------------------------------------------------

void repo::gui::RepoWidgetRepository::setItemSize(QStandardItem * item, unsigned long long value)
{
	setItem(item, RepoLogger::toFileSize(value), value);
}

QIcon repo::gui::RepoWidgetRepository::getIcon(const QString &collection) const
{
	QIcon icon;		
	QColor color = QWidget::palette().mid().color();
		
	// see http://docs.mongodb.org/manual/reference/system-collections/
	if (collection.contains("scene") || collection.contains("nodes") || collection.contains("repo.scene"))
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_sitemap, color);
	else if (collection.contains("history") || collection.contains("revisions") || collection.contains("repo.history"))
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_code_fork, color);
	else if (collection.contains("system.indexes"))
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_bolt, color);
	else if (collection.contains("system.users"))
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_user, color);
	else if (collection.contains("system.profile"))
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_tachometer, color);
	else if (collection.contains("system.namespaces"))
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_bookmark_o, color);
	else if (collection.contains("system.js"))
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_wrench, color);
	else if (collection.contains("comments"))
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_comment_o, color);
	else
		icon = RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_bars, color);

	return icon;
}

