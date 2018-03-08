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

#include "repo_standard_item_factory.h"

using namespace repo::gui::primitive;

RepoStandardItemRow RepoStandardItemFactory::makeHostRow(
        repo::RepoController *controller,
        repo::RepoController::RepoToken* token)
{
    std::string alias;
    std::string host;
    uint32_t port;
    controller->getInfoFromToken(token, alias, host, port, std::string(), std::string());

    RepoStandardItemRow row;
    RepoStandardItem* item = new RepoStandardItem(
                QString::fromStdString(alias),
                qVariantFromValue((void *) token),
                Qt::UserRole + 2); // UR+2 ensures tokens are not used for filtering

    item->setToolTip(QString::fromStdString(host) + ":" + QString::number(port));
    item->setIcon(RepoFontAwesome::getHostIcon());
    item->setType(RepoDatabasesTypes::HOST_DIRTY);
    row.append(item);

    row.append(new RepoStandardItem(0, true)); // count
    row.append(new RepoStandardItem(0, true)); // allocated
    row.append(new RepoStandardItem(0, true)); // storage size
    return row;
}

RepoStandardItemRow RepoStandardItemFactory::makeDatabaseRow(const std::string &database)
{
    QString db = QString::fromStdString(database);

    RepoStandardItemRow row;
    RepoStandardItem *item = new RepoStandardItem(db, db);
    item->setData(db);
    item->setIcon(RepoFontAwesome::getDatabaseIcon());
    item->setType(RepoDatabasesTypes::DATABASE_DIRTY);
    row.append(item);
    row.append(new RepoStandardItem(0, true)); // count
    row.append(new RepoStandardItem(0, true)); // allocated
    row.append(new RepoStandardItem(0, true)); // storage size

    // Empty sub-item with ".." as text
    item->appendRow({new RepoStandardItem("..", QVariant()),
                     new RepoStandardItem(0, true),
                     new RepoStandardItem(0, true),
                     new RepoStandardItem(0, true)});
    return row;
}

RepoStandardItemRow RepoStandardItemFactory::makeProjectRow(const std::string &project)
{
    QString pr = QString::fromStdString(project);
    RepoStandardItemRow row;
    RepoStandardItem *item = new RepoStandardItem(pr, pr);
    item->setData(pr);
    item->setIcon(RepoFontAwesome::getProjectIcon());
    item->setType(RepoDatabasesTypes::PROJECT_DIRTY);
    row.append(item);
    row.append(new RepoStandardItem(0, true)); // count
    row.append(new RepoStandardItem(0, true)); // allocated
    row.append(new RepoStandardItem(0, true)); // storage size
    return row;
}
