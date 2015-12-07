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

#include "repo_multithreader.h"

using namespace repo::worker;

RepoMultithreader::~RepoMultithreader()
{
    cancelAllThreads();
}

bool RepoMultithreader::cancelAllThreads()
{
    mutex.emitCancelSignal();
    return threadPool.waitForDone(); // msecs
}

void RepoMultithreader::connectWorker(
        const RepoAbstractWorker *worker,
        QProgressBar *progressBar)
{
    if (worker)
    {
        // Direct connection ensures cancel signal is processed ASAP
        QObject::connect(
                    &mutex, &RepoMutex::cancel,
                    worker, &repo::worker::RepoAbstractWorker::cancel,
                    Qt::DirectConnection);

        QObject::connect(
                    worker, &RepoAbstractWorker::finished,
                    &mutex, &RepoMutex::unlockSlot);

        // Link progress bar, too, if not null
        if (progressBar)
        {
            QObject::connect(
                        worker, &repo::worker::RepoAbstractWorker::progressRangeChanged,
                        progressBar, &QProgressBar::setRange);

            QObject::connect(
                        worker, &repo::worker::RepoAbstractWorker::progressValueChanged,
                        progressBar, &QProgressBar::setValue);

            // This will automatically hide the progress bar once finished. CAREFUL!
            QObject::connect(
                        worker, &repo::worker::RepoAbstractWorker::finished,
                        progressBar, &QProgressBar::hide);

            progressBar->show();
        }
    }
}

void RepoMultithreader::connectAndStartWorker(
        RepoAbstractWorker *worker,
        QProgressBar *progressBar)
{
    connectWorker(worker, progressBar);
    startWorker(worker);
}

bool RepoMultithreader::isReady()
{
    return mutex.tryLock() && cancelAllThreads();
}

void RepoMultithreader::startWorker(RepoAbstractWorker *worker)
{
    worker->setAutoDelete(true);
    threadPool.start(worker);
}
