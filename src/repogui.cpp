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

repo::gui::RepoGUI::RepoGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RepoGUI)
{
    ui->setupUi(this);
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
    // Connect
    QObject::connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connect()));
    ui->actionConnect->setIcon(RepoDialogConnect::getIcon());

    //--------------------------------------------------------------------------
    // Refresh
    QObject::connect(ui->actionRefresh, SIGNAL(triggered()), this, SLOT(refresh()));
    ui->actionRefresh->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_refresh));

    //--------------------------------------------------------------------------
    // Drop
    QObject::connect(ui->actionDrop, SIGNAL(triggered()), this, SLOT(dropDatabase()));
    ui->actionDrop->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_trash_o));

    //--------------------------------------------------------------------------
    // Exit
    QObject::connect(ui->actionExit, SIGNAL(triggered()),
                     QApplication::instance(), SLOT(quit()));
    ui->actionExit->setIcon(
                RepoFontAwesome::getInstance().getIcon(
                    RepoFontAwesome::fa_sign_out, QColor(Qt::darkRed)));
}

repo::gui::RepoGUI::~RepoGUI()
{
    delete ui;
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

void repo::gui::RepoGUI::dropDatabase()
{
    QString dbName = ui->widgetRepository->getSelectedDatabase();
    if (!dbName.isNull() &&
        !dbName.isEmpty() &&
        dbName != "local" &&
        dbName != "admin")
    {
        switch (QMessageBox::warning(this,
            "Drop Database?",
            "Are you sure you want to drop '" + dbName + "' repository?",
            "&Yes",
            "&No",
            QString::null, 1, 1))
        {
            case 0:
                // yes
                if (ui->widgetRepository->getSelectedConnection().deleteDatabase(dbName.toStdString()))
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
    else
    {
        std::cout << "You are not allowed to delete 'local' and 'admin' databases." << std::endl;
    }
}
