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


#include "repo_widget_tree_clickable.h"

using namespace repo::gui::widget;

ClickableTreeWidget::ClickableTreeWidget(QWidget *parent)
    : QTreeView(parent)
{
    QObject::connect(this, SIGNAL(doubleClicked(QModelIndex)),
                     this, SLOT(setDoubleClickMutex()));

    resetDoubleClickMutex();
}

void ClickableTreeWidget::setDoubleClickMutex()
{
    doubleClickMutex = false;
}

void ClickableTreeWidget::resetDoubleClickMutex()
{
    doubleClickMutex = true;
}

void ClickableTreeWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    resetDoubleClickMutex();

    // The default double click sets the mutex.
    QTreeView::mouseDoubleClickEvent(e);

    // If double click not yet emitted, emit double click with invalid index.
    if (doubleClickMutex)
    {
        emit doubleClicked(QModelIndex());
    }
}

QFont ClickableTreeWidget::getFont() const
{
    QStyleOptionViewItem options = viewOptions();
    return options.font;
}
