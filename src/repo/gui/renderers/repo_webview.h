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

#include <QWidget>
#include <QUrl>
#include <QtNetwork>
#include <QWebSettings>
#include <QGraphicsWebView>
#include <QGLWidget>

namespace Ui {
class RepoWebView;
}

namespace repo {
namespace gui {
namespace renderer {

	class RepoWebView : public QWidget
	{
		Q_OBJECT

	public:
        explicit RepoWebView(const QUrl& url = QUrl("https://www.3drepo.io"),
			QWidget *parent = 0);
		~RepoWebView();


		public slots:

		void loadFromAddressBar();

		void setAddressBar(const QUrl& url);
		void setAddressBar(const QString& url);

	private:
		Ui::RepoWebView *ui;

	}; // end class

}
} // end namespace gui
} // end namespace repo

