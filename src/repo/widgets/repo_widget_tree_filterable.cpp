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


using namespace repo::widgets;

const QString RepoWidgetTreeFilterable::COLUMNS_SETTINGS =
        "RepoWidgetTreeFilterableColumnsSettings";


RepoWidgetTreeFilterable::RepoWidgetTreeFilterable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RepoWidgetTreeFilterable)
    , model(0)
    , proxy(0)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    setProxyModel();
    ui->progressBar->hide();

    QObject::connect(
                proxy, &QSortFilterProxyModel::rowsInserted,
                this, &RepoWidgetTreeFilterable::updateCountLabel);

    QObject::connect(
                proxy, &QSortFilterProxyModel::rowsRemoved,
                this, &RepoWidgetTreeFilterable::updateCountLabel);

    QSettings settings(parent);
    ui->treeView->header()->restoreState(
                settings.value(COLUMNS_SETTINGS).toByteArray());

}

RepoWidgetTreeFilterable::~RepoWidgetTreeFilterable()
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

void RepoWidgetTreeFilterable::setCollapsedUI()
{
    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSpacing(0);
    ui->lineEdit->setFrame(false);
    ui->treeView->setFrameShape(QFrame::NoFrame);
    ui->treeView->setLineWidth(0);
}

void RepoWidgetTreeFilterable::setExpandedUI()
{
    this->layout()->setContentsMargins(9,9,9,9);
    this->layout()->setSpacing(6);
    ui->lineEdit->setFrame(true);
    ui->treeView->setFrameShape(QFrame::StyledPanel);
    ui->treeView->setLineWidth(1);
}

void RepoWidgetTreeFilterable::setMargins(int m)
{
    this->layout()->setContentsMargins(m,m,m,m);
}

void RepoWidgetTreeFilterable::expandTopLevelItems() const
{
    ui->treeView->expandToDepth(1);
}

void RepoWidgetTreeFilterable::expandItem(const QStandardItem *item) const
{
    ui->treeView->expand(proxy->mapFromSource(model->indexFromItem(item)));
}

void RepoWidgetTreeFilterable::clear()
{
    model->removeRows(0, model->rowCount());
    ui->lineEdit->clear();
}


void RepoWidgetTreeFilterable::updateCountLabel() const
{
    ui->countLabel->setText(tr("Showing %1 of %2").arg(proxy->rowCount()).arg(model->rowCount()));
}

