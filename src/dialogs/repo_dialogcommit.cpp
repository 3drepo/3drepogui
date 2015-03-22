/**
 *  Copyright (C) 2014 3D Repo Ltd
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

#include "repo_dialogcommit.h"
//------------------------------------------------------------------------------
#include <QtGui>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QCheckBox>
#include <QPushButton>
//------------------------------------------------------------------------------
#include "../primitives/repo_fontawesome.h"

//------------------------------------------------------------------------------
repo::gui::RepoDialogCommit::RepoDialogCommit(const core::MongoClientWrapper &server,
    QWidget *parent,
    Qt::WindowFlags flags,
    const QString &database,
    const QString &project,
    const QString &branch,
    const core::RepoGraphAbstract *scene,
    core::RepoNodeRevision *revision)
	: QDialog(parent, flags)
	, scene(scene)
	, revision(revision)
    , ui(new Ui::RepoDialogCommit)
{
    ui->setupUi(this);
    this->setWindowIcon(getIcon());

	//this->splitter->setStretchFactor(1, );
    //--------------------------------------------------------------------------
	model = new QStandardItemModel(0, 5, this); // 0 row, 5 cols 
    model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Type"));
    model->setHeaderData(2, Qt::Horizontal, tr("Status"));
    model->setHeaderData(3, Qt::Horizontal, tr("Unique ID"));
    model->setHeaderData(4, Qt::Horizontal, tr("Shared ID"));
	
    //--------------------------------------------------------------------------
	proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setFilterKeyColumn(-1); // filter all columns
	proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

	proxyModel->setSourceModel(model);
    ui->treeView->setModel(proxyModel);

    //--------------------------------------------------------------------------
	// Connect filtering text input to the filtering proxy model
	QObject::connect(
        ui->filterLineEdit, &QLineEdit::textChanged,
		proxyModel, &QSortFilterProxyModel::setFilterFixedString);	
    QObject::connect(
        proxyModel, &QSortFilterProxyModel::rowsInserted,
        this, &RepoDialogCommit::updateCountLabel);

    QObject::connect(
        proxyModel, &QSortFilterProxyModel::rowsRemoved,
        this, &RepoDialogCommit::updateCountLabel);


    ui->branchComboBox->addItem(
                RepoFontAwesome::getBranchIcon(),
                branch);

    //--------------------------------------------------------------------------
    // Add DB connections to selector
    // TODO: for loop to add multiple servers
    ui->serverComboBox->addItem(
                RepoFontAwesome::getHostIcon(),
                QString::fromStdString(server.getHostAndPort()));

    ui->databaseComboBox->addItem(
                RepoFontAwesome::getDatabaseIcon(),
                database);

    ui->projectComboBox->addItem(
                RepoFontAwesome::getProjectIcon(),
                project);
}

//------------------------------------------------------------------------------
repo::gui::RepoDialogCommit::~RepoDialogCommit() 
{
	delete proxyModel;
	delete model;
}


//------------------------------------------------------------------------------
QString repo::gui::RepoDialogCommit::getMessage()
{
    return ui->messagePlainTextEdit->toPlainText();
}

//------------------------------------------------------------------------------
QIcon repo::gui::RepoDialogCommit::getIcon()
{
	return RepoFontAwesome::getInstance().getIcon(
		RepoFontAwesome::fa_upload, QColor(Qt::darkGreen));
}

//------------------------------------------------------------------------------
int repo::gui::RepoDialogCommit::exec()
{
    //--------------------------------------------------------------------------
	// Blocking operation
	this->setCursor(Qt::WaitCursor);
	setModifiedObjects();
	this->setCursor(Qt::ArrowCursor);

    //--------------------------------------------------------------------------
	// If user clicked OK
	int result;
    if (result = QDialog::exec() && revision)
	{
		// TODO: modify the revision object according to user selection
		revision->setMessage(getMessage().toStdString());
	}
	return result;
}

//------------------------------------------------------------------------------
void repo::gui::RepoDialogCommit::setModifiedObjects()
{	
    std::set<repo::core::RepoNodeAbstract*> modifiedObjects;
    if (scene)
        modifiedObjects = scene->getNodes();

    //--------------------------------------------------------------------------
	// Number of changes
	QLocale locale;
    ui->countLabel->setText(locale.toString((long long)(modifiedObjects.size()))
                              + " "
                              + tr("changes"));

    //--------------------------------------------------------------------------
	// Populate data model
	QList<QStandardItem *> row;
	QStandardItem *item;

    std::set<repo::core::RepoNodeAbstract *>::const_iterator it;

    for (it = modifiedObjects.begin(); it != modifiedObjects.end(); ++it)
    {
        core::RepoNodeAbstract *node = *it;
		row.clear();
        //----------------------------------------------------------------------
		// Name
		item = new QStandardItem(QString::fromStdString(node->getName()));
		item->setCheckable(true);
		item->setCheckState(Qt::Checked);
		item->setTristate(false);
		item->setEditable(false);
		row.append(item);

        //----------------------------------------------------------------------
		// Type		
		item = new QStandardItem(QString::fromStdString(node->getType()));
		item->setEditable(false);
		row.append(item);
		
        //----------------------------------------------------------------------
		// Status
        item = new QStandardItem(tr("added"));
		item->setEditable(false);
		row.append(item);
		
        //----------------------------------------------------------------------
		// UID
        item = new QStandardItem(QString::fromStdString(
                                      core::MongoClientWrapper::uuidToString(
                                         node->getUniqueID())));
		item->setEditable(false);
		row.append(item);
		
        //----------------------------------------------------------------------
		// SID
        item = new QStandardItem(
                    QString::fromStdString(
                         core::MongoClientWrapper::uuidToString(node->getSharedID())));
		item->setEditable(false);
		row.append(item);

        //----------------------------------------------------------------------
		model->appendRow(row);
	}	
}

QString repo::gui::RepoDialogCommit::getCurrentDatabaseName() const
{
    return ui->databaseComboBox->currentText();
}

QString repo::gui::RepoDialogCommit::getCurrentProjectName() const
{
    return ui->projectComboBox->currentText();
}

void repo::gui::RepoDialogCommit::updateCountLabel() const
{
    ui->countLabel->setText(tr("Showing %1 of %2").arg(proxyModel->rowCount()).arg(model->rowCount()));
}
