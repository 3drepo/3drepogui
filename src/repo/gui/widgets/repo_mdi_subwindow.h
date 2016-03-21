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

#pragma once

//------------------------------------------------------------------------------
// Core
#include <repo/repo_controller.h>

//------------------------------------------------------------------------------
// GUI

//------------------------------------------------------------------------------
#include <GLC_World>
//------------------------------------------------------------------------------
#include <QtGui>
#include <QThreadPool>
#include <QVBoxLayout>
#include <QBoxLayout>
#include <QtWidgets/QMdiSubWindow>
#include <QProgressBar>
//------------------------------------------------------------------------------

#include "repo_widget_rendering.h"

namespace repo {
namespace gui {
namespace widget{

/*!
        * Custom extension of the QMdiSubWindow for internal windowing mechanism that
        * enables progress bar and other custom features for smooth asynchronous GUI
        * within the main application window.
        */
class RepoMdiSubWindow : public QMdiSubWindow
{
    Q_OBJECT

public:

    //! Default constructor.
    RepoMdiSubWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);

    //! Destructor.
    ~RepoMdiSubWindow();

signals:

    //! Emitted when deleting this object.
    void aboutToDelete();
    void updateOffsetVector(
            const std::vector<double>&,
            RepoMdiSubWindow*);

public:

    //--------------------------------------------------------------------------
    //
    // Setters
    //
    //--------------------------------------------------------------------------

    void setOffsetVector(
            const std::vector<double> &offsetVector) {
        repoLog("Subwindow: offset vector is now being set to"
                + std::to_string(offsetVector[0]) + ", "
                + std::to_string(offsetVector[1]) + ", "
                + std::to_string(offsetVector[2]));
        this->offsetVector = offsetVector;}

    void setWidget3D(
            const QString& windowTitle,
            repo::gui::renderer::NavMode navMode,
            const std::vector<double> &offsetVector);

    void setWidget2D(const core::model::RepoScene *scene, const QString &windowTitle);

    //! Asynchronously loads 3D file into a RepoGCLWidget.
    /*!
            * Loads a 3D file from the given file path using Assimp via a separate
            * thread. During the loading a progress bar is visible counting the
            * progress of individual processes and format conversions
            * from Assimp aiScene to GLC_World and RepoGraphScene.
            */
    void setWidgetFromFile(const QString& fullFilePath,
                           repo::RepoController *controller,
                           repo::gui::renderer::NavMode navMode,
                           const std::vector<double>   &offsetVector);

    /*!
            * Sets the widget as the internal widget of this subwindow. The internal
            * widget is displayed in the center of the subwindow beneath the title
            * bar and above the progress bar.
            *
            * RepoMdiSubWindow takes temporary ownership of the widget; you do not
            * need to delete it.
            */
    void setWidget(QWidget* widget);

    //! Deletes the internal widget if any.
    void removeWidget();

    //--------------------------------------------------------------------------
    //
    // Getters
    //
    //--------------------------------------------------------------------------

    //! Returns the current internal widget.
    QWidget * widget() const;

    /*!
            * Returns the current internal widget cast to the requested type,
            * potentially NULL (if not set or not of the desired type).
            */
    template <class T>
    T widget() const
    {
        return dynamic_cast<T>(widget());
    }


public slots :

    //! Sets the two scene representations on the widget.
    void finishedLoadingScene(repo::core::model::RepoScene *);

    /*!
            * Updates the current state of the progress bar with the values specified.
            * This method makes the progress bar visible unless the value is non-zero
            * and equals the maximum in which case the progress bar is hidden.
            */
    void progress(int value, int maximum);

protected:

    void closeEvent(QCloseEvent *closeEvent);    

private:

    //--------------------------------------------------------------------------
    //
    // Private variables
    //
    //--------------------------------------------------------------------------

     std::vector<double>    offsetVector;

    QBoxLayout * boxLayout; //!< Box layout of the window.

    QProgressBar * progressBar; //!< Progress bar

    bool awaitingClose;
};
}
} // end namespace gui
} // end namespace repo
