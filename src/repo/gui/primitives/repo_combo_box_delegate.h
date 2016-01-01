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
#include <QItemDelegate>

//------------------------------------------------------------------------------
// GUI
#include "repo_combo_box_editor.h"

namespace repo {
namespace gui {
namespace primitive{

	/*!
		* Enables drop down selectors for editing delegate.
		* See http://doc.qt.io/qt-5/qtwidgets-itemviews-coloreditorfactory-example.html
		*/
	class RepoComboBoxDelegate : public QItemDelegate
	{

	public:

		//    repo_combo_box_delegate() {}

		//! A list of combo box entries per column.
        RepoComboBoxDelegate(
            const QList<RepoComboBoxEditor::SeparatedEntries> &comboBoxLists);

        ~RepoComboBoxDelegate();

		QWidget *createEditor(
			QWidget * parent,
			const QStyleOptionViewItem &,
			const QModelIndex &index) const;

	private:

		//! Standard item factories list per column
		QList<QItemEditorFactory *> factories;
	};

}
} // end namespace gui
} // end namespace repo
