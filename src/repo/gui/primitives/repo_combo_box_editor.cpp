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

#include "repo_combo_box_editor.h"
#include <iostream>

#include <QStylePainter>

using namespace repo::gui::primitive;

RepoComboBoxEditor::RepoComboBoxEditor(const SeparatedEntries &entries,
        QWidget *parent)
    : QComboBox(parent)
    , entries(entries)
{ 
    this->setDuplicatesEnabled(true);

    int index = 0;
    for (int j = 0; j < this->entries.size(); ++j)
    {
        std::list<std::string> l = this->entries[j];
        std::list<std::string>::iterator it = l.begin();
        for (int i = index; it != l.end(); ++it, ++i)
        {
            QString str = QString::fromStdString(*it);
            insertItem(i, str);
            setItemData(i, str, Qt::DecorationRole);
        }

        if (index != 0)
            insertSeparator(index);

        index += l.size();
    }
}

RepoComboBoxEditor::~RepoComboBoxEditor() {}

QString RepoComboBoxEditor::value() const
{
   return qvariant_cast<QString>(itemData(currentIndex(), Qt::DecorationRole));
}

void RepoComboBoxEditor::setValue(QString value)
{
   setCurrentIndex(findData(value, int(Qt::DecorationRole)));
}

QWidget * RepoComboBoxEditor::createWidget(QWidget * parent) const
{
    return new RepoComboBoxEditor(entries, parent);
}

QByteArray RepoComboBoxEditor::valuePropertyName() const
{
    return QByteArray();
}

RepoComboBoxEditor::SeparatedEntries RepoComboBoxEditor::getSeparatedEntries(
        const std::list<std::string> &list)
{
    RepoComboBoxEditor::SeparatedEntries se;
    se << list;
    return se;
}

RepoComboBoxEditor::SeparatedEntries RepoComboBoxEditor::getSeparatedEntries(
        const QStringList &stringList)
{
    std::list<std::string > list;
    for (QString s : stringList)
    {
        list.push_back(s.toStdString());
    }
    return getSeparatedEntries(list);
}
