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

#ifndef QwitTools_cpp
#define QwitTools_cpp

#include <QTranslator>

#include "QwitTools.h"

QwitTools* QwitTools::instance = 0;

QwitTools::QwitTools() {
}

QwitTools* QwitTools::getInstance() {
	if (!instance) {
		instance = new QwitTools();
	}
	return instance;
}

QIcon QwitTools::getToolButtonIcon(const QString &iconFileName) {
	return getInstance()->_getToolButtonIcon(iconFileName);
}

QString QwitTools::formatDateTime(const QDateTime &time) {
	return getInstance()->_formatDateTime(time);
}

QIcon QwitTools::_getToolButtonIcon(const QString &iconFileName) {
	QIcon icon(iconFileName);
	QPixmap normalPixmap = icon.pixmap(16, 16, QIcon::Disabled, QIcon::Off);
	QPixmap activePixmap = icon.pixmap(16, 16, QIcon::Normal, QIcon::Off);
	icon.addPixmap(normalPixmap, QIcon::Normal, QIcon::Off);
	icon.addPixmap(activePixmap, QIcon::Active, QIcon::Off);
	return icon;
}

QString QwitTools::_formatDateTime(const QDateTime &time) {
	int seconds = time.secsTo(QDateTime::currentDateTime());
	if (seconds <= 15) return tr("Just now");
	if (seconds <= 45) return tr("about %n second(s) ago", "", seconds);
	int minutes = (seconds - 45 + 59) / 60;
	if (minutes <= 45) return tr("about %n minute(s) ago", "", minutes);
	int hours = (seconds - 45 * 60 + 3599) / 3600;
	if (hours <= 18) return tr("about %n hour(s) ago", "", hours);
	int days = (seconds - 18 * 3600 + 24 * 3600 - 1) / (24 * 3600);
	return tr("about %n day(s) ago", "", days);
}

#endif