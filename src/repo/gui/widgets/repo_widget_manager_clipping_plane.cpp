/**
 *  Copyright (C) 2016 3D Repo Ltd
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

#include "repo_widget_manager_clipping_plane.h"
#include "ui_repo_widget_manager_clipping_plane.h"

using namespace repo::gui;
using namespace repo::gui::widget;
using namespace repo::gui::renderer;

RepoClippingPlaneWidget::RepoClippingPlaneWidget(
        repo::gui::widget::RepoMdiArea *mdiArea,
        QWidget *parent) :
    QWidget(parent),
    mdiArea(mdiArea),
    ui(new Ui::RepoClippingPlaneWidget)
{
    ui->setupUi(this);

    QObject::connect(ui->horizontalSlider, &QSlider::valueChanged,
                     ui->doubleSpinBox, &QDoubleSpinBox::setValue);

    QObject::connect(ui->doubleSpinBox,
                     static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     ui->horizontalSlider, &QSlider::setValue);

    QObject::connect(ui->doubleSpinBox,
                     static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &RepoClippingPlaneWidget::setClipping);


}

RepoClippingPlaneWidget::~RepoClippingPlaneWidget()
{
    delete ui;
}

void RepoClippingPlaneWidget::setClipping()
{
    Rendering3DWidget* widget = mdiArea->getActiveWidget();
    if (widget)
    {
        widget->updateClippingPlane(getAxis(), ui->doubleSpinBox->value()/100.00);
    }
}

repo::gui::renderer::Axis RepoClippingPlaneWidget::getAxis()
{
    Axis axis;
    if (ui->xRadioButton->isChecked())
        axis = Axis::X;
    else if (ui->yRadioButton->isChecked())
        axis = Axis::Y;
    else
        axis = Axis::Z;
    return axis;
}
