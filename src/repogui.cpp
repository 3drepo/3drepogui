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

//------------------------------------------------------------------------------
// Qt
#include <QMessageBox>
#include <QtSvg>

//------------------------------------------------------------------------------
// Core
#include <RepoGraphHistory>
#include <RepoLogger>
#include <Repo3DDiff>
#include <RepoNodeRevision>
#include <RepoPCA>
#include <RepoCSV>
#include <RepoGraphOptimizer>

//------------------------------------------------------------------------------
// GUI
#include "repogui.h"
#include "ui_repogui.h"
#include "dialogs/repo_dialogcommit.h"
#include "dialogs/repo_dialogconnect.h"
#include "dialogs/repo_dialoghistory.h"
#include "dialogs/repodialogoculus.h"
#include "dialogs/repodialogusermanager.h"
#include "dialogs/repodialogsettings.h"
#include "dialogs/repodialogabout.h"
#include "dialogs/repoprojectmanagerdialog.h"
#include "widgets/repo_widgetrepository.h"
#include "widgets/repo_textbrowser.h"
#include "widgets/repowidgetassimpflags.h"
#include "widgets/reposelectiontreedockwidget.h"
#include "workers/repo_workercommit.h"
#include "renderers/repo_oculus.h"
#include "renderers/repooculustexturerenderer.h"
#include "primitives/repo_fontawesome.h"
#include "primitives/repo_color.h"
#include "dialogs/repoabstractmanagerdialog.h"
#include "dialogs/repofederationdialog.h"
#include "renderers/repo_3ddiffrenderer.h"

//------------------------------------------------------------------------------

const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_GEOMETRY    = "RepoGUI/geometry";
const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_STATE       = "RepoGUI/state";
const QString repo::gui::RepoGUI::REPO_SETTINGS_LINK_WINDOWS    = "RepoGUI/link";

