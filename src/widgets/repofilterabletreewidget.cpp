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

repo::gui::RepoFilterableTreeWidget::RepoFilterableTreeWidget(
        const QList<QString>& headers,
        QSortFilterProxyModel *proxy,
        QWidget *parent)
    : QWidget(parent)
    , proxy(proxy)
    , ui(new Ui::RepoFilterableTreeWidget)
{
    ui->setupUi(this);

    //--------------------------------------------------------------------------
    model = new QStandardItemModel(this);
    model->setColumnCount(headers.size());

    for (unsigned int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);

    proxy->setDynamicSortFilter(true);
    proxy->setFilterKeyColumn(-1); // filter all columns
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(model);

    ui->treeView->setModel(proxy);

    //--------------------------------------------------------------------------
    QObject::connect(
        ui->lineEdit, &QLineEdit::textChanged,
        proxy, &QSortFilterProxyModel::setFilterFixedString);

    //--------------------------------------------------------------------------
    ui->progressBar->hide();
}

repo::gui::RepoFilterableTreeWidget::~RepoFilterableTreeWidget()
{
    delete ui;
    delete model;
    delete proxy;
}

QProgressBar* repo::gui::RepoFilterableTreeWidget::getProgressBar() const
{
    return ui->progressBar;
}
