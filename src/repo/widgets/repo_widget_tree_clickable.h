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

#include <QTreeView>

namespace repo {
namespace widgets {

/*!
 * \brief
 * Standard QTreeView emits signals on click and double click only if an item
 * is selected. This extended class emits the same signal with an invalid
 * QModelIndex also when the widget is clicked outside of selection. This is
 * useful for instance to open an AddItem dialog that creates a new entry in the
 * TreeView.
 */
class RepoWidgetTreeClickable : public QTreeView
{    
    Q_OBJECT

public:

    RepoWidgetTreeClickable(QWidget *parent);

    ~RepoWidgetTreeClickable() {}

public slots:

    void setDoubleClickMutex();

    void resetDoubleClickMutex();

public :

    //! Returns font of the tree view.
    QFont getFont() const;

protected :

    virtual void mouseDoubleClickEvent(QMouseEvent*);

    bool doubleClickMutex;

};

} // end namespace widgets
} // end namespace repo

