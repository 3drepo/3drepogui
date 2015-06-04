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

#include "repo_dialoghistory.h"
//------------------------------------------------------------------------------
#include "../primitives/repo_fontawesome.h"
#include "../workers/repo_workerhistory.h"

//------------------------------------------------------------------------------

repo::gui::RepoDialogHistory::RepoDialogHistory(const repo::core::MongoClientWrapper& mongo,
    const QString& database,
    const QString &project,
    QWidget *parent,
    Qt::WindowFlags flags)
	: QDialog(parent, flags)
	, mongo(mongo)
	, database(database)
    , project(project)
    , ui(new Ui::RepoDialogHistory)
{
    ui->setupUi(this);
    setWindowIcon(RepoFontAwesome::getHistoryIcon());
	
    //--------------------------------------------------------------------------
	historyModel = new QStandardItemModel(this); 
	historyModel->setColumnCount(5);
    historyModel->setHeaderData(
                RepoHistoryColumns::TIMESTAMP,
                Qt::Horizontal,
                tr("Timestamp"));
    historyModel->setHeaderData(
                RepoHistoryColumns::MESSAGE,
                Qt::Horizontal,
                tr("Message"));
    historyModel->setHeaderData(
                RepoHistoryColumns::AUTHOR,
                Qt::Horizontal,
                tr("Author"));
    historyModel->setHeaderData(
                RepoHistoryColumns::REVISION,
                Qt::Horizontal,
                tr("Revision"));
    historyModel->setHeaderData(
                RepoHistoryColumns::BRANCH,
                Qt::Horizontal,
                tr("Branch"));
	
    //--------------------------------------------------------------------------
	historyProxy = new QSortFilterProxyModel(this);
	historyProxy->setFilterKeyColumn(-1); // filter all columns
	historyProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	historyProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	historyProxy->setSourceModel(historyModel);
    //--------------------------------------------------------------------------
    ui->historyTreeView->setModel(historyProxy);
    ui->historyTreeView->sortByColumn(RepoHistoryColumns::TIMESTAMP);
    //--------------------------------------------------------------------------
	clearHistoryModel();

    //--------------------------------------------------------------------------
    revisionModel = new QStandardItemModel(ui->revisionTreeView);
	revisionModel->setColumnCount(2);
    revisionModel->setHeaderData(
                RepoRevisionColumns::SID,
                Qt::Horizontal,
                tr("Shared ID"));
    revisionModel->setHeaderData(
                RepoRevisionColumns::ACTION,
                Qt::Horizontal,
                tr("Action"));
    //--------------------------------------------------------------------------
    ui->revisionTreeView->setModel(revisionModel);

    //--------------------------------------------------------------------------
	// Connect filtering text input to the filtering proxy model
	QObject::connect(
        ui->filterLineEdit, &QLineEdit::textChanged,
		historyProxy, &QSortFilterProxyModel::setFilterFixedString);    
    QObject::connect(
        historyProxy, &QSortFilterProxyModel::rowsInserted,
        this, &RepoDialogHistory::updateCountLabel);
    QObject::connect(
        historyProxy, &QSortFilterProxyModel::rowsRemoved,
        this, &RepoDialogHistory::updateCountLabel);


    //--------------------------------------------------------------------------
	QObject::connect(
        ui->refreshPushButton, &QPushButton::pressed,
		this, &RepoDialogHistory::refresh);

    QObject::connect(
                ui->historyTreeView->selectionModel(),
                SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(changeRevision(QModelIndex,QModelIndex)));
	
}

//------------------------------------------------------------------------------

repo::gui::RepoDialogHistory::~RepoDialogHistory() 
{
	cancelAllThreads();

	delete historyProxy;
	delete historyModel;
	delete revisionModel;

    delete ui;
}

bool repo::gui::RepoDialogHistory::cancelAllThreads()
{
	emit cancel();
	return threadPool.waitForDone(); // msecs
}

int repo::gui::RepoDialogHistory::exec()
{
	refresh();
	return QDialog::exec();
}
//------------------------------------------------------------------------------

