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


#include "repoprojectmanagerdialog.h"
#include "ui_repoabstractmanagerdialog.h"
#include "repoprojectsettingsdialog.h"
#include "../repo/workers/repo_worker_project_settings.h"


repo::gui::RepoProjectManagerDialog::RepoProjectManagerDialog(
	repo::RepoController *controller,
	const RepoIDBCache *cache,
	QWidget *parent)
	: RepoAbstractManagerDialog(cache, parent),
	controller(controller)
{
    setWindowTitle("Project Manager");

    //--------------------------------------------------------------------------
    // Users
    model->setColumnCount(7);
    model->setHeaderData(
                (int) Columns::PROJECT,
                Qt::Horizontal,
                tr("Project"));
    model->setHeaderData(
				(int) Columns::DESCRIPTION,
                Qt::Horizontal,
                tr("Description"));
    model->setHeaderData(
				(int) Columns::OWNER,
                Qt::Horizontal,
                tr("Owner"));
    model->setHeaderData(
				(int) Columns::GROUP,
                Qt::Horizontal,
                tr("Group"));
    model->setHeaderData(
				(int) Columns::PERMISSIONS,
                Qt::Horizontal,
                tr("Permissions"));
    model->setHeaderData(
				(int) Columns::TYPE,
                Qt::Horizontal,
                tr("Type"));
    model->setHeaderData(
				(int) Columns::USERS,
                Qt::Horizontal,
                tr("Users"));

	ui->treeView->sortByColumn((int)Columns::PROJECT, Qt::SortOrder::AscendingOrder);
    clear();
}

repo::gui::RepoProjectManagerDialog::~RepoProjectManagerDialog() {}

void repo::gui::RepoProjectManagerDialog::addProjectSettings(
        repo::core::model::bson::RepoProjectSettings projectSettings)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // User object itself
    QVariant var;
    var.setValue(projectSettings);

    // Project
    QStandardItem *item = createItem(QString::fromStdString(projectSettings.getProjectName()));
    item->setData(var);
    row.append(item);

    // Description
    row.append(createItem(QString::fromStdString(projectSettings.getDescription())));

    // Owner
    row.append(createItem(QString::fromStdString(projectSettings.getOwner())));

    // Group
    row.append(createItem(QString::fromStdString(projectSettings.getGroup())));

    // Permissions
    row.append(createItem(QString::fromStdString(projectSettings.getPermissionsString())));

    // Type
    row.append(createItem(QString::fromStdString(projectSettings.getType())));

    // Users count
    row.append(createItem((qulonglong)(projectSettings.getUsers().size())));

    //--------------------------------------------------------------------------
    model->invisibleRootItem()->appendRow(row);
}

void repo::gui::RepoProjectManagerDialog::clear(bool resizeColumns)
{
    RepoAbstractManagerDialog::clear(resizeColumns);

    ui->treeView->resizeColumnToContents((int)Columns::OWNER);
	ui->treeView->resizeColumnToContents((int)Columns::GROUP);
	ui->treeView->resizeColumnToContents((int)Columns::PERMISSIONS);
	ui->treeView->resizeColumnToContents((int)Columns::TYPE);
	ui->treeView->resizeColumnToContents((int)Columns::USERS);
}

repo::core::model::bson::RepoProjectSettings repo::gui::RepoProjectManagerDialog::getProjectSettings()
{
    return getProjectSettings(ui->treeView->selectionModel()->currentIndex());
}

repo::core::model::bson::RepoProjectSettings 
	repo::gui::RepoProjectManagerDialog::getProjectSettings(const QModelIndex &index)
{
	repo::core::model::bson::RepoProjectSettings projectSettings;
    if (index.isValid())
    {
        QModelIndex userIndex = index.sibling(index.row(), (int)Columns::PROJECT);
		projectSettings = userIndex.data(Qt::UserRole + 1).value<repo::core::model::bson::RepoProjectSettings>();
    }
    return projectSettings;
}



void repo::gui::RepoProjectManagerDialog::refresh(
	const repo::core::model::bson::RepoProjectSettings &settings,
	const bool                                         &isDelete)
{
    if (cancelAllThreads())
    {
        const RepoToken *token = dbCache->getConnection(ui->hostComboBox->currentText());
        std::string database = ui->databaseComboBox->currentText().toStdString();

		repo::worker::ProjectSettingsWorker* worker = 
			new repo::worker::ProjectSettingsWorker(token, controller, database, settings, isDelete);
        worker->setAutoDelete(true);

        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
            this, &RepoProjectManagerDialog::cancel,
			worker, &repo::worker::ProjectSettingsWorker::cancel, Qt::DirectConnection);

        QObject::connect(
			worker, &repo::worker::ProjectSettingsWorker::projectSettingsFetched,
            this, &RepoProjectManagerDialog::addProjectSettings);

        QObject::connect(
			worker, &repo::worker::ProjectSettingsWorker::finished,
            ui->progressBar, &QProgressBar::hide);

        QObject::connect(
			worker, &repo::worker::ProjectSettingsWorker::finished,
            this, &RepoProjectManagerDialog::finish);

        QObject::connect(
			worker, &repo::worker::ProjectSettingsWorker::progressRangeChanged,
            ui->progressBar, &QProgressBar::setRange);

        QObject::connect(
			worker, &repo::worker::ProjectSettingsWorker::progressValueChanged,
            ui->progressBar, &QProgressBar::setValue);

        //----------------------------------------------------------------------
        // Clear any previous entries
        clear();

        //----------------------------------------------------------------------
        ui->progressBar->show();
        ui->hostComboBox->setEnabled(false);
        ui->databaseComboBox->setEnabled(false);
        threadPool.start(worker);
    }
}

void repo::gui::RepoProjectManagerDialog::removeItem()
{
	repo::core::model::bson::RepoProjectSettings projectSettings = this->getProjectSettings();
    switch(QMessageBox::warning(this,
        tr("Remove project settings?"),
        tr("Are you sure you want to remove '") + QString::fromStdString(projectSettings.getProjectName()) + "'?",
        tr("&Yes"),
        tr("&No"),
        QString::null, 1, 1))
        {
            case 0: // yes
                refresh(projectSettings, true);
                break;
            case 1: // no
                std::cout << tr("Remove project settings warning box cancelled by user.").toStdString() << std::endl;
                break;
        }
}

void repo::gui::RepoProjectManagerDialog::showEditDialog(
	const repo::core::model::bson::RepoProjectSettings &projectSettings)
{
    RepoProjectSettingsDialog projectDialog(projectSettings, this);
    if (QDialog::Rejected == projectDialog.exec())
    {
        std::cout << tr("Project dialog cancelled by user.").toStdString() << std::endl;
    }
    else // QDialog::Accepted
    {
        // Create or update project
        refresh(projectDialog.getSettings(), false);
    }
}

