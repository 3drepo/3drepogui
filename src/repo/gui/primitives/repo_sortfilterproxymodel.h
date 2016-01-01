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

#ifndef REPO_SORT_FILTER_PROXY_MODEL_H
#define REPO_SORT_FILTER_PROXY_MODEL_H

//-----------------------------------------------------------------------------
// Qt
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Repo GUI

namespace repo {
namespace gui {
// See https://bugreports.qt-project.org/browse/QTBUG-14336?focusedCommentId=233105&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-233105
// Dynamic updates of source model cause empty rows to appear in proxy. This is a bug in Qt!
class RepoSortFilterProxyModel : public QSortFilterProxyModel
{
	//Q_OBJECT

public :

	//! Constructor
	RepoSortFilterProxyModel(QObject *parent = 0, bool filterTopMostItems = true);

protected :

	//! Returns true if given index is to be visible under filtering, false otherwise.
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

	//! Returns true if any of the parents of the index match filtering expression, false otherwise.
	bool filterAcceptsParents(int sourceRow, const QModelIndex &sourceParent) const;

	//! Returns true if any of the children of the index match filtering expression, false otherwise.
	bool filterAcceptsChildren(int sourceRow, const QModelIndex &sourceParent) const;

	bool filterTopMostItems;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_SORT_FILTER_PROXY_MODEL_H
