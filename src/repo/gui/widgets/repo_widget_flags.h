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
class FlagsWidget;
}

namespace repo {
namespace gui {
namespace widget {

	//! See http://assimp.sourceforge.net/lib_html/postprocess_8h.html
	class FlagsWidget : public QWidget
	{
		Q_OBJECT

			//! Databases header positions
		enum PreDefinedAssimpSettings { DEFAULT = 0, DIRECT_3D = 1, BASIC = 2, MEDIUM = 3, EXTREME = 4 };

	public:

		explicit FlagsWidget(QWidget *parent = 0);

		repo::settings::RepoSettings* getSettings() const { return settings; }

		~FlagsWidget();

		public slots :

		//! Saves the current state of the flags into settings.
		void apply();

		void reset();

		void setBasic();

		//! Sets the crease angle enabled or disabled based on the radio button.
		void setCreaseAngleEnabled(bool on);

		void setDirect3D();

		void setExtreme();

		void setMedium();

		void setPredefinedSettings(int);





	private:

		//! UI var.
		Ui::FlagsWidget *ui;

		//! Persistent application settings.
		repo::settings::RepoSettings *settings;

	};
}
} // end namespace gui
} // end namespace repo

