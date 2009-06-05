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
 *  PublicPage class implementation
 */

#ifndef PublicPage_cpp
#define PublicPage_cpp

#include <QGridLayout>
#include <QScrollArea>
#include <QScrollBar>

#include "PublicPage.h"
#include "QwitTools.h"

#include <iostream>

using namespace std;

PublicPage::PublicPage(QWidget* parent): AbstractPage(parent) {
	QwitTools::log("PublicPage::PublicPage()");

	twitterWidget = new TwitterWidget(this);
	twitterWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
	twitterWidget->setObjectName(QString::fromUtf8("publicPageTwitterWidget"));
	twitterWidget->disableMoreButton();
	twitterWidget->disableLessButton();

	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setObjectName(QString::fromUtf8("publicPageGridLayout"));

	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Light);
	scrollArea->setWidget(twitterWidget);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	gridLayout->addWidget(scrollArea, 0, 0, 1, 1);
}

void PublicPage::updateSize() {
	QwitTools::log("PublicPage::updateSize()");

	twitterWidget->resize(scrollArea->width() - scrollArea->verticalScrollBar()->width() - 5, 500);
}

QString PublicPage::title() {
	return tr("Public");
}

void PublicPage::update() {
	QwitTools::log("PublicPage::update()");
	Configuration *config = Configuration::getInstance();
	config->currentAccount()->receivePublicStatuses(config->messagesPerPage);
}

#endif
