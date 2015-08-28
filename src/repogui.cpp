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
// GUI
#include "repogui.h"
#include "ui_repogui.h"
#include "dialogs/repo_dialogcommit.h"
#include "dialogs/repo_dialogconnect.h"
#include "dialogs/repo_dialoghistory.h"
#include "dialogs/repodialogusermanager.h"
#include "dialogs/repodialogsettings.h"
#include "dialogs/repodialogabout.h"
#include "dialogs/repoprojectmanagerdialog.h"
#include "repo/logger/repo_logger.h"
#include "widgets/repo_widgetrepository.h"
#include "widgets/repo_textbrowser.h"
#include "widgets/repowidgetassimpflags.h"
#include "widgets/reposelectiontreedockwidget.h"
#include "repo/workers/repo_worker_commit.h"
#include "repo/workers/repo_worker_file_export.h"
#include "primitives/repo_fontawesome.h"
#include "primitives/repo_color.h"
#include "dialogs/repoabstractmanagerdialog.h"
#include "dialogs/repofederationdialog.h"
#include "dialogs/repo_maptilesdialog.h"
#include "renderers/repo_3ddiffrenderer.h"

//------------------------------------------------------------------------------

const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_GEOMETRY    = "RepoGUI/geometry";
const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_STATE       = "RepoGUI/state";
const QString repo::gui::RepoGUI::REPO_SETTINGS_LINK_WINDOWS    = "RepoGUI/link";

repo::gui::RepoGUI::RepoGUI(
	repo::RepoController *controller,
	QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RepoGUI)
	, controller(controller)
    , panelsMenu(0)
{
    ui->setupUi(this);
    restoreSettings();

	//Subscribe logger to broadcaster who taps into repo bouncer library logs
	repo::logger::RepoLogger::getInstance()->subscribe(ui->logTextBrowser);
	repoLog("Testing");
    QIcon icon(":/images/3drepo-icon.svg");
    this->setWindowIcon(icon);

    this->setWindowIcon(
                RepoFontAwesome::getInstance().getIcon(
                            RepoFontAwesome::fa_database,
                            QColor(246, 101, 60)));

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

    // Federate...
    QObject::connect(ui->actionFederate, SIGNAL(triggered()),
                     this, SLOT(federate()));

    // Add Map Tiles...
    QObject::connect(ui->actionAddMapTiles, SIGNAL(triggered()),
                     this, SLOT(addMapTiles()));

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

void repo::gui::RepoGUI::addMapTiles()
{
    RepoMapTilesDialog mapTilesDialog(this);
    if(mapTilesDialog.exec()){

		repo::core::model::RepoScene *scene = controller->createMapScene(mapTilesDialog.getMap());
		if (scene)
		{
			commit(scene);
		}

    }

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

    
	repo::core::model::RepoScene *repoScene;

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

        repo::RepoToken *token = ui->widgetRepository->getSelectedConnection();
    }   

    commit(repoScene, activeWindow);
}

void repo::gui::RepoGUI::commit(
        repo::core::model::RepoScene *scene,
        RepoMdiSubWindow *activeWindow)
{
	
	if (scene)
	{
		repo::gui::RepoDialogCommit commitDialog(
			this,
			Qt::Window,
			ui->widgetRepository,
			scene);



		if (!commitDialog.exec())
			std::cout << "Commit dialog cancelled by user" << std::endl;
		else // Clicked "OK"
		{
			//----------------------------------------------------------------------
			// Establish and connect the new worker.
			repo::worker::CommitWorker *worker = new repo::worker::CommitWorker(
				controller,
				ui->widgetRepository->getConnection(commitDialog.getCurrentHost()),
				scene);

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


void repo::gui::RepoGUI::connect()
{
    RepoDialogConnect connectionDialog(this);

    if(!connectionDialog.exec()) // if not clicked "Connect"
    {
        std::cout<< "Connection dialog cancelled by user" << std::endl;
    }
    else
    {
        // if not successfully connected

		std::string errMsg;

		repo::RepoToken* connectionToken =
			controller->authenticateToAdminDatabaseMongo(
			errMsg, 
			connectionDialog.getHost().toStdString(), 
			connectionDialog.getPort(),
			connectionDialog.getUsername().toStdString(), 
			connectionDialog.getPassword().toStdString());

		if (connectionToken)
		{
			//connection/authentication success
			ui->widgetRepository->fetchDatabases(controller, connectionToken);
			//-----------------------------------------------------------------
			// enable buttons
			ui->actionRefresh->setEnabled(true);
			ui->actionHead->setEnabled(true);
			ui->actionHistory->setEnabled(true);
			ui->actionCommit->setEnabled(true);
			ui->actionDrop->setEnabled(true);

		}
		else
		{
			//connection/authentication failed
			std::cerr << "Failed to connect/authenticate user." << std::endl;
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
		if (!QMessageBox::warning(this,
			"Drop?",
			"Are you sure you want to drop '" + ns + "'?",
			"&Yes",
			"&No",
			QString::null, 1, 1))
		{

			bool success;
			std::string errMsg;
			const RepoToken *token = ui->widgetRepository->getSelectedConnection();
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
    RepoFederationDialog fed(ui->widgetRepository, this);
    if (fed.exec())
    {
		repo::core::model::RepoScene *scene = controller->createFederatedScene(fed.getFederation());

		repoLog("federated scene...");
		std::stringstream sstream;
		scene->printStatistics(sstream);
		repoLog(sstream.str());
		repoLog("Committing scene...");
		commit(scene);
    }
}

void repo::gui::RepoGUI::fetchHead()
{
    // Head revision from master branch
    ui->mdiArea->addSubWindow(
				controller,
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

const repo::core::model::RepoScene* repo::gui::RepoGUI::getActiveScene()
{
	const repo::core::model::RepoScene *scene = 0;
    if (const RepoGLCWidget *widget = getActiveWidget())
        scene = widget->getRepoScene();
    return scene;
}

void repo::gui::RepoGUI::history()
{
    QString          database = ui->widgetRepository->getSelectedDatabase();
    QString          project  = ui->widgetRepository->getSelectedProject();
    repo::RepoToken *token    = ui->widgetRepository->getSelectedConnection();
	RepoDialogHistory historyDialog(controller, token, database, project, this);

    if(!historyDialog.exec()) // if not OK
        std::cout << "Revision History dialog cancelled by user." << std::endl;
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
        std::cout << "Loading " << fileName << " ..." << std::endl;

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
		QString(controller->getSupportedImportFormats().c_str()));
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

		repo::core::model::RepoScene *scene = widget->getRepoScene();
		scene->addMetadata(
			controller->loadMetadataFromFile(filePath.toStdString()), false);

		std::stringstream ss;
		scene->printStatistics(ss);
		repoLog("Scene statistics: \n" + ss.str());
		commit(scene);

    }
}

void repo::gui::RepoGUI::optimizeGraph()
{
    if (const RepoGLCWidget *widget = getActiveWidget())
    {
      /*  core::RepoGraphScene* scene = widget->getRepoScene();
        core::RepoGraphOptimizer optimizer(scene);

        optimizer.collapseZeroMeshTransformations();

        optimizer.collapseSingleMeshTransformations();*/
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
    RepoDialogUserManager um(controller, ui->widgetRepository, (QWidget*) this);
    um.exec();
}

void repo::gui::RepoGUI::openProjectManager() const
{
    RepoProjectManagerDialog pm(controller, ui->widgetRepository, (QWidget*) this);
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
