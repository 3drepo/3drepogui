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


#ifndef REPO_WORKER_ABSTRACT_H
#define REPO_WORKER_ABSTRACT_H

//------------------------------------------------------------------------------
#include <QtGui>
#include <QRunnable>
//------------------------------------------------------------------------------

namespace repo {
namespace gui {

//! Log scale worker priority when starting a thread from a pool.
enum RepoWorkerSeverity 
{ 
	EXTRA_SMALL = 0,
	SMALL		= 10,
	MEDIUM		= 100,
	LARGE		= 1000, 
	EXTRA_LARGE = 10000, 
};

/*!
 Pure virtual class that provides signature for asynchronous workers in 3D
 Repo. These are used with a QThreadPool or QThread directly.
 
 \sa http://qt-project.org/doc/note_revisions/5/8/view
 \sa http://qt-project.org/wiki/ThreadsEventsQObjects#8493c32c6b5b4020e2d007b5ee4fcd09
 \sa http://blog.bbv.ch/2012/10/03/multithreaded-programming-with-qt/
 */
class RepoWorkerAbstract : public QObject, public QRunnable {

	Q_OBJECT
 
public:

	//! Default constructor.
	RepoWorkerAbstract(); 

	//! Empty pure virtual destructor for proper cleanup.
	/*!
	 * \sa RepoWorkerAbstract()
	 */
	virtual ~RepoWorkerAbstract() = 0;

	//! Connects the processing thread to this worker.
	virtual void connect(QThread *);
 
public slots:

	//! Processes the work.
	virtual void run() = 0;

	/*! Sets the cancel flag to indicate that the run() method should exist as soon as possible.
		All subclasses need to regularly check the cancelled flag accordingly. When connecting 
		signal to this slot, always us Qt::DirectConnection to force exit as soon as possible.
	*/
	virtual void cancel();
 
signals:
	
	//! Emitted when the process has finished.
	void finished(); 

	//! Emitted when a single chunk of the maximum calculation is calculated. 
	void progress(int value, int maximum); 

	void pogressMaximumChanged(int maximum);

	void progressValueChanged(int value);

	void progressRangeChanged(int minimum, int maximum);
	
	//! Emitted when the process encounters an error.
	void error(QString err);

protected :

	//! Flag to indicate that the run method should exit as soon as possible.
	volatile bool cancelled;
 
}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_WORKER_ABSTRACT_H
