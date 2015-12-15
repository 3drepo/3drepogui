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
    , openGLWidget(new QOpenGLWidget())
{
    //--------------------------------------------------------------------------
    // Rendering optimisation
    QSurfaceFormat format;
    format.setSamples(2);
    openGLWidget->setFormat(format);
    this->setViewport(openGLWidget);
    this->setCacheMode(QGraphicsView::CacheBackground);
    this->setRenderHints(QPainter::Antialiasing);// | QPainter::SmoothPixmapTransform);
    //--------------------------------------------------------------------------
    this->setWindowTitle(windowTitle);
    this->setWindowIcon(repo::gui::RepoFontAwesome::getSceneGraphIcon());
}

RepoWidgetRendering2D::~RepoWidgetRendering2D()
{
    delete openGLWidget;
}

void RepoWidgetRendering2D::zoom(bool in)
{
    double scaleFactor = 1.15;
    if(in) // zoom in
        scale(scaleFactor, scaleFactor);
    else // zoom out
    {
        double out = 1.0 / scaleFactor;
        scale(out, out);
    }
}

void RepoWidgetRendering2D::zoomReset()
{
    this->setTransform(QTransform());
}

void RepoWidgetRendering2D::resizeEvent(QResizeEvent * event)
{
    QGraphicsView::resizeEvent(event);
    //-------------------------------------------------------------------------
    // Size of the scene rectangle only grows, it never shrinks.
    // See: http://qt-project.org/doc/qt-5.0/qtwidgets/qgraphicsscene.html#sceneRect-prop
    int sceneHeight = std::max(
        (int) scene()->sceneRect().height(),
        event ? event->size().height() : 0);
    scene()->setSceneRect(
        QRect(
            scene()->sceneRect().x(), scene()->sceneRect().y(),
            scene()->sceneRect().width(), sceneHeight));
}

void RepoWidgetRendering2D::wheelEvent(QWheelEvent* event)
{
    // Zoom if CTRL pressed and wheel rotated.
    if (Qt::Modifier::CTRL == event->modifiers())
    {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        zoom(event->delta() > 0);
    }
    else
        QGraphicsView::wheelEvent(event);
}

void RepoWidgetRendering2D::mousePressEvent(QMouseEvent *e)
{
    lastMousePosition = e->pos();
    QGraphicsView::mousePressEvent(e);
}


void RepoWidgetRendering2D::mouseReleaseEvent(QMouseEvent *e)
{
    this->setCursor(Qt::ArrowCursor);
    QGraphicsView::mousePressEvent(e);
}


void RepoWidgetRendering2D::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons().testFlag(Qt::MidButton))
    {
        this->setCursor(Qt::ClosedHandCursor);
        // Custom dragging instead of setDragMode(ScrollHandDrag);
        QScrollBar * horizontalBar = horizontalScrollBar();
        QScrollBar * verticalBar = verticalScrollBar();
        QPoint delta = e->pos() - lastMousePosition;
        lastMousePosition = e->pos();
        horizontalBar->setValue(horizontalBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
        verticalBar->setValue(verticalBar->value() - delta.y());
    }
    QGraphicsView::mousePressEvent(e);
}

void RepoWidgetRendering2D::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Plus :
    case Qt::Key_Equal :
        zoom(true); // zoom in
        break;
    case Qt::Key_Minus :
    case Qt::Key_Underscore :
        zoom(false); // zoom out
        break;
    case Qt::Key_0 :
        zoomReset();
        break;
    }
}