repo::gui::RepoGUI::RepoGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RepoGUI)
    , panelsMenu(0)
{
    ui->setupUi(this);
    restoreSettings();

    core::RepoLogger::instance().addListener(ui->logTextBrowser);


    QIcon icon(":/images/3drepo-icon.svg");
    this->setWindowIcon(icon);

//    this->setWindowIcon(
//                RepoFontAwesome::getInstance().getIcon(
//                            RepoFontAwesome::fa_database,
//                            QColor(246, 101, 60)));

    //--------------------------------------------------------------------------
    // For docks and windows not to update as they are slow to repaint.
    this->setAnimated(false);

    //--------------------------------------------------------------------------
    // Force opengl format settings by default.
    QGLFormat format(QGL::SampleBuffers);
    format.setSwapInterval(1); // vsync
    format.setSamples(16); // Antialiasing (multisample)
    QGLFormat::setDefaultFormat(format);

    //--------------------------------------------------------------------------
    //
    // File
    //
    //--------------------------------------------------------------------------
    // Open
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    ui->actionOpen->setIcon(repo::gui::RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_folder_open));

    // Save As
    QObject::connect(ui->actionSave_As, SIGNAL(triggered()), this,
                     SLOT(saveAs()));
    ui->actionSave_As->setIcon(repo::gui::RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_floppy_o));

    // Save Screenshot
    QObject::connect(ui->actionSave_Screenshot, SIGNAL(triggered()), this,
                     SLOT(saveScreenshot()));


    //--------------------------------------------------------------------------
    // Exit
    QObject::connect(ui->actionExit, SIGNAL(triggered()),
                     QApplication::instance(), SLOT(quit()));
    ui->actionExit->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_times, QColor(Qt::darkRed)));


    //--------------------------------------------------------------------------
    //
    // Repository
    //
    //--------------------------------------------------------------------------
    // Connect
    QObject::connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connect()));
    ui->actionConnect->setIcon(RepoDialogConnect::getIcon());

    // Refresh
    QObject::connect(ui->actionRefresh, SIGNAL(triggered()), this, SLOT(refresh()));
    ui->actionRefresh->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_refresh,
                    QColor(Qt::darkGreen)));

    // Head
    QObject::connect(ui->actionHead, SIGNAL(triggered()), this, SLOT(fetchHead()));
    ui->actionHead->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_download));

    // History
    QObject::connect(ui->actionHistory, SIGNAL(triggered()), this, SLOT(history()));
    ui->actionHistory->setIcon(RepoFontAwesome::getHistoryIcon());


    // Commit
    QObject::connect(ui->actionCommit, SIGNAL(triggered()), this, SLOT(commit()));
    ui->actionCommit->setIcon(RepoDialogCommit::getIcon());

    // Federate
    QObject::connect(ui->actionFederate, SIGNAL(triggered()),
                     this, SLOT(federate()));

    //--------------------------------------------------------------------------
    // User Management...
    QObject::connect(ui->actionUserManager, SIGNAL(triggered()), this, SLOT(openUserManager()));
    ui->actionUserManager->setIcon(RepoFontAwesome::getUserManagerIcon());

    // Project Manager...
    QObject::connect(ui->actionProject_Manager, SIGNAL(triggered()), this,
                     SLOT(openProjectManager()));

    //--------------------------------------------------------------------------
    // Drop
    QObject::connect(ui->actionDrop, SIGNAL(triggered()), this, SLOT(drop()));
    ui->actionDrop->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_trash_o));




    //--------------------------------------------------------------------------
    //
    // Rendering
    //
    //--------------------------------------------------------------------------
    // Link
    QObject::connect(ui->actionLink, SIGNAL(triggered(bool)), ui->mdiArea, SLOT(chainSubWindows(bool)));
    ui->actionLink->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_link,
                    RepoFontAwesome::fa_chain_broken));


    // Oculus VR
    QObject::connect(ui->actionOculus, SIGNAL(triggered()), this, SLOT(oculus()));
    ui->actionOculus->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_eye));

    // Web View
    QObject::connect(ui->actionWeb_View, &QAction::triggered,
                     ui->mdiArea, &RepoMdiArea::addWebViewSubWindow);
    ui->actionWeb_View->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_globe));


    //--------------------------------------------------------------------------
    //
    // Tools
    //
    //--------------------------------------------------------------------------

    // Metadata Management...
    QObject::connect(ui->actionMetadataManager, SIGNAL(triggered()), this, SLOT(openMetadataManager()));

    // Metadata Management...
    QObject::connect(ui->actionOptimize_Graph, SIGNAL(triggered()), this, SLOT(optimizeGraph()));


    // 3D Diff...
    QObject::connect(ui->action3D_Diff, SIGNAL(triggered()), this, SLOT(open3DDiff()));
    ui->action3D_Diff->setIcon(RepoFontAwesome::getInstance().getIcon(
                                    RepoFontAwesome::fa_wrench));

    // Options
    QObject::connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(openSettings()));




    //--------------------------------------------------------------------------
    //
    // Window
    //
    //--------------------------------------------------------------------------
    // Full Screen
    QObject::connect(ui->actionFull_Screen,
                     SIGNAL(triggered()),
                     this, SLOT(toggleFullScreen()));
    ui->actionFull_Screen->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_arrows_alt));

    // Maximize (Tile)
    QObject::connect(ui->actionMaximize, SIGNAL(triggered()),
                     ui->mdiArea, SLOT(maximizeSubWindows()));
    ui->actionMaximize->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_th));


    // Selection Tree (Scene Graph)
    QObject::connect(ui->actionSelection_Tree, SIGNAL(triggered()),
                     this, SLOT(addSelectionTree()));

    // Panels
    panelsMenu = createPanelsMenu();
    ui->menuWindow->addMenu(panelsMenu);


    //--------------------------------------------------------------------------
    //
    // Help
    //
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Email Technical Support
    QObject::connect(ui->actionEmail_Technical_Support, SIGNAL(triggered()),
                    this, SLOT(openSupportEmail()));
    ui->actionEmail_Technical_Support->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_envelope_o));

    //--------------------------------------------------------------------------
    // Report Issue
    QObject::connect(ui->actionReport_Issue, SIGNAL(triggered()),
                    this, SLOT(reportIssue()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()),
                    this, SLOT(about()));


    //--------------------------------------------------------------------------
    // Context menus
     QObject::connect(
        ui->widgetRepository->getDatabasesTreeView(),
        &QWidget::customContextMenuRequested,
        this,
        &RepoGUI::showDatabaseContextMenu);

    QObject::connect(
        ui->widgetRepository->getCollectionTreeView(),
        &QTreeView::customContextMenuRequested,
        this,
        &RepoGUI::showCollectionContextMenuSlot);
}

