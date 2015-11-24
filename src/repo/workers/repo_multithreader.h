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

#pragma once

//------------------------------------------------------------------------------
// Qt
#include <QThreadPool>
#include <QProgressBar>
#include <QObject>
#include <QMutex>
#include <QMutexLocker>

//------------------------------------------------------------------------------
// GUI
#include "repo_worker_abstract.h"
#include "repo_mutex.h"

namespace repo{
namespace worker{


class RepoMultithreader
{

public :

    ~RepoMultithreader();

public:

    /*!
     * Waits until all threads are cancelled and returns true if successful,
     * false otherwise. This is a blocking operation.
     */
    bool cancelAllThreads();

    //! Connects the worker's basic signals with progressing and finishing slots.
    void connectWorker(
            const RepoAbstractWorker *worker,
            const QProgressBar *progressBar = 0);

    void connectAndStartWorker(
            RepoAbstractWorker *worker,
            const QProgressBar *progressBar = 0);

    //! Returns true if safe to run another single worker, false otherwise.
    bool isSafeToRun();

    //! Starts the worker
    void startWorker(RepoAbstractWorker *worker);

protected :

    //! Mutex to ensure only one worker will run at any given time.
    RepoMutex mutex;

    //! Threadpool to run the workers from.
    QThreadPool threadPool;

}; // end class

} // end namespace worker
} // end namespace repo
