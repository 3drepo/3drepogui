/**
 *  Copyright (C) 2015 3D Repo Ltd
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

#include <QGLFormat>

#include "repo_dialog_about.h"
#include "ui_repo_dialog_about.h"

repo::gui::repo_dialog_about::repo_dialog_about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::repo_dialog_about)
{
    ui->setupUi(this);

    QObject::connect(ui->aboutQtPushButton, SIGNAL(pressed()),
                     QApplication::instance(),
                     SLOT(aboutQt()));

    ui->versionLabel->setText(getVersionInfo(" | "));
}

repo::gui::repo_dialog_about::~repo_dialog_about()
{
    delete ui;
}

QString repo::gui::repo_dialog_about::getVersionInfo(const QString &separator)
{
    QString info;
    info += QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion();
    info += separator;
    info += QString("Qt ") + QT_VERSION_STR;
    info += separator;
    info += "OpenGL " + QString::number(QGLFormat::defaultFormat().majorVersion());
    info += "." + QString::number(QGLFormat::defaultFormat().minorVersion());
    return info;
}
