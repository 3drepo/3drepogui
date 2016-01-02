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
#include <QByteArray>
#include <QComboBox>
#include <QList>
#include <QString>
#include <QItemEditorCreatorBase>

namespace repo {
namespace gui {
namespace primitive{

	// See http://doc-snapshot.qt-project.org/qt5-5.4/qitemeditorcreatorbase.html
	class RepoComboBoxEditor : public QComboBox, public QItemEditorCreatorBase
	{
		Q_OBJECT

			Q_PROPERTY(QString value READ value WRITE setValue USER true)

	public:

		typedef QList<std::list<std::string> > SeparatedEntries;

	public:

		explicit RepoComboBoxEditor(const SeparatedEntries &entries, QWidget *parent = 0);

		~RepoComboBoxEditor();

	public:

		QString value() const;

		void setValue(QString value);

		//! Returns an editor widget with the given parent.
		QWidget * createWidget(QWidget * parent) const;

		//! Returns empty QByteArray.
		QByteArray valuePropertyName() const;

	public:

		static SeparatedEntries getSeparatedEntries(
			const std::list<std::string> &list);

		static SeparatedEntries getSeparatedEntries(
			const QStringList &list);

	private:

		//! List of lists of items to be displayed in combo box (separated by a line).
		SeparatedEntries entries;
	};

}
} // end namespace gui
} // end namespace repo

