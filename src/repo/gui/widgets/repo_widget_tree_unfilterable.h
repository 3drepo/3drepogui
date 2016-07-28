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
#include <QLocale>

#include "../primitives/repo_combo_box_delegate.h"

namespace Ui {
class UnfilterableTreeWidget;
}

namespace repo {
namespace gui{
namespace widget {

	/**
		* Convenience class that makes use of simplified QTreeWidget instead of more
		* cumbersome QTreeView. This, however, does not provide any sorting or
		* filtering functionality and instead is targetting smaller dialogs with lesser
		* entries.
		*
		* @sa FilterableTreeWidget
		* @brief The UnfilterableTreeWidget class
		*/
	class UnfilterableTreeWidget : public QWidget
	{
		Q_OBJECT

	public:

		//! Constructs a default widget with no headers
		explicit UnfilterableTreeWidget(QWidget *parent = 0);

		~UnfilterableTreeWidget();

	signals:

		void rowCountChanged(int oldRowCount, int newRowCount);

		void tabTextChanged(int tab, QString text);

		public slots :

		void setButtonsEnabled(bool enabled);
        void setButtonsEnabled(bool enabledAdd, bool enableRemove);

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
		QTreeWidgetItem *addRow(const QStringList &list, bool enabled = true);

		QTreeWidgetItem *addRow(const QString &a, const QString &b);

		QTreeWidgetItem *addRow(const std::pair<std::string, std::string> &pair, bool enabled = true);

		void addRows(const std::list<std::pair<std::string, std::string> > &list);

		std::list<std::pair<std::string, std::string> > getItemsAsListOfPairsOfStrings() const;

		std::vector<std::string> getItemsAsVectorOfStrings() const;

		int getRowCount() const;

		void setItemDelegateForRow(const QString &item);

		void setItemDelegateForRow(const QString &item, int row);

		void setItemDelegateForColumn(const QString &item, int column);

		void setDelegates(const QHash<QString, repo::gui::primitive::RepoComboBoxDelegate* > &delegates)
		{
			this->delegates = delegates;
		}

		void setNewRowText(const QStringList &defaultRow)
		{
			this->newRowText = defaultRow;
		}

		//! Sets the appropriate delegate if the database column on the project item has changed.
		void updateDelegate(QTreeWidgetItem *current, int column);

		void notifyTabTextChange(int oldRowCount, int newRowCount);

	public:

		void registerTabWidget(QTabWidget *tabWidget, int tab);

		//! Chops last chars of type " (oldCount)" and appends " (newCount)"
		static QString updateCountString(QString string, int oldCount, int newCount);

        void removeAll();

	private:

		//! Items to add as default row when Add button is pressed.
		QStringList newRowText;

		//! Lookup table for roles delegates by database name.
		QHash<QString, repo::gui::primitive::RepoComboBoxDelegate*> delegates;

		QTabWidget *tabWidget;

		int tab;

		//! Ui var.
		Ui::UnfilterableTreeWidget *ui;
	};

} // end namespace widget
}
} // end namespace repo
