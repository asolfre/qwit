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
 *  StatusTextEdit class declaration
 */

#ifndef StatusTextEdit_h
#define StatusTextEdit_h

#include "QwitHeaders.h"

#include "Status.h"

class StatusTextEdit: public QTextEdit {
	Q_OBJECT

private:
	int inReplyToStatusId;
	
public:
	static const int MaxStatusCharacters = 140;
	static const int StandardHeight = 45;

	StatusTextEdit(QWidget *parent = 0);

	void focusInEvent(QFocusEvent *event);
	void focusOutEvent(QFocusEvent *event);
	int getMaxStatusCharactersNumber();

protected:

	void keyPressEvent(QKeyEvent *e);
	void contextMenuEvent(QContextMenuEvent *event);
	
signals:

	void statusEntered(const QString &, int);
	void leftCharsNumberChanged(int);

public slots:

	void textChangedToCharsNumberChanged();
	void updateSize();
	void retweet(const Status &status);
	void reply(const Status &status);
};

#endif
