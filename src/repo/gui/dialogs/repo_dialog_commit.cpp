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

#include "repo_dialog_commit.h"
//------------------------------------------------------------------------------
#include <QtGui>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QCheckBox>
#include <QPushButton>
//------------------------------------------------------------------------------
#include "../primitives/repo_fontawesome.h"
#include "../../logger/repo_logger.h"
//------------------------------------------------------------------------------

using namespace repo::gui::dialog;

CommitDialog::CommitDialog(QWidget *parent,
                           Qt::WindowFlags flags,
                           repo::gui::primitive::RepoIDBCache *dbCache,
                           repo::core::model::RepoScene * scene)
    : QDialog(parent, flags)
    , dbCache(dbCache)
    , scene(scene)
    , ui(new Ui::CommitDialog)
    , skip(0)
    , modifiedNodesCount(scene ? scene->getTotalNodesChanged() : 0)
{
    //FIXME: this should pop up the project and database names as default selection according to what's inside the scene graph
    ui->setupUi(this);
    this->setWindowIcon(repo::gui::primitive::RepoFontAwesome::getCommitIcon());
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
                this, &CommitDialog::updateCountLabel);

    QObject::connect(
                proxyModel, &QSortFilterProxyModel::rowsRemoved,
                this, &CommitDialog::updateCountLabel);

    //    QObject::connect(
    //                ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
    //                this, SLOT(editItem(QModelIndex)));


    QObject::connect(
                ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(showBSON(QModelIndex)));

    if (scene)
        ui->branchComboBox->addItem(
                    repo::gui::primitive::RepoFontAwesome::getBranchIcon(),
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
                     this, &CommitDialog::infiniteScroll);

}

//------------------------------------------------------------------------------
CommitDialog::~CommitDialog() 
{
    cancelAllThreads();

    delete proxyModel;
    delete model;
}

bool CommitDialog::cancelAllThreads()
{
    emit cancel();
    return threadPool.waitForDone(); // msecs
}

void CommitDialog::infiniteScroll(int sliderPosition)
{
    if (sliderPosition == ui->treeView->verticalScrollBar()->maximum())
        loadModifiedObjects();
}

//------------------------------------------------------------------------------
QString CommitDialog::getMessage()
{
    return ui->messagePlainTextEdit->toPlainText();
}

void CommitDialog::addNode(repo::core::model::RepoNode *node, const QString &status)
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

        //        item->setToolTip(QString::fromStdString(node->toString()));

        row.append(item);
        const repo::core::model::NodeType type = node->getTypeAsEnum();
        if (repo::core::model::NodeType::METADATA == type)
            item->setIcon(repo::gui::primitive::RepoFontAwesome::getMetadataIcon());

        //----------------------------------------------------------------------
        // Type
        item = new QStandardItem(QString::fromStdString(node->getType()));
        item->setEditable(false);
        row.append(item);
        //----------------------------------------------------------------------
        // Status
        item = new QStandardItem(status);
        item->setEditable(false);
        row.append(item);

        //----------------------------------------------------------------------
        // UID
        item = new QStandardItem(QString::fromStdString(
                                     node->getUniqueID().toString()));
        item->setEditable(false);
        row.append(item);
        //----------------------------------------------------------------------
        // SID
        item = new QStandardItem(
                    QString::fromStdString(
                        node->getSharedID().toString()));
        item->setEditable(false);
        row.append(item);
        //----------------------------------------------------------------------
        model->appendRow(row);
    }
    else
    {
        repoLogError("Error trying to fetch node information for a node that has a status of " + status.toStdString() + ": nullptr");
    }
}

void CommitDialog::editItem(const QModelIndex &proxyIndex)
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
                TransformationDialog transformationDialog(*transformation, this);
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

