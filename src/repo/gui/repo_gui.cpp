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

#include <ctime>
//------------------------------------------------------------------------------
// GUI
#include "repo_gui.h"
#include "ui_repo_gui.h"
#include "dialogs/repo_dialog_commit.h"
#include "dialogs/repo_dialog_history.h"
#include "dialogs/repo_dialog_settings.h"
#include "dialogs/repo_dialog_about.h"
#include "../logger/repo_logger.h"
#include "dialogs/repo_dialog_connect.h"
#include "widgets/repo_widget_repository.h"
#include "widgets/repo_text_browser.h"
#include "widgets/repo_widget_flags.h"
#include "widgets/repo_widget_tree_dock.h"
#include "widgets/repo_widget_manager_3ddiff.h"
#include "widgets/repo_widget_manager_clipping_plane.h"
#include "../workers/repo_worker_commit.h"
#include "../workers/repo_worker_file_export.h"
#include "../workers/repo_worker_optimize.h"
#include "primitives/repo_fontawesome.h"
#include "primitives/repo_color.h"
#include "dialogs/repo_dialog_manager_abstract.h"
#include "dialogs/repo_dialog_federation.h"


//------------------------------------------------------------------------------
#include "dialogs/repo_dialog_manager_access.h"
#include "dialogs/repo_dialog_manager_connect.h"
//------------------------------------------------------------------------------

using namespace repo::gui::widget;

const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_GEOMETRY    = "RepoGUI/geometry";
const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_STATE       = "RepoGUI/state";
const QString repo::gui::RepoGUI::REPO_SETTINGS_LINK_WINDOWS    = "RepoGUI/link";
const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_FLY_NAVIGATION = "RepoGUI/fly";
const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_HELICOPTER_NAVIGATION = "RepoGUI/helicopter";
const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_TRACKBALL_NAVIGATION = "RepoGUI/trackball";
const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_TURNTABLE_NAVIGATION = "RepoGUI/turntable";


static std::string getTimeAsString()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,80,"%d-%m-%Y_%Ih%Mm%S",timeinfo);
  return std::string(buffer);

}

static std::string getLogFileName()
{

    std::string fileName = getTimeAsString()+".log";
    auto fullPath = QDir::cleanPath(QDir::homePath() + QDir::separator() +
                                    QString("3drepo") +
                                    QDir::separator() +
                                    QString("gui") +
                                    QDir::separator() +
                                    QString(fileName.c_str()));


    return fullPath.toStdString();
}


