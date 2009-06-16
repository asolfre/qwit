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

#include "QwitHeaders.h"

#include "QwitTools.h"
#include "UserpicsDownloader.h"
#include "Configuration.h"

QwitTools* QwitTools::instance = 0;
QRegExp QwitTools::urlRegExp("((ht|f)tp(s?)\\:\\/\\/|~/|/)?(\\w+:\\w+@)?(([-\\w]+\\.)+(com|org|net|gov|mil|biz|info|mobi|name|aero|jobs|museum|travel|[a-z]{2}))(:[\\d]{1,5})?(((/([-\\w~!$+|.=]|%[a-f\\d]{2})+)+|/)+|\\?|#)?((\\?([-\\w]|%[a-f\\d{2}])+=([-\\w~!$+|*:=]|%[a-f\\d]{2})*)(&([-\\w~!$+|.,*:]|%[a-f\\d{2}])+=([-\\w~!$+|.,*:=]|%[a-f\\d]{2})*)*)*(#([-\\w~!$+|.,*:=]|%[a-f\\d]{2})*)?");
QRegExp QwitTools::usernameRegExp("(^|\\W+)@(\\w+)($|\\W+)");

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

QIcon QwitTools::getToolButtonIcon(const QString &iconFileName, bool active) {
	return getInstance()->_getToolButtonIcon(iconFileName, active);
}

QString QwitTools::formatDateTime(const QDateTime &time) {
	return getInstance()->_formatDateTime(time);
}

QVector<Status> QwitTools::parseStatuses(const QByteArray &data, Account *account) {
	return getInstance()->_parseStatuses(data, account);
}

QVector<Status> QwitTools::parseInboxMessages(const QByteArray &data, Account *account) {
	return getInstance()->_parseInboxMessages(data, account);
}

Status QwitTools::parseUser(const QByteArray &data, Account *account) {
	return getInstance()->_parseUser(data, account);
}

QDateTime QwitTools::_dateFromString(QString date) {
	char s[10];
	int year, day, hours, minutes, seconds;
	sscanf(qPrintable(date), "%*s %s %d %d:%d:%d %*s %d", s, &day, &hours, &minutes, &seconds, &year);
	int month = monthes[s];
	return QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds));
}

QIcon QwitTools::_getToolButtonIcon(const QString &iconFileName, bool active) {
	QIcon icon(iconFileName);
	if (!active) {
		QPixmap normalPixmap = icon.pixmap(16, 16, QIcon::Disabled, QIcon::Off);
		QPixmap activePixmap = icon.pixmap(16, 16, QIcon::Normal, QIcon::Off);
		icon.addPixmap(normalPixmap, QIcon::Normal, QIcon::Off);
		icon.addPixmap(activePixmap, QIcon::Active, QIcon::Off);
	} else {
		QPixmap activePixmap = icon.pixmap(16, 16, QIcon::Disabled, QIcon::Off);
		icon.addPixmap(activePixmap, QIcon::Active, QIcon::Off);
	}
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

QVector<Status> QwitTools::_parseStatuses(const QByteArray &data, Account *account) {
	QVector<Status> statuses;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "statuses") {
		return statuses;
	}
	
	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() != "status") {
			return statuses;
		}
		QDomNode node2 = node.firstChild();
		QString message = "", timeStr = "", user = "", image = "";
		uint id = 0;
		int replyUserID = 0, replyStatusId = 0;
		bool favorited = false;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				message = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toUInt();
			} else if (node2.toElement().tagName() == "in_reply_to_status_id") {
				replyStatusId = node2.toElement().text().toInt();
			} else if (node2.toElement().tagName() == "in_reply_to_user_id") {
				replyUserID = node2.toElement().text().toInt();
			} else if (node2.toElement().tagName() == "favorited") {
				favorited = node2.toElement().text() == "true";
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
			imageFileName = Configuration::CacheDirectory + imageFileName;
			UserpicsDownloader::getInstance()->download(image, imageFileName);
			statuses.push_back(Status(id, message.simplified(), user, imageFileName, time.toLocalTime(), favorited, account));
		}
		node = node.nextSibling();
	}
	return statuses;
}

