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

#ifndef RepliesPage_cpp
#define RepliesPage_cpp

#include <QGridLayout>
#include <QScrollArea>
#include <QScrollBar>

#include "RepliesPage.h"

#include <iostream>

using namespace std;

RepliesPage::RepliesPage(QWidget* parent): AbstractPage(parent) {
	twitterWidget = new TwitterWidget(this);
	twitterWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
	twitterWidget->setObjectName(QString::fromUtf8("homePageTwitterWidget"));

	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setObjectName(QString::fromUtf8("homePageGridLayout"));

	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Light);
	scrollArea->setWidget(twitterWidget);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	gridLayout->addWidget(scrollArea, 0, 0, 1, 1);
}

void RepliesPage::updateSize() {
	twitterWidget->resize(scrollArea->width() - scrollArea->verticalScrollBar()->width() - 5, 500);
}

QString RepliesPage::title() {
	return tr("Replies");
}

void RepliesPage::update() {
	Configuration::getInstance()->currentAccount()->twitter->receiveReplies(0, 20);
}

void RepliesPage::updateItems(const QVector<Status> &items) {
	cout << "RepliesPage" << endl;
	clear();
	for (int i = 0; i < items.size(); ++i) {
		addItem(items[i].userpicFilename, items[i].username, items[i].status, items[i].time, items[i].id);
	}
}

#endif
