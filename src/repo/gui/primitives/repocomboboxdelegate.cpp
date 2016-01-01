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
         const QList<RepoComboBoxEditor::SeparatedEntries> &comboBoxLists)    
{
    for (unsigned int i = 0; i < comboBoxLists.size(); ++i)
    {
        QItemEditorFactory *factory = new QItemEditorFactory();
        factory->registerEditor(QVariant::String, new RepoComboBoxEditor(comboBoxLists[i]));
        factories.append(factory);
    }
}

repo::gui::RepoComboBoxDelegate::~RepoComboBoxDelegate()
{
    for (unsigned int i = 0; i < factories.size(); ++i)
    {
        if (factories[i])
        {
            delete factories[i];
            factories[i] = 0;
        }
    }
}

QWidget * repo::gui::RepoComboBoxDelegate::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem &,
        const QModelIndex &index) const
{
    QWidget *widget = 0;
    if (index.isValid() && index.column() < factories.size())
    {
        widget = factories[index.column()]->createEditor(index.data(Qt::EditRole).userType(), parent);
        if (widget)
            widget->setFocusPolicy(Qt::WheelFocus);
    }
    return widget;
}


