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

#include "repo_mdisubwindow.h"
#include "../renderers/repo_glcwidget.h"
#include "../primitives/repo_fontawesome.h"

#include "../renderers/repo_oculus.h"

#include "../repo/workers/repo_worker_glc_export.h"
#include "../repo/logger/repo_logger.h"
using namespace repo::gui;

RepoMdiSubWindow::RepoMdiSubWindow(
        QWidget *parent,
        Qt::WindowFlags flags)
        : QMdiSubWindow(parent, flags)
{
    //--------------------------------------------------------------------------
	// General settings
	setAttribute(Qt::WA_DeleteOnClose);
	setStyleSheet("background-color:white;");
    setWindowIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_file));
	resize(400, 400);

    //--------------------------------------------------------------------------
	// Box layout
    QWidget *centralWidget = new QWidget();
	boxLayout = new QBoxLayout(QBoxLayout::BottomToTop, centralWidget);
	boxLayout->setSpacing(0);
	boxLayout->setMargin(0);

    //--------------------------------------------------------------------------
	// Progress bar
	progressBar = new QProgressBar();
	progressBar->hide();
	progressBar->setTextVisible(false);
	boxLayout->addWidget(progressBar);

    //--------------------------------------------------------------------------
	// The super-class assumes ownership of the centralWidget.
	QMdiSubWindow::setWidget(centralWidget);
}

RepoMdiSubWindow::~RepoMdiSubWindow()
{
	emit aboutToDelete();

	removeWidget();

	if (progressBar)
		delete progressBar;
	progressBar = NULL;

	if (boxLayout)
		delete boxLayout;
	boxLayout = NULL;
}

void RepoMdiSubWindow::setWidget(const QString& windowTitle)
{
	setWidget(new RepoGLCWidget(0, windowTitle));
    setWindowIcon(this->widget()->windowIcon());
}

void RepoMdiSubWindow::setWidgetFromFile(
    const QString& filePath)
{
    setWidget(new RepoGLCWidget(0, /*RepoWorkerAssimp::getFileName(filePath)*/""));

    //--------------------------------------------------------------------------
	// Establish and connect the new worker.
    // Assimp flags is a memory leak TODO: fixme!
   /* RepoWorkerAssimp *worker = new RepoWorkerAssimp(filePath, new RepoWidgetAssimpFlags());
	connect(worker, SIGNAL(finished(repo::core::RepoGraphScene *, GLC_World &)),
		this, SLOT(finishedLoading(repo::core::RepoGraphScene *, GLC_World &)));
	connect(worker, SIGNAL(progress(int, int)), this, SLOT(progress(int, int)));
	*///connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));

    //--------------------------------------------------------------------------
	// Fire up the asynchronous calculation.
	//QThreadPool::globalInstance()->start(worker);
}

void RepoMdiSubWindow::setWidget(QWidget * widget)
{
    //--------------------------------------------------------------------------
	// Delete the old widget if any.
	this->removeWidget();

    //--------------------------------------------------------------------------
	// Add the new widget.
	boxLayout->addWidget(widget);
	setWindowTitle(widget->windowTitle());
    setWindowIcon(widget->windowIcon());
}


void RepoMdiSubWindow::removeWidget()
{
	QWidget* oldWidget = this->widget();
	if (oldWidget)
	{
		boxLayout->removeWidget(oldWidget);
		delete oldWidget;
		oldWidget = 0;
	}
}

//------------------------------------------------------------------------------

QWidget * RepoMdiSubWindow::widget() const
{
	return boxLayout->itemAt(1) ? boxLayout->itemAt(1)->widget() : nullptr;
}

void RepoMdiSubWindow::finishedLoadingScene(
    repo::manipulator::graph::RepoScene *repoScene)
{
	repo::logger::RepoLogger::getInstance()->messageGenerated("finished loading repo scene");
	//We have a scene, fire up the GLC worker to get a GLC World representation
	//--------------------------------------------------------------------------
	// Establish and connect the new worker.
	repo::worker::GLCExportWorker* worker =
		new repo::worker::GLCExportWorker(repoScene);
	connect(worker, SIGNAL(finished(repo::manipulator::graph::RepoScene *, GLC_World &)),
		this, SLOT(finishedLoadingGLC(repo::manipulator::graph::RepoScene *,GLC_World &)));
	connect(worker, SIGNAL(progress(int, int)), this, SLOT(progress(int, int)));

	QObject::connect(
		this, &RepoMdiSubWindow::aboutToDelete,
		worker, &repo::worker::GLCExportWorker::cancel, Qt::DirectConnection);

	//--------------------------------------------------------------------------
	// Fire up the asynchronous calculation.
	QThreadPool::globalInstance()->start(worker);

}

void RepoMdiSubWindow::finishedLoadingGLC(repo::manipulator::graph::RepoScene *repoScene, GLC_World &glcWorld)
{
	repo::logger::RepoLogger::getInstance()->messageGenerated("finished Loading GLC");
	RepoGLCWidget *widget = dynamic_cast<RepoGLCWidget*>(this->widget());
	if (widget)
	{
		if (repoScene)
	           widget->setRepoScene(repoScene);
		widget->setGLCWorld(glcWorld);
	}
	else
	{
	    RepoOculus *oculusWidget = dynamic_cast<RepoOculus*>(this->widget());
	    if (oculusWidget)
	        oculusWidget->setGLCWorld(glcWorld);
	}
}

void RepoMdiSubWindow::progress(int value, int maximum)
{
	if (progressBar->maximum() != maximum)
		progressBar->setRange(0, maximum);
	progressBar->setValue(value);

	if (value > 0 && value == maximum)
	{
		progressBar->hide();
		//this->widget()->setCursor(QCursor(Qt::ArrowCursor));
		//boxLayout->update();
	}
	else if (!progressBar->isVisible())
	{
		progressBar->show();
		//this->widget()->setCursor(QCursor(Qt::WaitCursor));
		//boxLayout->update();
	}
}
