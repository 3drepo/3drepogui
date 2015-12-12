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

#include <QGraphicsView>
#include <QWidget>

#include "../../primitives/repo_fontawesome.h"
#include "../renderer/repo_renderer_graph.h"

#include <repo/core/model/collection/repo_scene.h>

namespace repo {
namespace widgets {

class RepoWidgetRendering2D : public QGraphicsView
{
    Q_OBJECT

public:

    RepoWidgetRendering2D(
            const repo::core::model::RepoScene *scene,
            const QString &windowTitle = QString(),
            QWidget *parent = 0);

    ~RepoWidgetRendering2D();

private :

};

} // end namespace widgets
} // end namespace repo