repo::gui::RepoGUI::RepoGUI(
        repo::RepoController *controller,
        QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RepoGUI)
    , controller(controller)
    , navigationModeActionGroup(new QActionGroup(this))
{
    ui->setupUi(this);
    restoreSettings();

    this->setWindowTitle(
                QCoreApplication::applicationName() +
                " v" +
                QCoreApplication::applicationVersion());

    //Subscribe logger to broadcaster who taps into repo bouncer library logs
    repo::logger::RepoLogger::getInstance()->subscribe(ui->logTextBrowser);
    controller->logToFile(getLogFileName());

    QIcon icon(":/images/3drepo-icon.svg");
    this->setWindowIcon(icon);

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
    ui->actionOpen->setIcon(primitive::RepoFontAwesome::getInstance().getIcon(primitive::RepoFontAwesome::fa_folder_open));

    // Save As
    QObject::connect(ui->actionSave_As, SIGNAL(triggered()), this,
                     SLOT(saveAs()));
    ui->actionSave_As->setIcon(primitive::RepoFontAwesome::getInstance().getIcon(primitive::RepoFontAwesome::fa_floppy_o));

    // Save Screenshot
    QObject::connect(ui->actionSave_Screenshot, SIGNAL(triggered()), this,
                     SLOT(saveScreenshot()));


    QObject::connect(ui->actionClose, &QAction::triggered,
                     ui->mdiArea, &widget::RepoMdiArea::closeActiveSubWindow);

    QObject::connect(ui->actionClose_All, &QAction::triggered,
                     ui->mdiArea, &widget::RepoMdiArea::closeAllSubWindows);

    QObject::connect(ui->actionClose_All, &QAction::triggered,
                     ui->mdiArea, &widget::RepoMdiArea::closeHiddenSubWindows);

    //--------------------------------------------------------------------------
    // Exit
    QObject::connect(ui->actionExit, SIGNAL(triggered()),
                     QApplication::instance(), SLOT(quit()));
    ui->actionExit->setIcon(
                primitive::RepoFontAwesome::getInstance().getIcon(
                    primitive::RepoFontAwesome::fa_times, QColor(Qt::darkRed)));


    //--------------------------------------------------------------------------
    //
    // Repository
    //
    //--------------------------------------------------------------------------
    // Connect
    QObject::connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connectDB()));
    ui->actionConnect->setIcon(primitive::RepoFontAwesome::getConnectIcon());

    // Disconnect
    QObject::connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectDB()));
    ui->actionDisconnect->setIcon(primitive::RepoFontAwesome::getDisconnectIcon());

    // Refresh
    QObject::connect(ui->actionRefresh, SIGNAL(triggered()), this, SLOT(refresh()));
    ui->actionRefresh->setIcon(
                primitive::RepoFontAwesome::getInstance().getIcon(
                    primitive::RepoFontAwesome::fa_refresh,
                    QColor(Qt::darkGreen)));

    // Head
    QObject::connect(ui->actionHead, SIGNAL(triggered()), this, SLOT(fetchHead()));
    ui->actionHead->setIcon(
                primitive::RepoFontAwesome::getInstance().getIcon(
                    primitive::RepoFontAwesome::fa_download));

    // History
    QObject::connect(ui->actionHistory, SIGNAL(triggered()), this, SLOT(history()));
    ui->actionHistory->setIcon(primitive::RepoFontAwesome::getHistoryIcon());


    // Commit
    QObject::connect(ui->actionCommit, SIGNAL(triggered()), this, SLOT(commit()));
    ui->actionCommit->setIcon(primitive::RepoFontAwesome::getCommitIcon());

    // Federate...
    QObject::connect(ui->actionFederate, SIGNAL(triggered()),
                     this, SLOT(federate()));

    // Access Manager
    QObject::connect(ui->actionAccessManager, SIGNAL(triggered()),
                     this, SLOT(openAccessManager()));
     ui->actionAccessManager->setIcon(primitive::RepoFontAwesome::getAccessManagerIcon());

    //--------------------------------------------------------------------------
    // Drop
    QObject::connect(ui->actionRemoveProject, SIGNAL(triggered()), this, SLOT(removeProject()));
    ui->actionRemoveProject->setIcon(primitive::RepoFontAwesome::getInstance().getIcon(primitive::RepoFontAwesome::fa_trash_o));
    QObject::connect(ui->actionDrop, SIGNAL(triggered()), this, SLOT(drop()));