void RepoWidgetTreeFilterable::selectRow(const QStandardItem *item) const
{
    getSelectionModel()->setCurrentIndex(
                proxy->mapFromSource(model->indexFromItem(item)),
                QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

//------------------------------------------------------------------------------

QFont RepoWidgetTreeFilterable::getTreeFont() const
{
    return ui->treeView->getFont();
}

QProgressBar* RepoWidgetTreeFilterable::getProgressBar() const
{
    return ui->progressBar;
}

QItemSelectionModel* RepoWidgetTreeFilterable::getSelectionModel() const
{
    return ui->treeView->selectionModel();
}

QStandardItem *RepoWidgetTreeFilterable::getCurrentItem(int column) const
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

QModelIndexList RepoWidgetTreeFilterable::getCurrentSelection() const
{
    return getProxyModel()->mapSelectionToSource(getSelectionModel()->selection()).indexes();
}

void RepoWidgetTreeFilterable::setHeaders(const QList<QString>& headers)
{
    model->setColumnCount(headers.size());
    for (int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
}

QTreeView * RepoWidgetTreeFilterable::getTreeView() const
{
    return ui->treeView;
}

QStandardItem *RepoWidgetTreeFilterable::getItemFromProxy(
        const QModelIndex &proxyIndex, int column) const
{
    QModelIndex sourceIndex = proxy->mapToSource(proxyIndex);
    return getItemFromSource(sourceIndex, column);
}

QStandardItem *RepoWidgetTreeFilterable::getItemFromSource(
        const QModelIndex &sourceIndex,
        int column) const
{
    QModelIndex index = model->index(sourceIndex.row(), column, sourceIndex.parent());
    return model->itemFromIndex(index);
}

void RepoWidgetTreeFilterable::setProxyModel(QSortFilterProxyModel* proxy)
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

void RepoWidgetTreeFilterable::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    ui->treeView->setSelectionMode(mode);
}

void RepoWidgetTreeFilterable::setRootIsDecorated(bool on)
{
    ui->treeView->setRootIsDecorated(on);
}

//QStandardItem *RepoWidgetTreeFilterable::createItem(
//        const QVariant& label,
//        const QVariant& data,
//        Qt::Alignment alignment,
//        bool enabled)
//{
//    QStandardItem* item = new QStandardItem();
//    if (label.type() == QVariant::String)
//        item->setText(label.toString());
//    item->setData(data);
//    item->setEditable(false);
//    item->setTextAlignment(alignment);
//    item->setEnabled(enabled);
//    item->setToolTip(label.toString());
//    return item;
//}

//QStandardItem *RepoWidgetTreeFilterable::createItem(const QString &data)
//{
//    QStandardItem *item = new QStandardItem(data);
//    item->setEditable(false);
//    item->setToolTip(data);
//    return item;
//}

//QStandardItem *RepoWidgetTreeFilterable::createItem(
//    const QVariant& data)
//{
//    QStandardItem* item = new QStandardItem();
//    item->setEditable(false);
//    item->setTextAlignment(Qt::AlignRight);
//    item->setText(data.toString());
//    item->setToolTip(data.toString());
//    item->setData(data);
//    return item;
//}


//------------------------------------------------------------------------------

QStandardItem * RepoWidgetTreeFilterable::createItem(
        const QString& text,
        const QVariant& data,
        Qt::Alignment alignment,
        bool enabled)
{
    QStandardItem* item = new QStandardItem();
    item->setEditable(false);
    item->setTextAlignment(alignment);
    item->setEnabled(enabled);
    setItem(item, text, data);
    return item;
}

//------------------------------------------------------------------------------

QStandardItem * RepoWidgetTreeFilterable::createItem(
        const QVariant& data,
        Qt::Alignment alignment)
{
    return createItem(data.toString(), data, alignment);
}

QStandardItem * RepoWidgetTreeFilterable::createItem(uint64_t number)
{
    return setItemNumber(createItem(number, Qt::AlignRight), number);
}

//------------------------------------------------------------------------------

QStandardItem *RepoWidgetTreeFilterable::setItem(
        QStandardItem * item,
        const QString& text,
        const QVariant& data,
        int role)
{
    item->setText(text);
    item->setToolTip(text);
    item->setData(data, role);
    return item;
}

//------------------------------------------------------------------------------

QStandardItem *RepoWidgetTreeFilterable::setItemNumber(QStandardItem * item, uint64_t value)
{
    return setItem(item, toLocaleString((qulonglong)value), (qulonglong)value, Qt::DisplayRole);
}

QStandardItem *RepoWidgetTreeFilterable::setItemFileSize(QStandardItem * item, uint64_t value)
{
    return setItem(item, toFileSize((qulonglong)value), (qulonglong)value, Qt::DisplayRole);
}

QString RepoWidgetTreeFilterable::toFileSize(uint64_t bytes)
{
    QString value;
    if (0 != bytes)
    {
        QLocale locale;
        //stream << setprecision(2) << fixed;
        static const int unit = 1024;
        if (bytes < unit)
            value = locale.toString((double) bytes, 'f', 2) + " B";
        else
        {
            int exp = (int) (std::log(bytes) / std::log(unit));
            static const std::string prefixes = ("KMGTPEZY");
            char prefix = prefixes.at(exp-1);
            value = locale.toString((bytes / pow(unit, exp)), 'f', 2) + " " + prefix + "B";
        }
    }
    return  value;
}
