/**
 *  Copyright (C) 2014 3D Repo Ltd
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

#ifndef REPO_COLOR_H
#define REPO_COLOR_H

#include <tuple>
#include <string>

//-----------------------------------------------------------------------------
#include <QColor>

namespace repo {
namespace gui {

// See http://colorschemedesigner.com/
class RepoColor : public QColor
{

public :

	//-------------------------------------------------------------------------
	//
	// Constructors
	//
	//-------------------------------------------------------------------------

	//! Default empty constructor with rgb 0,0,0 and invalid alpha channel.
	RepoColor();

	RepoColor(const QColor &);

	RepoColor(float r, float g, float b, float a = 1.0);

	RepoColor(const std::tuple<float, float, float> &rgb);

	~RepoColor();

	//! Returns red, green and blue 0.0 to 1.0 components as a tuple of floats.
	std::tuple<float, float, float> tupleF() const;

	//-------------------------------------------------------------------------
	//
	// Operators
	//
	//-------------------------------------------------------------------------
	
	/*!
	 * Compares the colors based on the rgba components, one at the time in 
	 * a decreasing order of red, green, blue and alpha.
	 */
	bool operator<(const RepoColor &color) const;

	//! Returns the next color from a pre-defined set of 138 colours.
	static RepoColor getNext();

	//! Generates a random colour.
	static RepoColor getNextRandom();

	//! Returns a colour opposite on the colour wheel (HSL style).
	RepoColor getComplement() const;

	//! Returns a string of [R, G, B, A].
	std::string toString() const;

private :

	static unsigned int colorCounter;

	//! 138 predefined HTML named colours (without black and shades of gray) randomly ordered.
	// See http://www.w3schools.com/html/html_colornames.asp
	// and http://www.color-hex.com/color-names.html
	static const std::string COLORS[];

};

} // end namespace gui
} // end namespace repo

#endif // REPO_COLOR_H
