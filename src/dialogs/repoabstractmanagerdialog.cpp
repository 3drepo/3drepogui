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


#include "repoabstractmanagerdialog.h"
#include "ui_repoabstractmanagerdialog.h"

#include "../primitives/repo_fontawesome.h"

repo::gui::RepoAbstractManagerDialog::RepoAbstractManagerDialog(
        const RepoIDBCache *dbCache,
        QWidget *parent)
    : QDialog(parent)
    , dbCache(dbCache)
    , ui(new Ui::RepoAbstractManagerDialog)
{
    ui->setupUi(this);
    setWindowIcon(RepoFontAwesome::getManagerIcon());

    //--------------------------------------------------------------------------
    QList<QString> hosts = dbCache->getHosts();
    QString selectedHost = dbCache->getSelectedHost();    
    setComboBox(ui->hostComboBox, RepoFontAwesome::getHostIcon(),
                hosts, selectedHost);

    //--------------------------------------------------------------------------
    QList<QString> databases = dbCache->getDatabases(dbCache->getSelectedHost());
    QString selectedDatabase = dbCache->getSelectedDatabase();
    setComboBox(ui->databaseComboBox, RepoFontAwesome::getDatabaseIcon(),
                databases, selectedDatabase);

    //--------------------------------------------------------------------------
    initialize();
}

repo::gui::RepoAbstractManagerDialog::~RepoAbstractManagerDialog()
{
    cancelAllThreads();
    delete model;
    delete proxy;
    delete ui;
}

bool repo::gui::RepoAbstractManagerDialog::cancelAllThreads()
{
    emit cancel();
    return threadPool.waitForDone(); // msecs
}


void repo::gui::RepoAbstractManagerDialog::clear(bool resizeColumns)
{
    model->removeRows(0, model->rowCount());

    if (resizeColumns)
        for (int i = 0; i < model->columnCount(); ++i)
            ui->treeView->resizeColumnToContents(i);

    ui->filterLineEdit->clear();
    ui->removePushButton->setEnabled(false);
    ui->editPushButton->setEnabled(false);
    proxy->clear();
    updateCountLabel();
}

void repo::gui::RepoAbstractManagerDialog::select(
        const QItemSelection &,
        const QItemSelection &)
{
    ui->removePushButton->setEnabled(true);
    ui->editPushButton->setEnabled(true);
}

int repo::gui::RepoAbstractManagerDialog::exec()
{
    refresh();
    return QDialog::exec();
}

void repo::gui::RepoAbstractManagerDialog::finish()
{
    ui->hostComboBox->setEnabled(true);
    ui->databaseComboBox->setEnabled(true);
}

void repo::gui::RepoAbstractManagerDialog::updateCountLabel() const
{
    ui->countLabel->setText(tr("Showing %1 of %2").arg(proxy->rowCount()).arg(model->rowCount()));
}

QStandardItem *repo::gui::RepoAbstractManagerDialog::createItem(const QString &data)
{
    QStandardItem *item = new QStandardItem(data);
    item->setEditable(false);
    item->setToolTip(data);
    return item;
}

QStandardItem *repo::gui::RepoAbstractManagerDialog::createItem(
    const QVariant& data)
{
    QStandardItem* item = new QStandardItem();
    item->setEditable(false);
    item->setTextAlignment(Qt::AlignRight);
    item->setText(data.toString());
    item->setToolTip(data.toString());
    item->setData(data);
    return item;
}

void repo::gui::RepoAbstractManagerDialog::initialize()
{
    model = new QStandardItemModel(this);
    proxy = new QSortFilterProxyModel(this);
    proxy->setFilterKeyColumn(-1); // filter all columns
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(model);
    ui->treeView->setModel(proxy);

    //--------------------------------------------------------------------------
    // Connect filtering text input to the filtering proxy model
    QObject::connect(
        ui->filterLineEdit, &QLineEdit::textChanged,
        proxy, &QSortFilterProxyModel::setFilterFixedString);

    QObject::connect(
        ui->refreshPushButton, SIGNAL(pressed()),
        this, SLOT(refresh()));

    QObject::connect(
        proxy, &QSortFilterProxyModel::rowsInserted,
        this, &RepoAbstractManagerDialog::updateCountLabel);

    QObject::connect(
        proxy, &QSortFilterProxyModel::rowsRemoved,
        this, &RepoAbstractManagerDialog::updateCountLabel);

    QObject::connect(
        ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &RepoAbstractManagerDialog::select);

    QObject::connect(ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)),
                     this, SLOT(edit(const QModelIndex &)));

    QObject::connect(
        ui->addPushButton, SIGNAL(pressed()),
                this, SLOT(showEditDialog()));

    QObject::connect(
        ui->removePushButton, SIGNAL(pressed()),
                this, SLOT(removeItem()));

    QObject::connect(ui->editPushButton, SIGNAL(pressed()),
                     this, SLOT(edit()));

    QObject::connect(ui->databaseComboBox, SIGNAL(currentIndexChanged(const QString &)),
                     this, SLOT(refresh()));
}

void repo::gui::RepoAbstractManagerDialog::setComboBox(
        QComboBox *comboBox,
        const QIcon &icon,
        const QList<QString> &list,
        const QString selected)
{
    for (int i = 0; i < list.size(); ++i)
    {
        comboBox->addItem(icon, list[i]);
        if (selected == list[i])
            comboBox->setCurrentIndex(i);
    }
}


