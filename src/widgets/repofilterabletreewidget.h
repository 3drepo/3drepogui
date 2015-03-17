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


#ifndef REPO_FILTERABLE_TREE_WIDGET_H
#define REPO_FILTERABLE_TREE_WIDGET_H

#include <QWidget>
#include<QStandardItemModel>
#include<QSortFilterProxyModel>
#include <QProgressBar>

namespace Ui {
    class RepoFilterableTreeWidget;
}

namespace repo {
namespace gui {


class RepoFilterableTreeWidget : public QWidget
{
    Q_OBJECT

public:

    RepoFilterableTreeWidget(
            const QList<QString>& headers,
            QSortFilterProxyModel* proxy = new QSortFilterProxyModel(),
            QWidget *parent = 0);

    //! Constructor to remove the UI elements and tree models.
    ~RepoFilterableTreeWidget();


public slots:

    //! Adds a single row to the tree view.
    void addRow(const QList<QStandardItem*>& row)
    { model->invisibleRootItem()->appendRow(row); }

public:

    //! Returns standard item model of the tree view
    QStandardItemModel* getModel() const
    { return model; }

    //! Returns filter proxy model of the tree view.
    QSortFilterProxyModel* getProxyModel() const
    { return proxy; }

    QProgressBar* getProgressBar() const;

private:

    Ui::RepoFilterableTreeWidget* ui;

    //! Default model for the databases.
    QStandardItemModel* model;

    //! Sorting model proxy for the databases.
    QSortFilterProxyModel* proxy;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_FILTERABLE_TREE_WIDGET_H
