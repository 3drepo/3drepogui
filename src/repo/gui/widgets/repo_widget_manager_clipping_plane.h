/**
 *  Copyright (C) 2016 3D Repo Ltd
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

#include <QWidget>

//------------------------------------------------------------------------------
// GUI
#include "repo_mdi_area.h"

namespace Ui {
class RepoClippingPlaneWidget;
}

namespace repo {
namespace gui {
namespace widget {

class RepoClippingPlaneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RepoClippingPlaneWidget(
            repo::gui::widget::RepoMdiArea *mdiArea,
            QWidget *parent = 0);
    ~RepoClippingPlaneWidget();


public slots:

    void setClipping();

    repo::gui::renderer::Axis getAxis();


private:

    Ui::RepoClippingPlaneWidget *ui;

    repo::gui::widget::RepoMdiArea *mdiArea;
};


} // widgets
} // gui
} // repo

