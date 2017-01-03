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
#include <QWidget>
#include <QSettings>

//------------------------------------------------------------------------------
// GUI
#include "../../settings/repo_settings.h"

namespace Ui {
class IFCFlagsWidget;
}

namespace repo {
namespace gui {
namespace widget {

    class IFCFlagsWidget : public QWidget
	{
		Q_OBJECT

			//! Databases header positions
        enum PreDefinedIFCSettings { DEFAULT = 0 };

	public:

        explicit IFCFlagsWidget(QWidget *parent = 0);

		repo::settings::RepoSettings* getSettings() const { return settings; }

        ~IFCFlagsWidget();

		public slots :

		//! Saves the current state of the flags into settings.
		void apply();

		void reset();

		void setPredefinedSettings(int);

	private:

		//! UI var.
        Ui::IFCFlagsWidget *ui;

		//! Persistent application settings.
		repo::settings::RepoSettings *settings;

	};
}
} // end namespace gui
} // end namespace repo

