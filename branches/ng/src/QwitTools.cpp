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
QRegExp QwitTools::hashtagRegExp("(^|\\W+)#(\\w+)($|\\W+)");

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

QVector<Message> QwitTools::parseMessages(const QByteArray &data, Account *account) {
	return getInstance()->_parseMessages(data, account);
}

QVector<Message> QwitTools::parseInboxMessages(const QByteArray &data, Account *account) {
	return getInstance()->_parseInboxMessages(data, account);
}

QVector<Message> QwitTools::parseOutboxMessages(const QByteArray &data, Account *account) {
	return getInstance()->_parseOutboxMessages(data, account);
}

Message QwitTools::parseUser(const QByteArray &data, Account *account) {
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

QVector<Message> QwitTools::_parseMessages(const QByteArray &data, Account *account) {
	QVector<Message> messages;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "statuses") {
		return messages;
	}
	
	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() != "status") {
			return messages;
		}
		QDomNode node2 = node.firstChild();
		QString text = "", timeStr = "", user = "", image = "", source = "";
		uint id = 0;
		uint inReplyToMessageId = 0;
		QString inReplyToUsername = "";
		bool favorited = false;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				text = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toUInt();
			} else if (node2.toElement().tagName() == "in_reply_to_status_id") {
				inReplyToMessageId = node2.toElement().text().toUInt();
			} else if (node2.toElement().tagName() == "in_reply_to_screen_name") {
				inReplyToUsername = node2.toElement().text();
			} else if (node2.toElement().tagName() == "favorited") {
				favorited = node2.toElement().text() == "true";
			} else if (node2.toElement().tagName() == "source") {
				source = node2.toElement().text();
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
			messages.push_back(Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), favorited, account, source, inReplyToMessageId, inReplyToUsername));
		}
		node = node.nextSibling();
	}
	return messages;
}

QVector<Message> QwitTools::_parseInboxMessages(const QByteArray &data, Account *account) {
	QVector<Message> messages;
	
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
		QString text = "", timeStr = "", user = "", image = "", source = "";
		uint id = 0;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				text = node2.toElement().text();
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
			messages.push_back(Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), false, account, source, 0, "", true));
		}
		node = node.nextSibling();
	}
	return messages;
}

QVector<Message> QwitTools::_parseOutboxMessages(const QByteArray &data, Account *account) {
	QVector<Message> messages;
	
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
		QString text = "", timeStr = "", user = "", image = "", source = "";
		uint id = 0;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				text = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toUInt();
			} else if (node2.toElement().tagName() == "recipient") {
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
			messages.push_back(Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), false, account, source, 0, "", true));
		}
		node = node.nextSibling();
	}
	return messages;
}

Message QwitTools::_parseUser(const QByteArray &data, Account *account) {
	Message message;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "user") {
		return message;
	}
	
	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() == "status") {
			QDomNode node2 = node.firstChild();
			QString text = "", timeStr = "", user = "", image = "", source = "";
			uint id = 0;
			int inReplyToMessageId = 0;
			QString inReplyToUsername = "";
			bool favorited = false;
			while (!node2.isNull()) {
				if (node2.toElement().tagName() == "created_at") {
					timeStr = node2.toElement().text();
				} else if (node2.toElement().tagName() == "text") {
					text = node2.toElement().text();
				} else if (node2.toElement().tagName() == "id") {
					id = node2.toElement().text().toUInt();
				} else if (node2.toElement().tagName() == "in_reply_to_status_id") {
					inReplyToMessageId = node2.toElement().text().toUInt();
				} else if (node2.toElement().tagName() == "in_reply_to_screen_name") {
					inReplyToUsername = node2.toElement().text();
				} else if (node2.toElement().tagName() == "screen_name") {
					user = node2.toElement().text();
				} else if (node2.toElement().tagName() == "profile_image_url") {
					image = node2.toElement().text();
				} else if (node2.toElement().tagName() == "favorited") {
					favorited = node2.toElement().text() == "true";
				} else if (node2.toElement().tagName() == "source") {
					source = node2.toElement().text();
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
				message = Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), favorited, account, source, inReplyToMessageId, inReplyToUsername);
			}
		} else {
		}
		node = node.nextSibling();
	}
	return message;
}

Message QwitTools::parseMessage(const QByteArray &data, Account *account) {
	return getInstance()->_parseMessage(data, account);
}

