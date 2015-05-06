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


#ifndef REPO_FPS_COUNTER_H
#define REPO_FPS_COUNTER_H

#include <ctime>
#include <chrono>
#include <iostream>

#include <QString>

namespace repo {
namespace gui {

/*!
 * Low pass filtered FPS counter.
 */
class RepoFPSCounter
{

public:

    RepoFPSCounter(float alpha = 0.05f);

    ~RepoFPSCounter();

    void initialize();

    //! Increments frame counter
    void increment();

    float getFPS() const
    { return fps; }

    QString getFPSString(int decimals = 0) const;

protected :

    //! Counter of rendered frames.
    unsigned int frameCounter;

    //! Previously recorded time for fps calculation.
    std::chrono::steady_clock::time_point timeZero;

    //! Number of frames rendered per second.
    float fps;

    //! Time constant RC
    float alpha;

}; // end class

} // end namespace gui
} // end namespace repo


#endif // REPO_FPS_COUNTER_H
