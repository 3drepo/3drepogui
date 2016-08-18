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



#include <QApplication>
#include <QResource>

#include <repo/repo_controller.h>
#include <repo/lib/repo_listener_abstract.h>

#include "repo/gui/repo_gui.h"
#include "repo/logger/repo_logger.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    a.setAttribute(Qt::AA_UseDesktopOpenGL); // Qt5.5 support
    a.setAttribute(Qt::AA_ShareOpenGLContexts); // https://blog.qt.io/blog/2014/09/10/qt-weekly-19-qopenglwidget/

    QCoreApplication::setOrganizationName("3D Repo");
    QCoreApplication::setOrganizationDomain("3drepo.org");
    QCoreApplication::setApplicationName("3D Repo GUI");
    QCoreApplication::setApplicationVersion("1.2.0");

    std::vector<repo::lib::RepoAbstractListener*> listeners;
    listeners.push_back(repo::logger::RepoLogger::getInstance());

    repo::RepoController *controller = new repo::RepoController(listeners);


	//check env var to see whether a debug level is set
	char* debug = getenv("REPO_DEBUG");
	char* verbose = getenv("REPO_VERBOSE");

    if (verbose)
	{
		controller->setLoggingLevel(repo::lib::RepoLog::RepoLogLevel::TRACE);
	}
	else if (debug)
	{
		controller->setLoggingLevel(repo::lib::RepoLog::RepoLogLevel::DEBUG);
	}
	else
	{
		controller->setLoggingLevel(repo::lib::RepoLog::RepoLogLevel::INFO);
    }

	if (verbose) free(verbose);
	if (debug)   free(debug);
	
    repo::gui::RepoGUI w(controller);

    w.show();
    w.startup();
    return a.exec();
}

