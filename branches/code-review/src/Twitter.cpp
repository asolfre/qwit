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
	http = new QHttp();
	connect(http, SIGNAL(requestStarted(int)), this, SLOT(requestStarted(int)));
	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
}

void Twitter::setupProxy() {
	Configuration *config = Configuration::getInstance();
	if (config->useProxy) {
		http->setProxy(config->proxyAddress, config->proxyPort, config->proxyUsername, config->proxyPassword);
	} else {
		http->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
	}
}

void Twitter::sendStatus(const QString &status, uint inReplyToStatusId) {
	qDebug() << ("Twitter::sendStatus()");
	
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
	data += QUrl::toPercentEncoding(status);
	if (inReplyToStatusId) {
		data += "&in_reply_to_status_id=" + QString::number(inReplyToStatusId);
	}
	data += "&source=qwit";

	buffer.open(QIODevice::WriteOnly);

	int id = http->request(header, data, &buffer);

	sendStatusRequests[id] = tr("Sending status: %1").arg(url.host() + url.path());
}

void Twitter::receiveFriendsStatuses(uint lastStatusId, int count) {
	qDebug() << ("Twitter::receiveFriendsStatuses()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["friends"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&since_id=" + QString::number(lastStatusId) : ""), &buffer);
	receiveFriendsStatusesRequests[id] = tr("Updating friends statuses: %1").arg(url.host() + url.path());
}

void Twitter::receiveReplies(uint lastStatusId, int count) {
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

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&since_id=" + QString::number(lastStatusId) : ""), &buffer);
	receiveRepliesRequests[id] = tr("Updating replies: %1").arg(url.host() + url.path());
}

void Twitter::receivePublicStatuses(uint lastStatusId, int count) {
	qDebug() << ("Twitter::receivePublicStatuses()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["public"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&since_id=" + QString::number(lastStatusId) : ""), &buffer);
	receivePublicStatusesRequests[id] = tr("Updating public statuses: %1").arg(url.host() + url.path());
}

