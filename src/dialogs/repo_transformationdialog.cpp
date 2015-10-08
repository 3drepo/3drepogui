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

#include <repo/core/model/bson/repo_bson_factory.h>
#include <qmatrix4x4.h>

repo::gui::RepoTransformationDialog::RepoTransformationDialog(
	const repo::core::model::TransformationNode &transformation,
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RepoTransformationDialog)
    , transformation(transformation)
{
    ui->setupUi(this);

    setName(transformation.getName());
    setMatrix(transformation.getTransMatrix(false));

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

void repo::gui::RepoTransformationDialog::setMatrix(const std::vector<float> &m)
{
	if (m.size() >=16)
	{
		setMatrix( m[0] , m[1] , m[2] , m[3] ,
			       m[4] , m[5] , m[6] , m[7] ,
			       m[8] , m[9] , m[10], m[11],
			       m[12], m[13], m[14], m[15]);
	}
    
}

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

repo::core::model::TransformationNode repo::gui::RepoTransformationDialog::getTransformation()
{
	return repo::core::model::RepoBSONFactory::makeTransformationNode(getMatrix2D(), getName());
}

std::vector<float> repo::gui::RepoTransformationDialog::getMatrix() const
{
	std::vector<float> mat;
	mat.push_back(ui->a1DoubleSpinBox->value());
	mat.push_back(ui->a2DoubleSpinBox->value());
	mat.push_back(ui->a3DoubleSpinBox->value());
	mat.push_back(ui->a4DoubleSpinBox->value());
	mat.push_back(ui->b1DoubleSpinBox->value());
	mat.push_back(ui->b2DoubleSpinBox->value());
	mat.push_back(ui->b3DoubleSpinBox->value());
	mat.push_back(ui->b4DoubleSpinBox->value());
	mat.push_back(ui->c1DoubleSpinBox->value());
	mat.push_back(ui->c2DoubleSpinBox->value());
	mat.push_back(ui->c3DoubleSpinBox->value());
	mat.push_back(ui->c4DoubleSpinBox->value());
	mat.push_back(ui->d1DoubleSpinBox->value());
	mat.push_back(ui->d2DoubleSpinBox->value());
	mat.push_back(ui->d3DoubleSpinBox->value());
	mat.push_back(ui->d4DoubleSpinBox->value());

	return mat;
}

std::vector<std::vector<float>> repo::gui::RepoTransformationDialog::getMatrix2D() const
{
	std::vector<std::vector<float>> mat;

	std::vector<float> rowA;
	rowA.push_back(ui->a1DoubleSpinBox->value());
	rowA.push_back(ui->a2DoubleSpinBox->value());
	rowA.push_back(ui->a3DoubleSpinBox->value());
	rowA.push_back(ui->a4DoubleSpinBox->value());
	mat.push_back(rowA);

	std::vector<float> rowB;
	rowB.push_back(ui->b1DoubleSpinBox->value());
	rowB.push_back(ui->b2DoubleSpinBox->value());
	rowB.push_back(ui->b3DoubleSpinBox->value());
	rowB.push_back(ui->b4DoubleSpinBox->value());
	mat.push_back(rowB);

	std::vector<float> rowC;
	rowC.push_back(ui->c1DoubleSpinBox->value());
	rowC.push_back(ui->c2DoubleSpinBox->value());
	rowC.push_back(ui->c3DoubleSpinBox->value());
	rowC.push_back(ui->c4DoubleSpinBox->value());
	mat.push_back(rowC);

	std::vector<float> rowD;
	rowD.push_back(ui->d1DoubleSpinBox->value());
	rowD.push_back(ui->d2DoubleSpinBox->value());
	rowD.push_back(ui->d3DoubleSpinBox->value());
	rowD.push_back(ui->d4DoubleSpinBox->value());
	mat.push_back(rowD);

	return mat;
}

std::string repo::gui::RepoTransformationDialog::getName() const
{
    return ui->nameLineEdit->text().toStdString();
}

//void repo::gui::RepoTransformationDialog::rotateX(double radians)
//{
//	/*aiMatrix4x4 tmp;
//    setMatrix(getMatrix() * aiMatrix4x4::RotationX(radians, tmp));*/
//}
//
//void repo::gui::RepoTransformationDialog::rotateY(double radians)
//{
//	/*aiMatrix4x4 tmp;
//    setMatrix(getMatrix() * aiMatrix4x4::RotationY(radians, tmp));*/
//}
//
//void repo::gui::RepoTransformationDialog::rotateZ(double radians)
//{
//	/*aiMatrix4x4 tmp;
//    setMatrix(getMatrix() * aiMatrix4x4::RotationZ(radians, tmp));*/
//}

void repo::gui::RepoTransformationDialog::rotateX90()
{
	std::vector<float> mat = getMatrix();
	//essentially, swap col 2 and 3, negate the 2nd row.

	for (uint32_t row = 0; row < 4; row++)
	{
		float tmp = mat[row * 4 + 1];
		mat[row * 4 + 1] = mat[row * 4 + 2];
		mat[row * 4 + 2] = -tmp;
	}

	setMatrix(mat);
}	

void repo::gui::RepoTransformationDialog::rotateY90()
{
	std::vector<float> mat = getMatrix();
	//essentially, swap col 1 and 3.

	for (uint32_t row = 0; row < 4; row++)
	{
		float tmp = mat[row * 4];
		mat[row * 4] = -mat[row * 4 + 2];
		mat[row * 4 + 2] = tmp;
	}

	setMatrix(mat);
}

void repo::gui::RepoTransformationDialog::rotateZ90()
{
	std::vector<float> mat = getMatrix();
	//essentially, swap col 1 and 2.

	for (uint32_t row = 0; row < 4; row++)
	{
		float tmp = mat[row * 4];
		mat[row * 4] = mat[row * 4 + 1];
		mat[row * 4 + 1] = -tmp;
	}

	setMatrix(mat);
}


void repo::gui::RepoTransformationDialog::inverse()
{

	//Use QMatrix instead of implementing something complicated myself..

	std::vector<float> m = getMatrix();
	QMatrix4x4 qMat(&m[0]);
	memcpy(&m.at(0), qMat.inverted().data(), sizeof(float) * 16);
	//Transposing as QMatrix transposed the matrix so we need to flip it again
	transposeMat(m);
	setMatrix(m);
}

void repo::gui::RepoTransformationDialog::transpose()
{
	std::vector<float> m = getMatrix();
	transposeMat(m);
	setMatrix(m);
}

void repo::gui::RepoTransformationDialog::transposeMat(std::vector<float> &mat)
{

	for (uint32_t row = 0; row < 3; row++)
	{
		for (uint32_t col = row + 1; col < 4; col++)
		{
			uint32_t index = row * 4 + col; //if index is 3 . (row = 0, col = 3)
			uint32_t swapTo = col * 4 + row; // swap is 12

			float tmp = mat[swapTo];
			mat[swapTo] = mat[index];
			mat[index] = tmp;

		}
	}
}
