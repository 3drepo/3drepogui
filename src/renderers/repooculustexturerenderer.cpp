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


#include "repooculustexturerenderer.h"

repo::gui::RepoOculusTextureRenderer::RepoOculusTextureRenderer(QWidget* parent)
    : QWidget(parent)
{
    eyes.push_back(createLabel());
    eyes.push_back(createLabel());

    QHBoxLayout *vbl = new QHBoxLayout();
    vbl->setSpacing(0);
    vbl->setMargin(0);
    vbl->addWidget(eyes[0]);
    vbl->addWidget(eyes[1]);
    this->setLayout(vbl);
}

repo::gui::RepoOculusTextureRenderer::~RepoOculusTextureRenderer()
{
    for (unsigned int i = 0; i < eyes.size(); ++i)
    {
        if (eyes[i])
            delete eyes[i];
    }
    delete (QVBoxLayout*)(this->layout());
}

void repo::gui::RepoOculusTextureRenderer::setImage(int eye, const QImage& image)
{
    eyes[eye]->setPixmap(QPixmap::fromImage(image));
    this->update();
}

QLabel* repo::gui::RepoOculusTextureRenderer::createLabel()
{
    QLabel* label = new QLabel();
    label->setBackgroundRole(QPalette::Base);
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    label->setScaledContents(true);
    return label;
}

void repo::gui::RepoOculusTextureRenderer::connect(RepoOculus *oculus)
{
    QObject::connect(
        oculus, &RepoOculus::eyeTextureChanged,
                this,&RepoOculusTextureRenderer::setImage);

    QObject::connect(this, &RepoOculusTextureRenderer::keyPressed,
                     oculus, &RepoOculus::keyPressEvent);

    QObject::connect(this, &RepoOculusTextureRenderer::mousePressed,
                     oculus, &RepoOculus::mousePressEvent);

    QObject::connect(this, &RepoOculusTextureRenderer::mouseMoved,
                     oculus, &RepoOculus::mouseMoveEvent);

    QObject::connect(this, &RepoOculusTextureRenderer::mouseReleased,
                     oculus, &RepoOculus::mouseReleaseEvent);

    QObject::connect(this, &RepoOculusTextureRenderer::wheelChanged,
                     oculus, &RepoOculus::wheelEvent);
}

void repo::gui::RepoOculusTextureRenderer::disconnect(RepoOculus *oculus)
{
    QObject::disconnect(
        oculus, &RepoOculus::eyeTextureChanged,
                this,&RepoOculusTextureRenderer::setImage);

    QObject::disconnect(this, &RepoOculusTextureRenderer::keyPressed,
                     oculus, &RepoOculus::keyPressEvent);

    QObject::disconnect(this, &RepoOculusTextureRenderer::mousePressed,
                     oculus, &RepoOculus::mousePressEvent);

    QObject::disconnect(this, &RepoOculusTextureRenderer::mouseMoved,
                     oculus, &RepoOculus::mouseMoveEvent);

    QObject::disconnect(this, &RepoOculusTextureRenderer::mouseReleased,
                     oculus, &RepoOculus::mouseReleaseEvent);

    QObject::disconnect(this, &RepoOculusTextureRenderer::wheelChanged,
                     oculus, &RepoOculus::wheelEvent);
}

void repo::gui::RepoOculusTextureRenderer::keyPressEvent(QKeyEvent *e)
{
    emit keyPressed(e);
    QWidget::keyPressEvent(e);
}

void repo::gui::RepoOculusTextureRenderer::mousePressEvent(QMouseEvent *e)
{
    emit mousePressed(e);
    QWidget::mousePressEvent(e);
}

void repo::gui::RepoOculusTextureRenderer::mouseMoveEvent(QMouseEvent *e)
{
    emit mouseMoved(e);
    QWidget::mouseMoveEvent(e);
}

void repo::gui::RepoOculusTextureRenderer::mouseReleaseEvent(QMouseEvent *e)
{
    emit mouseReleased(e);
    QWidget::mouseReleaseEvent(e);
}

void repo::gui::RepoOculusTextureRenderer::wheelEvent(QWheelEvent *e)
{
    emit wheelChanged(e);
    QWidget::wheelEvent(e);
}
