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


#include "repo_worker_abstract.h"
#include <repo/lib/repo_log.h>

using namespace repo::worker;

RepoAbstractWorker::RepoAbstractWorker()
	: QObject(), cancelled(false) {}

RepoAbstractWorker::~RepoAbstractWorker() {}

void RepoAbstractWorker::connect(QThread * thread)
{
	QObject::connect(thread, SIGNAL(started()), this, SLOT(run()));
	QObject::connect(this, SIGNAL(finished()), thread, SLOT(quit()));
	QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
	QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
}

void RepoAbstractWorker::cancel()
{
	cancelled = true;
	repoLog("Cancelling worker operations...");
}
