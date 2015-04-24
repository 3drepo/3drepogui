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


#include "repo_genericdialog.h"
#include "ui_repo_genericdialog.h"

repo::gui::RepoGenericDialog::RepoGenericDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoGenericDialog)
{
    ui->setupUi(this);

    RepoTransformationWidget *w = new RepoTransformationWidget();

    QWidget *previous = ui->widget;
    this->layout()->removeWidget(ui->widget);
    this->layout()->removeWidget(ui->buttonBox);

    this->layout()->addWidget(w);
    this->layout()->addWidget(ui->buttonBox);

    delete ui->widget;


}

repo::gui::RepoGenericDialog::~RepoGenericDialog()
{
    delete ui;
}
