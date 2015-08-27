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

#include "repo_logger.h"

using namespace repo::logger;

RepoLogger::RepoLogger()
{
}


RepoLogger::~RepoLogger()
{
}

RepoLogger* RepoLogger::getInstance()
{
	static RepoLogger *log = new RepoLogger();
	return log;
}

std::string RepoLogger::formatMessage(
	const std::string &message)
{

	size_t firstPos = message.find_first_of('%')+1;
	size_t secPos = message.find_first_of('%', firstPos + 1);

	std::string severity = "UNKNOWN";
	std::string actualMessage = message;
	if (firstPos != std::string::npos && secPos != std::string::npos)
	{
		severity = message.substr(firstPos, secPos - firstPos);
		std::transform(severity.begin(), severity.end(), severity.begin(), ::toupper);
		//+1 to skip the % after severity, 
		actualMessage = message.substr(secPos + 1); 

	}

	std::stringstream formatted;

	//---------------------------------------------------
	//Append time
	time_t t = time(0);   // get time now
	const struct tm *now = localtime(&t);
	formatted << formatNum(now->tm_hour);
	formatted << ":";
	formatted << formatNum(now->tm_min);
	formatted << ":";
	formatted << formatNum(now->tm_sec);
	formatted << " [";
	//-------------------------------------------------------------------------
	// add colors to severity level
	formatted << "<b><span style='color:";
	formatted << getSeverityColor(severity);
	formatted << "'>";
	formatted << severity;
	formatted << "</span></b>] ";

	formatted << actualMessage;

	formatted << "<br/>";

	return formatted.str();
}


std::string RepoLogger::formatNum(const uint32_t &n)
{
	std::stringstream str;
	if (n < 10)
		str << "0";
	str << n;
	return str.str();
}


std::string RepoLogger::getSeverityColor(std::string severity)
{
	//default is gray, which represents an unknown status
	std::string color = "gray";

	if (severity == "ERROR" || severity == "FATAL")
	{
		color = "red";
	}

	if (severity == "WARNING")
	{
		color = "orange";
	}

	if (severity == "INFO")
	{
		color = "green";
	}

	if (severity == "DEBUG")
	{
		color = "purple";
	}

	if (severity == "TRACE")
	{
		color = "blue";
	}

	return color;
}

void RepoLogger::messageGenerated(
	const std::string &message)
{
	std::string formattedMessage = formatMessage(message);

	for (auto sub : subs)
	{
		sub->newMessageReceived(formattedMessage);
	}
}

void RepoLogger::subscribe(AbstractSubscriber *sub)
{
	if (sub)
		subs.push_back(sub);
}
