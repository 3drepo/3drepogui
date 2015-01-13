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

#include "repocomboboxdelegate.h"

repo::gui::RepoComboBoxDelegate::RepoComboBoxDelegate(
        const std::list<std::string> &list)
{
    factory = new QItemEditorFactory();
    comboBoxEditor = new RepoComboBoxEditor(list);
    factory->registerEditor(QVariant::String, comboBoxEditor);
    this->setItemEditorFactory(factory);
}

repo::gui::RepoComboBoxDelegate::~RepoComboBoxDelegate()
{
    delete comboBoxEditor;
    //delete factory;
}


void repo::gui::RepoComboBoxDelegate::paint(
        QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    if (1 == index.column())
    {

        this->paint();
        // ohh it's my column
        // better do something creative
    }
    else // it's just a common column. Live it in default way
        QItemDelegate::paint(painter, option, index);
}