void CommitDialog::showBSON(const QModelIndex &proxyIndex)
{
    QModelIndex modelIndex = proxyModel->mapToSource(proxyIndex);
    QStandardItem *item = model->item(modelIndex.row(),Columns::NAME);

    if (item)
    {
        repo::core::model::RepoNode* node = item->data().value<repo::core::model::RepoNode*>();
        if (node)
        {
            QMessageBox box(QMessageBox::Information,
                            QString::fromStdString(node->getName()),
                            QString(),
                            QMessageBox::Close,
                            this,
                            Qt::Window);
            box.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

            // Cannot make resizable
            // Known bug: https://bugreports.qt.io/browse/QTBUG-3696
            // Workaround: https://forum.qt.io/topic/24213/qmessagebox-too-small/12
            // box.setSizeGripEnabled(true);

            QGridLayout* layout = (QGridLayout*) box.layout();
            QTextBrowser edit(&box);
            edit.setText(QString::fromStdString(node->jsonString(mongo::JsonStringFormat::TenGen, 1)));
            edit.setMinimumWidth(500);
            edit.setMinimumHeight(250);
            edit.setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
            edit.setReadOnly(true);
            layout->addWidget(&edit, 0, 1);

            box.exec();
        }

    }
}

//------------------------------------------------------------------------------
int CommitDialog::exec()
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

void CommitDialog::updateHosts()
{
    if (dbCache)
    {
        ui->serverComboBox->clear();
        dbCache->setHostsComboBox(ui->serverComboBox);
        updateDatabases();
    }
}

void CommitDialog::updateDatabases()
{
    if (dbCache)
    {
        ui->databaseComboBox->clear();
        dbCache->setComboBox(ui->databaseComboBox,
                             repo::gui::primitive::RepoFontAwesome::getDatabaseIcon(),
                             dbCache->getDatabases(ui->serverComboBox->currentText()));
        updateProjects();
    }
}

void CommitDialog::updateProjects()
{
    if (dbCache)
    {
        ui->projectComboBox->clear();
        dbCache->setComboBox(ui->projectComboBox,
                             repo::gui::primitive::RepoFontAwesome::getProjectIcon(),
                             dbCache->getProjects(ui->serverComboBox->currentText(),
                                                  ui->databaseComboBox->currentText()));
        updateBranches();
    }
}

void CommitDialog::updateBranches()
{
    // TODO: load branches depending on the selected project
}

//------------------------------------------------------------------------------
void CommitDialog::loadModifiedObjects()
{	
    if (mutex.tryLock() && scene && (skip < modifiedNodesCount || 0 == skip) && cancelAllThreads())
    {
        // TODO: add skip and limit to load when scrollbar reaches bottom.
        worker::RepoWorkerModifiedNodes* worker = new worker::RepoWorkerModifiedNodes(
                    scene, skip);
        worker->setAutoDelete(true);

        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
                    this, &CommitDialog::cancel,
                    worker, &repo::worker::RepoWorkerModifiedNodes::cancel,
                    Qt::DirectConnection);

        QObject::connect(
                    worker, &repo::worker::RepoWorkerModifiedNodes::modifiedNode,
                    this, &CommitDialog::addNode);

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
                    this, &CommitDialog::unlockMutex);

        ui->progressBar->show();
        //----------------------------------------------------------------------
        threadPool.start(worker);
    }
}

QString CommitDialog::getCurrentHost() const
{
    return ui->serverComboBox->currentText();
}

QString CommitDialog::getCurrentDatabase() const
{
    return ui->databaseComboBox->currentText();
}

QString CommitDialog::getCurrentProject() const
{
    return ui->projectComboBox->currentText();
}

void CommitDialog::updateCountLabel() const
{
    static QLocale locale;
    QString msg = skip != modifiedNodesCount ? tr(" - scroll down to reveal more") : "";
    ui->countLabel->setText(tr("Showing %1 of %2").arg(
                                locale.toString(proxyModel->rowCount())).arg(locale.toString(modifiedNodesCount)) + msg);
}
