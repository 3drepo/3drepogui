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

#include "repo_mdiarea.h"
#include "../repo/workers/repo_worker_scene_graph.h"
#include "../renderers/repo_glcwidget.h"

#include "../renderers/repo_oculus.h"
#include "../renderers/repooculustexturerenderer.h"
#include "../renderers/repo_webview.h"


repo::gui::RepoMdiArea::RepoMdiArea(QWidget * parent)
	: QMdiArea(parent)
    , logo(":/images/3drepo-bg.png")
{
	setTabsMovable(true);
	setAcceptDrops(true);
	setTabsClosable(true);
	setFrameShadow(QFrame::Plain);
	setFrameShape(QFrame::NoFrame);
	setFocusPolicy(Qt::StrongFocus);
	setViewMode(QMdiArea::SubWindowView);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	this->setActivationOrder(QMdiArea::StackingOrder);

    //--------------------------------------------------------------------------
	// Needed for 3D file loading and signal passing.
	qRegisterMetaType<GLC_World>("GLC_World&");
	//qRegisterMetaType<repo::core::RepoGraphScene>("repo::core::RepoGraphScene*");

	qRegisterMetaType<QVector<GLfloat>>("QVector<GLfloat>");


    //--------------------------------------------------------------------------
	// RepoGLCWidget selection
	qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
	qRegisterMetaType<const repo::gui::RepoGLCWidget*>("const repo::gui::RepoGLCWidget*");


    //--------------------------------------------------------------------------
    // FPS testing only
    // Do not use in production as it keeps redrawing constantly
//    if (QGuiApplication::screens().size() > 0)
//        fpsTimer.start(1/(QGuiApplication::screens()[0]->refreshRate()) * 1000);
}

repo::gui::RepoMdiArea::~RepoMdiArea()
{
    fpsTimer.stop();
}

//------------------------------------------------------------------------------
//
// Public slots
//
//------------------------------------------------------------------------------

void repo::gui::RepoMdiArea::chainSubWindows(bool checked)
{
	std::vector<RepoGLCWidget *> visited;
    std::vector<RepoGLCWidget*>::size_type i, j;
    std::vector<RepoGLCWidget*> widgets = getWidgets<RepoGLCWidget*>();
    for (j = 0; j < widgets.size(); ++j)
    {
        RepoGLCWidget * widget = widgets[j];
        // dis/connect hooks to all previous windows
		for(i = 0; i != visited.size(); ++i)
		{
			widget->linkCameras(visited.at(i), checked);
			visited.at(i)->linkCameras(widget, checked);
		}
		visited.push_back(widget); // store current window
	}
}

void repo::gui::RepoMdiArea::maximizeSubWindows(WindowOrder order)
{
	QList<RepoMdiSubWindow *> openWindows = subWindowList(true, order);

	QSize size = this->size();
	int widthHalf = size.width()/2;
	int heightFull = size.height();
	int heightHalf = size.height()/2;

	int widthThird = (size.width() / 3);
	int widthTwoThirds = (2 * size.width() / 3);

	switch (openWindows.size())
	{
		case 1 :
			openWindows[0]->showMaximized();
			break;
		case 2 :
			openWindows[0]->resize(widthHalf, heightFull);
			openWindows[0]->move((QPoint(0, 0)));
			openWindows[1]->resize(widthHalf, heightFull);
			openWindows[1]->move((QPoint(widthHalf, 0)));
			break;
		case 3 :
			openWindows[0]->resize( widthThird, heightHalf);
			openWindows[0]->move((QPoint(0, 0)));
			openWindows[1]->resize(widthThird, heightHalf);
			openWindows[1]->move((QPoint(0, heightHalf)));
			openWindows[2]->resize(widthTwoThirds, heightFull);
			openWindows[2]->move((QPoint(widthThird, 0)));
			break;
		default:
			this->tileSubWindows();
			break;
	}

//	qlonglong polyCount = 0;
//	qlonglong meshesCount = 0;
//    for (QList<RepoMdiSubWindow *>::iterator it = openWindows.begin();
//         it != openWindows.end();
//         ++it)
//	{
//        RepoMdiSubWindow *subWindow = *it;
//        RepoGLCWidget *widget = subWindow->widget<RepoGLCWidget*>();
//        std::vector<core::RepoNodeAbstract *> meshes = widget->getRepoScene()->getMeshesVector();
//        for (std::vector<core::RepoNodeAbstract *>::size_type i = 0;
//             i < meshes.size(); ++i)
//        {
//            core::RepoNodeMesh *m = static_cast<core::RepoNodeMesh*>(meshes[i]);
//            polyCount += m->getFaces()->size();
//			++meshesCount;
//		}
//	}

//    //--------------------------------------------------------------------------
//	// Polygon count
//    std::cout << polyCount << " polygons in " << meshesCount << " " ;
//    std::cout << ((meshesCount == 1) ? "mesh" : "meshes") << std::endl;

	this->update();
	this->repaint();
}

