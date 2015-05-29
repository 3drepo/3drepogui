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


#include "repo_fpscounter.h"

repo::gui::RepoFPSCounter::RepoFPSCounter(float alpha)
    : alpha(alpha)
    , frameCounter(0)
    , fps(0)
{}

repo::gui::RepoFPSCounter::~RepoFPSCounter() {}

void repo::gui::RepoFPSCounter::initialize()
{
    // FPS time (t0) in milliseconds
    timeZero = std::chrono::steady_clock::now();
}

void repo::gui::RepoFPSCounter::increment()
{
    frameCounter++;

    float newFPS = 0;
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    double elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(now - timeZero).count();

    if (elapsedTime > 1) {
        newFPS = ((float)frameCounter / elapsedTime) * 1e6;
        frameCounter = 0;
        timeZero = now;
    }

    // Low pass filtering
    fps = fps + alpha * (newFPS - fps);
}

QString repo::gui::RepoFPSCounter::getFPSString(int decimals) const
{
    return QString("FPS: ") + QString::number(fps, 'f', decimals);
}
