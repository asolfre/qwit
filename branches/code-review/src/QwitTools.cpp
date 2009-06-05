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
 *  QwitTools class implementation
 */

#ifndef QwitTools_cpp
#define QwitTools_cpp

#include <QTranslator>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDir>
#include <QCryptographicHash>

#include "QwitTools.h"
#include "UserpicsDownloader.h"
#include "Configuration.h"

#include <cstdlib>
#include <iostream>

using namespace std;

QwitTools* QwitTools::instance = 0;

QwitTools::QwitTools() {
	monthes["Jan"] = 1;
	monthes["Feb"] = 2;
	monthes["Mar"] = 3;
	monthes["Apr"] = 4;
	monthes["May"] = 5;
	monthes["Jun"] = 6;
	monthes["Jul"] = 7;
	monthes["Aug"] = 8;
	monthes["Sep"] = 9;
	monthes["Oct"] = 10;
	monthes["Nov"] = 11;
	monthes["Dec"] = 12;
}

QwitTools* QwitTools::getInstance() {
	if (!instance) {
		instance = new QwitTools();
	}
	return instance;
}

QDateTime QwitTools::dateFromString(QString date) {
	return getInstance()->_dateFromString(date);
}

QIcon QwitTools::getToolButtonIcon(const QString &iconFileName) {
	return getInstance()->_getToolButtonIcon(iconFileName);
}

QString QwitTools::formatDateTime(const QDateTime &time) {
	return getInstance()->_formatDateTime(time);
}

QVector<Status> QwitTools::parseStatuses(const QByteArray &data) {
	return getInstance()->_parseStatuses(data);
}

Status QwitTools::parseUser(const QByteArray &data) {
	return getInstance()->_parseUser(data);
}

QDateTime QwitTools::_dateFromString(QString date) {
	char s[10];
	int year, day, hours, minutes, seconds;
	sscanf(qPrintable(date), "%*s %s %d %d:%d:%d %*s %d", s, &day, &hours, &minutes, &seconds, &year);
	int month = monthes[s];
	return QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds));
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

QVector<Status> QwitTools::_parseStatuses(const QByteArray &data) {
	QVector<Status> statuses;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "statuses") {
		return statuses;
	}
	
	QDomNode node = root.firstChild();
	QString html = "";
	QString trayMessage = "";
	while (!node.isNull()) {
		if (node.toElement().tagName() != "status") {
			return statuses;
		}
		QDomNode node2 = node.firstChild();
		QString message = "", timeStr = "", user = "", image = "";
		int id = 0, replyUserID = 0, replyStatusId = 0;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				message = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toInt();
			} else if (node2.toElement().tagName() == "in_reply_to_status_id") {
				replyStatusId = node2.toElement().text().toInt();
			} else if (node2.toElement().tagName() == "in_reply_to_user_id") {
				replyUserID = node2.toElement().text().toInt();
			} else if (node2.toElement().tagName() == "user") {
				QDomNode node3 = node2.firstChild();
				while (!node3.isNull()) {
					if (node3.toElement().tagName() == "screen_name") {
						user = node3.toElement().text();
					} else if (node3.toElement().tagName() == "profile_image_url") {
						image = node3.toElement().text();
					}
					node3 = node3.nextSibling();
				}
			}
			node2 = node2.nextSibling();
		}
		if (id) {
			QDateTime time = QwitTools::dateFromString(timeStr);
			time = QDateTime(time.date(), time.time(), Qt::UTC);
			QByteArray hash = QCryptographicHash::hash(image.toAscii(), QCryptographicHash::Md5);
			QString imageFileName = "";
			for (int i = 0; i < hash.size(); ++i) {
				unsigned char c = hash[i];
				c >>= 4;
				if (c < 10) {
					c += '0';
				} else {
					c += 'A' - 10;
				}
				imageFileName += (char)c;
				c = hash[i];
				c &= 15;
				if (c < 10) {
					c += '0';
				} else {
					c += 'A' - 10;
				}
				imageFileName += (char)c;
			}
			QDir dir(QDir::homePath());
			dir.mkdir(Configuration::CacheDirectory);
			imageFileName = dir.absolutePath() + "/" + Configuration::CacheDirectory + "/" + imageFileName;
			UserpicsDownloader::getInstance()->download(image, imageFileName);
			statuses.push_back(Status(id, message.simplified(), user, imageFileName, time.toLocalTime()));
		}
		node = node.nextSibling();
	}
	return statuses;
}

