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


#ifndef REPO_ABSTRACT_MANAGER_DIALOG_H
#define REPO_ABSTRACT_MANAGER_DIALOG_H

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


// GUI
#include "../primitives/repoidbcache.h"

namespace Ui {
class RepoAbstractManagerDialog;
}

namespace repo {
namespace gui {

class RepoAbstractManagerDialog : public QDialog
{
    Q_OBJECT

public:

    //! Constructor.
//    RepoAbstractManagerDialog(
//            const core::MongoClientWrapper &mongo,
//            const std::string &database = core::MongoClientWrapper::ADMIN_DATABASE,
//            QWidget *parent = 0);

    //! Explicit constructor.
    explicit RepoAbstractManagerDialog(
            const RepoIDBCache *dbCache = 0,
            QWidget *parent = 0);

    //! Destructor to clean up model and proxy.
    ~RepoAbstractManagerDialog();

signals :

    //! Emitted whenever running threads are to be cancelled.
    void cancel();

public slots :

    //! Cancels all running threads and waits for their completion.
    virtual bool cancelAllThreads();

    //! Clears the model.
    virtual void clear(bool resizeColumns = false);

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
    virtual void refresh(/*const core::RepoBSON &command = core::RepoBSON()*/) = 0;

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
    Ui::RepoAbstractManagerDialog *ui;

    const RepoIDBCache *dbCache;

}; // end class

} // end namespace gui
} // end namespace repo

#endif // REPO_ABSTRACT_MANAGER_DIALOG_H
