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

#include "repogui.h"

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
    if(connectionDialog.exec()) // if clicked "Connect"
    {}
//    {
//        //---------------------------------------------------------------------
//        // if successfully connected
//        if (mongo->connect(connectionDialog.getHost().toStdString(), connectionDialog.getPort()))
//        {
//            if (!connectionDialog.getUsername().isEmpty())
//            {
//                mongo->authenticate(
//                    connectionDialog.getUsername().toStdString(),
//                    connectionDialog.getPassword().toStdString());
//            }
//            repo::gui::RepoLogger::getInstance().log(repo::REPO_INFO, mongo->getUsernameAtHostAndPort());
//            refreshSlot();

//            //-----------------------------------------------------------------
//            // enable buttons
//            actionRefresh->setEnabled(true);
//            actionHead->setEnabled(true);
//            actionHistory->setEnabled(true);
//            actionCommit->setEnabled(true);
//            actionDrop->setEnabled(true);
//            actionConnect->setEnabled(false);
//        }
//        else
//            repo::gui::RepoLogger::getInstance().log(repo::REPO_ERROR, "Connection error");
//    }
//    else
//        repo::gui::RepoLogger::getInstance().log(repo::REPO_INFO, "Connection dialog cancelled by user");
}
