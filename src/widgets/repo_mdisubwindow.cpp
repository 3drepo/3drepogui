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
#include "repo_glcwidget.h"
#include "../primitives/repo_fontawesome.h"

repo::gui::RepoMdiSubWindow::RepoMdiSubWindow(
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

repo::gui::RepoMdiSubWindow::~RepoMdiSubWindow()
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

void repo::gui::RepoMdiSubWindow::setWidget(const QString& windowTitle)
{
	setWidget(new RepoGLCWidget(this, windowTitle));
    setWindowIcon(this->widget()->windowIcon());
}

void repo::gui::RepoMdiSubWindow::setWidget(
	const QString& filePath,
	const unsigned int pFlags)
{
	setWidget(new RepoGLCWidget(this, RepoWorkerAssimp::getFileName(filePath)));

    //--------------------------------------------------------------------------
	// Establish and connect the new worker.
	RepoWorkerAssimp * worker = new RepoWorkerAssimp(filePath, pFlags);
	connect(worker, SIGNAL(finished(repo::core::RepoGraphScene *, GLC_World &)), 
		this, SLOT(finishedLoading(repo::core::RepoGraphScene *, GLC_World &)));
	connect(worker, SIGNAL(progress(int, int)), this, SLOT(progress(int, int)));
	//connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
	
    //--------------------------------------------------------------------------
	// Fire up the asynchronous calculation.
	QThreadPool::globalInstance()->start(worker);
}

void repo::gui::RepoMdiSubWindow::setWidget(QWidget * widget)
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


void repo::gui::RepoMdiSubWindow::removeWidget()
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

QWidget * repo::gui::RepoMdiSubWindow::widget() const 
{
	return boxLayout->itemAt(1) ? boxLayout->itemAt(1)->widget() : 0; 
}

void repo::gui::RepoMdiSubWindow::finishedLoading(
    repo::core::RepoGraphScene *repoScene,
	GLC_World& glcWorld)
{	
    RepoGLCWidget *widget = dynamic_cast<RepoGLCWidget*>(this->widget());
	if (widget)
	{
		if (repoScene)
            widget->setRepoScene(repoScene);
		widget->setGLCWorld(glcWorld);
	}
}

void repo::gui::RepoMdiSubWindow::progress(int value, int maximum)
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
