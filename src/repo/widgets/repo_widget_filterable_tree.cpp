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

#include "repo_widget_filterable_tree.h"
#include "ui_repo_widget_filterable_tree.h"


using namespace repo::widgets;

const QString RepoWidgetFilterableTree::COLUMNS_SETTINGS =
        "RepoWidgetFilterableTreeColumnsSettings";


RepoWidgetFilterableTree::RepoWidgetFilterableTree(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RepoWidgetFilterableTree)
    , model(0)
    , proxy(0)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    setProxyModel();
    ui->progressBar->hide();
    setCollapsedUI();

    QSettings settings(parent);
    ui->treeView->header()->restoreState(
                settings.value(COLUMNS_SETTINGS).toByteArray());

}

RepoWidgetFilterableTree::~RepoWidgetFilterableTree()
{    
    QSettings settings(this->parentWidget());
    settings.setValue(COLUMNS_SETTINGS, ui->treeView->header()->saveState());

    delete ui;

    if (model)
        delete model;
    model = nullptr;

    if (proxy)
        delete proxy;
    proxy = nullptr;
}

void RepoWidgetFilterableTree::setCollapsedUI()
{
    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSpacing(0);
    ui->lineEdit->setFrame(false);
    ui->treeView->setFrameShape(QFrame::NoFrame);
    ui->treeView->setLineWidth(0);
}

void RepoWidgetFilterableTree::setExpandedUI()
{
    this->layout()->setContentsMargins(9,9,9,9);
    this->layout()->setSpacing(6);
    ui->lineEdit->setFrame(true);
    ui->treeView->setFrameShape(QFrame::StyledPanel);
    ui->treeView->setLineWidth(1);
}

void RepoWidgetFilterableTree::expandTopLevelItems() const
{
    ui->treeView->expandToDepth(1);
}

void RepoWidgetFilterableTree::expandItem(const QStandardItem *item) const
{
    ui->treeView->expand(proxy->mapFromSource(model->indexFromItem(item)));
}

void RepoWidgetFilterableTree::clear()
{
    model->removeRows(0, model->rowCount());
    ui->lineEdit->clear();
}

void RepoWidgetFilterableTree::selectRow(const QStandardItem *item) const
{
    getSelectionModel()->setCurrentIndex(
                proxy->mapFromSource(model->indexFromItem(item)),
                QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

//------------------------------------------------------------------------------

QFont RepoWidgetFilterableTree::getTreeFont() const
{
    return ui->treeView->getFont();
}

QProgressBar* RepoWidgetFilterableTree::getProgressBar() const
{
    return ui->progressBar;
}

QItemSelectionModel* RepoWidgetFilterableTree::getSelectionModel() const
{
    return ui->treeView->selectionModel();
}

QStandardItem *RepoWidgetFilterableTree::getCurrentItem(int column) const
{
    QModelIndex proxyCurrent = getSelectionModel()->currentIndex();
    QStandardItem *item = 0;
    if (proxyCurrent.isValid())
    {
        QModelIndex modelCurrent = proxy->mapToSource(proxyCurrent);
        QModelIndex modelCurrentColumn = modelCurrent.sibling(modelCurrent.row(), column);
        item = model->itemFromIndex(modelCurrentColumn);
    }
    return item;
}

QModelIndexList RepoWidgetFilterableTree::getCurrentSelection() const
{
    return getProxyModel()->mapSelectionToSource(getSelectionModel()->selection()).indexes();
}

void RepoWidgetFilterableTree::setHeaders(const QList<QString>& headers)
{
    model->setColumnCount(headers.size());
    for (int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
}

QTreeView * RepoWidgetFilterableTree::getTreeView() const
{
    return ui->treeView;
}

QStandardItem *RepoWidgetFilterableTree::getItemFromProxy(
        const QModelIndex &proxyIndex, int column) const
{
    QModelIndex sourceIndex = proxy->mapToSource(proxyIndex);
    return getItemFromSource(sourceIndex, column);
}

QStandardItem *RepoWidgetFilterableTree::getItemFromSource(
        const QModelIndex &sourceIndex,
        int column) const
{
    QModelIndex index = model->index(sourceIndex.row(), column, sourceIndex.parent());
    return model->itemFromIndex(index);
}

void RepoWidgetFilterableTree::setProxyModel(QSortFilterProxyModel* proxy)
{
    if (this->proxy)
        delete this->proxy;

    this->proxy = proxy;
    this->proxy->setDynamicSortFilter(true);
    this->proxy->setFilterKeyColumn(-1); // filter all columns
    this->proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    this->proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->proxy->setSourceModel(model);

    ui->treeView->setModel(this->proxy);

    QObject::connect(
                ui->lineEdit, &QLineEdit::textChanged,
                proxy, &QSortFilterProxyModel::setFilterFixedString);
}

void RepoWidgetFilterableTree::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    ui->treeView->setSelectionMode(mode);
}

void RepoWidgetFilterableTree::setRootIsDecorated(bool on)
{
    ui->treeView->setRootIsDecorated(on);
}

QStandardItem *RepoWidgetFilterableTree::createItem(
        const QString& text,
        const QVariant& data,
        Qt::Alignment alignment,
        bool enabled)
{
    QStandardItem* item = new QStandardItem(text);
    item->setData(data);
    item->setEditable(false);
    item->setTextAlignment(alignment);
    item->setEnabled(enabled);
    item->setToolTip(text);
    return item;
}