void repo::gui::RepoDialogHistory::refresh()
{
	if (!database.isEmpty() && cancelAllThreads())
	{
        RepoWorkerHistory* worker = new RepoWorkerHistory(mongo, database, project);
		worker->setAutoDelete(true);

		// Direct connection ensures cancel signal is processed ASAP
		QObject::connect(
			this, &RepoDialogHistory::cancel,
			worker, &RepoWorkerHistory::cancel, Qt::DirectConnection);

		QObject::connect(
			worker, &RepoWorkerHistory::revisionFetched,
			this, &RepoDialogHistory::addRevision);//, Qt::BlockingQueuedConnection);
		
        //----------------------------------------------------------------------
		// Clear any previous entries : the collection model 
		clearHistoryModel();

        //----------------------------------------------------------------------
		threadPool.start(worker);
	}
}

void repo::gui::RepoDialogHistory::addRevision(core::RepoNodeRevision *revision)
{
	QList<QStandardItem *> row;	
    QDateTime datetime;
    datetime.setMSecsSinceEpoch(revision->getTimestamp());

    //--------------------------------------------------------------------------
    // Datetime
    QStandardItem *item = createItem(QVariant(datetime));
    item->setData(qVariantFromValue((void *) revision));
    row.append(item);

    // Message
    row.append(createItem(QVariant(QString::fromStdString(revision->getMessage()))));

    // Author
    row.append(createItem(QVariant(QString::fromStdString(revision->getAuthor()))));

    // UID
    row.append(createItem(QVariant(QUuid(core::MongoClientWrapper::uuidToString(revision->getUniqueID()).c_str()))));

    // SID
    row.append(createItem(QVariant(QUuid(core::MongoClientWrapper::uuidToString(revision->getSharedID()).c_str()))));

    //--------------------------------------------------------------------------
	historyModel->invisibleRootItem()->appendRow(row);
    //--------------------------------------------------------------------------
    updateCountLabel();
}

//------------------------------------------------------------------------------

void repo::gui::RepoDialogHistory::clearHistoryModel()
{
	historyModel->removeRows(0, historyModel->rowCount());	
    //--------------------------------------------------------------------------
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::TIMESTAMP);
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::MESSAGE);
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::AUTHOR);
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::REVISION);
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::BRANCH);
    //--------------------------------------------------------------------------
    ui->filterLineEdit->clear();
    updateCountLabel();
}

void repo::gui::RepoDialogHistory::updateCountLabel()
{
    ui->revisionsCountLabel->setText(
                tr("Showing %1 of %2").arg(historyProxy->rowCount()).arg(
        historyModel->rowCount()));
}

void repo::gui::RepoDialogHistory::changeRevision(const QModelIndex &current, const QModelIndex &)
{    
    // Clear any previous entries
    revisionModel->removeRows(0, revisionModel->rowCount());


    QModelIndex sourceIndex = historyProxy->mapToSource(current);
    QModelIndex timestampIndex = historyModel->index(sourceIndex.row(), RepoHistoryColumns::TIMESTAMP, sourceIndex.parent());
    QStandardItem *item = historyModel->itemFromIndex(timestampIndex);

    if (item)
    {
        core::RepoNodeRevision *revision = 0;
        revision = (core::RepoNodeRevision*)item->data(Qt::UserRole+1).value<void*>();
        if (revision)
        {

            ui->messageTextEdit->appendPlainText(QString::fromStdString(revision->getMessage()));

            // TODO: display lists of added, deleted, modified (rather than current)
            for (boost::uuids::uuid uuid : revision->getCurrentUniqueIDs())
            {
                QList<QStandardItem *> row;

                // UID // TODO: make SID
                row.append(createItem(QVariant(QUuid(core::MongoClientWrapper::uuidToString(uuid).c_str()))));

                // Action
                row.append(createItem(QVariant(tr("current"))));

                //--------------------------------------------------------------------------
                revisionModel->invisibleRootItem()->appendRow(row);
            }
        }
    }
}

//------------------------------------------------------------------------------

QList<QUuid> repo::gui::RepoDialogHistory::getSelectedRevisions()
{
	QList<QUuid> list;
    QModelIndexList selected = ui->historyTreeView->selectionModel()->selectedRows(
                RepoHistoryColumns::REVISION);
    for (QModelIndexList::iterator i = selected.begin();
         i != selected.end();
         ++i)
        list << ui->historyTreeView->model()->data(*i).toUuid();
	return list;
}

//------------------------------------------------------------------------------

QStandardItem *repo::gui::RepoDialogHistory::createItem(QVariant& data)
{
	QStandardItem* item = new QStandardItem(data.toString());
	item->setEditable(false);
	item->setToolTip(data.toString());
	item->setData(data);
	return item;
}
