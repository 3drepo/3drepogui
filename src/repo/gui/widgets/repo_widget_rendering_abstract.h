/**
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

#include "../renderers/repo_renderer_abstract.h"

namespace repo {
namespace gui {
namespace widget {

class RenderingAbstractWidget
{

public:

    RenderingAbstractWidget(repo::gui::renderer::NavMode navMode);

    virtual ~RenderingAbstractWidget() = 0;

public :

    void setNavigationMode(renderer::NavMode navMode)
    { this->navMode = navMode;  }

protected :

    renderer::NavMode navMode; //!< Navigation mode.


}; // end class
} //end namespace widget
} // end namespace gui
} // end namespace repo