QVector<Status> QwitTools::_parseInboxMessages(const QByteArray &data, Account *account) {
	QVector<Status> messages;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "direct-messages") {
		return messages;
	}
	
	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() != "direct_message") {
			return messages;
		}
		QDomNode node2 = node.firstChild();
		QString message = "", timeStr = "", user = "", image = "";
		uint id = 0;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				message = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toUInt();
			} else if (node2.toElement().tagName() == "sender") {
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
			imageFileName = Configuration::CacheDirectory + imageFileName;
			UserpicsDownloader::getInstance()->download(image, imageFileName);
			messages.push_back(Status(id, message.simplified(), user, imageFileName, time.toLocalTime(), false, account));
		}
		node = node.nextSibling();
	}
	return messages;
}

Status QwitTools::_parseUser(const QByteArray &data, Account *account) {
	Status status;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "user") {
		return status;
	}
	
	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() == "status") {
			QDomNode node2 = node.firstChild();
			QString message = "", timeStr = "", user = "", image = "";
			uint id = 0;
			int replyUserID = 0, replyStatusId = 0;
			bool favorited = false;
			while (!node2.isNull()) {
				if (node2.toElement().tagName() == "created_at") {
					timeStr = node2.toElement().text();
				} else if (node2.toElement().tagName() == "text") {
					message = node2.toElement().text();
				} else if (node2.toElement().tagName() == "id") {
					id = node2.toElement().text().toUInt();
				} else if (node2.toElement().tagName() == "in_reply_to_status_id") {
					replyStatusId = node2.toElement().text().toInt();
				} else if (node2.toElement().tagName() == "in_reply_to_user_id") {
					replyUserID = node2.toElement().text().toInt();
				} else if (node2.toElement().tagName() == "screen_name") {
					user = node2.toElement().text();
				} else if (node2.toElement().tagName() == "profile_image_url") {
					image = node2.toElement().text();
				} else if (node2.toElement().tagName() == "favorited") {
					favorited = node2.toElement().text() == "true";
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
				imageFileName = Configuration::CacheDirectory + imageFileName;
				UserpicsDownloader::getInstance()->download(image, imageFileName);
				status = Status(id, message.simplified(), user, imageFileName, time.toLocalTime(), favorited, account);
			}
		} else {
		}
		node = node.nextSibling();
	}
	return status;
}

Status QwitTools::parseStatus(const QByteArray &data, Account *account) {
	return getInstance()->_parseStatus(data, account);
}

