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
#include <QSettings>
#include <QDataStream>
#include <QMetaType>

#include <iostream>

namespace repo {
	namespace settings {
		class RepoSettingsCredentials : public QSettings
		{
			Q_OBJECT

				static const QString CREDENTIALS;
			static const QString CREDENTIALS_ARRAY;

		public:

			RepoSettingsCredentials();

			~RepoSettingsCredentials() {}

		signals:

			void credentialsAt(int i, std::string &credentials);

		public:

			//! Stores given credentials in settings
			void writeCredentials(QList<std::string> &credentialsList);

			//! Emits a singal upon each stored credentials and returns them in a list.
			QList<std::string> readCredentials();
		};
	} // end namespace settings
} // end namespace repo
