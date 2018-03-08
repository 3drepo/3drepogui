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

// Qt
#include <QStandardItem>
#include <QThreadPool>

// CORE
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson_database_stats.h>

// GUI
#include "../gui/widgets/repo_widget_tree_filterable.h"
#include "../gui/primitives/repo_fontawesome.h"
#include "../gui/primitives/repo_sort_filter_proxy_model.h"
#include "../workers/repo_worker_databases.h"
#include "../workers/repo_worker_repositories.h"

using namespace repo::gui::primitive;
using namespace repo::core::model;

namespace repo {
namespace models {

/**
 * @brief The RepositoriesModel class
 *
 * Main internal model managing all logic related to management and visualization
 * of repositories within a filterable tree widget as part of the main GUI.
 */
class RepositoriesModel : public QObject
{
    Q_OBJECT    

public :

    /**
     * Constructor which takes in a FilterableTreeWidget for visualization
     * of connected databases/repositories within the main GUI.
     *
     * @brief RepositoriesModel
     * @param widget FilterableTreeWidget to display the repositories in.
     */
    RepositoriesModel(RepoController *controller, gui::widget::FilterableTreeWidget *widget);

    //! Destructor which stores user preferences.
    ~RepositoriesModel();

    //! Returns true once all threads have been cancelled.
    bool cancelAllThreads();

    /**
     * Fetched repositories from a remote database via an async worker.
     *
     * @brief connect
     * @param controller
     * @param token
     */
    void connect(RepoController::RepoToken* token);

    void disconnect();

    void refresh();

signals :

    void cancel();


public slots :

    void refreshHost(RepoStandardItem *host);

    void refreshDatabase(RepoStandardItem *database);

    void addHost(RepoController::RepoToken* token);

    void addHost(const RepoStandardItemRow &hostRow);

    void addDatabase(const RepoStandardItemRow &hostRow,
                     const RepoStandardItemRow &databaseRow);

    void addProject(const RepoStandardItemRow &databaseRow,
                    const RepoStandardItemRow &projectRow);

    void setDatabaseStats(const RepoStandardItemRow &hostRow,
                          const RepoStandardItemRow &databaseRow,
                          const DatabaseStats &databaseStats);

    void expand(QStandardItem *item);

    void collapse(QStandardItem *item);

protected :

    RepoStandardItemRow getRow(const QStandardItem *) const;

private:

    RepoController *controller;

    //! Repositories widget to display the contents of the model in.
    gui::widget::FilterableTreeWidget *widget;

    gui::primitive::RepoSortFilterProxyModel *proxy;

    //! Private thread pool local to this object only.
    QThreadPool threadPool;

    static const QString REPO_SETTINGS_REPOSITORIES_MODEL_HEADERS;
};

} // models
} // repo