//    ui->actionDrop->setIcon(primitive::RepoFontAwesome::getInstance().getIcon(primitive::RepoFontAwesome::fa_trash_o));


    //--------------------------------------------------------------------------
    //
    // Rendering
    //
    //--------------------------------------------------------------------------
    // Link
    QObject::connect(ui->actionLink, SIGNAL(triggered(bool)), ui->mdiArea, SLOT(chainSubWindows(bool)));
    ui->actionLink->setIcon(
                primitive::RepoFontAwesome::getInstance().getIcon(
                    primitive::RepoFontAwesome::fa_link,
                    primitive::RepoFontAwesome::fa_chain_broken));



    //--------------------------------------------------------------------------
    // View settings
    ui->openGLToolBar->addAction(ui->menuNavigation->menuAction());
    QObject::connect(ui->menuNavigation->menuAction(), &QAction::triggered, this, &RepoGUI::toggleNavigationMode);

    navigationModeActionGroup->addAction(ui->actionTurntable);
    QObject::connect(ui->actionTurntable, &QAction::triggered, this, &RepoGUI::toggleNavigationMode);
    ui->actionTurntable->setIcon(primitive::RepoFontAwesome::getTurntableIcon());
    navigationModeActionGroup->addAction(ui->actionTrack_Ball);
    QObject::connect(ui->actionTrack_Ball, &QAction::triggered, this, &RepoGUI::toggleNavigationMode);
    ui->actionTrack_Ball->setIcon(primitive::RepoFontAwesome::getTrackBallIcon());
    navigationModeActionGroup->addAction(ui->actionFly);
    QObject::connect(ui->actionFly, &QAction::triggered, this, &RepoGUI::toggleNavigationMode);    
    ui->actionFly->setIcon(primitive::RepoFontAwesome::getFlyIcon());
    navigationModeActionGroup->addAction(ui->actionHelicopter);
    QObject::connect(ui->actionHelicopter, &QAction::triggered, this, &RepoGUI::toggleNavigationMode);
    ui->actionHelicopter->setIcon(primitive::RepoFontAwesome::getHelicopterIcon());

    if (ui->actionTurntable->isChecked())
        ui->actionTurntable->trigger();
    else if (ui->actionTrack_Ball->isChecked())
        ui->actionTrack_Ball->trigger();
    else if (ui->actionFly->isChecked())
        ui->actionFly->trigger();
    else if (ui->actionHelicopter->isChecked())
        ui->actionHelicopter->trigger();

    //	connect(actionISO, SIGNAL(triggered()), this, SLOT(cameraISOSlot()));
    //	actionISO->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_dot_circle_o));
    //	setMenuCamera(actionISO);

    //	connect(actionTop, SIGNAL(triggered()), this, SLOT(cameraTopSlot()));
    //	actionTop->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_chevron_circle_up));
    //	connect(actionBottom, SIGNAL(triggered()), this, SLOT(cameraBottomSlot()));
    //	actionBottom->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_chevron_circle_down));
    //	connect(actionLeft, SIGNAL(triggered()), this, SLOT(cameraLeftSlot()));
    //	actionLeft->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_chevron_circle_left));
    //	connect(actionRight, SIGNAL(triggered()), this, SLOT(cameraRightSlot()));
    //	actionRight->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_chevron_circle_right));
    //	connect(actionFront, SIGNAL(triggered()), this, SLOT(cameraFrontSlot()));
    //	actionFront->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_circle_o));
    //	connect(actionBack, SIGNAL(triggered()), this, SLOT(cameraBackSlot()));
    //	actionBack->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_circle));


    // Clipping Plane
    ui->clippingPlaneWidget->setMdiArea(ui->mdiArea);
    ui->clippingPlaneWidget->setLinkAction(ui->actionLink);

    QObject::connect(ui->clippingPlaneDockWidget, &QDockWidget::visibilityChanged,
                     ui->clippingPlaneWidget, &RepoClippingPlaneWidget::setClippingPlaneEnabled);





    // Scene Graph
    QObject::connect(ui->actionSceneGraph, SIGNAL(triggered()),
                     ui->mdiArea, SLOT(addSceneGraphSubWindow()));
    ui->actionSceneGraph->setIcon(primitive::RepoFontAwesome::getSceneGraphIcon());


    //--------------------------------------------------------------------------
    //
    // Repos
    //
    //--------------------------------------------------------------------------
    repos = new models::RepositoriesModel(controller, ui->reposWidget);


    //--------------------------------------------------------------------------
    //
    // Tools
    //
    //--------------------------------------------------------------------------

    // Metadata Management...
    QObject::connect(ui->actionMetadataManager, SIGNAL(triggered()), this, SLOT(openMetadataManager()));

    // Optimize Scene Graph
    QObject::connect(ui->actionOptimize_Graph, SIGNAL(triggered()), this, SLOT(optimizeGraph()));
    ui->actionOptimize_Graph->setIcon(primitive::RepoFontAwesome::getSceneGraphOptimizeIcon());






    // 3D Diff...
    ui->diffManagerWidget->initialize(ui->mdiArea,
                                      this->controller,
                                      repos->getSelectedConnection());//ui->widgetRepository->getSelectedConnection());

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
                primitive::RepoFontAwesome::getInstance().getIcon(
                    primitive::RepoFontAwesome::fa_arrows_alt));

    // Maximize (Tile)
    QObject::connect(ui->actionMaximize, SIGNAL(triggered()),
                     ui->mdiArea, SLOT(maximizeSubWindows()));
    ui->actionMaximize->setIcon(
                primitive::RepoFontAwesome::getInstance().getIcon(
                    primitive::RepoFontAwesome::fa_th));


    // Selection Tree (Scene Graph)
    QObject::connect(ui->actionSelection_Tree, SIGNAL(triggered()),
                     this, SLOT(addSelectionTree()));

    // Panels
    createPanelsMenu();



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
                primitive::RepoFontAwesome::getInstance().getIcon(
                    primitive::RepoFontAwesome::fa_envelope_o));

    //--------------------------------------------------------------------------
    // Report Issue
    QObject::connect(ui->actionReport_Issue, SIGNAL(triggered()),
                     this, SLOT(reportIssue()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()),
                     this, SLOT(about()));

    ui->actionAbout->setIcon(primitive::RepoFontAwesome::getAboutIcon());




    //--------------------------------------------------------------------------
    // Context menus
    QObject::connect(
                ui->reposWidget->getTreeView(), //ui->widgetRepository->getDatabasesTreeView(),
                &QWidget::customContextMenuRequested,
                this,
                &RepoGUI::showDatabaseContextMenu);

