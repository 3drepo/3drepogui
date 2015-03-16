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


#ifndef REPO_IRENDERER_H
#define REPO_IRENDERER_H

namespace repo {
namespace gui {

/**
 * @brief The RepoIRenderer class
 *
 * Renderer interface class which is common to all renderers in 3D Repo GUI.
 */
class RepoIRenderer
{    

public:

    /**
     * Virtual empty destructor to enable passing of pointer ownership without
     * revealing the concrete derived subclass itself.
     */
    virtual ~RepoIRenderer() {}

    // TODO: pure virtual operator= and copy constructor.

    //! Selects the entire model.
    virtual void selectAll() const = 0;

    //! Selects a section of the scene graph by UID or SID.
    virtual void select(const boost::uuids::uuid &id) const = 0;

    //! Deselects a section of the scene graph by UID or SID.
    virtual void deselect(const boost::uuids::uuid &id) const = 0;

}; // end class

} // end namespace gui
} // end namespace repo

#endif // REPO_IRENDERER_H