void repo::gui::RepoMdiArea::closeHiddenSubWindows()
{
    QList<RepoMdiSubWindow*> subwindows = this->subWindowList();
    for (int i = 0; i < subwindows.size(); ++i)
    {
        RepoMdiSubWindow* subwindow = subwindows[i];
        if (!subwindow->isVisible())
            subwindow->close();
    }
}

//------------------------------------------------------------------------------
//
// SubWindow management
//
//------------------------------------------------------------------------------

repo::gui::RepoMdiSubWindow* repo::gui::RepoMdiArea::addSubWidget(QWidget* widget)
{
    RepoMdiSubWindow * subWindow = new RepoMdiSubWindow();
    widget->setParent(subWindow);
    subWindow->setWidget(widget);

    QObject::connect(widget, &QWidget::windowTitleChanged,
                     subWindow, &RepoMdiSubWindow::setWindowTitle);

    QMdiArea::addSubWindow(subWindow);
    subWindow->show();

    this->update();
    this->repaint();
    return subWindow;
}

repo::gui::RepoMdiSubWindow* repo::gui::RepoMdiArea::addSubWindow(
	repo::RepoController *controller,
    const QString& fullPath)
{
    RepoMdiSubWindow *repoSubWindow = new RepoMdiSubWindow();
    repoSubWindow->setWidgetFromFile(fullPath, controller);
	QMdiArea::addSubWindow(repoSubWindow);
	repoSubWindow->show();

	QObject::connect(
        &fpsTimer, &QTimer::timeout,
        repoSubWindow->widget<QGLWidget*>(), &RepoGLCWidget::updateGL);

	this->update();
	this->repaint();
	return repoSubWindow;
}

repo::gui::RepoMdiSubWindow * repo::gui::RepoMdiArea::addSubWindow(
	repo::RepoController *controller,
	const repo::RepoToken      * token,
	const QString& database,
    const QString& project,
	const QUuid& id,
	bool headRevision)
{
	RepoMdiSubWindow* repoSubWindow = new RepoMdiSubWindow();
	repoSubWindow->setWidget(database + " " + id.toString());
	QMdiArea::addSubWindow(repoSubWindow);
	repoSubWindow->show();

	QObject::connect(
            &fpsTimer, &QTimer::timeout,
            repoSubWindow->widget<QGLWidget*>(), &RepoGLCWidget::updateGL);

    //--------------------------------------------------------------------------
	// Establish and connect the new worker.
   repo::worker::SceneGraphWorker* worker = 
	   new repo::worker::SceneGraphWorker(controller, token, database, project, id, headRevision);
	connect(worker, SIGNAL(finished(repo::core::model::RepoScene *)),
		repoSubWindow, SLOT(finishedLoadingScene(repo::core::model::RepoScene *)));
	connect(worker, SIGNAL(progress(int, int)), repoSubWindow, SLOT(progress(int, int)));

	QObject::connect(
		repoSubWindow, &RepoMdiSubWindow::aboutToDelete,
		worker, &repo::worker::SceneGraphWorker::cancel, Qt::DirectConnection);

    //--------------------------------------------------------------------------
	// Fire up the asynchronous calculation.
	QThreadPool::globalInstance()->start(worker);

	this->update();
	this->repaint();
	return repoSubWindow;
}