void Twitter::receiveLastStatus() {
	qDebug() << ("Twitter::receiveLastStatus()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["last"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?screen_name=" + account->username, &buffer);
	receiveLastStatusRequests[id] = tr("Updating last status: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousFriendsStatuses(uint lastStatusId, int count) {
	qDebug() << ("Twitter::receiveFriendsStatuses()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["friends"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&max_id=" + QString::number(lastStatusId) : ""), &buffer);
	receivePreviousFriendsStatusesRequests[id] = tr("Updating friends statuses: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousReplies(uint lastStatusId, int count) {
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

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&max_id=" + QString::number(lastStatusId) : ""), &buffer);
	receivePreviousRepliesRequests[id] = tr("Updating replies: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousPublicStatuses(uint lastStatusId, int count) {
	qDebug() << ("Twitter::receivePublicStatuses()");
	
	setupProxy();

	QUrl url(account->serviceApiUrl() + Services::options[account->type]["public"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&max_id=" + QString::number(lastStatusId) : ""), &buffer);
	receivePreviousPublicStatusesRequests[id] = tr("Updating public statuses: %1").arg(url.host() + url.path());
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

void Twitter::receiveInboxMessages(uint lastStatusId, int count) {
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

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&since_id=" + QString::number(lastStatusId) : ""), &buffer);
	receiveInboxMessagesRequests[id] = tr("Updating inbox messages: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousInboxMessages(uint lastStatusId, int count) {
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

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&max_id=" + QString::number(lastStatusId) : ""), &buffer);
	receivePreviousInboxMessagesRequests[id] = tr("Updating inbox messages: %1").arg(url.host() + url.path());
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

void Twitter::abort() {
	qDebug() << ("Twitter::abort()");

}

void Twitter::requestStarted(int id) {
	qDebug() << ("Twitter::requestStarted() " + QString::number(id));
	
	if (receiveFriendsStatusesRequests.find(id) != receiveFriendsStatusesRequests.end()) {
		qDebug() << ("Request started: " + receiveFriendsStatusesRequests[id]);
	} else if (receiveRepliesRequests.find(id) != receiveRepliesRequests.end()) {
		qDebug() << ("Request started: " + receiveRepliesRequests[id]);
	} else if (receivePublicStatusesRequests.find(id) != receivePublicStatusesRequests.end()) {
		qDebug() << ("Request started: " + receivePublicStatusesRequests[id]);
	} else if (receiveLastStatusRequests.find(id) != receiveLastStatusRequests.end()) {
		qDebug() << ("Request started: " + receiveLastStatusRequests[id]);
	} else if (sendStatusRequests.find(id) != sendStatusRequests.end()) {
		qDebug() << ("Request started: " + sendStatusRequests[id]);
	} else if (receiveFavoritesRequests.find(id) != receiveFavoritesRequests.end()) {
		qDebug() << ("Request started: " + receiveFavoritesRequests[id]);
	} else if (receivePreviousFriendsStatusesRequests.find(id) != receivePreviousFriendsStatusesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousFriendsStatusesRequests[id]);
	} else if (receivePreviousRepliesRequests.find(id) != receivePreviousRepliesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousRepliesRequests[id]);
	} else if (receivePreviousPublicStatusesRequests.find(id) != receivePreviousPublicStatusesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousPublicStatusesRequests[id]);
	} else if (receivePreviousFavoritesRequests.find(id) != receivePreviousFavoritesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousFavoritesRequests[id]);
	} else if (receiveInboxMessagesRequests.find(id) != receiveInboxMessagesRequests.end()) {
		qDebug() << ("Request started: " + receiveInboxMessagesRequests[id]);
	} else if (receivePreviousInboxMessagesRequests.find(id) != receivePreviousInboxMessagesRequests.end()) {
		qDebug() << ("Request started: " + receivePreviousInboxMessagesRequests[id]);
	} else if (sendDirectMessageRequests.find(id) != sendDirectMessageRequests.end()) {
		qDebug() << ("Request started: " + sendDirectMessageRequests[id]);
	}
}

void Twitter::requestFinished(int id, bool error) {
	if (!error) {
		qDebug() << ("Twitter::requestFinished() " + QString::number(id));

		if (receiveFriendsStatusesRequests.find(id) != receiveFriendsStatusesRequests.end()) {
			qDebug() << ("Request finished: " + receiveFriendsStatusesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit friendsStatusesReceived(buffer.data());
		} else if (receiveRepliesRequests.find(id) != receiveRepliesRequests.end()) {
			qDebug() << ("Request finished: " + receiveRepliesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit repliesReceived(buffer.data());
		} else if (receivePublicStatusesRequests.find(id) != receivePublicStatusesRequests.end()) {
			qDebug() << ("Request finished: " + receivePublicStatusesRequests[id]);
			buffer.close();
			emit publicStatusesReceived(buffer.data());
		} else if (receiveLastStatusRequests.find(id) != receiveLastStatusRequests.end()) {
			qDebug() << ("Request finished: " + receiveLastStatusRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit lastStatusReceived(buffer.data());
		} else if (sendStatusRequests.find(id) != sendStatusRequests.end()) {
			qDebug() << ("Request finished: " + sendStatusRequests[id]);
			buffer.close();
			emit statusSent(buffer.data());
		} else if (receiveFavoritesRequests.find(id) != receiveFavoritesRequests.end()) {
			qDebug() << ("Request finished: " + receiveFavoritesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit favoritesReceived(buffer.data());
		} else if (receivePreviousFriendsStatusesRequests.find(id) != receivePreviousFriendsStatusesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousFriendsStatusesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousFriendsStatusesReceived(buffer.data());
		} else if (receivePreviousRepliesRequests.find(id) != receivePreviousRepliesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousRepliesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousRepliesReceived(buffer.data());
		} else if (receivePreviousPublicStatusesRequests.find(id) != receivePreviousPublicStatusesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousPublicStatusesRequests[id]);
			buffer.close();
			emit previousPublicStatusesReceived(buffer.data());
		} else if (receivePreviousFavoritesRequests.find(id) != receivePreviousFavoritesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousFavoritesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousFavoritesReceived(buffer.data());
		} else if (receiveInboxMessagesRequests.find(id) != receiveInboxMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receiveInboxMessagesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit inboxMessagesReceived(buffer.data());
		} else if (receivePreviousInboxMessagesRequests.find(id) != receivePreviousInboxMessagesRequests.end()) {
			qDebug() << ("Request finished: " + receivePreviousInboxMessagesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit previousInboxMessagesReceived(buffer.data());
		} else if (sendDirectMessageRequests.find(id) != sendDirectMessageRequests.end()) {
			qDebug() << ("Request finished: " + sendDirectMessageRequests[id]);
			buffer.close();
			emit directMessageSent(buffer.data());
		}
	} else {
		qDebug() << ("Twitter::requestFinished() " + QString::number(id) + " error");
	}
}

#endif
