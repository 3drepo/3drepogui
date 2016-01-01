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

#include "repo_fontawesome.h"

//------------------------------------------------------------------------------
QColor repo::gui::RepoFontAwesome::COLOR_NORMAL		= QColor(64,64,64);
QColor repo::gui::RepoFontAwesome::COLOR_DISABLED	= QColor(70,70,70,60);
QColor repo::gui::RepoFontAwesome::COLOR_ACTIVE		= QColor(10,10,10);
QColor repo::gui::RepoFontAwesome::COLOR_SELECTED	= QColor(10,10,10);

repo::gui::RepoFontAwesome::RepoFontAwesome(QObject* parent) 
	: QObject(parent)
	, rawFont(0)
{
    QFile res(":/fontawesome");
	if(!res.open(QIODevice::ReadOnly)) 
        std::cerr << "Font Awesome not found." << std::endl;
	
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

repo::gui::RepoFontAwesome& repo::gui::RepoFontAwesome::getInstance()
{
	// Static variable is created and destroyed only once.
	static RepoFontAwesome instance;
	return instance;
}

QIcon repo::gui::RepoFontAwesome::getIcon(int iconOn)
{
    return QIcon(new RepoIconEngine(this, iconOn, iconOn));
}

QIcon repo::gui::RepoFontAwesome::getIcon(int iconOn, const QColor &color)
{
    return QIcon(new RepoIconEngine(this, iconOn, iconOn, color));
}

QIcon repo::gui::RepoFontAwesome::getIcon(int iconOn, int iconOff)
{
    return QIcon(new RepoIconEngine(this, iconOn, iconOff));
}

QFont repo::gui::RepoFontAwesome::getFont(int size)
{
    QFont font(fontName);
    font.setPixelSize(size);
    return font;
}

QRawFont *repo::gui::RepoFontAwesome::getRawFont()
{
	return rawFont;
}

quint32 repo::gui::RepoFontAwesome::getGlyphIndex(int icon)
{
	QChar c(icon);
	QVector<quint32> glyphs = rawFont->glyphIndexesForString(c);
	return glyphs.size() > 0 ? glyphs[0] : 0;
}

QPainterPath repo::gui::RepoFontAwesome::getPainterPath(int icon)
{
	return rawFont->pathForGlyph(getGlyphIndex(icon));
}