Message QwitTools::_parseMessage(const QByteArray &data, Account *account) {
	Message message;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "status") {
		return message;
	}
	
	QDomNode node = root.firstChild();
	QString text = "", timeStr = "", user = "", image = "", source = "";
	uint id = 0;
	int inReplyToMessageId = 0;
	QString inReplyToUsername = "";
	bool favorited = false;
	while (!node.isNull()) {
		if (node.toElement().tagName() == "created_at") {
			timeStr = node.toElement().text();
		} else if (node.toElement().tagName() == "text") {
			text = node.toElement().text();
		} else if (node.toElement().tagName() == "id") {
			id = node.toElement().text().toUInt();
		} else if (node.toElement().tagName() == "in_reply_to_status_id") {
			inReplyToMessageId = node.toElement().text().toUInt();
		} else if (node.toElement().tagName() == "in_reply_to_screen_name") {
			inReplyToUsername = node.toElement().text();
		} else if (node.toElement().tagName() == "favorited") {
			favorited = node.toElement().text() == "true";
		} else if (node.toElement().tagName() == "source") {
			source = node.toElement().text();
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
		message = Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), favorited, account, source, inReplyToMessageId, inReplyToUsername);
	}
	return message;
}

Message QwitTools::parseDirectMessage(const QByteArray &data, Account *account) {
	return getInstance()->_parseDirectMessage(data, account);
}

Message QwitTools::_parseDirectMessage(const QByteArray &data, Account *account) {
	Message message;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "direct_message") {
		return message;
	}
	
	QDomNode node = root.firstChild();

	QString text = "", timeStr = "", user = "", image = "", source = "";
	uint id = 0;
	while (!node.isNull()) {
		if (node.toElement().tagName() == "created_at") {
			timeStr = node.toElement().text();
		} else if (node.toElement().tagName() == "text") {
			text = node.toElement().text();
		} else if (node.toElement().tagName() == "id") {
			id = node.toElement().text().toUInt();
		} else if (node.toElement().tagName() == "sender") {
			QDomNode node2 = node.firstChild();
			while (!node2.isNull()) {
				if (node2.toElement().tagName() == "screen_name") {
					user = node2.toElement().text();
				} else if (node2.toElement().tagName() == "profile_image_url") {
					image = node2.toElement().text();
				}
				node2 = node2.nextSibling();
			}
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
		message = Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), false, account, source, 0, "", true);
	}
	return message;
}

void QwitTools::makeMessagesUnique(QVector<Message> &v) {
	getInstance()->_makeMessagesUnique(v);
}

void QwitTools::_makeMessagesUnique(QVector<Message> &v) {
	int n = 0;
	for (int i = 0; i < v.size(); ++i) {
		if (!n || (v[i] != v[n - 1])) {
			v[n++] = v[i];
		}
	}
	v.resize(n);
}

QString QwitTools::_prepareMessage(const QString &text, Account *account) {
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
// Process hashtags
	{
		int pos = 0;
		int lastPos = 0;
		QString t = "";
		while ((pos = hashtagRegExp.indexIn(s, lastPos)) != -1) {
			QStringList list = hashtagRegExp.capturedTexts();
			QStringList::iterator it = list.begin();
			pos = hashtagRegExp.pos(2);
			QString hashtag = hashtagRegExp.cap(2);
			t += s.mid(lastPos, pos - lastPos);
			t += "<a href=\"" + account->searchBaseUrl() + hashtag + "\" style=\"font-weight:bold;text-decoration:none\">" + hashtag + "</a>";
			lastPos = pos + hashtag.length();
		}
		t += s.mid(lastPos);
		s = t;
	}
	return s;
}

QString QwitTools::prepareMessage(const QString &text, Account *account) {
	return getInstance()->_prepareMessage(text, account);
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

QVector<Message> QwitTools::_mergeMessages(QVector<Message> &messages, QVector<Message> &receivedMessages) {
	QVector<Message> newMessages;
	for (int i = 0; i < receivedMessages.size(); ++i) {
		QVector<Message>::iterator j = qBinaryFind(messages.begin(), messages.end(), receivedMessages[i]);
		if (j == messages.end()) {
			newMessages.push_back(receivedMessages[i]);
		} else {
			*j = receivedMessages[i];
		}
	}
	for (int i = 0; i < newMessages.size(); ++i) {
		messages.push_back(newMessages[i]);
	}
	qSort(messages.begin(), messages.end());
	makeMessagesUnique(messages);
	return newMessages;
}

QVector<Message> QwitTools::mergeMessages(QVector<Message> &messages, QVector<Message> &receivedMessages) {
	return getInstance()->_mergeMessages(messages, receivedMessages);
}

QString QwitTools::parseError(const QByteArray &data) {
	return getInstance()->_parseError(data);
}

QString QwitTools::_parseError(const QByteArray &data) {
	Message message;
	
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