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

#include <QWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QProgressBar>
#include <QAbstractItemView>
#include <QTreeView>
#include <QSettings>

#include <stdint.h>

namespace Ui {
class RepoWidgetTreeFilterable;
}

namespace repo {
namespace widgets {

class RepoWidgetTreeFilterable : public QWidget
{
    Q_OBJECT

public:

    //! Constructs a default model (no headers) and default basic proxy.
    RepoWidgetTreeFilterable(QWidget *parent = 0);

    //! Destructor to remove the UI elements and tree models.
    ~RepoWidgetTreeFilterable();

signals :

    //! Emitted whenever the total count of items in the model changes.
    void totalCountChanged(int);

public slots:

    //! Makes the visual layout compacted without borders.
    void setCollapsedUI();

    //! Makes the visual layout expanded with borders and spacing.
    void setExpandedUI();

    //! Sets margin of the layout.
    void setMargins(int m);

    void addTopLevelRow(const QList<QStandardItem*>& row)
    { model->invisibleRootItem()->appendRow(row); }

    void addTopLevelRow(QStandardItem* item)
    { model->invisibleRootItem()->appendRow(item); }

    //! Expands all top level items.
    void expandTopLevelItems() const;

    //! Expands given item.
    void expandItem(const QStandardItem *item) const;

    //! Selects a row based on a given item.
    void selectRow(const QStandardItem *item) const;

    //! Removes currently selected row if any.
    void removeRow() { removeRow(getCurrentIndex()); }

    //! Removes row given by index if any.
    void removeRow(const QModelIndex &index);

    //! Clears the tree view.
    void clear();

    //! Updates count label.
    void updateCountLabel() const;

public:

    //! Returns tree font.
    QFont getTreeFont() const;

    //! Returns standard item model of the tree view
    QStandardItemModel *getModel() const
    { return model; }

    //! Returns filter proxy model of the tree view.
    QSortFilterProxyModel *getProxyModel() const
    { return proxy; }

    //! Returns progress bar.
    QProgressBar *getProgressBar() const;

    //! Returns selection model.
    QItemSelectionModel *getSelectionModel() const;

    //! Returns current item for specified column.
    QStandardItem *getCurrentItem(int column = 0) const;

    //! Returns currently selected index.
    QModelIndex getCurrentIndex() const
    { return getSelectionModel()->currentIndex(); }

    /*! Returns current selection indexes which is the number of columns by
     * the number of rows selected.
     */
    QModelIndexList getCurrentSelection() const;

    //! Returns tree view.
    QTreeView *getTreeView() const;

    //! Proxy
    QStandardItem *getItemFromProxy(const QModelIndex &proxyIndex, int column = 0) const;

    //! Model
    QStandardItem *getItemFromSource(const QModelIndex &sourceIndex, int column = 0) const;

public :

    //! Takes ownership of the given proxy pointer (and deletes memory appropriately).
    void setProxyModel(QSortFilterProxyModel* proxy = new QSortFilterProxyModel());

    /*!
     * Sets the selection mode of the tree view.
     * See http://doc.qt.io/qt-5/qabstractitemview.html#SelectionMode-enum
     */
    void setSelectionMode(QAbstractItemView::SelectionMode mode);

    /*!
     * When the user selects an item, any already-selected item becomes
     * unselected, and the user cannot unselect the selected item by clicking on
     * it.
     */
    void setSingleSelectionMode()
    { setSelectionMode(QAbstractItemView::SingleSelection); }

    /*!
     * When the user selects an item in the usual way, the selection is cleared
     * and the new item selected. However, if the user presses the Shift key
     * while clicking on an item, all items between the current item and the
     * clicked item are selected or unselected, depending on the state of the
     * clicked item.
     */
    void setContiguousSelection()
    { setSelectionMode(QAbstractItemView::ContiguousSelection); }

    /*!
     * When the user selects an item in the usual way, the selection is cleared
     * and the new item selected. However, if the user presses the Ctrl key when
     * clicking on an item, the clicked item gets toggled and all other items
     * are left untouched. If the user presses the Shift key while clicking on
     * an item, all items between the current item and the clicked item are
     * selected or unselected, depending on the state of the clicked item.
     * Multiple items can be selected by dragging the mouse over them.
     */
    void setExtendedSelection()
    { setSelectionMode(QAbstractItemView::ExtendedSelection); }

    /*!
     * When the user selects an item in the usual way, the selection status of
     * that item is toggled and the other items are left alone. Multiple items
     * can be toggled by dragging the mouse over them.
     */
    void setMultiSelection()
    { setSelectionMode(QAbstractItemView::MultiSelection); }

    //! Items cannot be selected.
    void setNoSelection()
    { setSelectionMode(QAbstractItemView::NoSelection); }

    //! Sets whether table items left-most items are offset from left or not.
    void setRootIsDecorated(bool on);

    //! Sets the headers on this model.
    void setHeaders(const QList<QString>& headers);

    //! Stores header settings to given label
    void storeHeaders(const QString &label);

    //! Sets header columns and restores header settings based on a given label.
    void restoreHeaders(const QList<QString> &headers, const QString &label);

protected :

    //! Emits the signal of totalChanged if number of items in model changes.
    void notifyOfTotalCountChange();

protected:

    //! UI var.
    Ui::RepoWidgetTreeFilterable* ui;

    //! Default model for the databases.
    QStandardItemModel* model;

    //! Sorting model proxy for the databases.
    QSortFilterProxyModel* proxy;

};

} // end namespace widgets
} // end namespace repo