//    QObject::connect(
//                ui->widgetRepository->getCollectionTreeView(),
//                &QTreeView::customContextMenuRequested,
//                this,
//                &RepoGUI::showCollectionContextMenuSlot);




    //--------------------------------------------------------------------------
    //
    // Restore previous GUI user settings
    //
    //--------------------------------------------------------------------------


    if (controller->getVersion() != EXPECTED_BOUNCER_VERSION)
    {
        repoLogError("Repo GUI expects bouncer library version " + std::string(EXPECTED_BOUNCER_VERSION) + " but it is running with " + controller->getVersion() + ". Unexpected behaviour may occur!");
    }
}

repo::gui::RepoGUI::~RepoGUI()
{
    delete repos;
    delete ui;
    delete navigationModeActionGroup;
}


//------------------------------------------------------------------------------
//
// Public Slots
//
//------------------------------------------------------------------------------

void repo::gui::RepoGUI::about()
{
    dialog::AboutDialog aboutDialog(this);
    aboutDialog.exec();
}


// TODO: fix me
void repo::gui::RepoGUI::addSelectionTree()
{
    addSelectionTree(ui->mdiArea->getActiveWidget());
}

// TODO: selection tree needs to be permanent dock, not dynamic one as done now!
// Speak to Jozef if not sure.
void repo::gui::RepoGUI::addSelectionTree(widget::Rendering3DWidget* widget, Qt::DockWidgetArea area)
{
    // This whole thing is wrong
    widget::TreeDockWidget* dock = new  widget::TreeDockWidget(widget, this);
    this->addDockWidget(area, dock);
    dock->show();
}

void repo::gui::RepoGUI::commit()
{
    widget::RepoMdiSubWindow *activeWindow = ui->mdiArea->activeSubWindow();
    const widget::Rendering3DWidget *widget = ui->mdiArea->getActiveWidget();

    QString database = repos->getSelectedDatabase();//ui->widgetRepository->getSelectedDatabase();
    QString project = repos->getSelectedProject();//ui->widgetRepository->getSelectedProject();
    
    repo::core::model::RepoScene *repoScene = nullptr;
    if (activeWindow && widget)
    {
        repoScene = widget->getRepoScene();
        
        // TODO: fix !!!
        if (project.isEmpty())
        {
            QFileInfo path(activeWindow->windowTitle());
            project = path.completeBaseName();
            repoScene->setDatabaseAndProjectName(database.toStdString(), project.toStdString());
        }
    }

    commit(repoScene, activeWindow);
}

void repo::gui::RepoGUI::commit(
        repo::core::model::RepoScene *scene,
        widget::RepoMdiSubWindow *activeWindow)
{	
    if (scene)
    {
        dialog::CommitDialog commitDialog(
                    this,
                    Qt::Window,
                    repos,
//                    ui->widgetRepository,
                    scene);



        if (!commitDialog.exec())
            repoLog("Commit dialog cancelled by user");
        else // Clicked "OK"
        {
            //----------------------------------------------------------------------
            // Establish and connect the new worker.
            repo::worker::CommitWorker *worker = new repo::worker::CommitWorker(
                        controller,
//                        ui->widgetRepository->getConnection(commitDialog.getCurrentHost()), scene);
                        repos->getConnection(commitDialog.getCurrentHost()), scene);

            if (activeWindow)
                QObject::connect(worker, SIGNAL(progress(int, int)), activeWindow, SLOT(progress(int, int)));

            QObject::connect(worker, SIGNAL(finished()), this, SLOT(refresh()));
            //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));

            //----------------------------------------------------------------------
            // Fire up the asynchronous calculation.
            QThreadPool::globalInstance()->start(worker);
        }
    }
}


