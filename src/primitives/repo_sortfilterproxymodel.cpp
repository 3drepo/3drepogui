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

#include "repo_sortfilterproxymodel.h"

repo::gui::RepoSortFilterProxyModel::RepoSortFilterProxyModel(
	QObject *parent,
	bool filterTopMostItems)
	: QSortFilterProxyModel(parent)
	, filterTopMostItems(filterTopMostItems)
{}

bool repo::gui::RepoSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	// During sorting make sure the children of the top most (invisible root) parent are visible OR
	// the given index fulfils the filtering expression OR
	// any of its parents does OR
	// any of its children do.
	return (!filterTopMostItems && !sourceParent.isValid()) ||
		QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent) ||
		filterAcceptsParents(sourceRow, sourceParent) ||
		filterAcceptsChildren(sourceRow, sourceParent);
}

bool repo::gui::RepoSortFilterProxyModel::filterAcceptsParents(int sourceRow, const QModelIndex &sourceParent) const
{
	bool accept = false;
	QModelIndex parent = sourceParent;
    while (parent.isValid()) 
	{
		if (accept = QSortFilterProxyModel::filterAcceptsRow(parent.row(), parent.parent()))
			break;            
        parent = parent.parent();
    }
	return accept;
}

bool repo::gui::RepoSortFilterProxyModel::filterAcceptsChildren(int sourceRow, const QModelIndex &sourceParent) const
{
	bool accept = false;
	QModelIndex item = sourceModel()->index(sourceRow, 0, sourceParent);
	if (item.isValid())
	{
		for (int i = 0; i < item.model()->rowCount(item); ++i) 
		{
			// Depth first search
			if (accept = (QSortFilterProxyModel::filterAcceptsRow(i, item) || 
				filterAcceptsChildren(i, item)))
				break;
		}
	}
    return accept;
}
