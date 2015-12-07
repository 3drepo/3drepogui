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

#include <QObject>
#include <QMutex>

namespace repo{
namespace worker{

/*!
 * \brief The RepoMutex class
 *
 * This class is needed as Qt does not allow mutliple QObject inheritance and
 * we need to use the signals and slot mechanism with workers. RepoMutex
 * therefore provides a convenient way how to encapsulate the signalling
 * functionality.
 */
class RepoMutex : public QObject, public QMutex
{

    Q_OBJECT

signals :

    //! Emitted to indicate that the current worker should cancel.
    void cancel();

public slots :

    //! Forces emit of cance signal.
    void emitCancelSignal();

    //! Unlocks the mutex.
    void unlockSlot();


}; // end class

} // end namespace worker
} // end namespace repo