void repo::gui::RepoGUI::connectDB()
{
    // Disconnect previous connection if any
    // TODO: remove when expanding to multiple connections
//    ui->widgetRepository->disconnectDB();

    dialog::ConnectManagerDialog connectManager(controller, (QWidget*)this);

    if(! connectManager.exec()) // if not clicked "Connect"
        repoLog("Connection Manager Dialog cancelled by user");
    else
    {
        // if not successfully connected
        std::string errMsg;

        auto credentials = connectManager.getConnection();
        repo::RepoController::RepoToken *token = controller->createTokenFromSerialised(credentials);

        if (token && controller->authenticateMongo(errMsg, token))
        {
            //connection/authentication success
            ui->widgetRepository->fetchDatabases(controller, token);

            //-----------------------------------------------------------------
            // enable buttons
            ui->actionRefresh->setEnabled(true);
            ui->actionHead->setEnabled(true);
            ui->actionHistory->setEnabled(true);
            ui->actionCommit->setEnabled(true);
            ui->actionDrop->setEnabled(true);
            ui->actionDisconnect->setEnabled(true);
            ui->actionAccessManager->setEnabled(true);
            ui->actionFederate->setEnabled(true);
            ui->actionRemoveProject->setEnabled(true);


            repos->connect(token);
        }
        else
        {
            //connection/authentication failed
            repoLogError("Failed to connect/authenticate user: " + errMsg);
        }
    }
}

QMenu* repo::gui::RepoGUI::createPanelsMenu()
{
    int dockWidgetsCount = this->findChildren<QDockWidget *>().size();
    QMenu* panelsMenu = QMainWindow::createPopupMenu();
    int i = 0;
    for (auto a : panelsMenu->actions())
    {
        ui->menuWindow->addAction(a);        
        if (i < dockWidgetsCount)
        {
            ui->panelsToolBar->addAction(a);
        }
        ++i;
    }
    if (panelsMenu->actions().size() >= dockWidgetsCount)
    {
        panelsMenu->actions()[0]->setShortcut(QKeySequence(Qt::AltModifier + Qt::Key_R));
        panelsMenu->actions()[0]->setIcon(primitive::RepoFontAwesome::getRepositoriesIcon());

        panelsMenu->actions()[1]->setShortcut(QKeySequence(Qt::AltModifier + Qt::Key_L));
        panelsMenu->actions()[1]->setIcon(primitive::RepoFontAwesome::getLogIcon());

        panelsMenu->actions()[2]->setShortcut(QKeySequence(Qt::AltModifier + Qt::Key_C));
        panelsMenu->actions()[2]->setIcon(primitive::RepoFontAwesome::getClippingPlaneIcon());

        panelsMenu->actions()[3]->setShortcut(QKeySequence(Qt::AltModifier + Qt::Key_D));
        panelsMenu->actions()[3]->setIcon(primitive::RepoFontAwesome::get3DDiffIcon());
    }

    return panelsMenu;
}

void repo::gui::RepoGUI::disconnectDB()
{

    // TODO: disconnect selected DB or potentially all of them

    if (repos->disconnect()) //ui->widgetRepository->disconnectDB())
    {
        // disable buttons
        ui->actionRefresh->setEnabled(false);
        ui->actionHead->setEnabled(false);
        ui->actionHistory->setEnabled(false);
        ui->actionCommit->setEnabled(false);
        ui->actionDrop->setEnabled(false);
        ui->actionDisconnect->setEnabled(false);
        ui->actionAccessManager->setEnabled(false);
        ui->actionFederate->setEnabled(false);
        ui->actionRemoveProject->setEnabled(false);
    }
}

void repo::gui::RepoGUI::drop()
{
    QString database = repos->getSelectedDatabase(); //ui->widgetRepository->getSelectedDatabase();
    QString collection = repos->getSelectedProject(); //ui->widgetRepository->getSelectedCollection();


    if (database.isNull() || database.isEmpty())
        repoLog("A database must be selected.");
    else if ((database == "local" || database == "admin") && collection.isEmpty())
        repoLog("You are not allowed to delete 'local' or 'admin' databases.");
    else
    {
        QString ns = database + (!collection.isEmpty() ? "." + collection : "");
        if (!QMessageBox::warning(this,
                                  "Drop?",
                                  "Are you sure you want to drop '" + ns + "'?",
                                  "&Yes",
                                  "&No",
                                  QString::null, 1, 1))
        {

            bool success;
            std::string errMsg;
            const repo::RepoController::RepoToken *token = repos->getSelectedConnection(); //ui->widgetRepository->getSelectedConnection();
            // TODO: create a DB manager separate from repositories widget.
            if (collection.isEmpty())
                success = controller->removeDatabase(token, database.toStdString(), errMsg);
            else
                success = controller->removeCollection(token, database.toStdString(), collection.toStdString(), errMsg);

            refresh();
        }
    }
}

