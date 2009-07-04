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
 *  MessageTextEdit class implementation
 */

#ifndef MessageTextEdit_cpp
#define MessageTextEdit_cpp

#include "QwitHeaders.h"

#include "MessageTextEdit.h"
#include "Configuration.h"
#include "QwitTools.h"

const int MessageTextEdit::MaxMessageCharacters;
const int MessageTextEdit::StandardHeight;

MessageTextEdit::MessageTextEdit(QWidget *parent): QTextEdit(parent) {
	setFixedHeight(StandardHeight);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAcceptRichText(false);
	setTabChangesFocus(true);
	inReplyToMessageId = 0;
	emit leftCharsNumberChanged(MaxMessageCharacters);
	connect(this, SIGNAL(textChanged()), this, SLOT(textChangedToCharsNumberChanged()));
}

int MessageTextEdit::getMaxMessageCharactersNumber() {
	return MaxMessageCharacters;
}

void MessageTextEdit::focusInEvent(QFocusEvent *event) {
	QTextEdit::focusInEvent(event);
}

void MessageTextEdit::focusOutEvent(QFocusEvent *event) {
	QTextEdit::focusOutEvent(event);
}

void MessageTextEdit::clear() {
	QTextEdit::clear();
	if (height() != StandardHeight) {
		setFixedHeight(StandardHeight);
	}
}

void MessageTextEdit::keyPressEvent(QKeyEvent *e) {
	if ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)) {
		setEnabled(false);
		emit messageEntered(toPlainText(), inReplyToMessageId);
		e->accept();
		return;
	}
	QTextEdit::keyPressEvent(e);
}

void MessageTextEdit::textChangedToCharsNumberChanged() {
	emit leftCharsNumberChanged(MaxMessageCharacters - toPlainText().length());
	updateSize();
}

void MessageTextEdit::updateSize() {
	if (verticalScrollBar()->maximum() - verticalScrollBar()->minimum()) {
		setFixedHeight(height() + verticalScrollBar()->maximum() - verticalScrollBar()->minimum());
	}
}

void MessageTextEdit::contextMenuEvent(QContextMenuEvent *event) {
// custom context menu example
	QMenu *menu = createStandardContextMenu();
	menu->addAction(tr("My Menu Item"));
	menu->exec(event->globalPos());
	delete menu;
}

void MessageTextEdit::retweet(const Message &message) {
	Configuration *config = Configuration::getInstance();
	QString retweetTag = config->retweetTag;
	int i = retweetTag.indexOf("@");
	if (i != -1) {
		QString firsthalf = retweetTag.left(i + 1);
		QString secondhalf = retweetTag.right(retweetTag.size() - (i + 1));
		retweetTag = firsthalf + message.username + secondhalf;
	}
	QString text = message.text;
	if (config->retweetTagAfterText) {
		setText(text + " " + retweetTag);
	} else {
		setText(retweetTag + " " + text);
	}

	setFocus(Qt::OtherFocusReason);
	moveCursor(QTextCursor::End);
}

void MessageTextEdit::reply(const Message &message) {
	Configuration *config = Configuration::getInstance();

	QString text = toPlainText().simplified();
	setText("@" + message.username + " " + text);

	inReplyToMessageId = message.id;
	
	setFocus(Qt::OtherFocusReason);
	moveCursor(QTextCursor::NextWord);
}

void MessageTextEdit::insertFromMimeData(const QMimeData *source) {
	QString text = source->text();
	if (QwitTools::isUrl(text) && (text.length() > 20)) {
		setEnabled(false);
		UrlShortener::getInstance()->shorten(text);
	} else {
		QTextEdit::insertFromMimeData(source);
	}
}

void MessageTextEdit::insertUrl(const QString &url) {
	insertPlainText(url);
	setEnabled(true);
}

#endif
