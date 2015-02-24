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


#ifndef REPO_SELECTION_TREE_DOCK_WIDGET_H
#define REPO_SELECTION_TREE_DOCK_WIDGET_H

//------------------------------------------------------------------------------
// Qt
#include <QDockWidget>

//------------------------------------------------------------------------------
// Core
#include <RepoNodeAbstract>

//------------------------------------------------------------------------------
// GUI
#include "repo_glcwidget.h"

namespace Ui {
    class RepoSelectionTreeDockWidget;
}

namespace repo {
namespace gui {

class RepoSelectionTreeDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    explicit RepoSelectionTreeDockWidget(
            RepoGLCWidget *glcWidget,
            QWidget *parent = 0);

    ~RepoSelectionTreeDockWidget();

public slots :

    void addNode(QStandardItem *parentItem, const core::RepoNodeAbstract* node);


private:

    Ui::RepoSelectionTreeDockWidget *ui;

    RepoGLCWidget* glcWidget;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_SELECTION_TREE_DOCK_WIDGET_H