void repo::gui::RepoGUI::federate()
{
    dialog::FederationDialog fed(repos, this); //ui->widgetRepository, this);
    if (fed.exec())
    {
        repo::core::model::RepoScene *scene = controller->createFederatedScene(fed.getFederation());
        commit(scene);
    }
}

void repo::gui::RepoGUI::fetchHead()
{
    // Head revision from master branch
    ui->mdiArea->addSubWindow(
                controller,
                repos->getSelectedConnection(), //ui->widgetRepository->getSelectedConnection(),
                repos->getSelectedDatabase(), //ui->widgetRepository->getSelectedDatabase(),
                repos->getSelectedProject()); //ui->widgetRepository->getSelectedProject());

    // Make sure to hook controls if chain is on
    ui->mdiArea->chainSubWindows(ui->actionLink->isChecked());
}




void repo::gui::RepoGUI::history()
{
    QString database = repos->getSelectedDatabase(); //ui->widgetRepository->getSelectedDatabase();
    QString project  = repos->getSelectedProject(); //ui->widgetRepository->getSelectedProject();
    repo::RepoController::RepoToken *token = repos->getSelectedConnection(); //ui->widgetRepository->getSelectedConnection();
    dialog::HistoryDialog historyDialog(controller, token, database, project, this);

    if(!historyDialog.exec()) // if not OK
        repoLog("Revision History dialog cancelled by user.");
    else
    {
        QList<QUuid> revisions = historyDialog.getSelectedRevisions();
        for (QList<QUuid>::iterator uid = revisions.begin();
             uid != revisions.end();
             ++uid)
            ui->mdiArea->addSubWindow(controller, token, database, project, *uid, false);
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
        std::string fileName = pathInfo.fileName().toStdString();
        repoLog("Loading " + fileName + " ...");

        ui->mdiArea->addSubWindow(controller, filePath);
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

void repo::gui::RepoGUI::openAccessManager()
{
    dialog::AccessManagerDialog accessManager(
                //ui->widgetRepository,
                repos,
                controller,
                (QWidget*) this);
    accessManager.exec();
}

void repo::gui::RepoGUI::openFile()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
                this,
                tr("Select one or more files to open"),
                QString::null,
                QString(controller->getSupportedImportFormats().c_str()));
    loadFiles(filePaths);
}

void repo::gui::RepoGUI::openMetadataManager()
{
    if (const widget::Rendering3DWidget *widget = ui->mdiArea->getActiveWidget())
    {
        QString filePath = QFileDialog::getOpenFileName(
                    this,
                    tr("Select one or more files to open"),
                    QString::null,
                    "*.csv");

        repo::core::model::RepoScene *scene = widget->getRepoScene();
        repo::core::model::RepoNodeSet nodeSet = controller->loadMetadataFromFile(filePath.toStdString());

        scene->addMetadata(nodeSet, false);

        commit(scene);

    }
}

void repo::gui::RepoGUI::optimizeGraph()
{
    if (const widget::Rendering3DWidget *widget = ui->mdiArea->getActiveWidget())
    {
        widget::RepoMdiSubWindow *activeWindow = ui->mdiArea->activeSubWindow();
        repo::core::model::RepoScene* scene = widget->getRepoScene();
        repo::worker::OptimizeWorker *worker =
                new repo::worker::OptimizeWorker(controller,
                                                 repos->getSelectedConnection(), //ui->widgetRepository->getSelectedConnection(),
                                                 scene);

        if (activeWindow)
        {
            QObject::connect(worker, SIGNAL(progress(int, int)), activeWindow, SLOT(progress(int, int)));
        }


        //----------------------------------------------------------------------
        // Fire up the asynchronous calculation.
        QThreadPool::globalInstance()->start(worker);
    }
}

void repo::gui::RepoGUI::openSettings() const
{
    dialog::SettingsDialog settingsDialog((QWidget*) this);
    settingsDialog.exec();
}

void repo::gui::RepoGUI::openSupportEmail() const
{
    QString email = "support@3drepo.org";
    QString subject = "GUI Support Request";
    QDesktopServices::openUrl(
                QUrl("mailto:support@3drepo.org" + email +
                     "?subject=" + subject +
                     "&body=" + dialog::AboutDialog::getVersionInfo()));
}