repo::gui::RepoGUI::~RepoGUI()
{
    delete ui;
    if (panelsMenu)
        delete panelsMenu;
}


//------------------------------------------------------------------------------
//
// Public Slots
//
//------------------------------------------------------------------------------

void repo::gui::RepoGUI::about()
{
    RepoDialogAbout aboutDialog(this);
    aboutDialog.exec();
}

void repo::gui::RepoGUI::addSelectionTree(RepoGLCWidget* widget, Qt::DockWidgetArea area)
{
    RepoSelectionTreeDockWidget* dock = new RepoSelectionTreeDockWidget(widget, this);
    this->addDockWidget(area, dock);
    if (panelsMenu)
        delete panelsMenu;
    panelsMenu = createPanelsMenu();
    ui->menuWindow->addMenu(panelsMenu);
    dock->show();
}

void repo::gui::RepoGUI::commit()
{
    RepoMdiSubWindow *activeWindow = ui->mdiArea->activeSubWindow();
    const RepoGLCWidget *widget = getActiveWidget();

    QString database = ui->widgetRepository->getSelectedDatabase();
    QString project = ui->widgetRepository->getSelectedProject();

    core::RepoNodeAbstractSet nodes;
    core::RepoNodeRevision* revision = 0;

    if (activeWindow && widget)
    {
        const core::RepoGraphScene *repoScene = widget->getRepoScene();
        nodes = repoScene->getNodes();

        // TODO: fix !!!
        if (project.isEmpty())
        {
            QFileInfo path(activeWindow->windowTitle());
            project = RepoWorkerCommit::sanitizeCollectionName(path.completeBaseName());
        }

        core::MongoClientWrapper mongo = ui->widgetRepository->getSelectedConnection();
        core::RepoGraphHistory* history = new core::RepoGraphHistory();

        revision = new core::RepoNodeRevision(mongo.getUsername(database.toStdString()));
        revision->setCurrentUniqueIDs(repoScene->getUniqueIDs());
        history->setCommitRevision(revision);
    }   

    commit(nodes, revision, project, activeWindow);
}

void repo::gui::RepoGUI::commit(
        const core::RepoNodeAbstractSet &nodes,
        core::RepoNodeRevision* revision,
        const QString &project,
        RepoMdiSubWindow *activeWindow)
{
    std::cerr << "TEMPORARY COMMIT ONLY" << std::endl;

    repo::gui::RepoDialogCommit commitDialog(
                this,
                Qt::Window,
                ui->widgetRepository,
                project,
                nodes,
                revision);



    if(!commitDialog.exec())
        std::cout << "Commit dialog cancelled by user" << std::endl;
    else // Clicked "OK"
    {
        //----------------------------------------------------------------------
        // Establish and connect the new worker.
        RepoWorkerCommit *worker = new RepoWorkerCommit(
                     ui->widgetRepository->getConnection(commitDialog.getCurrentHost()),
                    commitDialog.getCurrentDatabase(),
                    commitDialog.getCurrentProject(),
                    revision,
                    commitDialog.getNodesToCommit());

        if (activeWindow)
            QObject::connect(worker, SIGNAL(progress(int, int)), activeWindow, SLOT(progress(int, int)));

        QObject::connect(worker, SIGNAL(finished()), this, SLOT(refresh()));
        //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));

        //----------------------------------------------------------------------
        // Fire up the asynchronous calculation.
        QThreadPool::globalInstance()->start(worker);
    }
}

void repo::gui::RepoGUI::connect()
{
    RepoDialogConnect connectionDialog(this);

    if(!connectionDialog.exec()) // if not clicked "Connect"
    {
        std::cout<< "Connection dialog cancelled by user" << std::endl;
    }
    else
    {
        // TODO move mongo creation outside of this main GUI to repository widget
        // or similar
        core::MongoClientWrapper mongo;

        //----------------------------------------------------------------------
        // if not successfully connected
        if (!mongo.connect(
                    connectionDialog.getHost().toStdString(),
                    connectionDialog.getPort()))
        {
            std::cerr << "Connection error" << std::endl;
        }
        else
        {
            if (!connectionDialog.getUsername().isEmpty())
            {
                mongo.authenticate(
                    connectionDialog.getUsername().toStdString(),
                    connectionDialog.getPassword().toStdString());
            }
            ui->widgetRepository->fetchDatabases(mongo);

            //-----------------------------------------------------------------
            // enable buttons
            ui->actionRefresh->setEnabled(true);
            ui->actionHead->setEnabled(true);
            ui->actionHistory->setEnabled(true);
            ui->actionCommit->setEnabled(true);
            ui->actionDrop->setEnabled(true);
        }
    }
}

