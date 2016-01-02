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

#include "repo_color.h"
#include <iostream>
#include <cmath>
#include <sstream>

using namespace repo::gui::primitive;

unsigned int RepoColor::colorCounter = 0;

RepoColor::RepoColor() : QColor() {}

RepoColor::RepoColor(float r, float g, float b, float a)
    : QColor()
{
    setRgbF(r, g, b, a);
}
RepoColor::RepoColor(const QColor & color) : QColor(color) {}

RepoColor::RepoColor(const std::tuple<float, float, float> &rgb)
    : QColor()
{
    setRgbF(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb));
}

RepoColor::~RepoColor() {}

std::tuple<float, float, float> RepoColor::tupleF() const
{
    return std::tuple<float, float, float>((float)redF(), (float)greenF(), (float)blueF());
}

bool RepoColor::operator<(const RepoColor &color) const
{
    return
        redF() != color.redF()
        ? redF() < color.redF()
        : greenF() != color.greenF()
        ? greenF() < color.greenF()
        : blueF() != color.blueF()
        ? blueF() < color.blueF()
        : alphaF() < color.alphaF();
}

RepoColor RepoColor::getNext()
{
    RepoColor color;

    // There is 138 pre-defined colours in total.
    const unsigned int totalColorsCount = RepoColor::numberOfColors;

    if (colorCounter >= totalColorsCount)
    {
        colorCounter %= totalColorsCount;
        std::cerr << "Too many colour requests, reusing colours." << std::endl;
    }

    color = RepoColor();
    color.setNamedColor(QString(COLORS[colorCounter].c_str()));
    colorCounter++;
    return color;
}

RepoColor RepoColor::getNextRandom()
{
    return RepoColor(
            (float)rand()/RAND_MAX,
            (float)rand()/RAND_MAX,
            (float)rand()/RAND_MAX);
}

RepoColor RepoColor::fromHex(const std::string &hexString)
{
    unsigned int colorValue;
    std::stringstream sstream;

    // TODO: add checking if the string starts with # or 0x for instance.
    std::string numbers = hexString;
    numbers.erase(0,1);

    sstream << std::hex << numbers;
    sstream >> colorValue;
    return RepoColor::fromRgb(colorValue);
}

RepoColor RepoColor::getComplement() const
{
    RepoColor complement(fromHslF(
        fmod(this->hslHueF() + 0.5, 1.0),
        this->hslSaturationF(),
        this->lightnessF(),
        this->alphaF()));
    return complement;
}

std::string RepoColor::toString() const
{
    std::string res;
    //res += "[" + core::RepoTranscoderString::toString(this->red());
    //res += ", " + core::RepoTranscoderString::toString(this->green());
    //res += ", " + core::RepoTranscoderString::toString(this->blue());
    //res += ", " + core::RepoTranscoderString::toString(this->alpha()) + "]";
    return res;
}

const std::string RepoColor::COLORS[RepoColor::numberOfColors] = {
"#48D1CC",
"#0000CD",
"#FFE4E1",
"#FF7F50",
"#1E90FF",
"#7B68EE",
"#800000",
"#FFF8DC",
"#BDB76B",
"#87CEEB",
"#00FFFF",
"#D2B48C",
"#FF4500",
"#B8860B",
"#00FA9A",
"#BA55D3",
"#9932CC",
"#778899",
"#87CEFA",
"#FFFFF0",
"#006400",
"#90EE90",
"#66CDAA",
"#556B2F",
"#FF8C00",
"#20B2AA",
"#4169E1",
"#FFFAF0",
"#6B8E23",
"#7FFF00",
"#F5DEB3",
"#708090",
"#00FF00",
"#8A2BE2",
"#008B8B",
"#2E8B57",
"#DCDCDC",
"#CD853F",
"#A0522D",
"#8B0000",
"#B0C4DE",
"#8B4513",
"#5F9EA0",
"#FF1493",
"#FAFAD2",
"#FA8072",
"#FF69B4",
"#7FFFD4",
"#191970",
"#F8F8FF",
"#00008B",
"#00BFFF",
"#FFEFD5",
"#8FBC8F",
"#6495ED",
"#228B22",
"#ADFF2F",
"#FFFF00",
"#C71585",
"#F5F5F5",
"#008000",
"#DEB887",
"#FF0000",
"#00FF7F",
"#FFDAB9",
"#F0FFFF",
"#FFD700",
"#008080",
"#FFA07A",
"#DC143C",
"#FFFAFA",
"#DAA520",
"#3CB371",
"#00CED1",
"#C0C0C0",
"#AFEEEE",
"#696969",
"#8B008B",
"#DA70D6",
"#FAF0E6",
"#9370DB",
"#F0FFF0",
"#000080",
"#F0E68C",
"#BC8F8F",
"#B0E0E6",
"#6A5ACD",
"#FFFFFF",
"#FAEBD7",
"#FFC0CB",
"#A52A2A",
"#FFA500",
"#800080",
"#4682B4",
"#E6E6FA",
"#FFF5EE",
"#483D8B",
"#808080",
"#F4A460",
"#FFFFE0",
"#FFFACD",
"#F5FFFA",
"#808000",
"#DB7093",
"#32CD32",
"#FFE4B5",
"#FFE4C4",
"#FFF0F5",
"#CD5C5C",
"#EEE8AA",
"#0000FF",
"#98FB98",
"#FF00FF",
"#FFEBCD",
"#EE82EE",
"#4B0082",
"#E0FFFF",
"#FFB6C1",
"#FDF5E6",
"#D3D3D3",
"#FF6347",
"#F5F5DC",
"#9400D3",
"#2F4F4F",
"#F08080",
"#ADD8E6",
"#40E0D0",
"#FFDEAD",
"#F0F8FF",
"#E9967A",
"#B22222",
"#D2691E",
"#DDA0DD",
"#A9A9A9",
"#7CFC00",
"#D8BFD8",
"#9ACD32",
};
