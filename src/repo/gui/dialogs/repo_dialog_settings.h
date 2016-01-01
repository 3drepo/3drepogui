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


#ifndef repo_dialog_settings_H
#define repo_dialog_settings_H

#include <QDialog>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

namespace Ui {
    class repo_dialog_settings;
}

namespace repo {
namespace gui {

class repo_dialog_settings : public QDialog
{
    Q_OBJECT

public:

    //! Default constructor.
    explicit repo_dialog_settings(QWidget *parent = 0);

    //! Destructor to clean up list view and model proxies.
    ~repo_dialog_settings();

public slots :

    //! Applies all of the currently selected settings.
    void apply();

    void changeOptionsPane(const QModelIndex &);

private:

    Ui::repo_dialog_settings *ui;

    QStandardItemModel *optionsModel;

    QSortFilterProxyModel *optionsProxy;

};

} // end namespace gui
} // end namespace repo

#endif // repo_dialog_settings_H
