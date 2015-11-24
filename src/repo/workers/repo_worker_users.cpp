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

#include "repo_worker_users.h"
#include "../logger/repo_logger.h"

using namespace repo::worker;

UsersWorker::UsersWorker(
	const repo::RepoToken                   *token,
	repo::RepoController                    *controller,
	const std::string                       &database,
	const repo::core::model::RepoUser &user,
	const Command                           &command)
	: token(token)
	, controller(controller)
	, database(database)
	, user(user)
	, command(command)
{
	qRegisterMetaType<repo::core::model::RepoUser>("repo::core::model::RepoUser");
	qRegisterMetaType<std::list<std::string> >("std::list<std::string>");
	qRegisterMetaType<std::map<std::string, std::list<std::string> > >("std::map<std::string, std::list<std::string> >");
}

//------------------------------------------------------------------------------

UsersWorker::~UsersWorker() {}

//------------------------------------------------------------------------------

void UsersWorker::run()
{
	int jobsCount = 3;
	int jobsDone = 0;
	emit progressRangeChanged(0, 0); // undetermined (moving) progress bar

    //--------------------------------------------------------------------------
	// Execute command (such as drop or update user) if any
	if (!user.isEmpty())
	{
		switch (command)
		{
			case Command::INSERT:
				repoLog("Adding new user\n");
				controller->insertUser(token, user);
				break;
			case Command::UPDATE:
				repoLog("Updating user\n");
				controller->updateUser(token, user);
				break;
			case Command::DROP:
				repoLog("Removing user\n");
				controller->removeUser(token, user);
		}
	}

    //--------------------------------------------------------------------------
	// Get mapping of databases with their associated projects.
	// This is long running job!
	std::list<std::string> databases = controller->getDatabases(token);
	std::map<std::string, std::list<std::string> > databasesWithProjects =
		controller->getDatabasesWithProjects(token, databases);
	emit databasesWithProjectsFetched(databasesWithProjects);
	emit progressRangeChanged(0, jobsCount);
	emit progressValueChanged(jobsDone++);

    //--------------------------------------------------------------------------
	std::auto_ptr<mongo::DBClientCursor> cursor;
	std::list<std::string> fields; // projection, empty at the moment
    //--------------------------------------------------------------------------
	// Get custom roles
	std::list<std::string> roles;
	fields.clear();
	fields.push_back(REPO_USER_LABEL_ROLE);

	uint64_t retrieved = 0;
	uint32_t nRoles = controller->countItemsInCollection(token, database, REPO_SYSTEM_ROLES);
	while (!cancelled && nRoles > retrieved)
	{

		std::vector<repo::core::model::RepoBSON> bsons =
			controller->getAllFromCollectionContinuous(token, database, REPO_SYSTEM_ROLES, fields, REPO_LABEL_ROLE, -1, retrieved);

		for (auto bson : bsons)
		{
			if (cancelled) break;
			if (bson.isValid() && !bson.isEmpty())
			{
				roles.push_back(bson.getStringField(REPO_USER_LABEL_ROLE));
			}
			++retrieved;
		}
	}

	emit customRolesFetched(roles);
	emit progressValueChanged(jobsDone++);

	//------------------------------------------------------------------
	// Get users
	fields.clear();

	retrieved = 0;

	uint32_t nUsers = controller->countItemsInCollection(token, database, REPO_SYSTEM_USERS);
	while (!cancelled && nUsers > retrieved)
	{

		std::vector<repo::core::model::RepoBSON> bsons =
			controller->getAllFromCollectionContinuous(token, database, REPO_SYSTEM_USERS, fields, REPO_LABEL_USER, -1, retrieved);

		for (auto bson : bsons)
		{
			if (cancelled) break;
			if (bson.isValid() && !bson.isEmpty())
			{
				emit userFetched(repo::core::model::RepoUser(bson));
			}
			++retrieved;
		}
	}
	emit progressValueChanged(jobsDone++);


	//--------------------------------------------------------------------------
	emit progressValueChanged(jobsCount);
	emit RepoAbstractWorker::finished();
}

