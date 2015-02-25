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

QProgressBar* repo::gui::RepoFilterableTreeWidget::getProgressBar() const
{
    return ui->progressBar;
}

QItemSelectionModel* repo::gui::RepoFilterableTreeWidget::getSelectionModel() const
{
    return ui->treeView->selectionModel();
}

void repo::gui::RepoFilterableTreeWidget::setHeaders(const QList<QString>& headers)
{
    model->setColumnCount(headers.size());
    for (unsigned int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
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
