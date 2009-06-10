/*!
 *  @file
 *  @author Artem Iglikov <artem.iglikov@gmail.com>
 *  
 *  @section LICENSE
 *  
 *  This file is part of Qwit.
 *  
 *  Copyright (C) 2008, 2009 Artem Iglikov
 *  
 *  Qwit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  Qwit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with Qwit.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *  @section DESCRIPTION
 *  
 *  StatusTextEdit class implementation
 */

#ifndef StatusTextEdit_cpp
#define StatusTextEdit_cpp

#include "QwitHeaders.h"

#include "StatusTextEdit.h"
#include "Configuration.h"

const int StatusTextEdit::MaxStatusCharacters;
const int StatusTextEdit::StandardHeight;

StatusTextEdit::StatusTextEdit(QWidget *parent): QTextEdit(parent) {
	setFixedHeight(StandardHeight);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAcceptRichText(false);
	setTabChangesFocus(true);
	inReplyToStatusId = 0;
	emit leftCharsNumberChanged(MaxStatusCharacters);
	connect(this, SIGNAL(textChanged()), this, SLOT(textChangedToCharsNumberChanged()));
}

int StatusTextEdit::getMaxStatusCharactersNumber() {
	return MaxStatusCharacters;
}

void StatusTextEdit::focusInEvent(QFocusEvent *event) {
	QTextEdit::focusInEvent(event);
}

void StatusTextEdit::focusOutEvent(QFocusEvent *event) {
	QTextEdit::focusOutEvent(event);
}

void StatusTextEdit::keyPressEvent(QKeyEvent *e) {
	if ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)) {
		emit statusEntered(toPlainText(), inReplyToStatusId);
		clear();
		if (height() != StandardHeight) {
			setFixedHeight(StandardHeight);
		}
		e->accept();
		return;
	}
	QTextEdit::keyPressEvent(e);
}

void StatusTextEdit::textChangedToCharsNumberChanged() {
	emit leftCharsNumberChanged(MaxStatusCharacters - toPlainText().length());
	updateSize();
}

void StatusTextEdit::updateSize() {
	if (verticalScrollBar()->maximum() - verticalScrollBar()->minimum()) {
		setFixedHeight(height() + verticalScrollBar()->maximum() - verticalScrollBar()->minimum());
	}
}

void StatusTextEdit::contextMenuEvent(QContextMenuEvent *event) {
// custom context menu example
	QMenu *menu = createStandardContextMenu();
	menu->addAction(tr("My Menu Item"));
	menu->exec(event->globalPos());
	delete menu;
}

void StatusTextEdit::retweet(const Status &status) {
	Configuration *config = Configuration::getInstance();
	QString retweetTag = config->retweetTag;
	int i = retweetTag.indexOf("@");
	if (i != -1) {
		QString firsthalf = retweetTag.left(i + 1);
		QString secondhalf = retweetTag.right(retweetTag.size() - (i + 1));
		retweetTag = firsthalf + status.username + secondhalf;
	}
	QString text = status.status;
	if (config->retweetTagAfterText) {
		setText(text + " " + retweetTag);
	} else {
		setText(retweetTag + " " + text);
	}

	setFocus(Qt::OtherFocusReason);
	moveCursor(QTextCursor::End);
}

void StatusTextEdit::reply(const Status &status) {
	Configuration *config = Configuration::getInstance();

	QString text = toPlainText().simplified();
	setText("@" + status.username + " " + text);

	inReplyToStatusId = status.id;
	
	setFocus(Qt::OtherFocusReason);
	moveCursor(QTextCursor::NextWord);
}

#endif