QMenu* repo::gui::RepoGUI::createPanelsMenu()
{
    QMenu* panelsMenu = QMainWindow::createPopupMenu();
    panelsMenu->setTitle(tr("Dock Widgets"));
    panelsMenu->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_columns));
    return panelsMenu;
}

void repo::gui::RepoGUI::drop()
{
    QString database = ui->widgetRepository->getSelectedDatabase();
    QString collection = ui->widgetRepository->getSelectedCollection();

    if (database.isNull() || database.isEmpty())
        std::cout << "A database must be selected." << std::endl;
    else if ((database == "local" || database == "admin") && collection.isEmpty())
        std::cout << "You are not allowed to delete 'local' or 'admin' databases." << std::endl;
    else
    {
        QString ns = database + (!collection.isEmpty() ? "." + collection : "");
        switch (QMessageBox::warning(this,
            "Drop?",
            "Are you sure you want to drop '" + ns + "'?",
            "&Yes",
            "&No",
            QString::null, 1, 1))
        {
            case 0: // yes

                // TODO: create a DB manager separate from repositories widget.
                core::MongoClientWrapper mongo = ui->widgetRepository->getSelectedConnection();
                mongo.reconnectAndReauthenticate();
                if (!collection.isEmpty())
                    mongo.dropCollection(ns.toStdString());
                else
                    mongo.dropDatabase(ns.toStdString());
                refresh();
                break;
            }
    }
}

void repo::gui::RepoGUI::federate()
{
    RepoFederationDialog fed(ui->widgetRepository, this);
    if (fed.exec())
    {
        core::RepoGraphScene *scene = fed.getFederation();

        // TODO: diff the scene with previous to get current, added, deleted and modified nodes.
        std::set<const core::RepoNodeAbstract *> nodes = scene->getNodesRecursively();
        std::string username = ui->widgetRepository->getSelectedConnection().getUsername(ui->widgetRepository->getSelectedDatabase().toStdString());

        core::RepoNodeRevision *revision = new core::RepoNodeRevision(username);
    //    revision->setCurrentUniqueIDs(nodes);

        core::RepoNodeAbstractSet nodesSet;
        for (const core::RepoNodeAbstract *node : nodes)
            nodesSet.insert(const_cast<core::RepoNodeAbstract*>(node));
        commit(nodesSet, revision);
    }
}

void repo::gui::RepoGUI::fetchHead()
{
    // Head revision from master branch
    ui->mdiArea->addSubWindow(
                ui->widgetRepository->getSelectedConnection(),
                ui->widgetRepository->getSelectedDatabase(),
                ui->widgetRepository->getSelectedProject());

    // Make sure to hook controls if chain is on
    ui->mdiArea->chainSubWindows(ui->actionLink->isChecked());
}

repo::gui::RepoGLCWidget* repo::gui::RepoGUI::getActiveWidget()
{
    RepoGLCWidget *widget = ui->mdiArea->activeSubWidget<repo::gui::RepoGLCWidget *>();
    if (!widget)
        std::cerr << tr("A 3D window has to be open.").toStdString() << std::endl;
    return widget;
}

const repo::core::RepoGraphScene* repo::gui::RepoGUI::getActiveScene()
{
    const core::RepoGraphScene *scene = 0;
    if (const RepoGLCWidget *widget = getActiveWidget())
        scene = widget->getRepoScene();
    return scene;
}

void repo::gui::RepoGUI::history()
{
    QString database = ui->widgetRepository->getSelectedDatabase();
    QString project = ui->widgetRepository->getSelectedProject();
    core::MongoClientWrapper mongo =
            ui->widgetRepository->getSelectedConnection();
    RepoDialogHistory historyDialog(mongo, database, project, this);

    if(!historyDialog.exec()) // if not OK
        std::cout << "Revision History dialog cancelled by user." << std::endl;
    else
    {
        QList<QUuid> revisions = historyDialog.getSelectedRevisions();
        for (QList<QUuid>::iterator uid = revisions.begin();
             uid != revisions.end();
             ++uid)
            ui->mdiArea->addSubWindow(mongo, database, project, *uid, false);
        //---------------------------------------------------------------------
        // make sure to hook controls if chain is on
        ui->mdiArea->chainSubWindows(ui->actionLink->isChecked());
    }
}

