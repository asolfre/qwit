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

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTextBrowser>
#include <QLabel>
#include <QDateTime>
#include <QToolButton>

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
};

const int ICON_SIZE = 48;
const int MARGIN = 5;

class TwitterWidget: public QWidget {
	Q_OBJECT

private:
	QVector<TwitterWidgetItem> items;

public:

	TwitterWidget(QWidget *parent);
	void clear();
	void addItem(const QString &userpic, const QString &username, const QString &status, const QDateTime &time, int messageId);
	void updateItems();
	void reloadUserpics();

protected:

	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
};

#endif