Status QwitTools::_parseUser(const QByteArray &data) {
	Status status;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "user") {
		return status;
	}
	
	QDomNode node = root.firstChild();
	QString html = "";
	QString trayMessage = "";
	while (!node.isNull()) {
		if (node.toElement().tagName() == "status") {
			QDomNode node2 = node.firstChild();
			QString message = "", timeStr = "", user = "", image = "";
			int id = 0, replyUserID = 0, replyStatusId = 0;
			while (!node2.isNull()) {
				if (node2.toElement().tagName() == "created_at") {
					timeStr = node2.toElement().text();
				} else if (node2.toElement().tagName() == "text") {
					message = node2.toElement().text();
				} else if (node2.toElement().tagName() == "id") {
					id = node2.toElement().text().toInt();
				} else if (node2.toElement().tagName() == "in_reply_to_status_id") {
					replyStatusId = node2.toElement().text().toInt();
				} else if (node2.toElement().tagName() == "in_reply_to_user_id") {
					replyUserID = node2.toElement().text().toInt();
				} else if (node2.toElement().tagName() == "screen_name") {
					user = node2.toElement().text();
				} else if (node2.toElement().tagName() == "profile_image_url") {
					image = node2.toElement().text();
				}
				node2 = node2.nextSibling();
			}
			if (id) {
				QDateTime time = QwitTools::dateFromString(timeStr);
				time = QDateTime(time.date(), time.time(), Qt::UTC);
				QByteArray hash = QCryptographicHash::hash(image.toAscii(), QCryptographicHash::Md5);
				QString imageFileName = "";
				for (int i = 0; i < hash.size(); ++i) {
					unsigned char c = hash[i];
					c >>= 4;
					if (c < 10) {
						c += '0';
					} else {
						c += 'A' - 10;
					}
					imageFileName += (char)c;
					c = hash[i];
					c &= 15;
					if (c < 10) {
						c += '0';
					} else {
						c += 'A' - 10;
					}
					imageFileName += (char)c;
				}
				QDir dir(QDir::homePath());
				dir.mkdir(Configuration::CacheDirectory);
				imageFileName = dir.absolutePath() + "/" + Configuration::CacheDirectory + "/" + imageFileName;
				UserpicsDownloader::getInstance()->download(image, imageFileName);
				status = Status(id, message.simplified(), user, imageFileName, time.toLocalTime());
			}
		} else {
		}
		node = node.nextSibling();
	}
	return status;
}

Status QwitTools::parseStatus(const QByteArray &data) {
	return getInstance()->_parseStatus(data);
}

Status QwitTools::_parseStatus(const QByteArray &data) {
	Status status;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "status") {
		return status;
	}
	
	QDomNode node = root.firstChild();
	QString html = "";
	QString trayMessage = "";
	QString message = "", timeStr = "", user = "", image = "";
	int id = 0, replyUserID = 0, replyStatusId = 0;
	while (!node.isNull()) {
		if (node.toElement().tagName() == "created_at") {
			timeStr = node.toElement().text();
		} else if (node.toElement().tagName() == "text") {
			message = node.toElement().text();
		} else if (node.toElement().tagName() == "id") {
			id = node.toElement().text().toInt();
		} else if (node.toElement().tagName() == "in_reply_to_status_id") {
			replyStatusId = node.toElement().text().toInt();
		} else if (node.toElement().tagName() == "in_reply_to_user_id") {
			replyUserID = node.toElement().text().toInt();
		}
		node = node.nextSibling();
	}
	if (id) {
		QDateTime time = QwitTools::dateFromString(timeStr);
		time = QDateTime(time.date(), time.time(), Qt::UTC);
		QByteArray hash = QCryptographicHash::hash(image.toAscii(), QCryptographicHash::Md5);
		QString imageFileName = "";
		for (int i = 0; i < hash.size(); ++i) {
			unsigned char c = hash[i];
			c >>= 4;
			if (c < 10) {
				c += '0';
			} else {
				c += 'A' - 10;
			}
			imageFileName += (char)c;
			c = hash[i];
			c &= 15;
			if (c < 10) {
				c += '0';
			} else {
				c += 'A' - 10;
			}
			imageFileName += (char)c;
		}
		QDir dir(QDir::homePath());
		dir.mkdir(Configuration::CacheDirectory);
		imageFileName = dir.absolutePath() + "/" + Configuration::CacheDirectory + "/" + imageFileName;
		UserpicsDownloader::getInstance()->download(image, imageFileName);
		status = Status(id, message.simplified(), user, imageFileName, time.toLocalTime());
	}
	return status;
}

void QwitTools::log(const QString &message) {
	getInstance()->_log(message);
}

void QwitTools::_log(const QString &message) {
	cout << qPrintable(QDateTime::currentDateTime().toString()) << ":  " << qPrintable(message) << endl;
}

void QwitTools::makeStatusesUnique(QVector<Status> &v) {
	getInstance()->_makeStatusesUnique(v);
}

void QwitTools::_makeStatusesUnique(QVector<Status> &v) {
	int n = 0;
	for (int i = 0; i < v.size(); ++i) {
		if (!n || (v[i] != v[n - 1])) {
			v[n++] = v[i];
		}
	}
	v.resize(n);
}

#endif