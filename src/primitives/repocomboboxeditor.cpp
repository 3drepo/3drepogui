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

#include "repocomboboxeditor.h"

repo::gui::RepoComboBoxEditor::RepoComboBoxEditor(QWidget *parent) :
    QComboBox(parent)
{
    populateList();
}

repo::gui::RepoComboBoxEditor::~RepoComboBoxEditor();

QColor repo::gui::RepoComboBoxEditor::color() const
{
   return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void repo::gui::RepoComboBoxEditor::setColor(QColor color)
{
   setCurrentIndex(findData(color, int(Qt::DecorationRole)));
}

void repo::gui::RepoComboBoxEditor::populateList()
{
   QStringList colorNames = QColor::colorNames();

   for (int i = 0; i < colorNames.size(); ++i) {
       QColor color(colorNames[i]);

       insertItem(i, colorNames[i]);
       setItemData(i, color, Qt::DecorationRole);
   }
}


