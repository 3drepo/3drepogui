/**
 *  Copyright (C) 2015 3D Repo Ltd
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

#include "repo_idbcache.h"

using namespace repo::gui::primitive;

void RepoIDBCache::setHostsComboBox(QComboBox *comboBox) const
{
    setComboBox(comboBox,
                RepoFontAwesome::getHostIcon(),
                getHosts(),
                getSelectedHost());
}

void RepoIDBCache::setDatabasesComboBox(QComboBox *comboBox) const
{
    setComboBox(comboBox,
                RepoFontAwesome::getDatabaseIcon(),
                getDatabases(getSelectedHost()),
                getSelectedDatabase());
}

void RepoIDBCache::setProjectsComboBox(QComboBox *comboBox) const
{
    setComboBox(comboBox,
                RepoFontAwesome::getProjectIcon(),
                getProjects(getSelectedHost(), getSelectedDatabase()),
                getSelectedProject());
}

void RepoIDBCache::setComboBox(
        QComboBox *comboBox,
        const QIcon &icon,
        const QList<QString> &list,
        const QString &selected) const
{
    for (int i = 0; i < list.size(); ++i)
    {
        comboBox->addItem(icon, list[i]);
        if (selected == list[i])
            comboBox->setCurrentIndex(i);
    }
}
