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


#include "repo_widget_tree_unfilterable.h"
#include "ui_repo_widget_tree_unfilterable.h"

using namespace repo::gui::widget;

UnfilterableTreeWidget::UnfilterableTreeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UnfilterableTreeWidget)
    , newRowText({tr("<empty>"), tr("<empty>")})
    , tabWidget(0)
    , tab(0)
    , emitChangeSignal(true)
{
    ui->setupUi(this);

    //--------------------------------------------------------------------------

    QObject::connect(
                ui->treeWidget, &QTreeWidget::itemChanged,
                this, &UnfilterableTreeWidget::updateDelegate);

    QObject::connect(
                ui->addPushButton, SIGNAL(pressed()),
                this, SLOT(addRow()));

    QObject::connect(
                ui->removePushButton, SIGNAL(pressed()),
                this, SLOT(removeRow()));

    QObject::connect(
                this, &UnfilterableTreeWidget::rowCountChanged,
                this, &UnfilterableTreeWidget::notifyTabTextChange);
}

UnfilterableTreeWidget::~UnfilterableTreeWidget()
{
    delete ui;

    //--------------------------------------------------------------------------
    // Dealocate pointers
    QListIterator<QString> i(delegates.keys());
    while (i.hasNext())
    {
        QString item = i.next();
        delete delegates.take(item);
    }
}

void UnfilterableTreeWidget::setButtonsEnabled(bool enabled)
{
    setButtonsEnabled(enabled, enabled);
}

void UnfilterableTreeWidget::setButtonsEnabled(bool enabledAdd, bool enableRemove)
{
    ui->addPushButton->setEnabled(enabledAdd);
    ui->removePushButton->setEnabled(enableRemove);
}

void UnfilterableTreeWidget::setHeaders(const QStringList &headers)
{
    ui->treeWidget->setHeaderLabels(headers);
}

void UnfilterableTreeWidget::removeAll()
{
    int oldRowCount = getRowCount();
    ui->treeWidget->clear();

    if(emitChangeSignal)
        emit rowCountChanged(oldRowCount, 0);
}

void UnfilterableTreeWidget::removeRow()
{
    int oldRowCount = getRowCount();
    QTreeWidgetItem *item = 0;
    item = ui->treeWidget->currentItem();

    if (item)
    {
        delete item;

        int newRowCount = getRowCount();
        if(emitChangeSignal)
            emit rowCountChanged(oldRowCount, newRowCount);
    }
}

QTreeWidgetItem *UnfilterableTreeWidget::addRow(const QStringList &list, bool enabled,
                                                 const bool editable)
{
    QTreeWidgetItem *item = 0;
    if (list.size() == ui->treeWidget->columnCount())
    {
        int oldRowCount = getRowCount();
        item = new QTreeWidgetItem(ui->treeWidget, list);
        int i = 0;
        for (QString s : list)
        {
            item->setData(i++, Qt::DecorationRole, s);
        }
        Qt::ItemFlags flags;
        if (enabled)
            flags |= Qt::ItemIsEnabled;
        if(editable)
            flags |= Qt::ItemIsEditable;
        item->setFlags(flags);

        ui->treeWidget->addTopLevelItem(item);

        setItemDelegateForRow(list[0]);

        ui->treeWidget->scrollToItem(item);
        ui->treeWidget->setCurrentItem(item ,0);

        int newRowCount = getRowCount();
        if(emitChangeSignal)
            emit rowCountChanged(oldRowCount, newRowCount);
    }
    return item;
}

QTreeWidgetItem *UnfilterableTreeWidget::addRow(const QString &a, const QString &b)
{
    return addRow({a, b});
}

QTreeWidgetItem *UnfilterableTreeWidget::addRow(
        const std::pair<std::string, std::string> &pair,
        bool enabled, const bool editable)
{
    return addRow(
        {QString::fromStdString(pair.first), QString::fromStdString(pair.second)},
        enabled, editable);
}

void UnfilterableTreeWidget::addRows(const std::list<std::pair<std::string, std::string> > &list,
                                     const bool enabled, const bool editable)
{
    for (auto pair : list)
        addRow(pair, enabled,editable );
}

std::list<std::pair<std::string, std::string> > UnfilterableTreeWidget::getItemsAsListOfPairsOfStrings() const
{
    std::list<std::pair<std::string, std::string> > list;
    if (ui->treeWidget->columnCount() >= 2)
    {
        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
        {
            QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
            std::string a = item->data(0, Qt::EditRole).toString().toStdString();
            std::string b = item->data(1, Qt::EditRole).toString().toStdString();
            list.push_back(std::make_pair(a, b));
        }
    }
    return list;
}

std::vector<std::string> UnfilterableTreeWidget::getItemsAsVectorOfStrings() const
{
    std::vector<std::string> vector(ui->treeWidget->topLevelItemCount());
    if (ui->treeWidget->columnCount() > 0)
    {
        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
        {
            QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
            vector[i] = item->data(0, Qt::EditRole).toString().toStdString();
        }
    }
    return vector;
}

int UnfilterableTreeWidget::getRowCount() const
{
    return ui->treeWidget->topLevelItemCount();
}

void UnfilterableTreeWidget::setItemDelegateForRow(const QString &item)
{
    setItemDelegateForRow(item, ui->treeWidget->topLevelItemCount() - 1);
}

void UnfilterableTreeWidget::setItemDelegateForRow(const QString &item, int row)
{
    if (delegates.contains(item))
    {
        ui->treeWidget->setItemDelegateForRow(row, delegates.value(item));
    }
}

void UnfilterableTreeWidget::setItemDelegateForColumn(const QString &item, int column)
{
    if (delegates.contains(item))
    {
        ui->treeWidget->setItemDelegateForColumn(column, delegates.value(item));
    }
}

void UnfilterableTreeWidget::updateDelegate(QTreeWidgetItem *current, int column)
{
    if (delegates.size() > 0 && current && 0 == column)
    {
        int row = ui->treeWidget->indexOfTopLevelItem(current);
        QString item = current->data(column, Qt::EditRole).toString();
        if (delegates.contains(item))
        {
            setItemDelegateForRow(item, row);
        }
        ui->treeWidget->setCurrentItem(current,
                                       ui->treeWidget->columnCount() > 1
                                       ? 1
                                       : 0);
    }
}

void UnfilterableTreeWidget::notifyTabTextChange(int oldRowCount, int newRowCount)
{
    QString text;
    if (tabWidget)
    {
        text = updateCountString(tabWidget->tabText(tab), oldRowCount, newRowCount);
    }
    if(emitChangeSignal)
        emit tabTextChanged(tab, text);
}

void UnfilterableTreeWidget::registerTabWidget(QTabWidget *tabWidget, int tab)
{
    if (this->tabWidget)
    {
        QObject::disconnect(this, &UnfilterableTreeWidget::tabTextChanged,
                            this->tabWidget, &QTabWidget::setTabText);
    }

    this->tabWidget = tabWidget;
    this->tab = tab;
    QObject::connect(this, &UnfilterableTreeWidget::tabTextChanged,
                     tabWidget, &QTabWidget::setTabText);
}

QString UnfilterableTreeWidget::updateCountString(
        QString string,
        int oldCount,
        int newCount)
{
    QLocale locale;
    int chopCount = locale.toString(oldCount).length() + 3; // 2 brackets + 1 space
    string.chop(chopCount);
    string += QString(" (") + locale.toString(newCount) + QString(")");
    return string;
}
