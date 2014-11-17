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

#ifndef REPO_TEXT_BROWSER_H
#define REPO_TEXT_BROWSER_H

//------------------------------------------------------------------------------
// Qt
#include <QFile>
#include <QPair>
#include <QTextBrowser>
#include <QTextStream>
#include <QFileSystemWatcher>

//------------------------------------------------------------------------------
// Core
#include <RepoAbstractListener>

namespace repo {
namespace gui {


class RepoTextBrowser
        : public QTextBrowser
        , public core::RepoAbstractListener
{
	Q_OBJECT
		
public :

    //! Default constructor that allocates a file system watcher.
    RepoTextBrowser(QWidget * parent = 0);

    //! Deallocates file system watcher and associated text streams if any.
    ~RepoTextBrowser();

    //! Reimplemented from abstract listener.
    void messageGenerated(const std::string &msg);

public slots :

    //! Adds full file path to a log file that is to be monitored.
    void addFilePath(const QString &filePath);

    //! Appends the last line from the given file to the text browser.
    void watchedFileChanged(const QString &filePath);

private :

    //! On old macs there can be only 256 file watchers on the OS.
    QFileSystemWatcher *fileWatcher;

    //! Hash map of full file paths and the associated streams.
    QHash<QString, QPair<QFile*, QTextStream*>> watchedFiles;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_TEXT_BROWSER_H

