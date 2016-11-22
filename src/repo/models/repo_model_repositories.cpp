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

const QString RepositoriesModel::REPO_SETTINGS_REPOSITORIES_MODEL_HEADERS = "RepositoriesModel/headers";


RepositoriesModel::RepositoriesModel(
        RepoController *controller,
        FilterableTreeWidget *widget)
    : controller(controller)
    , widget(widget)
{

    widget->restoreHeaders({"Name", "Count", "Allocated", "Size"},
                           REPO_SETTINGS_REPOSITORIES_MODEL_HEADERS);

    proxy = new repo::gui::primitive::RepoSortFilterProxyModel((QWidget*) widget, false);
    widget->setProxyModel(proxy);
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
    //--------------------------------------------------------------------------
    // Cancel any previously running threads.
    if (cancelAllThreads())
    {
        repoLog("Fetching databases...");

        //----------------------------------------------------------------------
        repo::worker::DatabasesWorker * worker = new repo::worker::DatabasesWorker(controller, token);
        worker->setAutoDelete(true);

        //----------------------------------------------------------------------
        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
            this, &RepositoriesModel::cancel,
            worker, &repo::worker::DatabasesWorker::cancel, Qt::DirectConnection);

                QObject::connect(
                    worker, &repo::worker::DatabasesWorker::hostFetched,
                    this, &RepositoriesModel::addHost);

        QObject::connect(
            worker, &repo::worker::DatabasesWorker::databaseFetched,
            this, &RepositoriesModel::addDatabase);

//		QObject::connect(
//			worker, &repo::worker::DatabasesWorker::finished,
//            ui->databasesProgressBar, &QProgressBar::hide);

//		QObject::connect(
//			worker, &repo::worker::DatabasesWorker::progressRangeChanged,
//            ui->databasesProgressBar, &QProgressBar::setRange);

//		QObject::connect(
//			worker, &repo::wgorker::DatabasesWorker::progressValueChanged,
//            ui->databasesProgressBar, &QProgressBar::setValue);

        //----------------------------------------------------------------------
        // Clear any previous entries in the databases and collection models
//		clearDatabaseModel();

        //----------------------------------------------------------------------
//        ui->databasesProgressBar->show();
        threadPool.start(worker);
    }
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

void RepositoriesModel::addHost(RepoStandardItemRow hostRow)
{
    widget->addTopLevelRow(hostRow);
    widget->expandItem(hostRow[0]);

//    for (std::string db : controller->getDatabases(token))
//    {
//        addDatabase(host, QString::fromStdString(db));
//    }
}

void RepositoriesModel::addDatabase(RepoStandardItemRow hostRow, RepoStandardItemRow databaseRow)
{
//    RepoController::RepoToken* token = (RepoController::RepoToken*) host->data(Qt::UserRole + 2).value<void *>();
//    repo::core::model::DatabaseStats dbStats = controller->getDatabaseStats(token, database.toStdString());
//    QList<QStandardItem *> row;


    hostRow[0]->appendRow(databaseRow);
}
