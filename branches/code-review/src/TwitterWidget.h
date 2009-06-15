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
	QTextBrowser *statusTextBrowser;
	QLabel *userpicLabel;
	QLabel *signLabel;
	QToolButton *replyButton;
	QToolButton *favorButton;
	QToolButton *retweetButton;
	QToolButton *unfollowButton;
	QToolButton *directMessageButton;
	
	int top;
	int height;
	Status status;
	QColor color;

	void loadUserpic();
	~TwitterWidgetItem();
};

const int ICON_SIZE = 48;
const int MARGIN = 5;

class TwitterWidget: public QWidget {
	Q_OBJECT

private:
	QVector<TwitterWidgetItem*> items;
	QButtonGroup retweetButtonGroup;
	QButtonGroup replyButtonGroup;
	QButtonGroup directMessageButtonGroup;

public:
	
	QToolButton *moreToolButton;
	QToolButton *lessToolButton;
	
	TwitterWidget(QWidget *parent);
	void clear();
	void addItem(const Status &status);
	void updateItems();
	void reloadUserpics();
	void addMoreButton();
	void addLessButton();
	void removeMoreButton();
	void removeLessButton();
	
protected:

	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);

public slots:

	void retweetButtonClicked(int id);
	void replyButtonClicked(int id);
	void directMessageButtonClicked(int id);
	void disableMoreButton();
	void disableLessButton();
	void enableMoreButton();
	void enableLessButton();

signals:
	
	void moreButtonClicked();
	void lessButtonClicked();
	void retweet(const Status &status);
	void reply(const Status &status);
	void directMessage(const Status &status);
};

#endif
