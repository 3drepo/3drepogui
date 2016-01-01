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


#include "repo_webview.h"
#include "ui_repo_webview.h"
#include "../primitives/repo_fontawesome.h"


repo::gui::RepoWebView::RepoWebView(
        const QUrl &url,
        QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RepoWebView)
{

    // Global settings for all pages
    QWebSettings *webSettings = QWebSettings::globalSettings();
    webSettings->setAttribute(QWebSettings::WebGLEnabled, true);
    webSettings->setAttribute(QWebSettings::PluginsEnabled, true);
    webSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    webSettings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);

    ui->setupUi(this);


    setAddressBar(url);
    loadFromAddressBar();

    //--------------------------------------------------------------------------
    // Accelerated view
    // See https://trac.webkit.org/wiki/QtWebKitWebGL
    // See bug report: https://bugreports.qt.io/browse/QTBUG-30405
//     ui->graphicsView->setViewport(new QGLWidget());
//    QGraphicsScene* scene = new QGraphicsScene();
//    QGraphicsWebView graphicsWebView = new QGraphicsWebView();
//    graphicsWebView->setResizesToContents(true);
//    scene->addItem(graphicsWebView);
//    ui->graphicsView->setScene(scene);
//    graphicsWebView->load(url);


    QObject::connect(ui->reloadPushButton, &QPushButton::pressed,
                      ui->webView, &QWebView::reload);
    ui->reloadPushButton->setIcon(
                repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                    repo::gui::primitive::RepoFontAwesome::fa_repeat));

    QObject::connect(ui->backwardPushButton, &QPushButton::pressed,
                      ui->webView, &QWebView::back);
    ui->backwardPushButton->setIcon(
                repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                    repo::gui::primitive::RepoFontAwesome::fa_arrow_left));

    QObject::connect(ui->forwardPushButton, &QPushButton::pressed,
                      ui->webView, &QWebView::forward);
    ui->forwardPushButton->setIcon(
                repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                    repo::gui::primitive::RepoFontAwesome::fa_arrow_right));



    QObject::connect(ui->webView, SIGNAL(urlChanged(const QUrl&)),
                     this, SLOT(setAddressBar(const QUrl&)));

    QObject::connect(ui->webView, SIGNAL(titleChanged(const QString&)),
                     this, SLOT(setWindowTitle(const QString&)));

    QObject::connect(ui->addressBar, &QLineEdit::returnPressed,
                     this, &RepoWebView::loadFromAddressBar);


}

repo::gui::RepoWebView::~RepoWebView()
{
    delete ui;
}

void repo::gui::RepoWebView::loadFromAddressBar()
{
    QString address = ui->addressBar->text();
    QUrl url(address);
    if (!address.startsWith("http://") && !address.startsWith("https://")
            && !address.startsWith("file://"))
        url = QUrl("http://" + address);
    ui->webView->setUrl(url);
}

void repo::gui::RepoWebView::setAddressBar(const QUrl& url)
{
    setAddressBar(url.toString());
}

void repo::gui::RepoWebView::setAddressBar(const QString& url)
{
    ui->addressBar->setText(url);
}
