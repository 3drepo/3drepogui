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


#ifndef REPO_TRANSFORMATION_DIALOG_H
#define REPO_TRANSFORMATION_DIALOG_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#include <repo/core/model/bson/repo_node_transformation.h>

//------------------------------------------------------------------------------
// Qt
#include <QDialog>


namespace Ui {
    class RepoTransformationDialog;
}

namespace repo {
namespace gui {

class RepoTransformationDialog : public QDialog
{
    Q_OBJECT

public:

    explicit RepoTransformationDialog(
		const repo::core::model::bson::TransformationNode &transformation = repo::core::model::bson::TransformationNode(),
            QWidget *parent = 0);
    ~RepoTransformationDialog();

public slots :

    void setIdentity();

    void setName(const std::string &name);

    void setMatrix(const std::vector<float> &matrix);

    void setMatrix(double a1, double a2, double a3, double a4,
                   double b1, double b2, double b3, double b4,
                   double c1, double c2, double c3, double c4,
                   double d1, double d2, double d3, double d4);

	repo::core::model::bson::TransformationNode* getTransformation();

    std::vector<float> getMatrix() const;

	std::vector<std::vector<float>> getMatrix2D() const;

    std::string getName() const;

	//UI only allows rotation of 90 degrees, no point doing extra maths we don't need.
	void rotateX90();/* { rotateX(M_PI/2); }*/

    //void rotateX(double radians);

	void rotateY90(); /*{ rotateY(M_PI/2); }*/

    //void rotateY(double radians);

	void rotateZ90(); /*{ rotateZ(M_PI/2); }*/

    //void rotateZ(double radians);

    void inverse();

    void transpose();

private:

    Ui::RepoTransformationDialog *ui;

	repo::core::model::bson::TransformationNode transformation;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_TRANSFORMATION_DIALOG_H
