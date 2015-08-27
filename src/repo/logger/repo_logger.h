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

/**
* Repo Logger - responsible for handling all logging facilities
* Inherits RepoAbstractListener from the core library to allow
* instances of this object to be subscribed to the library logger
* broadcasts all messages to its subscribers
*/

#pragma once
#include <string>
#include <vector>

#include <repo/lib/repo_listener_abstract.h>
#include <repo/repo_controller.h>

#include "repo_subscriber_abstract.h"

#define repoLog(MSG) repo::logger::RepoLogger::getInstance()->messageGenerated(MSG)

namespace repo{
	namespace logger{
		class RepoLogger : public repo::lib::RepoAbstractListener
		{
		public:

			/**
			* Singleton logger
			* @return returns the instance of RepoLogger
			*/
			static RepoLogger *getInstance();

			~RepoLogger();

		

			/**
			* Function inherited from RepoAbstractListener
			* This function is called by the broadcaster of which this
			* object is subscribed to.
			* @param message message passed on by the broadcaster
			*/
			void messageGenerated(const std::string &message);

			/**
			* Subscribe a new subscriber
			* @param subscriber object who wishes to subscribe
			*/

			void subscribe(AbstractSubscriber *sub);

		private:

			RepoLogger();

			/**
			* Broadcasts messages to its subscribers
			*/
			void broadcast();

			/**
			* Format the given message into html tagged message
			* This function expects the parameter message to be something
			* like %SEVERITYLEVEL%Message Which should be what the logger
			* in the core should be passing in.
			* @param message pre-format message
			* @return returns formatted message with html tags
			*/
			std::string RepoLogger::formatMessage(
				const std::string &message);

			/**
			* normalize an integer and return it as a string
			* basically, if it's <10, append a 0. This is used for
			* printing the timestamp
			* @param n integer to normalize
			* @return returns the formatted number
			*/
			std::string formatNum(const uint32_t &n);


			/**
			* Get color (as string) based on severity level
			* @param severity severity level
			* @return returns a color given severity level, gray if unknown
			*/
			std::string getSeverityColor(std::string severity);

			std::vector<AbstractSubscriber *> subs;
		};

	}
}

