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

    //--------------------------------------------------------------------------

    QObject::connect(ui->refreshPushButton, &QPushButton::pressed,
                     this, &RepoDialogManagerAccess::refresh);

    QObject::connect(ui->databaseComboBox, SIGNAL(currentIndexChanged(const QString &)),
                     this, SLOT(refresh()));

    QObject::connect(
                ui->usersManagerWidget->getFilterableTree(),
                &RepoWidgetTreeFilterable::totalCountChanged,
                this,
                &RepoDialogManagerAccess::updateUsersTabCount);

    QObject::connect(
                ui->rolesManagerWidget->getFilterableTree(),
                &RepoWidgetTreeFilterable::totalCountChanged,
                this,
                &RepoDialogManagerAccess::updateRolesTabCount);

    QObject::connect(
                ui->projectsManagerWidget->getFilterableTree(),
                &RepoWidgetTreeFilterable::totalCountChanged,
                this,
                &RepoDialogManagerAccess::updateProjectsTabCount);

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
    //--------------------------------------------------------------------------
    // Retrieve databases with their projects
    if (isReady())
    {
        repo::worker::RepoWorkerProjects* worker =
                new repo::worker::RepoWorkerProjects(
                    controller,
                    getToken());

        QObject::connect(worker,
                         &repo::worker::RepoWorkerProjects::databasesWithProjectsFetched,
                         ui->rolesManagerWidget,
                         &RepoWidgetManagerRoles::setDatabasesWithProjects);

        QObject::connect(worker,
                         &repo::worker::RepoWorkerProjects::databasesWithProjectsFetched,
                         ui->usersManagerWidget,
                         &RepoWidgetManagerUsers::setDatabasesWithProjects);

        connectAndStartWorker(worker);
    }

    ui->usersManagerWidget->setDBConnection(controller, getToken(), getDatabase());
    ui->usersManagerWidget->refresh();

    ui->rolesManagerWidget->setDBConnection(controller, getToken(), getDatabase());
    ui->rolesManagerWidget->refresh();

    ui->projectsManagerWidget->setDBConnection(controller, getToken(), getDatabase());
    ui->projectsManagerWidget->refresh();

}

void RepoDialogManagerAccess::updateUsersTabCount(int count)
{
    updateTabCount(Tab::USERS, tr("Users"), count);
}

void RepoDialogManagerAccess::updateRolesTabCount(int count)
{
    updateTabCount(Tab::ROLES, tr("Roles"), count);
}

void RepoDialogManagerAccess::updateProjectsTabCount(int count)
{
    updateTabCount(Tab::PROJECTS, tr("Projects"), count);
}

void RepoDialogManagerAccess::updateTabCount(Tab tab, const QString &title, int count)
{
    ui->tabWidget->setTabText((int) tab,  title + " (" + QString::number(count) + ")");
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