void repo::gui::RepoGUI::loadFile(const QString &filePath)
{
    if (!filePath.isEmpty())
    {
        QFileInfo pathInfo(filePath);
        string fileName = pathInfo.fileName().toStdString();
        std::cout << "Loading " << fileName << " ..." << std::endl;

        ui->mdiArea->addSubWindow(filePath);
        ui->mdiArea->chainSubWindows(ui->actionLink->isChecked());
    }
}

void repo::gui::RepoGUI::loadFiles(const QStringList &filePaths)
{
    QStringList::ConstIterator it = filePaths.begin();
    while(it != filePaths.end())
    {
        loadFile(*it);
        ++it;
    }
}

void repo::gui::RepoGUI::loadFiles(const QList<QUrl> &urls)
{
    QList<QUrl>::ConstIterator it = urls.begin();
    while(it != urls.end())
    {
        QUrl url = *it;
        loadFile(url.toLocalFile());
        ++it;
    }
}


void repo::gui::RepoGUI::oculus()
{

    RepoGLCWidget *activeSubWidget = ui->mdiArea->activeSubWidget<RepoGLCWidget*>();
    if (!activeSubWidget)
        std::cout << "A 3D window has to be open." << std::endl;
    else
    {
        RepoMdiSubWindow* textureSubwindow = ui->mdiArea->addOculusTextureSubWindow();
        RepoDialogOculus oculusDialog(activeSubWidget,
                                      textureSubwindow->widget<RepoOculusTextureRenderer*>(),
                                      this);
        oculusDialog.exec();

    }

}

void repo::gui::RepoGUI::open3DDiff()
{
    ui->mdiArea->closeHiddenSubWindows();
    if (ui->mdiArea->subWindowList().count() == 2)
    {
        RepoGLCWidget *widgetA = dynamic_cast<RepoGLCWidget*>(ui->mdiArea->subWindowList().at(0)->widget());
        RepoGLCWidget *widgetB = dynamic_cast<RepoGLCWidget*>(ui->mdiArea->subWindowList().at(1)->widget());

        if (widgetA && widgetB)
        {
            // TODO: make asynchronous as trees can be very large
            //            addSelectionTree(widgetA, Qt::LeftDockWidgetArea);
            //            addSelectionTree(widgetB, Qt::RightDockWidgetArea);

            ui->mdiArea->maximizeSubWindows();
            ui->actionLink->setChecked(true);
            ui->mdiArea->chainSubWindows(ui->actionLink->isChecked());

            Repo3DDiffRenderer(widgetA, widgetB);
        }
    }
    else
        std::cerr << "Exactly 2 windows have to be open." << std::endl;
}

void repo::gui::RepoGUI::openFile()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        tr("Select one or more files to open"),
        QString::null,
        repo::core::AssimpWrapper::getImportFormats().c_str());
    loadFiles(filePaths);
}

void repo::gui::RepoGUI::openMetadataManager()
{
    if (const RepoGLCWidget *widget = getActiveWidget())
    {
        QString filePath = QFileDialog::getOpenFileName(
            this,
            tr("Select one or more files to open"),
            QString::null,
            "*.csv");

        core::RepoCSV repoCSV;
        core::RepoNodeAbstractSet metadata = repoCSV.readMetadata(filePath.toStdString());
        widget->getRepoScene()->addMetadata(metadata,false);
    }
}

void repo::gui::RepoGUI::optimizeGraph()
{
    if (const RepoGLCWidget *widget = getActiveWidget())
    {
        core::RepoGraphScene* scene = widget->getRepoScene();
        core::RepoGraphOptimizer optimizer(scene);

        optimizer.collapseZeroMeshTransformations();

        optimizer.collapseSingleMeshTransformations();
    }
}

void repo::gui::RepoGUI::openSettings() const
{
    RepoDialogSettings settingsDialog((QWidget*) this);
    settingsDialog.exec();
}

void repo::gui::RepoGUI::openSupportEmail() const
{
    QString email = "support@3drepo.org";
    QString subject = "GUI Support Request";
    QDesktopServices::openUrl(
                QUrl("mailto:support@3drepo.org" + email +
                     "?subject=" + subject +
                     "&body=" + RepoDialogAbout::getVersionInfo()));
}

