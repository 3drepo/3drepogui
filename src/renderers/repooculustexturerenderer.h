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


#ifndef REPO_OCULUS_TEXTURE_RENDERER_H
#define REPO_OCULUS_TEXTURE_RENDERER_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>

#include "repo_oculus.h"

namespace repo {
namespace gui {

class RepoOculusTextureRenderer : public QWidget
{
    Q_OBJECT

public:

    RepoOculusTextureRenderer(QWidget* parent = 0);

    ~RepoOculusTextureRenderer();

signals :

    void keyPressed(QKeyEvent *);

    void mousePressed(QMouseEvent *);

    void mouseMoved(QMouseEvent *);

    void mouseReleased(QMouseEvent *);

    void wheelChanged(QWheelEvent *);

public slots:

    void setImage(int eye, const QImage &);

public :

    void connect(RepoOculus *);

    void disconnect(RepoOculus *);

    void keyPressEvent(QKeyEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

    void wheelEvent(QWheelEvent *e);


private :

    static QLabel* createLabel();

private :

    std::vector<QLabel*> eyes;

}; // end class

} // end namespace gui
} // end namespace repo

#endif // REPO_OCULUS_TEXTURE_RENDERER_H
