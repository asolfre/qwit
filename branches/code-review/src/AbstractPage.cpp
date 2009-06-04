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

#include "MainWindow.h"
#include "TwitterWidget.h"

#include "AbstractPage.h"
#include "QwitTools.h"

#include <iostream>

using namespace std;

AbstractPage::AbstractPage(QWidget *parent): QWidget(parent) {
	QwitTools::log("AbstractPage::AbstractPage()");
}

void AbstractPage::addItem(const QString &userpic, const QString &username, const QString &status, const QDateTime &time, int messageId) {
	QwitTools::log("AbstractPage::addItem()");
	twitterWidget->addItem(userpic, username, status, time, messageId);
}

void AbstractPage::clear() {
	QwitTools::log("AbstractPage::clear()");
	twitterWidget->clear();
}

void AbstractPage::reloadUserpics() {
	QwitTools::log("AbstractPage::reloadUserpics()");
	twitterWidget->reloadUserpics();
}

#endif
