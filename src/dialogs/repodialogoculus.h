/**
 *  Copyright (C) 2014 3D Repo Ltd
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


#ifndef REPO_DIALOG_OCULUS_H
#define REPO_DIALOG_OCULUS_H

//------------------------------------------------------------------------------
// Qt
#include <QDialog>

//------------------------------------------------------------------------------
// GUI
#include "../widgets/repo_glcwidget.h"

//------------------------------------------------------------------------------
namespace Ui {
    class RepoDialogOculus;
}

namespace repo {
namespace gui {


class RepoDialogOculus : public QDialog
{
    Q_OBJECT

public:

    explicit RepoDialogOculus(const RepoGLCWidget *glcWidget, QWidget *parent);

    ~RepoDialogOculus();

    int exec();

private:

    Ui::RepoDialogOculus *ui;

    const RepoGLCWidget *glcWidget;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_OCULUS_H
