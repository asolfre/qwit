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
	completer = new QCompleter(this);
	completer->setModel(new QStringListModel(QStringList(), completer));
	completer->setWidget(this);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(completer, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}

int MessageTextEdit::getMaxMessageCharactersNumber() {
	return MaxMessageCharacters;
}

void MessageTextEdit::focusInEvent(QFocusEvent *event) {
	if (completer) {
		completer->setWidget(this);
	}
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
	if (completer && completer->popup()->isVisible()) {
	 // The following keys are forwarded by the completer to the widget
		switch (e->key()) {
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				return; // let the completer do default behavior
			default:
				break;
		}
	}
	if ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)) {
		setEnabled(false);
		emit messageEntered(toPlainText(), inReplyToMessageId);
		e->accept();
		return;
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!completer || !isShortcut) {
		// dont process the shortcut when we have a completer
		QTextEdit::keyPressEvent(e);
	}

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!completer || (ctrlOrShift && e->text().isEmpty())) {
		return;
	}

	static QString eow("~!#@$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if (!isShortcut && (hasModifier || e->text().isEmpty() || (completionPrefix.length() < 2) || eow.contains(e->text().right(1)))) {
		completer->popup()->hide();
		return;
	}

	if (completionPrefix != completer->completionPrefix()) {
		completer->setCompletionPrefix(completionPrefix);
		completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(completer->popup()->sizeHintForColumn(0) + completer->popup()->verticalScrollBar()->sizeHint().width());
	completer->complete(cr); // popup it up!
//QTextEdit::keyPressEvent(e);
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
/*
void MessageTextEdit::setCompleter(QCompleter *completer) {
	if (this->completer) {
		QObject::disconnect(completer, 0, this, 0);
	}

	this->completer = completer;

	if (!completer) {
		return;
	}

	completer->setWidget(this);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(completer, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}
*/
void MessageTextEdit::insertCompletion(const QString& completion) {
	if (completer->widget() != this) {
		return;
	}
	QTextCursor tc = textCursor();
	int extra = completion.length() - completer->completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	setTextCursor(tc);
}

QString MessageTextEdit::textUnderCursor() const {
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}

#endif
