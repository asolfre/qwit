/*  This file is part of Qwit.

    Copyright (C) 2008, 2009 Artem Iglikov
    
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

#ifndef AbstractPage_h
#define AbstractPage_h

#include <QWidget>

#include "TwitterWidget.h"
#include "Configuration.h"

class AbstractPage: public QWidget {
	Q_OBJECT
	
public:
	TwitterWidget *twitterWidget;
	bool visible;
	
	AbstractPage(QWidget *parent = 0);
	
	virtual void update() = 0;
	virtual void addItem(const QString &userpic, const QString &username, const QString &status, const QDateTime &time, int messageId);
	virtual void updateSize() = 0;
	virtual QString title() = 0;
	virtual void reloadUserpics();
	void clear();
};

#endif
