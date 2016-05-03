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

#include "../primitives/repo_fontawesome.h"

using namespace repo::gui;
using namespace repo::gui::widget;
using namespace repo::gui::renderer;

RepoClippingPlaneWidget::RepoClippingPlaneWidget(QWidget *parent)
	: QWidget(parent)
	, mdiArea(nullptr)
	, ui(new Ui::RepoClippingPlaneWidget)
{
	ui->setupUi(this);

	ui->reverseToolButton->setIcon(primitive::RepoFontAwesome::getInstance().getIcon(primitive::RepoFontAwesome::fa_exchange));

	QObject::connect(ui->reverseToolButton, &QPushButton::toggled,
		this, &RepoClippingPlaneWidget::setClippingPlane);

	QObject::connect(ui->horizontalSlider, &QSlider::valueChanged,
		ui->doubleSpinBox, &QDoubleSpinBox::setValue);

	QObject::connect(ui->doubleSpinBox,
		static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		ui->horizontalSlider, &QSlider::setValue);

	QObject::connect(ui->doubleSpinBox,
		static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &RepoClippingPlaneWidget::setClippingPlane);

	QObject::connect(ui->xRadioButton, &QRadioButton::toggled,
		this, &RepoClippingPlaneWidget::setClippingPlane);

	QObject::connect(ui->yRadioButton, &QRadioButton::toggled,
		this, &RepoClippingPlaneWidget::setClippingPlane);

	QObject::connect(ui->zRadioButton, &QRadioButton::toggled,
		this, &RepoClippingPlaneWidget::setClippingPlane);

	QObject::connect(ui->visibilityGroupBox, &QGroupBox::clicked,
		this, &RepoClippingPlaneWidget::setClippingPlaneEnabled);
}

RepoClippingPlaneWidget::~RepoClippingPlaneWidget()
{
    delete ui;    
}

repo::gui::renderer::Axis RepoClippingPlaneWidget::getAxis()
{
	Axis axis;
	if (ui->xRadioButton->isChecked())
		axis = Axis::X;
	else if (ui->yRadioButton->isChecked())
		axis = Axis::Y;
	else if (ui->zRadioButton->isChecked())
		axis = Axis::Z;
	return axis;
}

void RepoClippingPlaneWidget::setClippingPlaneEnabled(bool on)
{
	if (on && ui->visibilityGroupBox->isChecked())
	{
		QObject::connect(ui->linkToolButton, &QPushButton::toggled,
			this, &RepoClippingPlaneWidget::setClippingPlane);

		setClippingPlane();
	}
	else if (mdiArea)
	{
		QObject::disconnect(ui->linkToolButton, &QPushButton::toggled,
			this, &RepoClippingPlaneWidget::setClippingPlane);

		for (auto w : mdiArea->getWidgets<Rendering3DWidget*>())
		{
			w->setClippingPlaneVisibility(false);
		}
	}
}

void RepoClippingPlaneWidget::setClippingPlane()
{
	if (mdiArea)
	{
		Axis axis = getAxis();
		bool reverse = ui->reverseToolButton->isChecked();
		double value = ui->doubleSpinBox->value() / 100.00;
		value = reverse ? value : 1.0 - value;

		// If link is on, update all subwindows
		if (ui->linkToolButton->isChecked())
		{
			for (auto w : mdiArea->getWidgets<Rendering3DWidget*>())
			{
				w->updateClippingPlane(axis, value, reverse);
			}
		}
		else // otherwise update only active widget
		{
			Rendering3DWidget* widget = mdiArea->getActiveWidget();
			if (widget)
			{
				widget->updateClippingPlane(axis, value, reverse);
			}
		}
	}
}

void RepoClippingPlaneWidget::setLinkAction(QAction* actionLink)
{
	QObject::connect(ui->linkToolButton, &QPushButton::toggled,
		this, &RepoClippingPlaneWidget::setClippingPlane);
	ui->linkToolButton->setDefaultAction(actionLink);
}
