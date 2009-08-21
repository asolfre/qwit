/*!
 *  $Id$
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
 *  Twitter class implementation
 */

#ifndef Twitter_cpp
#define Twitter_cpp

#include "QwitHeaders.h"

#include "Twitter.h"
#include "QwitTools.h"
#include "Services.h"
#include "Configuration.h"

Twitter::Twitter(Account *account) {
	qDebug() << ("Twitter::Twitter()");

	this->account = account;
	http = new QHttp(this);
	connect(http, SIGNAL(requestStarted(int)), this, SLOT(requestStarted(int)));
	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
	connect(http, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErrors(const QList<QSslError> &)));
}

void Twitter::setupProxy() {
	Configuration *config = Configuration::getInstance();
	if (config->useProxy) {
		http->setProxy(config->proxyAddress, config->proxyPort, config->proxyUsername, config->proxyPassword);
	} else {
		http->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
	}
}

void Twitter::sendMessage(const QString &message, quint64 inReplyToMessageId) {
	qDebug() << ("Twitter::sendMessage()");
	
	setupProxy();
	
	QUrl url(account->serviceApiUrl() + Services::options[account->type]["update"] + ".xml");

	QHttpRequestHeader header;
	header.setRequest("POST", url.path());
	header.setValue("Host", url.host());
	header.setContentType("application/x-www-form-urlencoded");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	QByteArray data = "status=";
	data += QUrl::toPercentEncoding(message);
	if (inReplyToMessageId) {
		data += "&in_reply_to_status_id=" + QString::number(inReplyToMessageId);
	}
	data += "&source=qwit";

	buffer.open(QIODevice::WriteOnly);

	int id = http->request(header, data, &buffer);

	sendMessageRequests[id] = tr("Sending message: %1").arg(url.host() + url.path());
}

