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


#include "repo_transformationdialog.h"
#include "ui_repo_transformationdialog.h"

repo::gui::RepoTransformationDialog::RepoTransformationDialog(
        /*const core::RepoNodeTransformation &transformation,*/
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RepoTransformationDialog)
    //, transformation(transformation)
{
    ui->setupUi(this);

    //setName(transformation.getName());
    //setMatrix(transformation.getMatrix());

    QObject::connect(ui->identityPushButton, &QPushButton::pressed,
                     this, &RepoTransformationDialog::setIdentity);

    QObject::connect(ui->rotateXpushButton, &QPushButton::pressed,
                     this, &RepoTransformationDialog::rotateX90);

    QObject::connect(ui->rotateYpushButton, &QPushButton::pressed,
                     this, &RepoTransformationDialog::rotateY90);

    QObject::connect(ui->rotateZpushButton, &QPushButton::pressed,
                     this, &RepoTransformationDialog::rotateZ90);

    QObject::connect(ui->inversePushButton, &QPushButton::pressed,
                     this, &RepoTransformationDialog::inverse);

    QObject::connect(ui->transposePushButton, &QPushButton::pressed,
                     this, &RepoTransformationDialog::transpose);
}

repo::gui::RepoTransformationDialog::~RepoTransformationDialog()
{
    delete ui;
}

void repo::gui::RepoTransformationDialog::setIdentity()
{
    setMatrix(1.0, 0.0, 0.0, 0.0,
              0.0, 1.0, 0.0, 0.0,
              0.0, 0.0, 1.0, 0.0,
              0.0, 0.0, 0.0, 1.0);
}

void repo::gui::RepoTransformationDialog::setName(const std::string &name)
{
    ui->nameLineEdit->setText(QString::fromStdString(name));
}
//
//void repo::gui::RepoTransformationDialog::setMatrix(const aiMatrix4x4 &m)
//{
//    setMatrix(m.a1,m.a2, m.a3, m.a4,
//              m.b1, m.b2, m.b3, m.b4,
//              m.c1, m.c2, m.c3, m.c4,
//              m.d1, m.d2, m.d3, m.d4);
//}

void repo::gui::RepoTransformationDialog::setMatrix(
        double a1, double a2, double a3, double a4,
        double b1, double b2, double b3, double b4,
        double c1, double c2, double c3, double c4,
        double d1, double d2, double d3, double d4)
{
    ui->a1DoubleSpinBox->setValue(a1);
    ui->a2DoubleSpinBox->setValue(a2);
    ui->a3DoubleSpinBox->setValue(a3);
    ui->a4DoubleSpinBox->setValue(a4);

    ui->b1DoubleSpinBox->setValue(b1);
    ui->b2DoubleSpinBox->setValue(b2);
    ui->b3DoubleSpinBox->setValue(b3);
    ui->b4DoubleSpinBox->setValue(b4);

    ui->c1DoubleSpinBox->setValue(c1);
    ui->c2DoubleSpinBox->setValue(c2);
    ui->c3DoubleSpinBox->setValue(c3);
    ui->c4DoubleSpinBox->setValue(c4);

    ui->d1DoubleSpinBox->setValue(d1);
    ui->d2DoubleSpinBox->setValue(d2);
    ui->d3DoubleSpinBox->setValue(d3);
    ui->d4DoubleSpinBox->setValue(d4);
}

//repo::core::RepoNodeTransformation repo::gui::RepoTransformationDialog::getTransformation()
//{
//    transformation.setName(getName());
//    transformation.setMatrix(getMatrix());
//    return transformation;
//}

//aiMatrix4x4 repo::gui::RepoTransformationDialog::getMatrix() const
//{
//    return aiMatrix4x4(ui->a1DoubleSpinBox->value(),
//                  ui->a2DoubleSpinBox->value(),
//                  ui->a3DoubleSpinBox->value(),
//                  ui->a4DoubleSpinBox->value(),
//                  ui->b1DoubleSpinBox->value(),
//                  ui->b2DoubleSpinBox->value(),
//                  ui->b3DoubleSpinBox->value(),
//                  ui->b4DoubleSpinBox->value(),
//                  ui->c1DoubleSpinBox->value(),
//                  ui->c2DoubleSpinBox->value(),
//                  ui->c3DoubleSpinBox->value(),
//                  ui->c4DoubleSpinBox->value(),
//                  ui->d1DoubleSpinBox->value(),
//                  ui->d2DoubleSpinBox->value(),
//                  ui->d3DoubleSpinBox->value(),
//                  ui->d4DoubleSpinBox->value());
//}

std::string repo::gui::RepoTransformationDialog::getName() const
{
    return ui->nameLineEdit->text().toStdString();
}

void repo::gui::RepoTransformationDialog::rotateX(double radians)
{
	/*aiMatrix4x4 tmp;
    setMatrix(getMatrix() * aiMatrix4x4::RotationX(radians, tmp));*/
}

void repo::gui::RepoTransformationDialog::rotateY(double radians)
{
	/*aiMatrix4x4 tmp;
    setMatrix(getMatrix() * aiMatrix4x4::RotationY(radians, tmp));*/
}

void repo::gui::RepoTransformationDialog::rotateZ(double radians)
{
	/*aiMatrix4x4 tmp;
    setMatrix(getMatrix() * aiMatrix4x4::RotationZ(radians, tmp));*/
}


void repo::gui::RepoTransformationDialog::inverse()
{
    //setMatrix(getMatrix().Inverse());
}

void repo::gui::RepoTransformationDialog::transpose()
{
    //setMatrix(getMatrix().Transpose());
}
