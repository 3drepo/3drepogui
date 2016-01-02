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

#include <QStandardItem>
#include <QString>
#include <QLocale>

#include <string>
#include <stdint.h>
#include <cmath>

namespace repo {
namespace gui{
namespace primitive {

	/**
		* @brief The RepoStandardItem class
		* Convenienece class for creation of custom QStandardItem objects with
		* preset settings. For instance it enables easy creation of number and filesize
		* entries that are correctly sortable as numbers eg 1, 2, 3, 20, 30, rather
		* that as strings "1", "2", "20", "3", 30".
		*/
	class RepoStandardItem : public QStandardItem
	{

	public:

		//! Creates a standard non-editable item with tooltip and data set.
		RepoStandardItem(
			const QString& label = QString(),
			const QVariant& data = QVariant(),
			int role = Qt::UserRole + 1,
			Qt::Alignment alignment = Qt::AlignLeft,
			bool enabled = true);

		//! Creates default non-editable item from std::string.
		RepoStandardItem(const std::string &label)
			: RepoStandardItem(QString::fromStdString(label)) {}

		/**
			* Creates a sortable non-editable item from given number. If the filesie is
			* true, returns a sortable filesize string item.
			*/
		RepoStandardItem(uint64_t number, bool filesize = false)
			: RepoStandardItem(
			filesize
			? toFileSize((qlonglong)number)
			: toLocaleString((qulonglong)number),
			(qulonglong)number,
			Qt::DisplayRole, // this makes numbers sortable as numbers and not strings
			Qt::AlignRight)
		{}

	public:

		//! Returns a human readable string of kilobytes, megabytes etc.
		static QString toFileSize(uint64_t bytes);

		//! Returns the current locale string representation.
		template <class T>
		static QString toLocaleString(const T & value)
		{
			QLocale locale;
			return locale.toString(value);
		}
	};

} // primitives
}
} // repo
