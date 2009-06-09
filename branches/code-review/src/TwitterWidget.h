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
 *  TwitterWidget class declaration
 */

#ifndef TwitterWidget_h
#define TwitterWidget_h

#include "QwitHeaders.h"

#include "Status.h"

class TwitterWidgetItem {
public:
	QTextBrowser *status;
	QLabel *userpic;
	QLabel *sign;
	QToolButton *replyButton;
	QToolButton *favorButton;
	QToolButton *retweetButton;
	QToolButton *unfollowButton;
	QToolButton *directMessageButton;
	
	QString userpicFileName;
	int top;
	int height;
	QColor color;
	QDateTime time;
	QString username;
	int messageId;

	QString rawStatus;
	
	void loadUserpic();
	~TwitterWidgetItem();
};

const int ICON_SIZE = 48;
const int MARGIN = 5;

class TwitterWidget: public QWidget {
	Q_OBJECT

private:
	QVector<TwitterWidgetItem*> items;

public:

	QToolButton *moreToolButton;
	QToolButton *lessToolButton;
	
	TwitterWidget(QWidget *parent);
	void clear();
	void addItem(const Status &status);
	void updateItems();
	void reloadUserpics();
	void disableMoreButton();
	void disableLessButton();
	void enableMoreButton();
	void enableLessButton();
	
protected:

	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
	
signals:
	
	void moreButtonClicked();
	void lessButtonClicked();
};

#endif
