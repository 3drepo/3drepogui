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

#ifndef REPO_MDI_AREA_H
#define REPO_MDI_AREA_H

//------------------------------------------------------------------------------
#include "repo_mdisubwindow.h"
#include "../repo/widgets/repo_widget_rendering.h"
//------------------------------------------------------------------------------
#include <repo/repo_controller.h>
//------------------------------------------------------------------------------
#include <GLC_World>
//------------------------------------------------------------------------------
#include <QtGui>
#include <QUrl>
#include <QList>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
//------------------------------------------------------------------------------

namespace repo {
namespace gui {

/*!
 * Custom extension of the QMdiArea that provides convenience methods to manage
 * opengl widgets.
 */
class RepoMdiArea : public QMdiArea
{
	Q_OBJECT

public :
	
	//! Constructs an empty mdi area. Parent is passed to QWidget's constructor.
	RepoMdiArea(QWidget * parent = 0);

	//! Destroys the mdi area.
	~RepoMdiArea();	

public slots :

	/*!
     * Chains all subwindows that are of widgets::RepoRenderingWidget type.
	 */
	void chainSubWindows(bool);
		
	/*!
	 * Maximizes the subwindows. If three are visible, the last according
	 * to the window order becomes the main biggest view.
	 */
	void maximizeSubWindows(WindowOrder order = CreationOrder);

    //! Closes all subwindows that are not visible.
    void closeHiddenSubWindows();

public :

    //! Adds a widget into a sub window.
    RepoMdiSubWindow* addSubWidget(QWidget* widget);

	/*!
     * Adds a widgets::RepoRenderingWidget subWindow which is loaded via a separate thread
	 * using Assimp library from the given full file path.
	 */
	RepoMdiSubWindow* addSubWindow(repo::RepoController * controller, const QString& path);

	/*!
     * Adds a widgets::RepoRenderingWidget subWindow which is loaded via a separate thread
	 * from the database given a database.
	 */
    RepoMdiSubWindow* addSubWindow(
		repo::RepoController *controller,
		const repo::RepoToken      * token,
        const QString& database,
        const QString &project,
        const QUuid& id = QUuid(),
        bool headRevision = true);

    //! Adds a widgets::RepoRenderingWidget subWindow.
    RepoMdiSubWindow* addSubWindow(widgets::RepoRenderingWidget *widget);

	/*!
	 * Adds widget as a new subwindow to the MDI area. If windowFlags are 
	 * non-zero, they will override the flags set on the widget. The widget can
	 * be either a QMdiSubWindow or another QWidget (in which case the MDI area
	 * will create a subwindow and set the widget as the internal widget).
	 */
    QMdiSubWindow *addSubWindow(QWidget *widget, Qt::WindowFlags windowFlags = 0)
	{ return QMdiArea::addSubWindow(widget, windowFlags); }

    //! Adds a web view subwindow
    RepoMdiSubWindow* addWebViewSubWindow();

	/*!
	 * Returns a list of all subwindows in the MDI area. If order is 
	 * CreationOrder (the default), the windows are sorted in the order in 
	 * which they were inserted into the workspace. If order is StackingOrder,
	 * the windows are listed in their stacking order, with the topmost window 
	 * as the last item in the list. If order is ActivationHistoryOrder, the 
	 * windows are listed according to their recent activation history.
	 *
	 * See also WindowOrder.
	 */
	QList<RepoMdiSubWindow *> subWindowList(
		bool onlyVisible = false,
		WindowOrder order = CreationOrder) const; //ActivationHistoryOrder) const;

	/*!
	 * Returns a vector of currently opened widgets that are of the specified
	 * type.
	 */
	template <class T> 
	std::vector<T> getWidgets(WindowOrder order = CreationOrder) const
	{
		std::vector<T> openWidgets;

        QList<RepoMdiSubWindow*> subwindows = subWindowList(order);
        QList<RepoMdiSubWindow*>::iterator it;

        for (it = subwindows.begin(); it != subwindows.end(); ++it)
        {
            RepoMdiSubWindow * window = *it;
        //for each (RepoMdiSubWindow * window in subwindows)
        //{
			T widget = dynamic_cast<T>(window->widget());
			if (NULL != widget)
				openWidgets.push_back(widget);
		}
		return openWidgets;
	}

	//! Returns a requested widget if from the active RepoMdiSubWindow. Returns NULL if cannot convert.
	template <class T> 
	T activeSubWidget() const 
	{
		T widget = NULL;
		repo::gui::RepoMdiSubWindow* activeWindow = dynamic_cast<repo::gui::RepoMdiSubWindow*>(QMdiArea::activeSubWindow());
		if (activeWindow)
			widget = activeWindow->widget<T>();
		return widget;
	}

    //! Returns active subwindow.
    RepoMdiSubWindow *activeSubWindow() const;

protected :

	//! Updates the position of the background image on resize.
	virtual void resizeEvent(QResizeEvent * resizeEvent);

private :

	//! Logo image at the bottom right corner of the workspace area.
	QImage logo;

	//! FPS timer
    QTimer fpsTimer;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_MDI_AREA_H
