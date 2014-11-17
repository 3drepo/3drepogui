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

#include "repo_textbrowser.h"

//------------------------------------------------------------------------------
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>
//------------------------------------------------------------------------------

repo::gui::RepoTextBrowser::RepoTextBrowser(QWidget * parent)
        : QTextBrowser(parent)
        , RepoAbstractListener()
{
    fileWatcher = new QFileSystemWatcher(this);

    qRegisterMetaType<QTextBlock>("QTextBlock");
    qRegisterMetaType<QTextCursor>("QTextCursor");

    QObject::connect(
                fileWatcher, SIGNAL(fileChanged(const QString &)),
                this, SLOT(watchedFileChanged(const QString &)));

}

repo::gui::RepoTextBrowser::~RepoTextBrowser()
{
    delete fileWatcher;

    //--------------------------------------------------------------------------
    for (QHash<QString, QPair<QFile*,QTextStream*>>::iterator it =
         watchedFiles.begin();
         it != watchedFiles.end(); ++it)
    {
        delete it.value().second; // QTextStream
        delete it.value().first; // QFile
    }
    watchedFiles.clear();
}

void repo::gui::RepoTextBrowser::messageGenerated(const std::string &msg)
{ 
    QMetaObject::invokeMethod(this, "append", Qt::AutoConnection,
        Q_ARG(QString, QString::fromStdString(msg)));
}

void repo::gui::RepoTextBrowser::addFilePath(const QString &filePath)
{
    if(fileWatcher->addPath(filePath))
    {
        QFile *file = new QFile(filePath);
        if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
            delete file;
        else
        {
            QTextStream *textStream = new QTextStream(file);
            textStream->seek(file->size());
            watchedFiles.insert(filePath, QPair<QFile*, QTextStream*>(file, textStream));

            watchedFileChanged(filePath);
            watchedFileChanged(filePath);
        }
    }
}

void repo::gui::RepoTextBrowser::watchedFileChanged(const QString &filePath)
{
    QHash<QString, QPair<QFile*,QTextStream*>>::iterator it =
            watchedFiles.find(filePath);

    if (it != watchedFiles.end())
    {
        QMetaObject::invokeMethod(this, "append", Qt::AutoConnection,
            Q_ARG(QString, it.value().second->readLine()));
    }
}
