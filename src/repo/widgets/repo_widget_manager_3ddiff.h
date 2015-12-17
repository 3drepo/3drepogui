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


#pragma once

//------------------------------------------------------------------------------
// Qt
#include <QWidget>

//------------------------------------------------------------------------------
// GUI
#include "../../widgets/repo_mdiarea.h"
#include "../../renderers/repo_3ddiffrenderer.h"
#include "../workers/repo_multithreader.h"

namespace Ui {
class RepoWidgetManager3DDiff;
}

namespace repo {
namespace widgets {


class RepoWidgetManager3DDiff : public QWidget, public repo::worker::RepoMultithreader
{
    Q_OBJECT

    enum class Visualization { DIFF, CORRESPONDENCE };

    enum class Algorithm { BASIC, VISUAL, STATISTICAL };

public:

    explicit RepoWidgetManager3DDiff(
            repo::gui::RepoMdiArea *mdiArea,
            repo::RepoController *controller,
            const repo::RepoToken *token,
            QWidget *parent = 0);

    ~RepoWidgetManager3DDiff();

public slots :

    //! Populates selection combo boxes with available models.
    void populateModelComboBoxes();

    //! Reset the models within the widgets to its original states
    void resetModels();

    //! Run the selected diff algorithm.
    void runDiff();

private :

    void runBouncerDiff(gui::widgets::RepoRenderingWidget *widgetA,
                        gui::widgets::RepoRenderingWidget *widgetB,
                        manipulator::diff::Mode diffMode,
                        bool colourCorrespondence);

public :

    QList<repo::gui::RepoMdiSubWindow*> getSubWindows() const;

    //! Returns currectly selected visualization.
    Visualization getVisualization() const;

    //! Returns currectly selected diffing algorithm.
    Algorithm getDiffAlgorithm() const;

    repo::gui::widgets::RepoRenderingWidget* getModelWidget(int index) const;

    repo::gui::widgets::RepoRenderingWidget* getSelectedModelAWidget() const
    {    return getModelWidget(getSelectedModelAIndex()); }

    QString getSelectedModelAString() const;

    int getSelectedModelAIndex() const;

    repo::gui::widgets::RepoRenderingWidget* getSelectedModelBWidget() const
    {    return getModelWidget(getSelectedModelBIndex()); }

    QString getSelectedModelBString() const;

    int getSelectedModelBIndex() const;

private:

    Ui::RepoWidgetManager3DDiff *ui;

    repo::gui::RepoMdiArea *mdiArea;

    RepoController *controller;

    const RepoToken *token;

};

} // widgets
} // repo

