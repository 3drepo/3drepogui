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


#ifndef REPO_WIDGET_ASSIMP_FLAGS_H
#define REPO_WIDGET_ASSIMP_FLAGS_H

//------------------------------------------------------------------------------
// Qt
#include <QWidget>
#include <QSettings>

//------------------------------------------------------------------------------
// Core
#include <RepoWrapperAssimp>
#include <assimp/postprocess.h>

//------------------------------------------------------------------------------
// GUI
#include "../reposettings.h"

namespace Ui {
class RepoWidgetAssimpFlags;
}

namespace repo {
namespace gui {

//! See http://assimp.sourceforge.net/lib_html/postprocess_8h.html
class RepoWidgetAssimpFlags : public QWidget
{
    Q_OBJECT

    //! Databases header positions
    enum PreDefinedAssimpSettings { DEFAULT = 0, DIRECT_3D = 1, BASIC = 2, MEDIUM = 3, EXTREME = 4 };

public:

    explicit RepoWidgetAssimpFlags(QWidget *parent = 0);

    ~RepoWidgetAssimpFlags();

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





private :

    //! UI var.
    Ui::RepoWidgetAssimpFlags *ui;

    //! Persistent application settings.
    RepoSettings settings;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_WIDGET_ASSIMP_FLAGS_H
