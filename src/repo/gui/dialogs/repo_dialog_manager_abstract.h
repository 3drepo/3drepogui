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
// Qt
#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QThreadPool>
#include <QItemSelection>
#include <QComboBox>
#include <QMenu>
#include <QWidgetAction>
#include <QSettings>


// GUI
#include "../primitives/repoidbcache.h"

namespace Ui {
class AbstractManagerDialog;
}

namespace repo {
namespace gui {
namespace dialog{

class AbstractManagerDialog : public QDialog
{
    Q_OBJECT

    static const QString COLUMNS_SETTINGS;

public:

    //! Explicit constructor.
    explicit AbstractManagerDialog(
            const repo::gui::RepoIDBCache *dbCache = 0,
            QWidget *parent = 0);

    //! Destructor to clean up model and proxy.
    ~AbstractManagerDialog();

signals :

    //! Emitted whenever running threads are to be cancelled.
    void cancel();

public slots :

    //! Cancels all running threads and waits for their completion.
    virtual bool cancelAllThreads();

    //! Clears the model.
    virtual void clear();

    //! Updates selected item.
    virtual void edit() = 0;

    //! Updates item based on model index.
    virtual void edit(const QModelIndex &index) = 0;

    //! Called when loading is finished.
    virtual void finish();

    //! Shows the dialog in a modal window and waits for user input.
    virtual int exec();

    //! Sets the number of items shown as "Showing x of y" label.
    virtual void updateCountLabel() const;

    //! Refreshes the current list
    virtual void refresh() = 0;

    //! Removes item and refreshes the DB if necessary.
    virtual void removeItem() = 0;

    //! Selects the data from the given item.
    virtual void select(const QItemSelection &, const QItemSelection &);

    //! Shows edit dialog.
    virtual void showEditDialog() = 0;

    //! Shows custom context menu for treeView.
    virtual void showCustomContextMenu(const QPoint &);

public :

    //! Creates a default non-editable item from a given string.
    static QStandardItem *createItem(const QString &);

    //! Creates a default non-editable item from a given variant.
    static QStandardItem *createItem(const QVariant &);

protected :

    //! Model of the users table.
    QStandardItemModel *model;

    //! Proxy model for users table to enable filtering.
    QSortFilterProxyModel *proxy;

    //! Threadpool for this object only.
    QThreadPool threadPool;

    //! Ui var.
    Ui::AbstractManagerDialog *ui;

    const repo::gui::RepoIDBCache* dbCache;

}; // end class

}
} // end namespace gui
} // end namespace repo