void Twitter::receiveFriendsMessages(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receiveFriendsMessages()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["friends"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	QString path = url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&since_id=" + QString::number(lastMessageId) : "");
	int id = http->get(path, &buffer);
	receiveFriendsMessagesRequests[id] = tr("Updating friends messages: %1").arg(url.host() + path);
}

void Twitter::receiveReplies(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receiveReplies()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["replies"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&since_id=" + QString::number(lastMessageId) : ""), &buffer);
	receiveRepliesRequests[id] = tr("Updating replies: %1").arg(url.host() + url.path());
}

void Twitter::receivePublicMessages(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receivePublicMessages()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["public"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&since_id=" + QString::number(lastMessageId) : ""), &buffer);
	receivePublicMessagesRequests[id] = tr("Updating public messages: %1").arg(url.host() + url.path());
}

void Twitter::receiveSearchMessages(int count, const QString &query) {
	qDebug() << ("Twitter::receiveSearchMessages()");

	setupProxy();

	QUrl url(account->searchApiUrl() + Services::options[account->type]["search"] + ".atom");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?q=" + QUrl::toPercentEncoding(query) + "&rpp=" + QString::number(count), &buffer);
	receiveSearchMessagesRequests[id] = tr("Updating search messages: %1").arg(url.host() + url.path() + "?q=" + QUrl::toPercentEncoding(query) + "&rpp=" + QString::number(count));
}

void Twitter::receivePreviousSearchMessages(int page, int count, const QString &query) {
	qDebug() << ("Twitter::receivePreviousSearchMessages()");

	setupProxy();

	QUrl url(account->searchApiUrl() + Services::options[account->type]["search"] + ".atom");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?q=" + query + "&rpp=" + QString::number(count) + "&page=" + QString::number(page), &buffer);
	receivePreviousSearchMessagesRequests[id] = tr("Updating previous search messages: %1").arg(url.host() + url.path());
}

void Twitter::receiveLastMessage() {
	qDebug() << ("Twitter::receiveLastMessage()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["last"] + account->username + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path(), &buffer);
	receiveLastMessageRequests[id] = tr("Updating last message: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousFriendsMessages(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receiveFriendsMessages()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["friends"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&max_id=" + QString::number(lastMessageId) : ""), &buffer);
	receivePreviousFriendsMessagesRequests[id] = tr("Updating friends messages: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousReplies(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receiveReplies()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["replies"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&max_id=" + QString::number(lastMessageId) : ""), &buffer);
	receivePreviousRepliesRequests[id] = tr("Updating replies: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousPublicMessages(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receivePreviousPublicMessages()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["public"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&max_id=" + QString::number(lastMessageId) : ""), &buffer);
	receivePreviousPublicMessagesRequests[id] = tr("Updating public messages: %1").arg(url.host() + url.path());
}

void Twitter::receiveFavorites() {
	qDebug() << ("Twitter::receiveFavorites()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["favorites"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path(), &buffer);
	receiveFavoritesRequests[id] = tr("Updating favorites: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousFavorites(int page) {
	qDebug() << ("Twitter::receivePreviousFavorites()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["favorites"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?page=" + QString::number(page), &buffer);
	receivePreviousFavoritesRequests[id] = tr("Updating favorites: %1").arg(url.host() + url.path());
}

void Twitter::receiveInboxMessages(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receiveInboxMessages()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["inbox"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&since_id=" + QString::number(lastMessageId) : ""), &buffer);
	receiveInboxMessagesRequests[id] = tr("Updating inbox messages: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousInboxMessages(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receivePreviousInboxMessages()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["inbox"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&max_id=" + QString::number(lastMessageId) : ""), &buffer);
	receivePreviousInboxMessagesRequests[id] = tr("Updating inbox messages: %1").arg(url.host() + url.path());
}

void Twitter::receiveOutboxMessages(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receiveOutboxMessages()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["outbox"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&since_id=" + QString::number(lastMessageId) : ""), &buffer);
	receiveOutboxMessagesRequests[id] = tr("Updating outbox messages: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousOutboxMessages(quint64 lastMessageId, int count) {
	qDebug() << ("Twitter::receivePreviousOutboxMessages()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["outbox"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastMessageId ? "&max_id=" + QString::number(lastMessageId) : ""), &buffer);
	receivePreviousOutboxMessagesRequests[id] = tr("Updating outbox messages: %1").arg(url.host() + url.path());
}

void Twitter::sendDirectMessage(const QString &username, const QString &message) {
	qDebug() << ("Twitter::sendDirectMessage()");
	
	setupProxy();
	
	QUrl url(account->serviceApiUrl() + Services::options[account->type]["send"] + ".xml");

	QHttpRequestHeader header;
	header.setRequest("POST", url.path());
	header.setValue("Host", url.host());
	header.setContentType("application/x-www-form-urlencoded");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	QByteArray data = "user=" + QUrl::toPercentEncoding(username) + "&text=" + QUrl::toPercentEncoding(message);

	buffer.open(QIODevice::WriteOnly);

	int id = http->request(header, data, &buffer);

	sendDirectMessageRequests[id] = tr("Sending direct message: %1").arg(url.host() + url.path());
}

void Twitter::favorMessage(quint64 messageId) {
	qDebug() << ("Twitter::favorMessage()");
	
	setupProxy();
	
	QUrl url(account->serviceApiUrl() + Services::options[account->type]["favor"] + QString::number(messageId) + ".xml");

	QHttpRequestHeader header;
	header.setRequest("POST", url.path());
	header.setValue("Host", url.host());
	header.setContentType("application/x-www-form-urlencoded");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	QByteArray data;

	buffer.open(QIODevice::WriteOnly);

	int id = http->request(header, data, &buffer);

	favorMessageRequests[id] = tr("Favoring message: %1").arg(url.host() + url.path());
}

void Twitter::unfavorMessage(quint64 messageId) {
	qDebug() << ("Twitter::unfavorMessage()");
	
	setupProxy();
	
	QUrl url(account->serviceApiUrl() + Services::options[account->type]["unfavor"] + QString::number(messageId) + ".xml");

	QHttpRequestHeader header;
	header.setRequest("POST", url.path());
	header.setValue("Host", url.host());
	header.setContentType("application/x-www-form-urlencoded");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	QByteArray data;

	buffer.open(QIODevice::WriteOnly);

	int id = http->request(header, data, &buffer);

	unfavorMessageRequests[id] = tr("Unfavoring message: %1").arg(url.host() + url.path());
}

void Twitter::destroyMessage(quint64 messageId) {
	qDebug() << ("Twitter::destroyMessage()");
	
	setupProxy();
	
	QUrl url(account->serviceApiUrl() + Services::options[account->type]["destroy"] + QString::number(messageId) + ".xml");

	QHttpRequestHeader header;
	header.setRequest("POST", url.path());
	header.setValue("Host", url.host());
	header.setContentType("application/x-www-form-urlencoded");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	QByteArray data;

	buffer.open(QIODevice::WriteOnly);

	int id = http->request(header, data, &buffer);

	destroyMessageRequests[id] = tr("Destroying message: %1").arg(url.host() + url.path());
}

void Twitter::destroyDirectMessage(quint64 messageId) {
	qDebug() << ("Twitter::destroyDirectMessage()");
	
	setupProxy();
	
	QUrl url(account->serviceApiUrl() + Services::options[account->type]["destroydirectmessage"] + QString::number(messageId) + ".xml");

	QHttpRequestHeader header;
	header.setRequest("POST", url.path());
	header.setValue("Host", url.host());
	header.setContentType("application/x-www-form-urlencoded");

	if(url.toString().indexOf("https") == 0) {
		http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
	} else {
		http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
	}

	http->setUser(account->username, account->password);

	QByteArray data;

	buffer.open(QIODevice::WriteOnly);

	int id = http->request(header, data, &buffer);

	destroyDirectMessageRequests[id] = tr("Destroying direct message: %1").arg(url.host() + url.path());
}

void Twitter::abort() {
	qDebug() << ("Twitter::abort()");
}

void Twitter::requestStarted(int id) {
	qDebug() << ("Twitter::requestStarted() " + QString::number(id));
	
	if (receiveFriendsMessagesRequests.find(id) != receiveFriendsMessagesRequests.end()) {
		qDebug() << ("Request started: " + receiveFriendsMessagesRequests[id]);
	} else if (receiveRepliesRequests.find(id) != receiveRepliesRequests.end()) {
		qDebug() << ("Request started: " + receiveRepliesRequests[id]);
	} else if (receivePublicMessagesRequests.find(id) != receivePublicMessagesRequests.end()) {
		qDebug() << ("Request started: " + receivePublicMessagesRequests[id]);
	} else if (receiveLastMessageRequests.find(id) != receiveLastMessageRequests.end()) {
		qDebug() << ("Request started: " + receiveLastMessageRequests[id]);
	} else if (sendMessageRequests.find(id) != sendMessageRequests.end()) {
		qDebug() << ("Request started: " + sendMessageRequests[id]);
	} else if (receiveFavoritesRequests.find(id) != receiveFavoritesRequests.end()) {
		qDebug() << ("Request started: " + receiveFavoritesRequests[id]);
	} else if (receivePreviousFriendsMessagesRequests.find(id) != receivePreviousFriendsMessagesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousFriendsMessagesRequests[id]);
	} else if (receivePreviousRepliesRequests.find(id) != receivePreviousRepliesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousRepliesRequests[id]);
	} else if (receivePreviousPublicMessagesRequests.find(id) != receivePreviousPublicMessagesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousPublicMessagesRequests[id]);
	} else if (receivePreviousFavoritesRequests.find(id) != receivePreviousFavoritesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousFavoritesRequests[id]);
	} else if (receiveInboxMessagesRequests.find(id) != receiveInboxMessagesRequests.end()) {
		qDebug() << ("Request started: " + receiveInboxMessagesRequests[id]);
	} else if (receivePreviousInboxMessagesRequests.find(id) != receivePreviousInboxMessagesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousInboxMessagesRequests[id]);
	} else if (receiveOutboxMessagesRequests.find(id) != receiveOutboxMessagesRequests.end()) {
		qDebug() << ("Request started: " + receiveOutboxMessagesRequests[id]);
	} else if (receivePreviousOutboxMessagesRequests.find(id) != receivePreviousOutboxMessagesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousOutboxMessagesRequests[id]);
	} else if (sendDirectMessageRequests.find(id) != sendDirectMessageRequests.end()) {
		qDebug() << ("Request started: " + sendDirectMessageRequests[id]);
	} else if (favorMessageRequests.find(id) != favorMessageRequests.end()) {
		qDebug() << ("Request started: " + favorMessageRequests[id]);
	} else if (unfavorMessageRequests.find(id) != unfavorMessageRequests.end()) {
		qDebug() << ("Request started: " + unfavorMessageRequests[id]);
	} else if (destroyMessageRequests.find(id) != destroyMessageRequests.end()) {
		qDebug() << ("Request started: " + destroyMessageRequests[id]);
	} else if (destroyDirectMessageRequests.find(id) != destroyDirectMessageRequests.end()) {
		qDebug() << ("Request started: " + destroyDirectMessageRequests[id]);
	} else if (receiveSearchMessagesRequests.find(id) != receiveSearchMessagesRequests.end()) {
		qDebug() << ("Request started: " + receiveSearchMessagesRequests[id]);
	} else if (receivePreviousSearchMessagesRequests.find(id) != receivePreviousSearchMessagesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousSearchMessagesRequests[id]);
	}
}

