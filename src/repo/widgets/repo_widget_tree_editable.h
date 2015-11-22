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

#pragma once

//------------------------------------------------------------------------------
// GUI
#include "repo_widget_tree_filterable.h"

//------------------------------------------------------------------------------
// Qt
#include <QMutex>
#include <QWidget>
#include <QThreadPool>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

//------------------------------------------------------------------------------

namespace Ui { class RepoWidgetTreeEditable; }

namespace repo {
namespace widgets {

class RepoWidgetTreeEditable : public QWidget
{
    Q_OBJECT

public:

    explicit RepoWidgetTreeEditable(QWidget *parent = 0);

    ~RepoWidgetTreeEditable();

signals :

    //! Emitted whenever running threads are to be cancelled.
    void cancel();

public slots :

    //! Updates selected item.
    virtual void edit() = 0;

    //! Updates item based on model index.
    virtual void edit(const QModelIndex &index) = 0;

    //! Refreshes the current list
    virtual void refresh() = 0;

    //! Removes item and refreshes the DB if necessary.
    virtual void removeItem() = 0;

    //! Shows edit dialog.
    virtual void showEditDialog() = 0;

public slots :

    //! Cancels all running threads and waits for their completion.
    virtual bool cancelAllThreads();

    //! Clears the model.
    virtual void clear();

    //! Called when loading is finished.
    virtual void finish();

    //! Selects the data from the given item.
    virtual void select(const QItemSelection &, const QItemSelection &);

    //! Shows custom context menu for treeView.
    virtual void showCustomContextMenu(const QPoint &);

    //! Unlocks refresh mutex.
    virtual void unlockMutex() { mutex.unlock(); }


public :

    virtual RepoWidgetTreeFilterable* getFilterableTree() const;

protected :

    //! Threadpool for this object only.
    QThreadPool threadPool;

    //! Refresh mutex.
    QMutex mutex;

    Ui::RepoWidgetTreeEditable *ui;
};

} // end widgets
} // end repo
