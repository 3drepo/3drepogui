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
#include "../repo/logger/repo_logger.h"
//------------------------------------------------------------------------------

repo::gui::RepoDialogCommit::RepoDialogCommit(QWidget *parent,
                                              Qt::WindowFlags flags,
                                              RepoIDBCache *dbCache,
                                              repo::core::model::RepoScene * scene)
    : QDialog(parent, flags)
    , dbCache(dbCache)
    , scene(scene)
    , ui(new Ui::RepoDialogCommit)
    , skip(0)
    , modifiedNodesCount(scene ? scene->getModifiedNodesID().size() : 0)
{
    //FIXME: this should pop up the project and database names as default selection according to what's inside the scene graph
    ui->setupUi(this);
    this->setWindowIcon(RepoFontAwesome::getCommitIcon());
    //this->splitter->setStretchFactor(1, );
    //--------------------------------------------------------------------------
    model = new QStandardItemModel(0, 5, this); // 0 rows, 5 cols
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

    ui->splitter->setSizes(QList<int>()<< 15 << 200);

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

    if (scene)
        ui->branchComboBox->addItem(
                    RepoFontAwesome::getBranchIcon(),
                    QString::fromStdString(scene->getBranchName()));

    //--------------------------------------------------------------------------

    QObject::connect(ui->databaseComboBox,
                     SIGNAL(currentIndexChanged(const QString &)),
                     this, SLOT(updateProjects()));
    QObject::connect(ui->projectComboBox,
                     SIGNAL(currentIndexChanged(const QString &)),
                     this, SLOT(updateBranches()));

    //--------------------------------------------------------------------------

    QObject::connect(ui->treeView->verticalScrollBar(), &QScrollBar::sliderMoved,
                     this, &RepoDialogCommit::infiniteScroll);

}

//------------------------------------------------------------------------------
repo::gui::RepoDialogCommit::~RepoDialogCommit() 
{
    cancelAllThreads();

    delete proxyModel;
    delete model;
}

bool repo::gui::RepoDialogCommit::cancelAllThreads()
{
    emit cancel();
    return threadPool.waitForDone(); // msecs
}

void repo::gui::RepoDialogCommit::infiniteScroll(int sliderPosition)
{
    if (sliderPosition == ui->treeView->verticalScrollBar()->maximum())
        loadModifiedObjects();
}

//------------------------------------------------------------------------------
QString repo::gui::RepoDialogCommit::getMessage()
{
    return ui->messagePlainTextEdit->toPlainText();
}

void repo::gui::RepoDialogCommit::addNode(repo::core::model::RepoNode *node)
{
    if (node)
    {
        ++skip;

        QList<QStandardItem *> row;
        QStandardItem *item;

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
                                     UUIDtoString(
                                         node->getUniqueID())));
        item->setEditable(false);
        row.append(item);

        //----------------------------------------------------------------------
        // SID
        item = new QStandardItem(
                    QString::fromStdString(
                        UUIDtoString(node->getSharedID())));
        item->setEditable(false);
        row.append(item);

        //----------------------------------------------------------------------
        model->appendRow(row);
    }
}

void repo::gui::RepoDialogCommit::editItem(const QModelIndex &proxyIndex)
{
    /*
   FIXME: Alteration of the transformation would only update the one in unoptimized, not the stash.
   So this is disabled to prevent user from doing something they do not expect.

   QModelIndex modelIndex = proxyModel->mapToSource(proxyIndex);
    QStandardItem *item = model->item(modelIndex.row(),Columns::NAME);

    if (item)
    {
        repo::core::model::RepoNode* node = item->data().value<repo::core::model::RepoNode*>();
        if (node && repo::core::model::NodeType::TRANSFORMATION == node->getTypeAsEnum())
        {
            repo::core::model::TransformationNode *transformation = dynamic_cast<repo::core::model::TransformationNode*>(node);
            if (transformation)
            {
                RepoTransformationDialog transformationDialog(*transformation, this);
                if (transformationDialog.exec())
                {
                    repo::core::model::TransformationNode *t =
                            new repo::core::model::TransformationNode(transformationDialog.getTransformation());
                    if (t)
                    {
                        scene->modifyNode(repo::core::model::RepoScene::GraphType::DEFAULT, transformation->getSharedID(), t);
                        delete t;
                    }

                    proxyModel->setData(proxyIndex.sibling(proxyIndex.row(), Columns::NAME),
                                        QString::fromStdString(transformation->getName()));
                }
            }
        }
    }*/
}

//------------------------------------------------------------------------------
int repo::gui::RepoDialogCommit::exec()
{    
    //--------------------------------------------------------------------------
    // Blocking operation
    // TODO: make into asynchronous worker
    // Cascading updates: change of host triggers change of databases and
    // that of projects and that of branches.
    updateHosts();

    // Set the currently selected project instead of the very first one.
    ui->databaseComboBox->setCurrentText(dbCache->getSelectedDatabase());
    ui->projectComboBox->setCurrentText(projectName);

    loadModifiedObjects();

    //--------------------------------------------------------------------------
    // If user clicked OK
    int result;
    if (result = QDialog::exec())
    {
        scene->setCommitMessage(getMessage().toStdString());
        scene->setDatabaseAndProjectName(getCurrentDatabase().toStdString(),
                                         getCurrentProject().toStdString());
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
void repo::gui::RepoDialogCommit::loadModifiedObjects()
{	
    if (mutex.tryLock() && scene && (skip < modifiedNodesCount || 0 == skip) && cancelAllThreads())
    {
        // TODO: add skip and limit to load when scrollbar reaches bottom.
        worker::RepoWorkerModifiedNodes* worker = new worker::RepoWorkerModifiedNodes(
                    scene, skip);
        worker->setAutoDelete(true);

        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
                    this, &RepoDialogCommit::cancel,
                    worker, &repo::worker::RepoWorkerModifiedNodes::cancel,
                    Qt::DirectConnection);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerModifiedNodes::modifiedNode,
                    this, &RepoDialogCommit::addNode);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerModifiedNodes::finished,
                    ui->progressBar, &QProgressBar::hide);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerModifiedNodes::progressRangeChanged,
                    ui->progressBar, &QProgressBar::setRange);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerModifiedNodes::progressValueChanged,
                    ui->progressBar, &QProgressBar::setValue);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerModifiedNodes::finished,
                    this, &RepoDialogCommit::unlockMutex);

        ui->progressBar->show();
        //----------------------------------------------------------------------
        threadPool.start(worker);
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

void repo::gui::RepoDialogCommit::updateCountLabel() const
{
    static QLocale locale;
    QString msg = skip != modifiedNodesCount ? tr(" - scroll down to reveal more") : "";
    ui->countLabel->setText(tr("Showing %1 of %2").arg(
                                locale.toString(proxyModel->rowCount())).arg(locale.toString(modifiedNodesCount)) + msg);
}
