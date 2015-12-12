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


#include "repo_widget_rendering_2d.h"

using namespace repo::widgets;

RepoWidgetRendering2D::RepoWidgetRendering2D(
        const repo::core::model::RepoScene *scene,
        const QString &windowTitle,
        QWidget *parent)
    : QGraphicsView(new renderer::RepoRendererGraph(scene), parent)
{
    //--------------------------------------------------------------------------
    // Rendering optimisation
    this->setViewport(new QOpenGLWidget());
    this->setCacheMode(QGraphicsView::CacheBackground);
    //--------------------------------------------------------------------------
    this->setWindowTitle(windowTitle);
    this->setWindowIcon(repo::gui::RepoFontAwesome::getSceneGraphIcon());
}

RepoWidgetRendering2D::~RepoWidgetRendering2D()
{

}