void repo::gui::RepoGUI::refresh()
{
//    ui->widgetRepository->refresh();
    repos->refresh();
}

void repo::gui::RepoGUI::removeProject()
{
    QString database = repos->getSelectedDatabase();//ui->widgetRepository->getSelectedDatabase();
    QString project = repos->getSelectedProject(); //ui->widgetRepository->getSelectedProject();


    if (database.isNull() || database.isEmpty())
        repoLog("A database must be selected.");
    else if (database == "admin")
        repoLog("You are not allowed to delete projects from 'admin' databases.");
    else if (project.isNull() || project.isEmpty())
        repoLog("A project must be selected.");
    else
    {
        QString ns = database +  "." + project;
        if (!QMessageBox::warning(this,
                                  "Drop?",
                                  "Are you sure you want to remove '" + ns + "'?",
                                  "&Yes",
                                  "&No",
                                  QString::null, 1, 1))
        {

            bool success;
            std::string errMsg;
            const repo::RepoController::RepoToken *token = repos->getSelectedConnection();//ui->widgetRepository->getSelectedConnection();
            // TODO: create a DB manager separate from repositories widget.
            success = controller->removeProject(token, database.toStdString(), project.toStdString(), errMsg);
            refresh();
        }
    }
}


void repo::gui::RepoGUI::reportIssue() const
{
    QDesktopServices::openUrl(QUrl("https://github.com/3drepo/3drepogui/issues"));
}

void repo::gui::RepoGUI::saveAs()
{
    if (const widget::Rendering3DWidget *widget = ui->mdiArea->getActiveWidget())
    {
        // TODO: create export worker
        QString path = QFileDialog::getSaveFileName(
                    this,
                    tr("Select a file to save"),
                    QString(QDir::separator()) + widget->windowTitle(),
                    tr(controller->getSupportedExportFormats().c_str()));
        const repo::core::model::RepoScene *repoScene = widget->getRepoScene();

        //Instantiate worker
        repo::worker::FileExportWorker *worker = new repo::worker::FileExportWorker(
                    path.toStdString(),
                    controller,
                    repoScene);

        QObject::connect(worker, SIGNAL(progress(int, int)), widget, SLOT(progress(int, int)));

        QObject::connect(worker, SIGNAL(finished()), this, SLOT(refresh()));

        //----------------------------------------------------------------------
        // Fire up the asynchronous calculation.
        QThreadPool::globalInstance()->start(worker);

    }
    else
    {
        repoLog("Failed to export model: No active window found.");
    }
}

void repo::gui::RepoGUI::saveScreenshot()
{
    const repo::gui::widget::Rendering3DWidget * widget = ui->mdiArea->getActiveWidget();
    std::vector<repo::gui::widget::Rendering3DWidget *> widgets = ui->mdiArea->getWidgets<repo::gui::widget::Rendering3DWidget*>();

    if (widgets.size() == 0)
    {
        repoLogError(tr("A window has to be open.").toStdString());
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
            repo::gui::widget::Rendering3DWidget *widget = widgets[i];
            if (widget)
            {
                QString newPath = fileInfo.absolutePath() + QDir::separator() + fileInfo.baseName() + QString::number(i);
                if (!fileInfo.completeSuffix().isEmpty())
                    newPath += "." + fileInfo.completeSuffix();

                repoLog(tr("Exporting image to ").toStdString() + newPath.toStdString());
                // See https://bugreports.qt-project.org/browse/QTBUG-33186
                //QPixmap pixmap = widget->renderPixmap(3840, 2160);
                QImage image = widget->renderQImage(13440,7560); // HD x 7 res
                if (!image.save(newPath, 0, 100)) // 100% quality
                    repoLogError(tr("Export failed.").toStdString());
            }
        }
    }
}

void repo::gui::RepoGUI::toggleNavigationMode()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action != ui->menuNavigation->menuAction())
    {
        ui->menuNavigation->menuAction()->setText(action->text());
        ui->menuNavigation->menuAction()->setIcon(action->icon());
    }

    //--------------------------------------------------------------------------

    if (action == ui->actionTurntable ||
        ui->menuNavigation->menuAction()->text() == ui->actionTurntable->text())
    {
        ui->mdiArea->setNavigationMode(repo::gui::renderer::NavMode::TURNTABLE);
    }
    else if (action == ui->actionTrack_Ball ||
             ui->menuNavigation->menuAction()->text() == ui->actionTrack_Ball->text())
    {
        ui->mdiArea->setNavigationMode(repo::gui::renderer::NavMode::ORBIT);
    }
    else if (action == ui->actionFly ||
              ui->menuNavigation->menuAction()->text() == ui->actionFly->text())
    {
        ui->mdiArea->setNavigationMode(repo::gui::renderer::NavMode::FLY);
    }
    else if (action == ui->actionHelicopter ||
              ui->menuNavigation->menuAction()->text() == ui->actionHelicopter->text())
    {
        ui->mdiArea->setNavigationMode(repo::gui::renderer::NavMode::HELICOPTER);
    }

}

