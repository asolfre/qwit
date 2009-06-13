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
 *  AbstractPage class implementation
 */

#ifndef AbstractPage_cpp
#define AbstractPage_cpp

#include "QwitHeaders.h"

#include "MainWindow.h"
#include "TwitterWidget.h"

#include "AbstractPage.h"
#include "QwitTools.h"

AbstractPage::AbstractPage(QWidget *parent): QWidget(parent) {
	qDebug() << ("AbstractPage::AbstractPage()");
	twitterWidget = new TwitterWidget(this);
	twitterWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
	connect(twitterWidget, SIGNAL(retweet(const Status &)), MainWindow::getInstance(), SIGNAL(retweet(const Status &)));
	connect(twitterWidget, SIGNAL(reply(const Status &)), MainWindow::getInstance(), SIGNAL(reply(const Status &)));
}

void AbstractPage::addItem(const Status &status) {
//	qDebug() << ("AbstractPage::addItem()");
	twitterWidget->addItem(status);
}

void AbstractPage::clear() {
	qDebug() << ("AbstractPage::clear()");
	twitterWidget->clear();
}

void AbstractPage::reloadUserpics() {
	qDebug() << ("AbstractPage::reloadUserpics()");
	twitterWidget->reloadUserpics();
}

void AbstractPage::updateItems(const QVector<Status> &items, Account *account) {
	qDebug() << ("AbstractPage::updateItems()");
	int scrollPosition = scrollArea->verticalScrollBar()->value();
	clear();
	for (int i = 0; i < items.size(); ++i) {
		addItem(items[i]);
	}
	twitterWidget->updateItems();
	scrollArea->verticalScrollBar()->setValue(scrollPosition);
}

void AbstractPage::updatePrevious() {
}

void AbstractPage::removePrevious() {
}

void AbstractPage::redraw() {
	twitterWidget->updateItems();
}

#endif
