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

#ifndef REPO_LINE_EDIT_H
#define REPO_LINE_EDIT_H

//-----------------------------------------------------------------------------
// Qt
#include <QLineEdit>
#include <QToolButton>
#include <QStyle>

//-----------------------------------------------------------------------------
// Repo GUI
#include "../primitives/repo_fontawesome.h"

namespace repo {
namespace gui {

/*!	
	Line edit with clear button
	\sa http://blog.qt.digia.com/blog/2007/06/06/lineedit-with-a-clear-button/


    This function is not needed as from QT5.2 line edit now impelements clear
    button by default!
*/
class RepoLineEdit : public QLineEdit
{
	Q_OBJECT
		
public :

	RepoLineEdit(QWidget * parent = 0);
	~RepoLineEdit();

public slots :

	//! Sets clear button enabled is text is not empty, disabled otherwise.
	void setClearButtonEnabled(const QString &text);

protected :

	void resizeEvent(QResizeEvent *);

private :

    QToolButton *clearButton;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_LINE_EDIT_H
