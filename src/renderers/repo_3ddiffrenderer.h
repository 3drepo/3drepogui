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

#include "../repo/widgets/repo_widget_rendering.h"
#include "../primitives/repo_color.h"
#include "../repo/workers/repo_worker_diff.h"
#include <repo/repo_controller.h>

Q_DECLARE_METATYPE(repo::manipulator::diff::DiffResult)
namespace repo {
namespace gui {

class Repo3DDiffRenderer : public QObject
{
	Q_OBJECT
public:
    Repo3DDiffRenderer(
		repo::RepoController *controller,
		const repo::RepoToken *token, 
        widgets::RepoRenderingWidget *widgetA,
        widgets::RepoRenderingWidget *widgetB);

    ~Repo3DDiffRenderer();

	void resultingDifference(const repo::manipulator::diff::DiffResult &res,
		widgets::RepoRenderingWidget *widget,
		const QColor &modColor,
		const QColor &addColor);

	public slots:

	void resultingDifferenceA(const repo::manipulator::diff::DiffResult &res)
	{
		repoLog("Slot called A");
		resultingDifference(res, widgetA, Qt::cyan, Qt::red);
	}
	void resultingDifferenceB(const repo::manipulator::diff::DiffResult &res)
	{
		repoLog("Slot called B");
		resultingDifference(res, widgetB, Qt::cyan, Qt::green);
	}

	
protected :

    widgets::RepoRenderingWidget *widgetA;

    widgets::RepoRenderingWidget *widgetB;

	repo::RepoController *controller;
	const repo::RepoToken *token;

}; // end class

} // end namespace gui
} // end namespace repo

