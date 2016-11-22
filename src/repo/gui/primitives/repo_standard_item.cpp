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

#include "repo_standard_item.h"

#include <iostream>

using namespace repo::gui::primitive;

//------------------------------------------------------------------------------

RepoStandardItem::RepoStandardItem(
        const QString& label,
        const QVariant& data,
        int role,
        Qt::Alignment alignment,
        bool enabled)
    : QStandardItem(label)
{
    setToolTip(label);

    if (!data.isNull())
        setData(data, role);

    setTextAlignment(alignment);
    setEnabled(enabled);
    setEditable(false);
}

QString RepoStandardItem::toFileSize(uint64_t bytes)
{
    QString value;
    if (0 != bytes)
    {
        QLocale locale;
        //stream << setprecision(2) << fixed;
        static const int unit = 1024;
        if (bytes < unit)
            value = locale.toString((double) bytes, 'f', 2) + " B";
        else
        {
            int exp = (int) (std::log(bytes) / std::log(unit));
            static const std::string prefixes = ("KMGTPEZY");
            char prefix = prefixes.at(exp-1);
            value = locale.toString((bytes / pow(unit, exp)), 'f', 2) + " " + prefix + "B";
        }
    }
    return  value;
}
