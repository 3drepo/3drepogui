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
* Logger Redirect - redirects messages to the bouncer logger 
*/

#pragma once

#include <iostream>
#include <algorithm>                       // copy, min
#include <iosfwd>                          // streamsize
#include <boost/iostreams/categories.hpp>  // sink_tag


#include <boost/iostreams/concepts.hpp> 
#include <boost/iostreams/stream_buffer.hpp>

namespace repo{
	namespace logger{
		class StreamRedirect : public boost::iostreams::sink
		{
		public:

			enum class Severity {INFO, ERR};

			StreamRedirect(const Severity &level);
			~StreamRedirect();

		

			/**
			* When called, it will notify all subscribers of the message.
			*/
			std::streamsize write(const char* s, std::streamsize n);

		private:
			const Severity logLevel;
			
		};

	}
}

