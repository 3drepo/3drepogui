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

repo::gui::RepoDialogHistory::RepoDialogHistory(
	const repo::core::MongoClientWrapper& mongo, 
	const QString& database,
	QWidget *parent, 
	Qt::WindowFlags flags)
	: QDialog(parent, flags)
	, mongo(mongo)
	, database(database)
    , ui(new Ui::RepoDialogHistory)
{
    ui->setupUi(this);
	setWindowIcon(getIcon());
	
    //--------------------------------------------------------------------------
	historyModel = new QStandardItemModel(this); 
	historyModel->setColumnCount(5);
    historyModel->setHeaderData(
                RepoHistoryColumns::REVISION,
                Qt::Horizontal,
                tr("Revision"));
    historyModel->setHeaderData(
                RepoHistoryColumns::BRANCH,
                Qt::Horizontal,
                tr("Branch"));
    historyModel->setHeaderData(
                RepoHistoryColumns::MESSAGE,
                Qt::Horizontal,
                tr("Message"));
    historyModel->setHeaderData(
                RepoHistoryColumns::AUTHOR,
                Qt::Horizontal,
                tr("Author"));
    historyModel->setHeaderData(
                RepoHistoryColumns::TIMESTAMP,
                Qt::Horizontal,
                tr("Timestamp"));
	
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
	revisionModel = new QStandardItemModel(this); 
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
    //--------------------------------------------------------------------------
	QObject::connect(
        ui->refreshPushButton, &QPushButton::pressed,
		this, &RepoDialogHistory::refresh);
	
}

//------------------------------------------------------------------------------

repo::gui::RepoDialogHistory::~RepoDialogHistory() 
{
	cancelAllThreads();

	delete historyProxy;
	delete historyModel;
	delete revisionModel;
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
		RepoWorkerHistory* worker = new RepoWorkerHistory(mongo, database);	
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

void repo::gui::RepoDialogHistory::addRevision(
	QVariant uid, 
	QVariant sid, 
	QVariant message, 
	QVariant author, 
	QVariant timestamp)
{
	QList<QStandardItem *> row;	
    //--------------------------------------------------------------------------
	row.append(createItem(uid));
	row.append(createItem(sid));
	row.append(createItem(message));
	row.append(createItem(author));
	row.append(createItem(timestamp));	
    //--------------------------------------------------------------------------
	historyModel->invisibleRootItem()->appendRow(row);
    //--------------------------------------------------------------------------
    ui->revisionsCountLabel->setText(tr("Showing %1 revision(s)").arg(
		historyModel->invisibleRootItem()->rowCount()));
}

//------------------------------------------------------------------------------

void repo::gui::RepoDialogHistory::clearHistoryModel()
{
	historyModel->removeRows(0, historyModel->rowCount());	
    //--------------------------------------------------------------------------
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::REVISION);
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::BRANCH);
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::MESSAGE);
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::AUTHOR);
    ui->historyTreeView->resizeColumnToContents(RepoHistoryColumns::TIMESTAMP);
    //--------------------------------------------------------------------------
    ui->filterLineEdit->clear();
    ui->revisionsCountLabel->setText(tr("Showing %1 revision(s)").arg(0));
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

QIcon repo::gui::RepoDialogHistory::getIcon()
{
    return RepoFontAwesome::getInstance().getIcon(
                repo::gui::RepoFontAwesome::fa_clock_o);
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
