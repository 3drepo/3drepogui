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
#include <iostream>

repo::gui::RepoComboBoxEditor::RepoComboBoxEditor(const SeparatedEntries &entries,
        QWidget *parent)
    : QComboBox(parent)
    , entries(entries)
{ 
    int separatorCounter = 0;
    for (int j = 0; j < this->entries.size(); ++j)
    {
        std::list<std::string> l = this->entries[j];
        std::list<std::string>::iterator it = l.begin();
        for (int i = separatorCounter; it != l.end(); ++it, ++i)
        {
            QString str = QString::fromStdString(*it);
            insertItem(i, str);
            setItemData(i, str, Qt::DecorationRole);
        }
        separatorCounter += l.size();
        if (this->entries.size() - 1 != j)
            insertSeparator(separatorCounter);
    }
}

repo::gui::RepoComboBoxEditor::~RepoComboBoxEditor() {}

QString repo::gui::RepoComboBoxEditor::value() const
{
   return qvariant_cast<QString>(itemData(currentIndex(), Qt::DecorationRole));
}

void repo::gui::RepoComboBoxEditor::setValue(QString value)
{
   setCurrentIndex(findData(value, int(Qt::DecorationRole)));
}

QWidget * repo::gui::RepoComboBoxEditor::createWidget(QWidget * parent) const
{
    return new RepoComboBoxEditor(entries, parent);
}

QByteArray repo::gui::RepoComboBoxEditor::valuePropertyName() const
{
    return QByteArray();
}
