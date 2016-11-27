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
#include "repo_model_repositories.h"

using namespace repo;
using namespace repo::gui::widget;
using namespace repo::models;
using namespace repo::gui::primitive;
using namespace repo::worker;

const QString RepositoriesModel::REPO_SETTINGS_REPOSITORIES_MODEL_HEADERS = "RepositoriesModel/headers";


RepositoriesModel::RepositoriesModel(
        RepoController *controller,
        FilterableTreeWidget *widget)
    : controller(controller)
    , widget(widget)
{

    widget->restoreHeaders({"Name", "Count", "Allocated", "Actual"},
                           REPO_SETTINGS_REPOSITORIES_MODEL_HEADERS);

    proxy = new repo::gui::primitive::RepoSortFilterProxyModel((QWidget*) widget, false);
    widget->setProxyModel(proxy);

    QObject::connect(
                widget, &FilterableTreeWidget::expanded,
                this, &RepositoriesModel::expand);

    QObject::connect(
                widget, &FilterableTreeWidget::collapsed,
                this, &RepositoriesModel::collapse);
}

RepositoriesModel::~RepositoriesModel()
{
    cancelAllThreads();
    widget->storeHeaders(REPO_SETTINGS_REPOSITORIES_MODEL_HEADERS);
    delete proxy;
}

bool RepositoriesModel::cancelAllThreads()
{
    emit cancel();
    return threadPool.waitForDone(); // msecs
}

void RepositoriesModel::connect(RepoController::RepoToken* token)
{    
    RepoStandardItemRow hostRow = addHost(token);
}

void RepositoriesModel::disconnect()
{
//    controller->disconnectFromDatabase(token);
//    controller->destroyToken(token);
//    token = nullptr;
//    clearDatabaseModel();
//    clearCollectionModel();

}


void RepositoriesModel::refresh()
{
    //    RepoController::RepoToken* token = (RepoController::RepoToken*) host->data().value<void *>();

}

void RepositoriesModel::refreshHost(RepoStandardItem *host)
{
    //--------------------------------------------------------------------------
    // Cancel any previously running threads.
    if (cancelAllThreads())
    {
        repoLog("Fetching databases...");
        RepoController::RepoToken* token = (RepoController::RepoToken*)
                host->data(Qt::UserRole + 2).value<void *>();

        //----------------------------------------------------------------------
        // Clear any previous entries in the databases and collection models
        host->removeRows(0, host->rowCount());

        RepoStandardItemRow hostRow = getRow(host);

        // Rest counts
        hostRow[COUNT]->setDataNumber(0);
        hostRow[ALLOCATED]->setDataNumber(0, true);
        hostRow[RepoDatabasesColumns::SIZE]->setDataNumber(0, true);

        //----------------------------------------------------------------------
        DatabasesWorker * worker = new DatabasesWorker(controller, token, hostRow);
        worker->setAutoDelete(true);

        //----------------------------------------------------------------------
        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
                    this, &RepositoriesModel::cancel,
                    worker, &DatabasesWorker::cancel, Qt::DirectConnection);

        QObject::connect(
                    worker, &repo::worker::DatabasesWorker::databaseFetched,
                    this, &RepositoriesModel::addDatabase);

        QObject::connect(
                    worker, &repo::worker::DatabasesWorker::databaseStatsFetched,
                    this, &RepositoriesModel::setDatabaseStats);

        QObject::connect(
                    worker, &repo::worker::DatabasesWorker::finished,
                    widget->getProgressBar(), &QProgressBar::hide);

        QObject::connect(
                    worker, &repo::worker::DatabasesWorker::progressRangeChanged,
                    widget->getProgressBar(), &QProgressBar::setRange);

        QObject::connect(
                    worker, &repo::worker::DatabasesWorker::progressValueChanged,
                    widget->getProgressBar(), &QProgressBar::setValue);

        //----------------------------------------------------------------------
        widget->getProgressBar()->show();
        threadPool.start(worker);
    }
}

RepoStandardItemRow RepositoriesModel::addHost(RepoController::RepoToken* token)
{
    RepoStandardItemRow hostRow = RepoStandardItemFactory::makeHostRow(controller, token);
    addHost(hostRow);
    return hostRow;
}

void RepositoriesModel::addHost(const RepoStandardItemRow &hostRow)
{    
    widget->addTopLevelRow(hostRow.toQList());
    widget->expandItem(hostRow[NAME]);
}

void RepositoriesModel::addDatabase(const RepoStandardItemRow &hostRow,
                                    const RepoStandardItemRow &databaseRow)
{
    hostRow[NAME]->appendRow(databaseRow.toQList());
}

void RepositoriesModel::setDatabaseStats(const RepoStandardItemRow &hostRow,
                                         const RepoStandardItemRow &databaseRow,
                                         const core::model::DatabaseStats &databaseStats)
{  
    uint64_t count = databaseStats.getCollectionsCount();
    uint64_t allocated = databaseStats.getStorageSize();
    uint64_t size = databaseStats.getDataSize();

    databaseRow[COUNT]->setDataNumber(count);
    databaseRow[ALLOCATED]->setDataNumber(allocated, true);
    databaseRow[repo::gui::primitive::RepoDatabasesColumns::SIZE]->setDataNumber(
                size, true);

    hostRow[COUNT]->setDataNumber(hostRow[COUNT]->data().toLongLong() + count);
    hostRow[ALLOCATED]->setDataNumber(hostRow[ALLOCATED]->data().toLongLong() + allocated, true);
    hostRow[RepoDatabasesColumns::SIZE]->setDataNumber(
                hostRow[RepoDatabasesColumns::SIZE]->data().toLongLong() + size, true);
}

void RepositoriesModel::expand(QStandardItem *item)
{
//    RepoStandardItem *repoItem = (RepoStandardItem *)(item);
    std::cout << "Expand triggered " << item->type() << std::endl;

    switch (item->type())
    {
    case RepoDatabasesTypes::HOST :
        refreshHost((RepoStandardItem *)item);
        break;
    case RepoDatabasesTypes::DATABASE :
        break;
    case RepoDatabasesTypes::PROJECT :
        break;
    }
}

void RepositoriesModel::collapse(QStandardItem *item)
{
    std::cout << "Collapse triggered " << item->type() << std::endl;

    switch (item->type())
    {
    case RepoDatabasesTypes::HOST :
        // cancel loading
        cancelAllThreads();
        break;
    case RepoDatabasesTypes::DATABASE :
        // cancel loading
        break;
    case RepoDatabasesTypes::PROJECT :
        // cancel ??
        break;
    }
}

RepoStandardItemRow RepositoriesModel::getRow(const QStandardItem *item) const
{
    RepoStandardItemRow row;
    row.append((RepoStandardItem *)widget->getSiblingFromItem(item, NAME));
    row.append((RepoStandardItem *)widget->getSiblingFromItem(item, COUNT));
    row.append((RepoStandardItem *)widget->getSiblingFromItem(item, ALLOCATED));
    row.append((RepoStandardItem *)widget->getSiblingFromItem(item, RepoDatabasesColumns::SIZE));
    return row;
}
