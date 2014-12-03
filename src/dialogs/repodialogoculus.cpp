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

#include <QDesktopWidget>
#include <QMainWindow>

#include "../oculus/repo_oculus.h"
#include "repodialogoculus.h"
#include "ui_repodialogoculus.h"

repo::gui::RepoDialogOculus::RepoDialogOculus(
        const RepoGLCWidget *glcWidget,
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RepoDialogOculus)
    , glcWidget(glcWidget)
{
    ui->setupUi(this);

    const QDesktopWidget *desktop = QApplication::desktop();
    for (int i = 0; i < desktop->screenCount(); ++i)
    {
        const QRect size = desktop->screenGeometry(i);
        QString description = tr("Display");
        description += " ";
        description += QString::number(i + 1);
        description += " (";
        description += QString::number(size.width());
        description += " x ";
        description += QString::number(size.height());
        description += ")";

        ui->monitorComboBox->addItem(description);
    }
}

repo::gui::RepoDialogOculus::~RepoDialogOculus()
{
    delete ui;
}


int repo::gui::RepoDialogOculus::exec()
{
    int result;
    if (result = QDialog::exec())
    {
        QMainWindow *oculusWindow = new QMainWindow((QWidget *) this->parent());
        oculusWindow->setAttribute(Qt::WA_DeleteOnClose);
        oculusWindow->setFocusPolicy(Qt::StrongFocus);
        oculusWindow->show();

        //----------------------------------------------------------------------
        // Create Oculus widget
        RepoOculus *oculusWidget = new RepoOculus(
                    oculusWindow,
                    RepoOculus::singleBufferFormat(),
                    glcWidget->windowTitle());
        oculusWidget->setGLCWorld(glcWidget->getGLCWorld());
        oculusWindow->setCentralWidget(oculusWidget);

        //----------------------------------------------------------------------
        // Move to selected screen
        int selectedScreen = ui->monitorComboBox->currentIndex() + 1;

        std::cout << "Selected screen index: " << selectedScreen << std::endl;


        QRect screenres = QApplication::desktop()->screenGeometry(selectedScreen);
        oculusWindow->move(QPoint(screenres.x(), screenres.y()));

        oculusWindow->showFullScreen();
    }
    return result;
}
