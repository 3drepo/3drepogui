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


#include "repodialogsettings.h"
#include "ui_repodialogsettings.h"
#include "../primitives/repo_fontawesome.h"

repo::gui::RepoDialogSettings::RepoDialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoDialogSettings)
{
    ui->setupUi(this);

    QObject::connect((const QObject*) (ui->buttonBox->button(QDialogButtonBox::Apply)),
                     SIGNAL(clicked()), this, SLOT(apply()));

    QObject::connect((const QObject*) (ui->buttonBox->button(QDialogButtonBox::Ok)),
                     SIGNAL(clicked()), this, SLOT(apply()));

    //--------------------------------------------------------------------------

    optionsModel = new QStandardItemModel(this);
    optionsProxy = new QSortFilterProxyModel(this);
    optionsProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    optionsProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    optionsProxy->setSourceModel(optionsModel);

    ui->optionsListView->setModel(optionsProxy);

    //--------------------------------------------------------------------------
    // Connect filtering text input to the filtering proxy model
    QObject::connect(
        ui->filterLineEdit, &QLineEdit::textChanged,
        optionsProxy, &QSortFilterProxyModel::setFilterFixedString);

    QList<QStandardItem *> options;

    //--------------------------------------------------------------------------
    // Importer
    QStandardItem* item = new QStandardItem(tr("Importer"));
    item->setEditable(false);
    item->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_upload, QColor(Qt::darkGreen)));
    options.append(item);

    //--------------------------------------------------------------------------
    // Oculus VR
    item = new QStandardItem(tr("Oculus VR"));
    item->setEditable(false);
    item->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_eye, QColor(Qt::black)));
    options.append(item);

    optionsModel->appendColumn(options);

    QObject::connect(ui->optionsListView, SIGNAL(activated(QModelIndex)),
                     this, SLOT(changeOptionsPane(QModelIndex)));

    QObject::connect(ui->optionsListView, SIGNAL(clicked(QModelIndex)),
                     this, SLOT(changeOptionsPane(QModelIndex)));
}

repo::gui::RepoDialogSettings::~RepoDialogSettings()
{
    delete ui;
    delete optionsModel;
    delete optionsProxy;
}

void repo::gui::RepoDialogSettings::apply()
{
    ui->assimpFlagsWidget->apply();
}

void repo::gui::RepoDialogSettings::changeOptionsPane(const QModelIndex &index)
{
    if (index.isValid())
    {
        ui->stackedWidget->setCurrentIndex(index.row());
        ui->optionsLabel->setText(index.data().toString());
    }
}
