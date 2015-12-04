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
#include <QWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QHash>

#include "../../primitives/repocomboboxdelegate.h"

namespace Ui {
class RepoWidgetTreeUnfilterable;
}

namespace repo {
namespace widgets {

/**
 * Convenience class that makes use of simplified QTreeWidget instead of more
 * cumbersome QTreeView. This, however, does not provide any sorting or
 * filtering functionality and instead is targetting smaller dialogs with lesser
 * entries.
 *
 * @sa RepoWidgetTreeFilterable
 * @brief The RepoWidgetTreeUnfilterable class
 */
class RepoWidgetTreeUnfilterable : public QWidget
{
    Q_OBJECT

public:

    //! Constructs a default widget with no headers
    explicit RepoWidgetTreeUnfilterable(QWidget *parent = 0);

    ~RepoWidgetTreeUnfilterable();

signals :

    void rowCountChanged(int oldRowCount, int newRowCount);

public slots :

    //! Sets the headers on this model.
    void setHeaders(const QStringList &headers);

    //! Adds a default row depending on what it is set to.
    virtual QTreeWidgetItem *addRow()
    {
        return addRow(newRowText);
    }

    //! Removes currently selected row item.
    virtual void removeRow();

    //! Adds items as a list. Makes sure the size of the list mataches columns count.
    QTreeWidgetItem *addRow(const QStringList &list);

    QTreeWidgetItem *addRow(const std::pair<std::string, std::string> &pair);

    void addRows(const std::list<std::pair<std::string, std::string> > &list);

    std::list<std::pair<std::string, std::string> > getItems() const;

    int getRowCount() const;

    void setItemDelegateForRow(const QString &item);

    void setItemDelegateForRow(const QString &item, int row);

    void setDelegates(const QHash<QString, repo::gui::RepoComboBoxDelegate* > &delegates)
    {
        this->delegates = delegates;
    }

    void setNewRowText(const QStringList &defaultRow)
    {
        this->newRowText = defaultRow;
    }

    //! Sets the appropriate delegate if the database column on the project item has changed.
    void updateDelegate(QTreeWidgetItem *current, int column);

public :

    //! Chops last chars of type " (oldCount)" and appends " (newCount)"
    static QString updateCountString(QString &string, int oldCount, int newCount);

private:

    //! Items to add as default row when Add button is pressed.
    QStringList newRowText;

    //! Lookup table for roles delegates by database name.
    QHash<QString, repo::gui::RepoComboBoxDelegate*> delegates;

    //! Ui var.
    Ui::RepoWidgetTreeUnfilterable *ui;
};

} // end namespace widgets
} // end namespace repo
