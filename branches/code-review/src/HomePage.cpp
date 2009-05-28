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

#ifndef HomePage_cpp
#define HomePage_cpp

#include <QGridLayout>
#include <QScrollArea>
#include <QScrollBar>

#include "HomePage.h"

HomePage::HomePage(QWidget* parent): AbstractPage(parent) {
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
	
	addItem("/home/arti/.qwit/0850B339F43DBD4C82B194D1E1CFE525", "arti", "Programing Qwit", QDateTime::currentDateTime(), 0);
	addItem("/home/arti/.qwit/0850B339F43DBD4C82B194D1E1CFE525", "arti", "me and Chris Skyping about NewEurasia.net and other things :) I love this job :) http://murl.kz/NRk3", QDateTime::currentDateTime(), 1);
}

void HomePage::updateSize() {
	twitterWidget->resize(scrollArea->width() - scrollArea->verticalScrollBar()->width() - 5, 500);
}

QString HomePage::title() {
	return tr("Home");
}

#endif
