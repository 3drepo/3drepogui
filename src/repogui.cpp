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

//-----------------------------------------------------------------------------
// Qt
#include <QMessageBox>
//------------------------------------------------------------------------------
// Repo
#include "repogui.h"
#include "ui_repogui.h"
#include "widgets/repo_widgetrepository.h"
//------------------------------------------------------------------------------

const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_GEOMETRY = "RepoGUI/geometry";
const QString repo::gui::RepoGUI::REPO_SETTINGS_GUI_STATE = "RepoGUI/state";
const QString repo::gui::RepoGUI::REPO_SETTINGS_LINK_WINDOWS = "RepoGUI/link";

repo::gui::RepoGUI::RepoGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RepoGUI)
{
    ui->setupUi(this);
    restoreSettings();

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


    //--------------------------------------------------------------------------
    // Exit
    QObject::connect(ui->actionExit, SIGNAL(triggered()),
                     QApplication::instance(), SLOT(quit()));
    ui->actionExit->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_sign_out, QColor(Qt::darkRed)));


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

    //--------------------------------------------------------------------------
    // Drop
    QObject::connect(ui->actionDrop, SIGNAL(triggered()), this, SLOT(dropDatabase()));
    ui->actionDrop->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_trash_o));


    //--------------------------------------------------------------------------
    //
    // View
    //
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Full Screen
    QObject::connect(ui->actionFull_Screen,
                     SIGNAL(triggered()),
                     this, SLOT(toggleFullScreen()));
    ui->actionFull_Screen->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_arrows_alt));

    // Link
    ui->actionLink->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_link,
                    RepoFontAwesome::fa_chain_broken));

    // Panels
    QMenu *menuPanels = QMainWindow::createPopupMenu();
    menuPanels->setTitle(QString("Panels"));
    menuPanels->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_columns));
    ui->menuWindow->addMenu(menuPanels);


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
    ui->actionReport_Issue->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_globe));






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
}


//------------------------------------------------------------------------------
//
// Public
//
//------------------------------------------------------------------------------

void repo::gui::RepoGUI::startup()
{
    //--------------------------------------------------------------------------
    // Connection dialog
    RepoDialogConnect connectionDialog(this);
    if (connectionDialog.isShowAtStartup())
        connect();
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

void repo::gui::RepoGUI::refresh()
{
    ui->widgetRepository->refresh();
}

void repo::gui::RepoGUI::fetchHead()
{
    QString database = ui->widgetRepository->getSelectedDatabase();
    ui->mdiArea->addSubWindow(
                ui->widgetRepository->getSelectedConnection(),
                database); // head revision from master branch

    // make sure to hook controls if chain is on
    ui->mdiArea->chainSubWindows(ui->actionLink->isChecked());
}

void repo::gui::RepoGUI::dropDatabase()
{
    QString dbName = ui->widgetRepository->getSelectedDatabase();
    if (dbName.isNull() || dbName.isEmpty())
    {
        std::cout << "A database must be selected." << std::endl;
    }
    else if (dbName == "local" || dbName == "admin")
    {
        std::cout << "You are not allowed to delete 'local' or 'admin' databases." << std::endl;
    }
    else
    {
        switch (QMessageBox::warning(this,
            "Drop Database?",
            "Are you sure you want to drop '" + dbName + "' repository?",
            "&Yes",
            "&No",
            QString::null, 1, 1))
        {
            case 0: // yes

                // TODO: create a DB manager separate from repositories widget.
                core::MongoClientWrapper mongo = ui->widgetRepository->getSelectedConnection();
                mongo.reconnectAndReauthenticate();
                if (mongo.dropDatabase(dbName.toStdString()))
                {
                    std::cout << dbName.toStdString() << " deleted successfully."
                                 << std::endl;
                }
                else
                   std::cout << "Delete unsuccessful" << std::endl;
                refresh();
                break;
            }
    }
}

void repo::gui::RepoGUI::showDatabaseContextMenu(const QPoint &pos)
{
    QMenu menu(ui->widgetRepository->getDatabasesTreeView());
    menu.addAction(ui->actionConnect);
    menu.addAction(ui->actionRefresh);
    menu.addSeparator();
    menu.addAction(ui->actionHead);
    menu.addAction(ui->actionHistory);
    menu.addAction(ui->actionSwitch);
    menu.addSeparator();
    menu.addAction(ui->actionDrop);
    menu.exec(ui->widgetRepository->mapToGlobalDatabasesTreeView(pos));
}

void repo::gui::RepoGUI::showCollectionContextMenuSlot(const QPoint &pos)
{
    QMenu menu(ui->widgetRepository->getCollectionTreeView());
    QAction *a = menu.addAction(
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

void repo::gui::RepoGUI::openFile()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        tr("Select one or more files to open"),
        QString::null,
        repo::core::AssimpWrapper::getImportFormats().c_str());
    loadFiles(filePaths);
}

void repo::gui::RepoGUI::openSupportEmail() const
{
    QString email = "support@3drepo.org";
    QString subject = "GUI Support Request";

    // TODO: get system state printout directly from About dialog.
    QString body;
    body += QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion();
    body += "\n";
    body += QString("Qt ") + QT_VERSION_STR;
    body += "\n";
    body += "OpenGL " + QString::number(QGLFormat::defaultFormat().majorVersion());
    body += "." + QString::number(QGLFormat::defaultFormat().minorVersion());

    QDesktopServices::openUrl(
                QUrl("mailto:" + email +
                     "?subject=" + subject +
                     "&body=" + body));
}

void repo::gui::RepoGUI::reportIssue() const
{
    QDesktopServices::openUrl(QUrl("https://github.com/3drepo/3drepogui/issues"));
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

void repo::gui::RepoGUI::loadFile(const QString &filePath)
{
    if (!filePath.isEmpty())
    {
        QFileInfo pathInfo(filePath);
        string fileName = pathInfo.fileName().toStdString();
        std::cout << "Loading " << fileName << " ..." << std::endl;

        // TODO: get assimp post processing flags from settings dialog box.
        unsigned int postProcessingFlags = 0;
        ui->mdiArea->addSubWindow(filePath, postProcessingFlags);
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

void repo::gui::RepoGUI::storeSettings()
{
    QSettings settings;
    settings.setValue(REPO_SETTINGS_GUI_GEOMETRY, saveGeometry());
    settings.setValue(REPO_SETTINGS_GUI_STATE, saveState());
    settings.setValue(REPO_SETTINGS_LINK_WINDOWS, ui->actionLink->isChecked());
}

void repo::gui::RepoGUI::restoreSettings()
{
    QSettings settings;
    restoreGeometry(settings.value(REPO_SETTINGS_GUI_GEOMETRY).toByteArray());
    restoreState(settings.value(REPO_SETTINGS_GUI_STATE).toByteArray());
    ui->actionLink->setChecked(settings.value(REPO_SETTINGS_LINK_WINDOWS, false).toBool());
}
