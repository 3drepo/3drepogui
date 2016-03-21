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

#include "repo_mdi_subwindow.h"
#include "repo_widget_flags.h"
#include "repo_widget_rendering_2d.h"
#include "../primitives/repo_fontawesome.h"

#include "../../workers/repo_worker_glc_export.h"
#include "../../logger/repo_logger.h"

#include "../../workers/repo_worker_file_import.h"

#include <boost/filesystem.hpp>

using namespace repo::gui;
using namespace repo::gui::widget;

RepoMdiSubWindow::RepoMdiSubWindow(
        QWidget *parent,
        Qt::WindowFlags flags)
        : QMdiSubWindow(parent, flags)
        , awaitingClose(nullptr)
{
    //--------------------------------------------------------------------------
	// General settings
	setAttribute(Qt::WA_DeleteOnClose);
	setStyleSheet("background-color:white;");
    setWindowIcon(repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                      repo::gui::primitive::RepoFontAwesome::fa_file));
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
	removeWidget();

	if (progressBar)
		delete progressBar;
	progressBar = NULL;

	if (boxLayout)
		delete boxLayout;
	boxLayout = NULL;
}

void RepoMdiSubWindow::closeEvent(QCloseEvent *closeEvent)
{
    emit aboutToDelete();
	if (progressBar->isVisible())
	{
		//need to wait for worker to finish before closing or 
		//this may cause seg faults.
		repoLog("waiting on worker to recover before closing..");
		awaitingClose = true;
		closeEvent->ignore();
	}
	else
	{
		QMdiSubWindow::closeEvent(closeEvent);
	}

}

void RepoMdiSubWindow::setWidget3D(
        const QString& windowTitle,
        repo::gui::renderer::NavMode navMode,
        const std::vector<double> &offsetVector)
{
    this->offsetVector = offsetVector;
    setWidget(new widget::Rendering3DWidget(this, widget::Renderer::GLC, navMode, windowTitle));
    setWindowIcon(this->widget()->windowIcon());
}

void RepoMdiSubWindow::setWidget2D(
        const repo::core::model::RepoScene *scene,
        const QString &windowTitle)
{
    setWidget(new repo::gui::widget::Rendering2DWidget(scene, windowTitle, this));
    setWindowIcon(this->widget()->windowIcon());
}

void RepoMdiSubWindow::setWidgetFromFile(
    const QString& filePath,
    repo::RepoController *controller,
    repo::gui::renderer::NavMode navMode,
    const std::vector<double>    &offsetVector)
{
    this->offsetVector = offsetVector;
	boost::filesystem::path filePathPath(filePath.toStdString());
    setWidget(new widget::Rendering3DWidget(this, widget::Renderer::GLC, navMode, QString(filePathPath.filename().string().c_str())));

    //--------------------------------------------------------------------------
	// Establish and connect the new worker.
    repo::worker::FileImportWorker *worker =
            new repo::worker::FileImportWorker(filePath.toStdString(), controller);
    connect(worker, SIGNAL(finished(repo::core::model::RepoScene *)),
        this, SLOT(finishedLoadingScene(repo::core::model::RepoScene *)));
	connect(worker, SIGNAL(progress(int, int)), this, SLOT(progress(int, int)));
	//connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));

    //--------------------------------------------------------------------------
	// Fire up the asynchronous calculation.
	QThreadPool::globalInstance()->start(worker);
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
    repo::core::model::RepoScene *repoScene)
{
	repoLog("finished loading repo scene");

	widget::Rendering3DWidget *widget = dynamic_cast<widget::Rendering3DWidget*>(this->widget());

	if (widget)
	{
		connect(
			widget, &widget::Rendering3DWidget::modelLoadProgress,
			this, &RepoMdiSubWindow::progress);

		QObject::connect(
		 this, &RepoMdiSubWindow::aboutToDelete,
		 widget, &widget::Rendering3DWidget::cancelOperations, Qt::DirectConnection);


        if (repoScene)
        {
            if(!offsetVector.size())
            {
                //this is the first scene being loaded, set the global offset vector
                repoLogDebug("OffsetVector not found, emitting signal to set current scene as world scale");
                emit updateOffsetVector(repoScene->getWorldOffset(), this);
            }
            widget->setRepoScene(repoScene, offsetVector);
        }
		
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
		if (awaitingClose)
		{
			close();
		}
		awaitingClose = false;
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
