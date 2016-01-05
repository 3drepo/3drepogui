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

#include <repo/lib/repo_log.h>
#include "repo_fontawesome.h"

using namespace repo::gui::primitive;

//------------------------------------------------------------------------------
QColor RepoFontAwesome::COLOR_NORMAL		= QColor(64,64,64);
QColor RepoFontAwesome::COLOR_DISABLED	= QColor(70,70,70,60);
QColor RepoFontAwesome::COLOR_ACTIVE		= QColor(10,10,10);
QColor RepoFontAwesome::COLOR_SELECTED	= QColor(10,10,10);

RepoFontAwesome::RepoFontAwesome(QObject* parent)
	: QObject(parent)
	, rawFont(0)
{
    QFile res(":/fontawesome");
	if(!res.open(QIODevice::ReadOnly)) 
		repoLogError("Font Awesome not found.");
	
	QByteArray fontData(res.readAll());
	res.close();
    static int fontAwesomeFontId =
            QFontDatabase::addApplicationFontFromData(fontData);
    static QStringList loadedFontFamilies =
            QFontDatabase::applicationFontFamilies(fontAwesomeFontId);
    if(!loadedFontFamilies.empty()) 
		fontName = loadedFontFamilies.at(0);
		rawFont = new QRawFont(fontData, 14);
}

RepoFontAwesome& RepoFontAwesome::getInstance()
{
	// Static variable is created and destroyed only once.
	static RepoFontAwesome instance;
	return instance;
}

QIcon RepoFontAwesome::getIcon(int iconOn)
{
    return QIcon(new RepoIconEngine(this, iconOn, iconOn));
}

QIcon RepoFontAwesome::getIcon(int iconOn, const QColor &color)
{
    return QIcon(new RepoIconEngine(this, iconOn, iconOn, color));
}

QIcon RepoFontAwesome::getIcon(int iconOn, int iconOff)
{
    return QIcon(new RepoIconEngine(this, iconOn, iconOff));
}

QFont RepoFontAwesome::getFont(int size)
{
    QFont font(fontName);
    font.setPixelSize(size);
    return font;
}

QRawFont *RepoFontAwesome::getRawFont()
{
	return rawFont;
}

quint32 RepoFontAwesome::getGlyphIndex(int icon)
{
	QChar c(icon);
	QVector<quint32> glyphs = rawFont->glyphIndexesForString(c);
	return glyphs.size() > 0 ? glyphs[0] : 0;
}

QPainterPath RepoFontAwesome::getPainterPath(int icon)
{
	return rawFont->pathForGlyph(getGlyphIndex(icon));
}
