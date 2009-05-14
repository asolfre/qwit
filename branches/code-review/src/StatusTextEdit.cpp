/*  This file is part of Qwit.

    Qwit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Qwit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Qwit.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef StatusTextEdit_cpp
#define StatusTextEdit_cpp

#include <QToolTip>
#include <QScrollBar>

#include "StatusTextEdit.h"

#include <iostream>

using namespace std;

const int MAX_STATUS_CHARACTERS = 140;
const int STANDARD_HEIGHT = 45;

StatusTextEdit::StatusTextEdit(QWidget *parent): QTextEdit(parent) {
	setFixedHeight(STANDARD_HEIGHT);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAcceptRichText(false);
	emit leftCharsNumberChanged(MAX_STATUS_CHARACTERS);
	connect(this, SIGNAL(textChanged()), this, SLOT(textChangedToCharsNumberChanged()));
}

int StatusTextEdit::getMaxStatusCharactersNumber() {
	return MAX_STATUS_CHARACTERS;
}

void StatusTextEdit::focusInEvent(QFocusEvent *event) {
	QTextEdit::focusInEvent(event);
}

void StatusTextEdit::focusOutEvent(QFocusEvent *event) {
	QTextEdit::focusOutEvent(event);
}

void StatusTextEdit::keyPressEvent(QKeyEvent *e) {
	if ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)) {
		emit returnPressed();
		if (height() != STANDARD_HEIGHT) {
			setFixedHeight(STANDARD_HEIGHT);
		}
		e->accept();
		return;
	}
	QTextEdit::keyPressEvent(e);
}

void StatusTextEdit::textChangedToCharsNumberChanged() {
	emit leftCharsNumberChanged(MAX_STATUS_CHARACTERS - toPlainText().length());
	updateSize();
}

void StatusTextEdit::updateSize() {
	if (verticalScrollBar()->maximum() - verticalScrollBar()->minimum()) {
		setFixedHeight(height() + verticalScrollBar()->maximum() - verticalScrollBar()->minimum());
	}
}

#endif
