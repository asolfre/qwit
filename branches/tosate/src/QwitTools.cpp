/*!
 *  $Id: QwitTools.cpp 196 2009-07-19 14:09:42Z artem.iglikov $
 *
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

QMap<QString, int> QwitTools::monthes;
QRegExp QwitTools::urlRegExp("((ht|f)tp(s?)\\:\\/\\/|~/|/)?(\\w+:\\w+@)?((([-\\w]+\\.)+(com|org|net|gov|mil|biz|edu|info|mobi|name|aero|jobs|museum|travel|[a-z]{2}))|(([\\d]{1,3}\\.){3}[\\d]{1,3}))(:[\\d]{1,5})?(((/([-\\w~!$+|.=]|%[a-f\\d]{2})+)+|/)+|\\?|#)?((\\?([-\\w]|%[a-f\\d{2}])+=([-\\w~!$+|*:=]|%[a-f\\d]{2})*)(&([-\\w~!$+|.,*:]|%[a-f\\d{2}])+=([-\\w~!$+|.,*:=]|%[a-f\\d]{2})*)*)*(#([-\\w~!$+|.,*:=]|%[a-f\\d]{2})*)?");
QRegExp QwitTools::ipAddressRegExp("([\\d]{1,3}\\.){3}[\\d]{1,3}");
QRegExp QwitTools::usernameRegExp("(^|\\W+)@(\\w+)($|\\W+)");
QRegExp QwitTools::hashtagRegExp("(^|\\W+)#([-._\\w]+)($|\\W+)");

QDateTime QwitTools::dateFromString(QString date) {
	if (!monthes.size()) {
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
	char s[10];
	int year, day, hours, minutes, seconds;
	sscanf(qPrintable(date), "%*s %s %d %d:%d:%d %*s %d", s, &day, &hours, &minutes, &seconds, &year);
	int month = monthes[s];
	return QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds));
}

QDateTime QwitTools::dateFromAtomString(QString date) {
	int year, month, day, hours, minutes, seconds;
	sscanf(qPrintable(date), "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hours, &minutes, &seconds);
	return QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds));
}

QIcon QwitTools::getToolButtonIcon(const QString &iconFileName, bool active) {
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

QString QwitTools::formatDateTime(const QDateTime &time) {
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

QVector<Message> QwitTools::parseMessages(const QByteArray &data, Account *account) {
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
		quint64 id = 0;
		quint64 inReplyToMessageId = 0;
		QString inReplyToUsername = "";
                bool following = false;
		bool favorited = false;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				text = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toULongLong();
			} else if (node2.toElement().tagName() == "in_reply_to_status_id") {
				inReplyToMessageId = node2.toElement().text().toULongLong();
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
                                        } else if (node3.toElement().tagName() == "following") {
                                                following = node3.toElement().text() == "true";
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
			UserpicsDownloader::getInstance()->setUserImageFileName(account->serviceBaseUrl(), user, imageFileName);
			messages.push_back(Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), favorited, account, source, inReplyToMessageId, inReplyToUsername, following, false));
		}
		node = node.nextSibling();
	}
	return messages;
}

QVector<Message> QwitTools::parseSearchMessages(const QByteArray &data, Account *account) {
	QVector<Message> messages;

	QDomDocument document;
	document.setContent(data);
	QDomElement root = document.documentElement();

	if (root.tagName() != "feed") {
		return messages;
	}

	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() == "entry") {
			QDomNode node2 = node.firstChild();
			QString text = "", timeStr = "", user = "", image = "", source = "";
			quint64 id = 0;
			quint64 inReplyToMessageId = 0;
			QString inReplyToUsername = "";
			bool following = false;
			bool favorited = false;
			while (!node2.isNull()) {
				if (node2.toElement().tagName() == "published") {
					timeStr = node2.toElement().text();
				} else if (node2.toElement().tagName() == "title") {
					text = node2.toElement().text();
				} else if (node2.toElement().tagName() == "id") {
					QString s = node2.toElement().text();
					s = s.mid(s.lastIndexOf(':') + 1);
					id = s.toULongLong();
				} else if (node2.toElement().tagName() == "twitter:source") {
					source = node2.toElement().text();
				} else if (node2.toElement().tagName() == "author") {
					QDomNode node3 = node2.firstChild();
					while (!node3.isNull()) {
						if (node3.toElement().tagName() == "name") {
							user = node3.toElement().text();
							user = user.mid(0, user.indexOf(' '));
						}
						node3 = node3.nextSibling();
					}
				} else if (node2.toElement().tagName() == "link") {
					if (node2.toElement().attribute("rel") == "image") {
						image = node2.toElement().attribute("href");
					}
				}
				node2 = node2.nextSibling();
			}
			if (id) {
				QDateTime time = QwitTools::dateFromAtomString(timeStr);
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
				messages.push_back(Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), favorited, account, source, inReplyToMessageId, inReplyToUsername, following, false));
			}
		}
		node = node.nextSibling();
	}
	return messages;
}

QVector<Message> QwitTools::parseInboxMessages(const QByteArray &data, Account *account) {
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
                bool following = false;
                quint64 id = 0;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				text = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toULongLong();
			} else if (node2.toElement().tagName() == "sender") {
				QDomNode node3 = node2.firstChild();
				while (!node3.isNull()) {
					if (node3.toElement().tagName() == "screen_name") {
						user = node3.toElement().text();
					} else if (node3.toElement().tagName() == "profile_image_url") {
						image = node3.toElement().text();
                                        } else if (node3.toElement().tagName() == "following") {
                                                following = node3.toElement().text() == "true";
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
                        messages.push_back(Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), false, account, source, 0, "", following, true));
		}
		node = node.nextSibling();
	}
	return messages;
}

QVector<Message> QwitTools::parseOutboxMessages(const QByteArray &data, Account *account) {
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
                bool following = false;
                quint64 id = 0;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				text = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toULongLong();
			} else if (node2.toElement().tagName() == "recipient") {
				QDomNode node3 = node2.firstChild();
				while (!node3.isNull()) {
					if (node3.toElement().tagName() == "screen_name") {
						user = node3.toElement().text();
					} else if (node3.toElement().tagName() == "profile_image_url") {
						image = node3.toElement().text();
                                        } else if (node3.toElement().tagName() == "following") {
                                                following = node3.toElement().text() == "true";
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
                        messages.push_back(Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), false, account, source, 0, "", following, true));
		}
		node = node.nextSibling();
	}
	return messages;
}

User QwitTools::parseUser(const QByteArray &data, Account *account) {
	User user;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "user")
		return user;
	
	QDomNode node = root.firstChild();
	user = parseUser(node, account);

	return user;
}

Message QwitTools::parseMessage(const QByteArray &data, Account *account) {
	Message message;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "status") {
		return message;
	}
	
	QDomNode node = root.firstChild();
	QString text = "", timeStr = "", user = "", image = "", source = "";
	quint64 id = 0;
	int inReplyToMessageId = 0;
	QString inReplyToUsername = "";
	bool favorited = false;
        bool following = false;
        while (!node.isNull()) {
		if (node.toElement().tagName() == "created_at") {
			timeStr = node.toElement().text();
		} else if (node.toElement().tagName() == "text") {
			text = node.toElement().text();
		} else if (node.toElement().tagName() == "id") {
			id = node.toElement().text().toULongLong();
		} else if (node.toElement().tagName() == "in_reply_to_status_id") {
			inReplyToMessageId = node.toElement().text().toULongLong();
		} else if (node.toElement().tagName() == "in_reply_to_screen_name") {
			inReplyToUsername = node.toElement().text();
		} else if (node.toElement().tagName() == "favorited") {
			favorited = node.toElement().text() == "true";
		} else if (node.toElement().tagName() == "source") {
			source = node.toElement().text();
                } else if (node.toElement().tagName() == "user") {
                        QDomNode node2 = node.firstChild();
                        while (!node2.isNull()) {
                                if (node2.toElement().tagName() == "following") {
                                        following = node2.toElement().text() == "true";
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
//		UserpicsDownloader::getInstance()->download(image, imageFileName);
                message = Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), favorited, account, source, inReplyToMessageId, inReplyToUsername, following, false);
	}
	return message;
}

Message QwitTools::parseDirectMessage(const QByteArray &data, Account *account) {
	Message message;
	
	QDomDocument document;
	document.setContent(data);

	QDomElement root = document.documentElement();
	
	if (root.tagName() != "direct_message") {
		return message;
	}
	
	QDomNode node = root.firstChild();

	QString text = "", timeStr = "", user = "", image = "", source = "";
	quint64 id = 0;
        bool following = false;
        while (!node.isNull()) {
		if (node.toElement().tagName() == "created_at") {
			timeStr = node.toElement().text();
		} else if (node.toElement().tagName() == "text") {
			text = node.toElement().text();
		} else if (node.toElement().tagName() == "id") {
			id = node.toElement().text().toULongLong();
		} else if (node.toElement().tagName() == "sender") {
			QDomNode node2 = node.firstChild();
			while (!node2.isNull()) {
				if (node2.toElement().tagName() == "screen_name") {
					user = node2.toElement().text();
				} else if (node2.toElement().tagName() == "profile_image_url") {
					image = node2.toElement().text();
                                } else if (node2.toElement().tagName() == "following") {
                                        following = node2.toElement().text() == "true";
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
                message = Message(id, text.simplified(), user, imageFileName, time.toLocalTime(), false, account, source, 0, "", following, true);
	}
	return message;
}

void QwitTools::makeMessagesUnique(QVector<Message> &v) {
	int n = 0;
	for (int i = 0; i < v.size(); ++i) {
		if (!n || (v[i] != v[n - 1])) {
			v[n++] = v[i];
		}
	}
	v.resize(n);
}

QString QwitTools::prepareMessage(const QString &text, Account *account) {
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
			while (url.at(url.length() - 1).isPunct() && (url[url.length() - 1] != '/')) {
				url = url.mid(0, url.length() - 1);
			}
			if ((pos && ((s.at(pos - 1) == '@') || s.at(pos - 1).isLetterOrNumber() || ((pos + url.length() < s.length()) && (s.at(pos + url.length()).isLetterOrNumber())))) || ipAddressRegExp.exactMatch(url)) {
				t += s.mid(lastPos, pos - lastPos);
				t += url;
				lastPos = pos + url.length();
			} else {
				t += s.mid(lastPos, pos - lastPos);
				QString href = url;
				if (list[1] == "") {
					href = "http://" + href;
				}
				t += "<a href=\"" + href + "\" style=\"font-weight:bold;text-decoration:none\">" + url + "</a>";
				lastPos = pos + url.length();
			}
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
			bool isHashtag = false;
			for (int i = 0; i < hashtag.length(); ++i) {
				if (!hashtag[i].isDigit()) {
					isHashtag = true;
				}
			}
			t += s.mid(lastPos, pos - lastPos);
			if (isHashtag) {
				t += "<a href=\"" + account->searchBaseUrl() + hashtag + "\" style=\"font-weight:bold;text-decoration:none\">" + hashtag + "</a>";
			} else {
				t += hashtag;
			}
			lastPos = pos + hashtag.length();
		}
		t += s.mid(lastPos);
		s = t;
	}
	return s;
}

void handleMessage(QtMsgType type, const char *msg) {
	switch (type) {
		case QtDebugMsg:
//			cout << qPrintable(QDateTime::currentDateTime().toString()) << " Debug: " << msg << endl;
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

QVector<Message> QwitTools::mergeMessages(QVector<Message> &messages, QVector<Message> &receivedMessages) {
	QVector<Message> newMessages;
	QStringList usernames;
	if (!receivedMessages.size()) {
		return newMessages;
	}
	bool addingOldMessages = (messages.size() && (messages[0].id > receivedMessages[0].id));

	for (int i = 0; i < receivedMessages.size(); ++i) {
		usernames << receivedMessages[i].username;
		QVector<Message>::iterator j = qBinaryFind(messages.begin(), messages.end(), receivedMessages[i]);
		if (j == messages.end()) {
			newMessages.push_back(receivedMessages[i]);
		} else {
			*j = receivedMessages[i];
		}
	}
	receivedMessages[0].account->addUsernamesToCache(usernames);
	for (int i = 0; i < newMessages.size(); ++i) {
		messages.push_back(newMessages[i]);
	}
	qSort(messages.begin(), messages.end());
	makeMessagesUnique(messages);
	if (addingOldMessages) newMessages.clear();
	return newMessages;
}

QString QwitTools::parseError(const QByteArray &data) {
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

bool QwitTools::isUrl(const QString &s) {
	return urlRegExp.exactMatch(s);
}

bool QwitTools::isMention(const Message &message) {
	QRegExp re("(^|\\W+)@" + message.account->username + "($|\\W+)");
	return (re.indexIn(message.text) != -1);
}

QVector<User> QwitTools::parseUsers(const QByteArray &data, Account *account) {
    QVector<User> users;

    QDomDocument document;
    document.setContent(data);

    QDomElement root = document.documentElement();

    if(root.tagName() != "users")
	return users;

    QDomNode node = root.firstChild();
    while(!node.isNull()) {
	if(node.toElement().tagName() != "user") {
	    node = node.nextSibling();
	    continue;
	}

	User user = parseUser(node, account);
	if(user.id)
	    users.push_back(user);

	node = node.nextSibling();
    }

    return users;
}

QVector<List> QwitTools::parseLists(const QByteArray &data, Account *account) {
    QVector<List> lists;

    QDomDocument document;
    document.setContent(data);

    QDomElement root = document.documentElement();

    if(root.tagName() != "lists_list")
	return lists;

    QDomNode node = root.firstChild();

    if(node.toElement().tagName() != "lists")
	return lists;

    node = node.firstChild();
    while(!node.isNull()) {
	if(node.toElement().tagName() != "list") {
	    node = node.nextSibling();
	    continue;
	}

	List list = parseList(node, account);
	if(list.id)
	    lists.push_back(list);
	node = node.nextSibling();
    }

    return lists;
}

List QwitTools::parseList(QDomNode &node, Account *account) {
    List list;
    if(node.toElement().tagName() != "list")
	return list;

    quint64 id = 0;
    QString name = "", full_name = "", slug = "", description = "", uri = "", mode = "";
    User user;
    int subscriber_count = 0, member_count = 0;
    QDomNode node2 = node.firstChild();
    while(!node2.isNull()) {
	if(node2.toElement().tagName() == "id") {
	    id = node2.toElement().text().toULongLong();
	}
	else if(node2.toElement().tagName() == "name") {
	    name = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "full_name") {
	    full_name = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "slug") {
	    slug = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "description") {
	    description = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "subscriber_count") {
	    subscriber_count = node2.toElement().text().toInt();
	}
	else if(node2.toElement().tagName() == "member_count") {
	    member_count = node2.toElement().text().toInt();
	}
	else if(node2.toElement().tagName() == "uri") {
	    uri = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "mode") {
	    mode = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "user") {
	    user = parseUser(node2, account);
	}
	node2 = node2.nextSibling();
    }

    if(id) {
	list = List(id, name, full_name, slug, description, uri, mode, subscriber_count, member_count, user.id, user.name, user.screen_name, account);
    }
    return list;
}

User QwitTools::parseUser(QDomNode &node, Account *account) {
    User user;
    if(node.toElement().tagName() != "user")
	return user;

    quint64 id = 0, statusId= 0, statusIn_reply_to_status_id = 0, statusIn_reply_to_user_id = 0;
    QString name = "", screen_name = "", location = "", description = "", profile_image_url = "", url = "", profile_background_color = "", profile_text_color = "", profile_link_color = "", profile_sidebar_fill_color = "", profile_sidebar_border_color = "", created_at = "", time_zone = "", profile_background_image_url = "", profile_background_tile = "", statusCreated_at = "", statusText = "", statusSource = "", statusIn_reply_to_screen_name = "";
    bool isProtected = false, notifications = false, geo_enabled = false, verified = false, following = false, statusTruncated = false, statusFavorited = false;
    int followers_count = 0, friends_count = 0, favourites_count = 0, utc_offset = 0, statuses_count = 0;
    QDomNode node2 = node.firstChild();
    while(!node2.isNull()) {
	if(node2.toElement().tagName() == "id") {
	    id = node2.toElement().text().toULongLong();
	}
	else if(node2.toElement().tagName() == "name") {
	    name = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "screen_name") {
	    screen_name = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "location") {
	    location = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "description") {
	    description = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "profile_image_url") {
	    profile_image_url = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "url") {
	    url = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "protected") {
	    isProtected = node2.toElement().text() == "true";
	}
	else if(node2.toElement().tagName() == "followers_count") {
	    followers_count = node2.toElement().text().toInt();
	}
	else if(node2.toElement().tagName() == "profile_background_color") {
	    profile_background_color = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "profile_text_color") {
	    profile_text_color = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "profile_link_color") {
	    profile_link_color = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "profile_sidebar_fill_color") {
	    profile_sidebar_fill_color = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "profile_sidebar_border_color") {
	    profile_sidebar_border_color = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "friends_count") {
	    friends_count = node2.toElement().text().toInt();
	}
	else if(node2.toElement().tagName() == "created_at") {
	    created_at = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "favourites_count") {
	    favourites_count = node2.toElement().text().toInt();
	}
	else if(node2.toElement().tagName() == "utc_offset") {
	    utc_offset = node2.toElement().text().toInt();
	}
	else if(node2.toElement().tagName() == "time_zone") {
	    time_zone = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "profile_background_image_url") {
	    profile_background_image_url = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "profile_background_tile") {
	    profile_background_tile = node2.toElement().text();
	}
	else if(node2.toElement().tagName() == "statuses_count") {
	    statuses_count = node2.toElement().text().toInt();
	}
	else if(node2.toElement().tagName() == "notifications") {
	    notifications = node2.toElement().text() == "true";
	}
	else if(node2.toElement().tagName() == "following") {
	    following = node2.toElement().text() == "true";
	}
	else if(node2.toElement().tagName() == "verified") {
	    verified = node2.toElement().text() == "true";
	}
	else if(node2.toElement().tagName() == "status") {
	    QDomNode node3 = node2.firstChild();
	    while(!node3.isNull()) {
		if(node3.toElement().tagName() == "id") {
		    statusId = node3.toElement().text().toULongLong();
		}
		else if(node3.toElement().tagName() == "created_at") {
		    statusCreated_at = node3.toElement().text();
		}
		else if(node3.toElement().tagName() == "text") {
		    statusText = node3.toElement().text();
		}
		else if(node3.toElement().tagName() == "source") {
		    statusSource = node3.toElement().text();
		}
		else if(node3.toElement().tagName() == "truncated") {
		    statusTruncated = node3.toElement().text() == "true";
		}
		else if(node3.toElement().tagName() == "in_reply_to_status_id") {
		    statusIn_reply_to_status_id = node3.toElement().text().toULongLong();
		}
		else if(node3.toElement().tagName() == "in_reply_to_user_id") {
		    statusIn_reply_to_user_id = node3.toElement().text().toULongLong();
		}
		else if(node3.toElement().tagName() == "favorited") {
		    statusFavorited = node3.toElement().text() == "true";
		}
		else if(node3.toElement().tagName() == "in_reply_to_screen_name") {
		    statusIn_reply_to_screen_name = node3.toElement().text();
		}
		node3 = node3.nextSibling();
	    }
	}
	node2 = node2.nextSibling();
    }

    if(id) {
	user = User(id, name, screen_name, location, description, profile_image_url, url, isProtected, followers_count, profile_background_color, profile_text_color, profile_link_color, profile_sidebar_fill_color, profile_sidebar_border_color, friends_count, created_at, favourites_count, utc_offset, time_zone, profile_background_image_url, profile_background_tile, statuses_count, notifications, geo_enabled, verified, following, account);

	if(statusId) {
	    Status *status = new Status(statusId, statusCreated_at, statusText, statusSource, statusTruncated, statusIn_reply_to_status_id, statusIn_reply_to_user_id, statusFavorited, statusIn_reply_to_screen_name);
	    user.setStatus(status);
	}
    }
    return user;
}

QVector<User> QwitTools::parseListMembers(const QByteArray &data, Account *account) {
    QVector<User> listMembers;

    QDomDocument document;
    document.setContent(data);

    QDomElement root = document.documentElement();

    if(root.tagName() != "users_list")
	return listMembers;

    QDomNode node = root.firstChild();

    if(node.toElement().tagName() != "users")
	return listMembers;

    node = node.firstChild();
    while(!node.isNull()) {
	if(node.toElement().tagName() != "user") {
	    node = node.nextSibling();
	    continue;
	}

	User user = parseUser(node, account);
	if(user.id)
	    listMembers.push_back(user);
	node = node.nextSibling();
    }

    return listMembers;
}
#endif