Status QwitTools::_parseStatus(const QByteArray &data, Account *account) {
	Status status;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "status") {
		return status;
	}
	
	QDomNode node = root.firstChild();
	QString message = "", timeStr = "", user = "", image = "";
	uint id = 0;
	int replyUserID = 0, replyStatusId = 0;
	bool favorited = false;
	while (!node.isNull()) {
		if (node.toElement().tagName() == "created_at") {
			timeStr = node.toElement().text();
		} else if (node.toElement().tagName() == "text") {
			message = node.toElement().text();
		} else if (node.toElement().tagName() == "id") {
			id = node.toElement().text().toUInt();
		} else if (node.toElement().tagName() == "in_reply_to_status_id") {
			replyStatusId = node.toElement().text().toInt();
		} else if (node.toElement().tagName() == "in_reply_to_user_id") {
			replyUserID = node.toElement().text().toInt();
		} else if (node.toElement().tagName() == "favorited") {
			favorited = node.toElement().text() == "true";
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
		imageFileName = Configuration::CacheDirectory + imageFileName;
		UserpicsDownloader::getInstance()->download(image, imageFileName);
		status = Status(id, message.simplified(), user, imageFileName, time.toLocalTime(), favorited, account);
	}
	return status;
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

QString QwitTools::_prepareStatus(const QString &text, Account *account) {
	QString s = text;
// Process URLs
	{
		s.replace(" www.", " http://www.");
		if (s.startsWith("www.")) s = "http://" + s;
		int pos = 0;
		int lastPos = 0;
		QString t = "";
		while ((pos = urlRegExp.indexIn(s, lastPos)) != -1) {
			QStringList list = urlRegExp.capturedTexts();
			QStringList::iterator it = list.begin();
			QString url = *it;
			t += s.mid(lastPos, pos - lastPos);
			QString href = url;
			if (list[1] == "") {
				href = "http://" + href;
			}
			t += "<a href=\"" + href + "\" style=\"font-weight:bold;text-decoration:none\">" + url + "</a>";
			lastPos = pos + url.length();
		}
		t += s.mid(lastPos);
		s = t;
	}
// Process usernames
	{
		int pos = 0;
		int lastPos = 0;
		QString t = "";
		while ((pos = usernameRegExp.indexIn(s, lastPos)) != -1) {
			QStringList list = usernameRegExp.capturedTexts();
			QStringList::iterator it = list.begin();
			pos = usernameRegExp.pos(2);
			QString username = usernameRegExp.cap(2);
			t += s.mid(lastPos, pos - lastPos);
			t += "<a href=\"" + account->serviceBaseUrl() + "/" + username + "\" style=\"font-weight:bold;text-decoration:none\">" + username + "</a>";
			lastPos = pos + username.length();
		}
		t += s.mid(lastPos);
		s = t;
	}
	return s;
}

QString QwitTools::prepareStatus(const QString &text, Account *account) {
	return getInstance()->_prepareStatus(text, account);
}

void handleMessage(QtMsgType type, const char *msg) {
	switch (type) {
		case QtDebugMsg:
			cout << qPrintable(QDateTime::currentDateTime().toString()) << " Debug: " << msg << endl;
			break;
		case QtWarningMsg:
			cerr << qPrintable(QDateTime::currentDateTime().toString()) << " Warning: " << msg << endl;
			break;
		case QtCriticalMsg:
			cerr << qPrintable(QDateTime::currentDateTime().toString()) << " Critical: " << msg << endl;
			break;
		case QtFatalMsg:
			cerr << qPrintable(QDateTime::currentDateTime().toString()) << " Fatal: " << msg << endl;
			abort();
	}
}

QVector<Status> QwitTools::_mergeStatuses(QVector<Status> &statuses, QVector<Status> &receivedStatuses) {
	QVector<Status> newStatuses;
	for (int i = 0; i < receivedStatuses.size(); ++i) {
		QVector<Status>::iterator j = qBinaryFind(statuses.begin(), statuses.end(), receivedStatuses[i]);
		if (j == statuses.end()) {
			newStatuses.push_back(receivedStatuses[i]);
		} else {
			*j = receivedStatuses[i];
		}
	}
	for (int i = 0; i < newStatuses.size(); ++i) {
		statuses.push_back(newStatuses[i]);
	}
	qSort(statuses.begin(), statuses.end());
	makeStatusesUnique(statuses);
	return newStatuses;
}

QVector<Status> QwitTools::mergeStatuses(QVector<Status> &statuses, QVector<Status> &receivedStatuses) {
	return getInstance()->_mergeStatuses(statuses, receivedStatuses);
}

QString QwitTools::parseError(const QByteArray &data) {
	return getInstance()->_parseError(data);
}

QString QwitTools::_parseError(const QByteArray &data) {
	Status status;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "hash") {
		return "";
	}
	
	QDomNode node = root.firstChild();
	QString request = "";
	while (!node.isNull()) {
		if (node.toElement().tagName() == "request") {
			request = node.toElement().text();
		}
		node = node.nextSibling();
	}
	return request;
}


#endif