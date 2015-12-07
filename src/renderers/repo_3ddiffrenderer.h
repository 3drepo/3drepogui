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


#ifndef REPO_3D_DIFF_RENDERER_H
#define REPO_3D_DIFF_RENDERER_H

#include "../repo/widgets/repo_widget_rendering.h"
#include "../primitives/repo_color.h"
#include <repo/repo_controller.h>

namespace repo {
namespace gui {


class Repo3DDiffRenderer
{

public:
    Repo3DDiffRenderer(
		repo::RepoController *controller,
		const repo::RepoToken *token, 
        widgets::RepoRenderingWidget *widgetA,
        widgets::RepoRenderingWidget *widgetB);

    ~Repo3DDiffRenderer();

protected :

    widgets::RepoRenderingWidget *widgetA;

    widgets::RepoRenderingWidget *widgetB;

	repo::RepoController *controller;
	const repo::RepoToken *token;
/*
    core::Repo3DDiff *diff;*/

}; // end class

} // end namespace gui
} // end namespace repo

#endif // REPO_3D_DIFF_RENDERER_H
