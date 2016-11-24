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

#include "repo_widget_tree_filterable.h"
#include "ui_repo_widget_tree_filterable.h"

using namespace repo::gui::widget;

FilterableTreeWidget::FilterableTreeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FilterableTreeWidget)
    , model(0)
    , proxy(0)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    setProxyModel();
    ui->progressBar->hide();

    QObject::connect(
                proxy, &QSortFilterProxyModel::rowsInserted,
                this, &FilterableTreeWidget::updateCountLabel);

    QObject::connect(
                proxy, &QSortFilterProxyModel::rowsRemoved,
                this, &FilterableTreeWidget::updateCountLabel);

    QObject::connect(
                model, &QStandardItemModel::rowsInserted,
                this, &FilterableTreeWidget::notifyOfTotalCountChange);

    QObject::connect(
                model, &QStandardItemModel::rowsRemoved,
                this, &FilterableTreeWidget::notifyOfTotalCountChange);
}

FilterableTreeWidget::~FilterableTreeWidget()
{    
    delete ui;

    if (model)
        delete model;
    model = nullptr;

    if (proxy)
        delete proxy;
    proxy = nullptr;
}

void FilterableTreeWidget::setCollapsedUI()
{
    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSpacing(0);
    ui->lineEdit->setFrame(false);
    ui->treeView->setFrameShape(QFrame::NoFrame);
    ui->treeView->setLineWidth(0);
}

void FilterableTreeWidget::setExpandedUI()
{
    this->layout()->setContentsMargins(9,9,9,9);
    this->layout()->setSpacing(6);
    ui->lineEdit->setFrame(true);
    ui->treeView->setFrameShape(QFrame::StyledPanel);
    ui->treeView->setLineWidth(1);
}

void FilterableTreeWidget::setMargins(int m)
{
    this->layout()->setContentsMargins(m,m,m,m);
}

void FilterableTreeWidget::expandTopLevelItems() const
{
    ui->treeView->expandToDepth(1);
}

void FilterableTreeWidget::expandItem(const QStandardItem *item) const
{
    ui->treeView->expand(proxy->mapFromSource(model->indexFromItem(item)));
}

void FilterableTreeWidget::clear()
{
    model->removeRows(0, model->rowCount());
    ui->lineEdit->clear();
    updateCountLabel();
}


void FilterableTreeWidget::updateCountLabel() const
{
    ui->countLabel->setText(tr("Showing %1 of %2").arg(proxy->rowCount()).arg(model->rowCount()));
}

void FilterableTreeWidget::selectRow(const QStandardItem *item) const
{
    getSelectionModel()->setCurrentIndex(
                proxy->mapFromSource(model->indexFromItem(item)),
                QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void FilterableTreeWidget::removeRow(const QModelIndex &index)
{
    if (index.isValid())
    {
        model->removeRow(proxy->mapToSource(index).row());
        updateCountLabel();
    }
}


void FilterableTreeWidget::notifyOfTotalCountChange()
{
    emit totalCountChanged(model->rowCount());
}

//------------------------------------------------------------------------------

QFont FilterableTreeWidget::getTreeFont() const
{
    return ui->treeView->getFont();
}

QProgressBar* FilterableTreeWidget::getProgressBar() const
{
    return ui->progressBar;
}

QItemSelectionModel* FilterableTreeWidget::getSelectionModel() const
{
    return ui->treeView->selectionModel();
}

QStandardItem *FilterableTreeWidget::getCurrentItem(int column) const
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

QModelIndexList FilterableTreeWidget::getCurrentSelection() const
{
    return getProxyModel()->mapSelectionToSource(getSelectionModel()->selection()).indexes();
}

void FilterableTreeWidget::setHeaders(const QList<QString>& headers)
{
    model->setColumnCount(headers.size());
    for (int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
}

QTreeView *FilterableTreeWidget::getTreeView() const
{
    return ui->treeView;
}

QStandardItem *FilterableTreeWidget::getItemFromProxy(
        const QModelIndex &proxyIndex, int column) const
{
    QModelIndex sourceIndex = proxy->mapToSource(proxyIndex);
    return getItemFromSource(sourceIndex, column);
}

QStandardItem *FilterableTreeWidget::getItemFromSource(
        const QModelIndex &sourceIndex,
        int column) const
{
    QModelIndex index = model->index(sourceIndex.row(), column, sourceIndex.parent());
    return model->itemFromIndex(index);
}

void FilterableTreeWidget::setProxyModel(QSortFilterProxyModel* proxy)
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

void FilterableTreeWidget::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    ui->treeView->setSelectionMode(mode);
}

void FilterableTreeWidget::setRootIsDecorated(bool on)
{
    ui->treeView->setRootIsDecorated(on);
}

void FilterableTreeWidget::storeHeaders(const QString &label)
{
    QSettings settings(this);
    settings.setValue(label, ui->treeView->header()->saveState());
}

void FilterableTreeWidget::storeSelection(const QString &label)
{
    QSettings settings(this);
    // See  https://bugreports.qt.io/browse/QTBUG-42438
    // settings.setValue(label, proxy->mapToSource(getSelectionModel()->currentIndex()));

    // Temp solution
    int row = proxy->mapToSource(getSelectionModel()->currentIndex()).row();
    settings.setValue(label, row);
}

void FilterableTreeWidget::restoreHeaders(
        const QList<QString> &headers,
        const QString &label)
{
    setHeaders(headers);
    QSettings settings(this);
    ui->treeView->header()->restoreState(settings.value(label).toByteArray());
}

void FilterableTreeWidget::restoreSelection(const QString &label)
{
    // See https://bugreports.qt.io/browse/QTBUG-42438
    // This should magically work from Qt 5.4.0 RC onwards
    QSettings settings(this);
//    getSelectionModel()->setCurrentIndex(
//                proxy->mapFromSource(settings.value(label).toModelIndex()),
//                QItemSelectionModel::ClearAndSelect|
//    QItemSelectionModel::Rows);

    // Temp solution
    int row = settings.value(label).toInt();
    QModelIndex index = model->indexFromItem(model->invisibleRootItem()->child(row, 0));
    getSelectionModel()->setCurrentIndex(proxy->mapFromSource(index),
                                         QItemSelectionModel::ClearAndSelect|
                                         QItemSelectionModel::Rows);
}