void repo::gui::RepoGUI::showCollectionContextMenuSlot(const QPoint &pos)
{
    QMenu menu(ui->reposWidget->getTreeView()); //ui->widgetRepository->getCollectionTreeView());
    QAction* a = menu.addAction(
                tr("Copy"),
                ui->reposWidget, //ui->widgetRepository,
                SLOT(copySelectedCollectionCellToClipboard()));
    a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    menu.addAction(
                tr("Expand all"),
                ui->reposWidget, //ui->widgetRepository,
                SLOT(expandAllCollectionRecords()));
    menu.addSeparator();

    a = menu.addAction(tr("Delete record"), this, SLOT(deleteRecordSlot()));
    a->setEnabled(false);
    a = menu.addAction(tr("Delete all records"), this, SLOT(deleteAllRecordsSlot()));
    a->setEnabled(false);

    menu.exec(ui->reposWidget->getTreeView()->mapToGlobal(pos)); //ui->widgetRepository->mapToGlobalCollectionTreeView(pos));
}

void repo::gui::RepoGUI::showDatabaseContextMenu(const QPoint &pos)
{
    ui->menuRepository->exec(ui->reposWidget->getTreeView()->mapToGlobal(pos)); //ui->widgetRepository->mapToGlobalDatabasesTreeView(pos));
}

void repo::gui::RepoGUI::startup()
{
    // TODO: add this functionality back in
    //    ConnectDialog connectionDialog(this);
    //    if (connectionDialog.isShowAtStartup())
    connectDB();
}

void repo::gui::RepoGUI::toggleFullScreen()
{
    if (ui->actionFull_Screen->isChecked())
    {
        storeSettings();
        ui->menuBar->hide();
        ui->dockWidgetRepositories->hide();
        ui->dockWidgetLog->hide();
        ui->clippingPlaneDockWidget->hide();
        ui->diffDockWidget->hide();
        ui->repositoriesToolBar->hide();
        ui->openGLToolBar->hide();
        ui->panelsToolBar->hide();
        showFullScreen();
        this->repaint();
    }
    else
    {
        ui->menuBar->show();
        showNormal();
        restoreSettings();
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
    ui->actionLink->setChecked(settings.value(REPO_SETTINGS_LINK_WINDOWS, true).toBool());
    ui->actionHelicopter->setChecked(settings.value(REPO_SETTINGS_GUI_HELICOPTER_NAVIGATION, false).toBool());
    ui->actionFly->setChecked(settings.value(REPO_SETTINGS_GUI_FLY_NAVIGATION, false).toBool());
    ui->actionTrack_Ball->setChecked(settings.value(REPO_SETTINGS_GUI_TRACKBALL_NAVIGATION, false).toBool());
    ui->actionTurntable->setChecked(settings.value(REPO_SETTINGS_GUI_TURNTABLE_NAVIGATION, true).toBool());
}

void repo::gui::RepoGUI::storeSettings()
{
    QSettings settings;
    settings.setValue(REPO_SETTINGS_GUI_GEOMETRY, saveGeometry());
    settings.setValue(REPO_SETTINGS_GUI_STATE, saveState());
    settings.setValue(REPO_SETTINGS_LINK_WINDOWS, ui->actionLink->isChecked());
    settings.setValue(REPO_SETTINGS_GUI_HELICOPTER_NAVIGATION, ui->actionHelicopter->isChecked());
    settings.setValue(REPO_SETTINGS_GUI_FLY_NAVIGATION, ui->actionFly->isChecked());
    settings.setValue(REPO_SETTINGS_GUI_TRACKBALL_NAVIGATION, ui->actionTrack_Ball->isChecked());
    settings.setValue(REPO_SETTINGS_GUI_TURNTABLE_NAVIGATION, ui->actionTurntable->isChecked());
}