void repo::gui::RepoGUI::openUserManager() const
{
//    core::MongoClientWrapper mongo = ui->widgetRepository->getSelectedConnection();
    RepoDialogUserManager um(ui->widgetRepository, (QWidget*) this);
    um.exec();
}

void repo::gui::RepoGUI::openProjectManager() const
{
    RepoProjectManagerDialog pm(ui->widgetRepository, (QWidget*) this);
    pm.exec();
}

void repo::gui::RepoGUI::refresh()
{
    ui->widgetRepository->refresh();
}

void repo::gui::RepoGUI::reportIssue() const
{
    QDesktopServices::openUrl(QUrl("https://github.com/3drepo/3drepogui/issues"));
}

void repo::gui::RepoGUI::saveAs()
{
    if (const RepoGLCWidget *widget = getActiveWidget())
    {
        // TODO: create export worker
        QString path = QFileDialog::getSaveFileName(
            this,
            tr("Select a file to save"),
            QString(QDir::separator()) + widget->windowTitle(),
            tr(core::AssimpWrapper::getExportFormats().c_str()));
        QFileInfo fileInfo(path);
        QDir directory = fileInfo.absoluteDir();
        QString fileExtension = fileInfo.completeSuffix();
        const core::RepoGraphScene *repoScene = widget->getRepoScene();

        std::cout << "Exporting to " << path.toStdString() << std::endl;

        string embeddedTextureExtension = ".jpg";
        aiScene *scene = new aiScene();
        scene->mFlags = 0; //getPostProcessingFlags(); // TODO FIX ME!
        repoScene->toAssimp(scene);
        core::AssimpWrapper exporter;

        bool successful = exporter.exportModel(scene,
            core::AssimpWrapper::getExportFormatID(fileExtension.toStdString()),
            path.toStdString(),
            embeddedTextureExtension);

        if (!successful)
           std::cerr << "Export unsuccessful." << std::endl;
        else
        {
           std::vector<core::RepoNodeTexture *> textures = repoScene->getTextures();
           for (size_t i = 0; i < textures.size(); ++i)
           {
               core::RepoNodeTexture *repoTex = textures[i];
                const unsigned char *data = (unsigned char*) repoTex->getRawData();
                QImage image = QImage::fromData(data, repoTex->getRawDataSize());
                QString filename = QString::fromStdString(repoTex->getName());

                if (scene->HasTextures())
                {
                    string name = repoTex->getName();
                    name = name.substr(1, name.size()); // remove leading '*' char
                    filename = QString::fromStdString(name + embeddedTextureExtension);
                }
                QFileInfo fi(directory,filename);
                image.save(fi.absoluteFilePath());
            }
            /* TODO: textures
            const map<string, QImage> textures = widget->getTextures();
            for (map<string, QImage>::const_iterator it = textures.begin(); it != textures.end(); it++)
            {
                QString filename(((*it).first).c_str());

                // if embedded textures
                if (scene->HasTextures())
                {
                    string name = (*it).first;
                    name = name.substr(1, name.size()); // remove leading '*' char
                    filename = QString((name + embeddedTextureExtension).c_str());
                }
                QFileInfo fi(directory,filename);
                QImage image = (*it).second;
                image.save(fi.absoluteFilePath());
            }*/
            std::cout << "Export successful." << std::endl;
        }
        delete scene;
    }
}

void repo::gui::RepoGUI::saveScreenshot()
{
    const repo::gui::RepoGLCWidget * widget = getActiveWidget();
    std::vector<repo::gui::RepoGLCWidget *> widgets = ui->mdiArea->getWidgets<repo::gui::RepoGLCWidget*>();

    if (widgets.size() == 0)
    {
        std::cerr << tr("A window has to be open.").toStdString() << std::endl;
    }
    else
    {
        QString path = QFileDialog::getSaveFileName(
                    this,
                    tr("Choose a file to save"),
                    QString(QDir::separator()) + widget->windowTitle(),
                    tr("Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm)"));

        QFileInfo fileInfo(path);
        for (int i = 0; i < widgets.size(); ++i)
        {
            repo::gui::RepoGLCWidget *widget = widgets[i];
            if (widget)
            {
                QString newPath = fileInfo.absolutePath() + QDir::separator() + fileInfo.baseName() + QString::number(i);
                if (!fileInfo.completeSuffix().isEmpty())
                    newPath += "." + fileInfo.completeSuffix();

                std::cout << tr("Exporting image to ").toStdString() + newPath.toStdString() << std::endl;
                // See https://bugreports.qt-project.org/browse/QTBUG-33186
                //QPixmap pixmap = widget->renderPixmap(3840, 2160);
                QImage image = widget->renderQImage(13440,7560); // HD x 7 res
                if (!image.save(newPath, 0, 100)) // 100% quality
                    std::cerr << tr("Export failed.").toStdString() << std::endl;
            }
        }
    }
}

