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
repo::gui::RepoDialogCommit::RepoDialogCommit(QWidget *parent,
    Qt::WindowFlags flags,
    RepoIDBCache *dbCache,
    const QString &projectName,
    const core::RepoNodeAbstractSet &nodes,
    core::RepoNodeRevision *revision)
	: QDialog(parent, flags)
    , projectName(projectName)
    , nodes(nodes)
	, revision(revision)
    , dbCache(dbCache)
    , ui(new Ui::RepoDialogCommit)
{
    ui->setupUi(this);
    this->setWindowIcon(getIcon());

	//this->splitter->setStretchFactor(1, );
    //--------------------------------------------------------------------------
	model = new QStandardItemModel(0, 5, this); // 0 row, 5 cols 
    model->setHeaderData(Columns::NAME, Qt::Horizontal, tr("Name"));
    model->setHeaderData(Columns::TYPE, Qt::Horizontal, tr("Type"));
    model->setHeaderData(Columns::STATUS, Qt::Horizontal, tr("Status"));
    model->setHeaderData(Columns::UID, Qt::Horizontal, tr("Unique ID"));
    model->setHeaderData(Columns::SID, Qt::Horizontal, tr("Shared ID"));
	
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

    QObject::connect(
                ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(editItem(QModelIndex)));


    ui->branchComboBox->addItem(
                RepoFontAwesome::getBranchIcon(),
                QString::fromStdString(revision->getName()));

    //--------------------------------------------------------------------------

    QObject::connect(ui->databaseComboBox,
                     SIGNAL(currentIndexChanged(const QString &)),
                     this, SLOT(updateProjects()));
    QObject::connect(ui->projectComboBox,
                     SIGNAL(currentIndexChanged(const QString &)),
                     this, SLOT(updateBranches()));

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

void repo::gui::RepoDialogCommit::editItem(const QModelIndex &proxyIndex)
{
    QModelIndex modelIndex = proxyModel->mapToSource(proxyIndex);
    QStandardItem *item = model->item(modelIndex.row(),Columns::NAME);

    if (item)
    {
        core::RepoNodeAbstract* node = item->data().value<core::RepoNodeAbstract*>();
        if (node && REPO_NODE_TYPE_TRANSFORMATION == node->getType())
        {
            core::RepoNodeTransformation *transformation = dynamic_cast<core::RepoNodeTransformation*>(node);
            if (transformation)
            {
                RepoTransformationDialog transformationDialog(*transformation, this);
                if (transformationDialog.exec())
                {
                    core::RepoNodeTransformation t = transformationDialog.getTransformation();
                    transformation->setName(t.getName());
                    transformation->setMatrix(t.getMatrix());
                    proxyModel->setData(proxyIndex.sibling(proxyIndex.row(), Columns::NAME),
                                        QString::fromStdString(t.getName()));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
int repo::gui::RepoDialogCommit::exec()
{    
    //--------------------------------------------------------------------------
	// Blocking operation
	this->setCursor(Qt::WaitCursor);
    // TODO: make into asynchronous worker


    // Cascading updates: change of host triggers change of databases and
    // that of projects and that of branches.
    updateHosts();


    // Set the currently selected project instead of the very first one.
    ui->databaseComboBox->setCurrentText(dbCache->getSelectedDatabase());
    ui->projectComboBox->setCurrentText(projectName);


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

void repo::gui::RepoDialogCommit::updateHosts()
{
    if (dbCache)
    {
        ui->serverComboBox->clear();
        dbCache->setHostsComboBox(ui->serverComboBox);
        updateDatabases();
    }
}

void repo::gui::RepoDialogCommit::updateDatabases()
{
    if (dbCache)
    {
        ui->databaseComboBox->clear();
        dbCache->setComboBox(ui->databaseComboBox,
                    RepoFontAwesome::getDatabaseIcon(),
                    dbCache->getDatabases(ui->serverComboBox->currentText()));
        updateProjects();
    }
}

void repo::gui::RepoDialogCommit::updateProjects()
{
    if (dbCache)
    {
        ui->projectComboBox->clear();
        dbCache->setComboBox(ui->projectComboBox,
                    RepoFontAwesome::getProjectIcon(),
                    dbCache->getProjects(ui->serverComboBox->currentText(),
                    ui->databaseComboBox->currentText()));
        updateBranches();
    }
}

void repo::gui::RepoDialogCommit::updateBranches()
{
    // TODO: load branches depending on the selected project
}

//------------------------------------------------------------------------------
void repo::gui::RepoDialogCommit::setModifiedObjects()
{	
    // TODO: make into asynchronous worker


    core::RepoNodeAbstractSet modifiedObjects = nodes;

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

    core::RepoNodeAbstractSet::const_iterator it;
    for (it = modifiedObjects.begin(); it != modifiedObjects.end(); ++it)
    {
        core::RepoNodeAbstract *node = *it;
		row.clear();

        QVariant var;
        var.setValue(node);

        //----------------------------------------------------------------------
		// Name
		item = new QStandardItem(QString::fromStdString(node->getName()));
        item->setData(var);
		item->setCheckable(true);
		item->setCheckState(Qt::Checked);
		item->setTristate(false);
		item->setEditable(false);
		row.append(item);

        std::string type = node->getType();
        if (REPO_NODE_TYPE_METADATA == type)
            item->setIcon(RepoFontAwesome::getMetadataIcon());

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

QString repo::gui::RepoDialogCommit::getCurrentHost() const
{
    return ui->serverComboBox->currentText();
}

QString repo::gui::RepoDialogCommit::getCurrentDatabase() const
{
    return ui->databaseComboBox->currentText();
}

QString repo::gui::RepoDialogCommit::getCurrentProject() const
{
    return ui->projectComboBox->currentText();
}


repo::core::RepoNodeAbstractSet repo::gui::RepoDialogCommit::getNodesToCommit() const
{
    core::RepoNodeAbstractSet nodes;
    for (int i = 0; i < model->rowCount(); ++i)
    {
        QStandardItem *item = model->invisibleRootItem()->child(i,Columns::NAME);
        if (item->checkState() != Qt::CheckState::Unchecked)
            nodes.insert(item->data().value<core::RepoNodeAbstract*>());
    }
    return nodes;
}

void repo::gui::RepoDialogCommit::updateCountLabel() const
{
    static QLocale locale;
    ui->countLabel->setText(tr("Showing %1 of %2").arg(locale.toString(proxyModel->rowCount())).arg(locale.toString(model->rowCount())));
}
