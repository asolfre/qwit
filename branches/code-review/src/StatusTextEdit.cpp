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

#include <QToolTip>
#include <QScrollBar>
#include <QMenu>

#include "StatusTextEdit.h"

#include <iostream>

using namespace std;

const int StatusTextEdit::MaxStatusCharacters;
const int StatusTextEdit::StandardHeight;

StatusTextEdit::StatusTextEdit(QWidget *parent): QTextEdit(parent) {
	setFixedHeight(StandardHeight);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAcceptRichText(false);
	setTabChangesFocus(true);
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
		emit statusEntered(toPlainText());
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

#endif