void Twitter::requestFinished(int id, bool error) {
	if (!error && http->lastResponse().isValid() && (http->lastResponse().statusCode() == 200)) {
		qDebug() << ("Twitter::requestFinished() " + QString::number(id));
		buffer.close();
		QHttpResponseHeader response = http->lastResponse();
		QString remainingRequests = response.value("X-RateLimit-Remaining");
		if (receiveFriendsMessagesRequests.find(id) != receiveFriendsMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receiveFriendsMessagesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit friendsMessagesReceived(buffer.data());
			receiveFriendsMessagesRequests.remove(id);
		} else if (receiveRepliesRequests.find(id) != receiveRepliesRequests.end()) {
			qDebug() << ("Request finished: " + receiveRepliesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit repliesReceived(buffer.data());
			receiveRepliesRequests.remove(id);
		} else if (receivePublicMessagesRequests.find(id) != receivePublicMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receivePublicMessagesRequests[id]);
			emit publicMessagesReceived(buffer.data());
			receivePublicMessagesRequests.remove(id);
		} else if (receiveLastMessageRequests.find(id) != receiveLastMessageRequests.end()) {
			qDebug() << ("Request finished: " + receiveLastMessageRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit lastMessageReceived(buffer.data());
			receiveLastMessageRequests.remove(id);
		} else if (sendMessageRequests.find(id) != sendMessageRequests.end()) {
			qDebug() << ("Request finished: " + sendMessageRequests[id]);
			emit messageSent(buffer.data());
			sendMessageRequests.remove(id);
		} else if (receiveFavoritesRequests.find(id) != receiveFavoritesRequests.end()) {
			qDebug() << ("Request finished: " + receiveFavoritesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit favoritesReceived(buffer.data());
			receiveFavoritesRequests.remove(id);
		} else if (receivePreviousFriendsMessagesRequests.find(id) != receivePreviousFriendsMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousFriendsMessagesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousFriendsMessagesReceived(buffer.data());
			receivePreviousFriendsMessagesRequests.remove(id);
		} else if (receivePreviousRepliesRequests.find(id) != receivePreviousRepliesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousRepliesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousRepliesReceived(buffer.data());
			receivePreviousRepliesRequests.remove(id);
		} else if (receivePreviousPublicMessagesRequests.find(id) != receivePreviousPublicMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousPublicMessagesRequests[id]);
			buffer.close();
			emit previousPublicMessagesReceived(buffer.data());
			receivePreviousPublicMessagesRequests.remove(id);
		} else if (receivePreviousFavoritesRequests.find(id) != receivePreviousFavoritesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousFavoritesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousFavoritesReceived(buffer.data());
			receivePreviousFavoritesRequests.remove(id);
		} else if (receiveInboxMessagesRequests.find(id) != receiveInboxMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receiveInboxMessagesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit inboxMessagesReceived(buffer.data());
			receiveInboxMessagesRequests.remove(id);
		} else if (receivePreviousInboxMessagesRequests.find(id) != receivePreviousInboxMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousInboxMessagesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousInboxMessagesReceived(buffer.data());
			receivePreviousInboxMessagesRequests.remove(id);
		} else if (receiveOutboxMessagesRequests.find(id) != receiveOutboxMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receiveOutboxMessagesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit outboxMessagesReceived(buffer.data());
			receiveOutboxMessagesRequests.remove(id);
		} else if (receivePreviousOutboxMessagesRequests.find(id) != receivePreviousOutboxMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousOutboxMessagesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousOutboxMessagesReceived(buffer.data());
			receivePreviousOutboxMessagesRequests.remove(id);
		} else if (sendDirectMessageRequests.find(id) != sendDirectMessageRequests.end()) {
			qDebug() << ("Request finished: " + sendDirectMessageRequests[id]);
			emit directMessageSent(buffer.data());
			sendDirectMessageRequests.remove(id);
		} else if (favorMessageRequests.find(id) != favorMessageRequests.end()) {
			qDebug() << ("Request finished: " + favorMessageRequests[id]);
			emit messageFavored(buffer.data());
			favorMessageRequests.remove(id);
		} else if (unfavorMessageRequests.find(id) != unfavorMessageRequests.end()) {
			qDebug() << ("Request finished: " + unfavorMessageRequests[id]);
			buffer.close();
			emit messageUnfavored(buffer.data());
			unfavorMessageRequests.remove(id);
		} else if (destroyMessageRequests.find(id) != destroyMessageRequests.end()) {
			qDebug() << ("Request finished: " + destroyMessageRequests[id]);
			emit messageDestroyed(buffer.data());
			destroyMessageRequests.remove(id);
		} else if (destroyDirectMessageRequests.find(id) != destroyDirectMessageRequests.end()) {
			qDebug() << ("Request finished: " + destroyDirectMessageRequests[id]);
			emit directMessageDestroyed(buffer.data());
			destroyDirectMessageRequests.remove(id);
		} else if (receiveSearchMessagesRequests.find(id) != receiveSearchMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receiveSearchMessagesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit searchMessagesReceived(buffer.data());
			receiveSearchMessagesRequests.remove(id);
		} else if (receivePreviousSearchMessagesRequests.find(id) != receivePreviousSearchMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousSearchMessagesRequests[id]);
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousSearchMessagesReceived(buffer.data());
			receivePreviousSearchMessagesRequests.remove(id);
		}
	} else {
		if (sendMessageRequests.find(id) != sendMessageRequests.end()) {
			qDebug() << ("Request failed (message not sent): " + sendMessageRequests[id]);
			emit messageNotSent();
			sendMessageRequests.remove(id);
		}
		if (http->lastResponse().isValid()) {
			qDebug() << ("Twitter::requestFinished() " + QString::number(id) + " error " + QString::number(http->lastResponse().statusCode()) + " " + http->lastResponse().reasonPhrase());
		} else if (error) {
			qDebug() << ("Twitter::requestFinished() " + QString::number(id) + " error " + QString::number(http->error()) + " " + http->errorString());
		} else {
			qDebug() << ("Twitter::requestFinished() " + QString::number(id));
		}
	}
}

void Twitter::sslErrors(const QList<QSslError> &errors) {
	qDebug() << "Twitter::sslErrors()" << endl;
	for (QList<QSslError>::const_iterator it = errors.constBegin(); it != errors.constEnd(); ++it) {
		qDebug() << "SSL error " << *it << endl;
	}
}

#endif