repo::gui::RepoMdiSubWindow * repo::gui::RepoMdiArea::addSubWindow(
	RepoGLCWidget* widget)
{
	RepoMdiSubWindow * repoSubWindow = new RepoMdiSubWindow();
	repoSubWindow->setWidget(widget);
	QMdiArea::addSubWindow(repoSubWindow);
	repoSubWindow->show();

	QObject::connect(
            &fpsTimer, &QTimer::timeout,
			repoSubWindow->widget<RepoGLCWidget*>(), &RepoGLCWidget::updateGL);

	this->update();
	this->repaint();
	return repoSubWindow;
}

repo::gui::RepoMdiSubWindow* repo::gui::RepoMdiArea::addOculusTextureSubWindow()
{
    return addSubWidget(new RepoOculusTextureRenderer());
}

repo::gui::RepoMdiSubWindow* repo::gui::RepoMdiArea::addWebViewSubWindow()
{
    return addSubWidget(new RepoWebView());
}

repo::gui::RepoMdiSubWindow *repo::gui::RepoMdiArea::activeSubWindowToOculus()
{
    RepoMdiSubWindow* repoSubWindow = activeSubWindow();    
    RepoGLCWidget* oldWidget = repoSubWindow->widget<RepoGLCWidget*>();

    if (!oldWidget)
        repoSubWindow = NULL;
    else
    {

        //----------------------------------------------------------------------
        // Disable double buffering
        RepoOculus *oculus = new RepoOculus(
                    repoSubWindow,
                    RepoOculus::singleBufferFormat(),
                    oldWidget->windowTitle());
        oculus->setGLCWorld(oldWidget->getGLCWorld());

        repoSubWindow->setWidget(oculus);
        repoSubWindow->show();

        this->update();
        this->repaint();
    }
    return repoSubWindow;
}



QList<repo::gui::RepoMdiSubWindow *> repo::gui::RepoMdiArea::subWindowList(
	bool onlyVisible,
	WindowOrder order) const
{
    QList<QMdiSubWindow *> subwindows = QMdiArea::subWindowList(order);
    QList<QMdiSubWindow *>::iterator it;
    QList<RepoMdiSubWindow *> subWindowList;

    for (it = subwindows.begin(); it != subwindows.end(); ++it)
	{
        QMdiSubWindow * window = *it;
		if (!onlyVisible || window->isVisible())
		{
			RepoMdiSubWindow * subWindow = static_cast<RepoMdiSubWindow *>(window);
			if (NULL != subWindow)
				subWindowList.push_back(subWindow);
		}
	}
	return subWindowList;
}

repo::gui::RepoMdiSubWindow *repo::gui::RepoMdiArea::activeSubWindow() const
{
    return dynamic_cast<RepoMdiSubWindow*>(QMdiArea::activeSubWindow());
}

void repo::gui::RepoMdiArea::resizeEvent(QResizeEvent *resizeEvent)
{
	//this->maximizeSubWindows();
    // TODO: improve performance
    QImage background(resizeEvent->size(), QImage::Format_ARGB32_Premultiplied);
	QPainter backgroundPainter(&background);
	backgroundPainter.fillRect(background.rect(), Qt::gray);
    //QImage scaled = backgroundImage.scaled(resizeEvent->size(),Qt::KeepAspectRatio);
	QRect scaledRect = logo.rect();
	scaledRect.moveBottomRight(
		QPoint(resizeEvent->size().width(),
			resizeEvent->size().height()));
    backgroundPainter.drawImage(scaledRect, logo);
	setBackground(background);

    //--------------------------------------------------------------------------
	// Pass the event to the super class.
	QMdiArea::resizeEvent(resizeEvent);
}
