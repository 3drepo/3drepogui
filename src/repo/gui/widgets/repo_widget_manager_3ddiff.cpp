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


#include "repo_widget_manager_3ddiff.h"
#include "ui_repo_widget_manager_3ddiff.h"

using namespace repo::gui::widget;
using namespace repo::gui;

Repo3DDiffManagerWidget::Repo3DDiffManagerWidget(
        repo::gui::widget::RepoMdiArea *mdiArea,
        repo::RepoController *controller,
        const repo::RepoToken *token,
        QWidget *parent)
    : QWidget(parent)
    , mdiArea(mdiArea)
    , controller(controller)
    , token(token)
    , ui(new Ui::Repo3DDiffManagerWidget)
{
    ui->setupUi(this);
    populateModelComboBoxes();

    // Connecting mdiArea signal which is emmitted whenever a new window is created
    // or an existing window is removed
    QObject::connect(mdiArea, &repo::gui::widget::RepoMdiArea::subWindowActivated,
                     this, &Repo3DDiffManagerWidget::populateModelComboBoxes);

    QObject::connect(ui->diffPushButton, &QPushButton::pressed,
                     this, &Repo3DDiffManagerWidget::runDiff);

    QObject::connect(ui->resetPushButton, &QPushButton::pressed,
                     this, &Repo3DDiffManagerWidget::resetModels);
}

Repo3DDiffManagerWidget::~Repo3DDiffManagerWidget()
{
    delete ui;
}

void Repo3DDiffManagerWidget::populateModelComboBoxes()
{
//    QString selectedA = getSelectedModelAString();
//    QString selectedB = getSelectedModelBString();

    int indexA = getSelectedModelAIndex();
    int indexB = getSelectedModelBIndex();

    //--------------------------------------------------------------------------
    // Clear old entries
    ui->modelAComboBox->clear();
    ui->modelBComboBox->clear();

    //--------------------------------------------------------------------------
    // Populate
    QStringList subWindows;
    for (repo::gui::widget::RepoMdiSubWindow* subWindow : getSubWindows())
    {
        subWindows.append(subWindow->windowTitle());
    }
    ui->modelAComboBox->addItems(subWindows);
    ui->modelBComboBox->addItems(subWindows);

    //--------------------------------------------------------------------------
    // Reset selection
    ui->modelAComboBox->setCurrentIndex(indexA);
    ui->modelBComboBox->setCurrentIndex(indexB);

    if (getSelectedModelAIndex() == -1)
        ui->modelAComboBox->setCurrentIndex(0);

    if (getSelectedModelBIndex() == -1)
        ui->modelBComboBox->setCurrentIndex(subWindows.size() > 1 ? 1 : 0);

}

void Repo3DDiffManagerWidget::resetModels()
{
    repo::gui::widget::Rendering3DWidget* widgetA = getSelectedModelAWidget();
    repo::gui::widget::Rendering3DWidget* widgetB = getSelectedModelBWidget();

    widgetA->resetColors();
    widgetB->resetColors();
}

void Repo3DDiffManagerWidget::runDiff()
{
    //Make sure the widgets are at its original state before starting
    resetModels();
    repo::gui::widget::Rendering3DWidget* widgetA = getSelectedModelAWidget();
    repo::gui::widget::Rendering3DWidget* widgetB = getSelectedModelBWidget();

    if (!widgetA)
        std::cerr << tr("Widget A is null.").toStdString() << std::endl;
    else if(!widgetB)
        std::cerr << tr("Widget B is null.").toStdString() << std::endl;
    else
    {
        std::cout << tr("Starting 3D Diff calculation").toStdString();
        bool colorCorrespondence = (getVisualization() == Visualization::CORRESPONDENCE);
        switch (getDiffAlgorithm())
        {
        case Algorithm::BASIC :
            runBouncerDiff(widgetA,
                           widgetB,
                           repo::manipulator::diff::Mode::DIFF_BY_NAME,
                           colorCorrespondence);
            break;
        case Algorithm::STATISTICAL :
            runBouncerDiff(widgetA,
                           widgetB,
                           repo::manipulator::diff::Mode::DIFF_BY_ID,
                           colorCorrespondence);
            break;
        case Algorithm::VISUAL :
            // TODO: Jozef to add hooks.
            break;
        }
    }
}

void Repo3DDiffManagerWidget::runBouncerDiff(
        repo::gui::widget::Rendering3DWidget* widgetA,
        repo::gui::widget::Rendering3DWidget* widgetB,
        repo::manipulator::diff::Mode diffMode,
        bool colourCorrespondence)
{
    if (isReady())
    {
        repo::core::model::RepoScene *sceneA = widgetA->getRepoScene();
        repo::core::model::RepoScene *sceneB = widgetB->getRepoScene();

        repo::worker::DiffWorker *worker = new repo::worker::DiffWorker(
                    controller,
                    token,
                    sceneA,
                    sceneB,
                    diffMode,
                    colourCorrespondence);

        QObject::connect(worker, &repo::worker::DiffWorker::colorChangeOnA,
                         widgetA, &repo::gui::widget::Rendering3DWidget::setMeshColor);
        QObject::connect(worker, &repo::worker::DiffWorker::colorChangeOnB,
                         widgetB, &repo::gui::widget::Rendering3DWidget::setMeshColor);

        //----------------------------------------------------------------------
        // Fire up the asynchronous calculation.
        connectAndStartWorker(worker);
    }
}

repo::gui::widget::Rendering3DWidget* Repo3DDiffManagerWidget::getModelWidget(int index) const
{
    repo::gui::widget::Rendering3DWidget* widget = nullptr;
    auto subWindows = getSubWindows();
    if (index >= 0 && subWindows.size() > index)
        widget = dynamic_cast<repo::gui::widget::Rendering3DWidget*>(
                    subWindows.at(index)->widget());
    return widget;
}

QString Repo3DDiffManagerWidget::getSelectedModelAString() const
{
    return ui->modelAComboBox->currentText();
}

int Repo3DDiffManagerWidget::getSelectedModelAIndex() const
{
    return ui->modelAComboBox->currentIndex();
}

QString Repo3DDiffManagerWidget::getSelectedModelBString() const
{
    return ui->modelBComboBox->currentText();
}

int Repo3DDiffManagerWidget::getSelectedModelBIndex() const
{
    return ui->modelBComboBox->currentIndex();
}

QList<repo::gui::widget::RepoMdiSubWindow*> Repo3DDiffManagerWidget::getSubWindows() const
{
    return mdiArea->subWindowList(true, repo::gui::widget::RepoMdiArea::WindowOrder::CreationOrder);
}

Repo3DDiffManagerWidget::Visualization Repo3DDiffManagerWidget::getVisualization() const
{
    Visualization viz;
    if (ui->showCorrespondenceRadioButton->isChecked())
        viz = Visualization::CORRESPONDENCE;
    else if (ui->showDifferenceRadioButton->isChecked())
        viz = Visualization::DIFF;
    return viz;
}

Repo3DDiffManagerWidget::Algorithm Repo3DDiffManagerWidget::getDiffAlgorithm() const
{
    return (Algorithm) ui->diffAlgorithmComboBox->currentIndex();
}

