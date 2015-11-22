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


#include "repo_dialog_manager_access.h"
#include "ui_repo_dialog_manager_access.h"

using namespace repo::widgets;

RepoDialogManagerAccess::RepoDialogManagerAccess(
        const repo::gui::RepoIDBCache *dbCache,
        repo::RepoController *controller,
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RepoDialogManagerAccess)
    , dbCache(dbCache)
    , controller(controller)
{
    ui->setupUi(this);
    setWindowIcon(repo::gui::RepoFontAwesome::getAccessManagerIcon());

    if (dbCache)
    {
        dbCache->setHostsComboBox(ui->hostComboBox);
        dbCache->setDatabasesComboBox(ui->databaseComboBox);
    }


    QObject::connect(ui->refreshPushButton, &QPushButton::pressed,
                     this, &RepoDialogManagerAccess::refresh);

    QObject::connect(ui->databaseComboBox, SIGNAL(currentIndexChanged(const QString &)),
                     this, SLOT(refresh()));

}

RepoDialogManagerAccess::~RepoDialogManagerAccess()
{
    delete ui;
}

int RepoDialogManagerAccess::exec()
{
    refresh();
    return QDialog::exec();
}

void RepoDialogManagerAccess::refresh()
{
    ui->userManagerWidget->setDBConnection(controller, getToken(), getDatabase());
    ui->userManagerWidget->refresh();

    QStringList projects = dbCache->getProjects(ui->hostComboBox->currentText(),
                                                ui->databaseComboBox->currentText());
    ui->rolesManagerWidget->setDBConnection(controller, getToken(), getDatabase(), projects);




    ui->rolesManagerWidget->refresh();
}

void RepoDialogManagerAccess::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
    case Qt::Key_Tab:
        ui->tabWidget->setCurrentIndex((ui->tabWidget->currentIndex() + 1) % ui->tabWidget->count());
        break;
    }

    QWidget::keyPressEvent(e);
}

repo::RepoToken* RepoDialogManagerAccess::getToken() const
{
    return dbCache ? dbCache->getConnection(ui->hostComboBox->currentText()) : 0;
}

std::string RepoDialogManagerAccess::getDatabase() const
{
    return ui->databaseComboBox->currentText().toStdString();
}
