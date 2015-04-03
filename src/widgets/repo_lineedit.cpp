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

#include "repo_lineedit.h"

repo::gui::RepoLineEdit::RepoLineEdit(QWidget * parent) 
	: QLineEdit(parent)
{
	this->setPlaceholderText(tr("Filter"));
	clearButton = new QToolButton(this);

    // See http://doc.qt.io/qt-5/qstyle.html#StandardPixmap-enum
    clearButton->setIcon(this->style()->standardIcon(QStyle::SP_LineEditClearButton));
//	clearButton->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_ban, QColor(Qt::darkRed)));
	clearButton->setToolTip(tr("Clear"));
	clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");    
    //--------------------------------------------------------------------------
	QObject::connect(
		clearButton, &QToolButton::clicked,
		this, &RepoLineEdit::clear);
	QObject::connect(
		this, &RepoLineEdit::textChanged,
		this, &RepoLineEdit::setClearButtonEnabled);
    //--------------------------------------------------------------------------
	int width = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(
		clearButton->sizeHint().width() + width + 1));
	QSize sizeHint = minimumSizeHint();
	setMinimumSize(qMax(sizeHint.width(), clearButton->sizeHint().height() + width * 2 + 2),
		qMax(sizeHint.height(), clearButton->sizeHint().height() + width * 2 + 2));
	


	setClearButtonEnabled(this->text());
}

repo::gui::RepoLineEdit::~RepoLineEdit()
{
	delete clearButton;
}

 void repo::gui::RepoLineEdit::resizeEvent(QResizeEvent *)
{
	QSize size = clearButton->sizeHint();
	int width = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	clearButton->move(rect().right() - width - size.width(),
		(rect().bottom() + 1 - size.height())/2);
}

 void repo::gui::RepoLineEdit::setClearButtonEnabled(const QString &text)
{
	bool on = !text.isEmpty();
	clearButton->setVisible(on);
	clearButton->setEnabled(on);
//    QString color = on ? "#fff2cc" : "#fff"; // TODO: move colours to settings
//    this->setStyleSheet("background-color: " + color);
 }
