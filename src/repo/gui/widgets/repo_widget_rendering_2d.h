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
#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSurfaceFormat>

#include "../../../primitives/repo_fontawesome.h"
#include "../renderers/repo_renderer_graph.h"

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

public slots :



public :

    //! Zoom in one step if true, zoom out one step otherwise.
    void zoom(bool in);

    //! Resets the zooming to default level.
    void zoomReset();

protected :

    //! Updates the height of the timeline scene.
    virtual void resizeEvent(QResizeEvent *);

    //! Zoom in/out CTRL + wheel.
    virtual void wheelEvent(QWheelEvent*);

    //! Enables panning on mid button press.
    virtual void mousePressEvent(QMouseEvent*);

    //! Release of mid button changes cursor back to normal.
    virtual void mouseReleaseEvent(QMouseEvent*);

    //! Pans the view on mid button move.
    virtual void mouseMoveEvent(QMouseEvent*);

    virtual void keyPressEvent(QKeyEvent *);

private :

    //! Position of the mouse cursor when last pressed.
    QPoint lastMousePosition;

    QOpenGLWidget *openGLWidget;

};

} // end namespace widgets
} // end namespace repo
