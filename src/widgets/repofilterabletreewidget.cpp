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

#include "repofilterabletreewidget.h"
#include "ui_repofilterabletreewidget.h"

repo::gui::RepoFilterableTreeWidget::RepoFilterableTreeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RepoFilterableTreeWidget)
    , model(0)
    , proxy(0)
{
    ui->setupUi(this);

    //--------------------------------------------------------------------------
    model = new QStandardItemModel(this);
    setProxyModel();
    ui->progressBar->hide();
    setCollapsedUI();
}

repo::gui::RepoFilterableTreeWidget::~RepoFilterableTreeWidget()
{
    delete ui;

    if (model)
        delete model;
    model = 0;

    if (proxy)
        delete proxy;
    proxy = 0;
}

void repo::gui::RepoFilterableTreeWidget::setCollapsedUI()
{
    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSpacing(0);
    ui->lineEdit->setFrame(false);
    ui->treeView->setFrameShape(QFrame::NoFrame);
    ui->treeView->setLineWidth(0);
}

void repo::gui::RepoFilterableTreeWidget::setExpandedUI()
{
    this->layout()->setContentsMargins(9,9,9,9);
    this->layout()->setSpacing(6);
    ui->lineEdit->setFrame(true);
    ui->treeView->setFrameShape(QFrame::StyledPanel);
    ui->treeView->setLineWidth(1);
}

void repo::gui::RepoFilterableTreeWidget::expandTopLevelItems() const
{
    ui->treeView->expandToDepth(1);
}

void repo::gui::RepoFilterableTreeWidget::expandItem(const QStandardItem *item) const
{
    ui->treeView->expand(proxy->mapFromSource(model->indexFromItem(item)));
}

void repo::gui::RepoFilterableTreeWidget::clear()
{
    model->removeRows(0, model->rowCount());
    ui->lineEdit->clear();
}

void repo::gui::RepoFilterableTreeWidget::selectRow(const QStandardItem *item) const
{
    getSelectionModel()->setCurrentIndex(proxy->mapFromSource(model->indexFromItem(item)),
                                         QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

//------------------------------------------------------------------------------

QFont repo::gui::RepoFilterableTreeWidget::getTreeFont() const
{
    return ui->treeView->getFont();
}

QProgressBar* repo::gui::RepoFilterableTreeWidget::getProgressBar() const
{
    return ui->progressBar;
}

QItemSelectionModel* repo::gui::RepoFilterableTreeWidget::getSelectionModel() const
{
    return ui->treeView->selectionModel();
}

QModelIndexList repo::gui::RepoFilterableTreeWidget::getCurrentSelection() const
{
    return getProxyModel()->mapSelectionToSource(getSelectionModel()->selection()).indexes();
}

void repo::gui::RepoFilterableTreeWidget::setHeaders(const QList<QString>& headers)
{
    model->setColumnCount(headers.size());
    for (int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
}

QTreeView * repo::gui::RepoFilterableTreeWidget::getTreeView() const
{
    return ui->treeView;
}

void repo::gui::RepoFilterableTreeWidget::setProxyModel(QSortFilterProxyModel* proxy)
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

void repo::gui::RepoFilterableTreeWidget::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    ui->treeView->setSelectionMode(mode);
}

void repo::gui::RepoFilterableTreeWidget::setRootIsDecorated(bool on)
{
    ui->treeView->setRootIsDecorated(on);
}

QStandardItem *repo::gui::RepoFilterableTreeWidget::createItem(
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