void repo::gui::RepoGUI::showCollectionContextMenuSlot(const QPoint &pos)
{
    QMenu menu(ui->widgetRepository->getCollectionTreeView());
    QAction* a = menu.addAction(
                tr("Copy"),
                ui->widgetRepository,
                SLOT(copySelectedCollectionCellToClipboard()));
    a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    menu.addAction(
                tr("Expand all"),
                ui->widgetRepository,
                SLOT(expandAllCollectionRecords()));
    menu.addSeparator();

    a = menu.addAction(tr("Delete record"), this, SLOT(deleteRecordSlot()));
    a->setEnabled(false);
    a = menu.addAction(tr("Delete all records"), this, SLOT(deleteAllRecordsSlot()));
    a->setEnabled(false);

    menu.exec(ui->widgetRepository->mapToGlobalCollectionTreeView(pos));
}

void repo::gui::RepoGUI::showDatabaseContextMenu(const QPoint &pos)
{
    ui->menuRepository->exec(ui->widgetRepository->mapToGlobalDatabasesTreeView(pos));
}

void repo::gui::RepoGUI::startup()
{
    RepoDialogConnect connectionDialog(this);
    if (connectionDialog.isShowAtStartup())
        connect();
}

void repo::gui::RepoGUI::toggleFullScreen()
{
    if (ui->actionFull_Screen->isChecked())
    {
        ui->menuBar->hide();
        ui->dockWidgetRepositories->hide();
        ui->dockWidgetLog->hide();
        ui->repositoriesToolBar->hide();
        ui->openGLToolBar->hide();
        showFullScreen();
    }
    else
    {
        ui->menuBar->show();
        ui->dockWidgetRepositories->show();
        ui->dockWidgetLog->show();
        ui->repositoriesToolBar->show();
        ui->openGLToolBar->show();
        showNormal();
    }
}

//------------------------------------------------------------------------------
//
// Protected
//
//------------------------------------------------------------------------------

void repo::gui::RepoGUI::closeEvent(QCloseEvent *event)
{
    storeSettings();
    QMainWindow::closeEvent(event);
}

void repo::gui::RepoGUI::dragEnterEvent(QDragEnterEvent *event)
{
    // accept only drag events that provide urls (paths) to files/resources
   if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void repo::gui::RepoGUI::dropEvent(QDropEvent *event)
{
    // a list of full paths to files dragged onto the window
    loadFiles(event->mimeData()->urls());
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    event->acceptProposedAction();
}

void repo::gui::RepoGUI::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_F11:
            ui->actionFull_Screen->setChecked(!ui->actionFull_Screen->isChecked());
            toggleFullScreen();
            break;
        case Qt::Key_Escape:
            ui->actionFull_Screen->setChecked(false);
            toggleFullScreen();
            break;
    }
    QMainWindow::keyPressEvent(event);
}

void repo::gui::RepoGUI::restoreSettings()
{
    QSettings settings;
    restoreGeometry(settings.value(REPO_SETTINGS_GUI_GEOMETRY).toByteArray());
    restoreState(settings.value(REPO_SETTINGS_GUI_STATE).toByteArray());
    ui->actionLink->setChecked(settings.value(REPO_SETTINGS_LINK_WINDOWS, false).toBool());
}

void repo::gui::RepoGUI::storeSettings()
{
    QSettings settings;
    settings.setValue(REPO_SETTINGS_GUI_GEOMETRY, saveGeometry());
    settings.setValue(REPO_SETTINGS_GUI_STATE, saveState());
    settings.setValue(REPO_SETTINGS_LINK_WINDOWS, ui->actionLink->isChecked());
}
