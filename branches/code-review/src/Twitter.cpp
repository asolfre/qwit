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
	QwitTools::log("Twitter::Twitter()");

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

void Twitter::sendStatus(const QString &status, int inReplyToStatusId) {
	QwitTools::log("Twitter::sendStatus()");
	
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

void Twitter::receiveFriendsStatuses(int lastStatusId, int count) {
	QwitTools::log("Twitter::receiveFriendsStatuses()");
	
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

void Twitter::receiveReplies(int lastStatusId, int count) {
	QwitTools::log("Twitter::receiveReplies()");
	
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

void Twitter::receivePublicStatuses(int lastStatusId, int count) {
	QwitTools::log("Twitter::receivePublicStatuses()");
	
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
	QwitTools::log("Twitter::receiveLastStatus()");
	
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

void Twitter::receivePreviousFriendsStatuses(int lastStatusId, int count) {
	QwitTools::log("Twitter::receiveFriendsStatuses()");
	
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
	receiveFriendsStatusesRequests[id] = tr("Updating friends statuses: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousReplies(int lastStatusId, int count) {
	QwitTools::log("Twitter::receiveReplies()");
	
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
	receiveRepliesRequests[id] = tr("Updating replies: %1").arg(url.host() + url.path());
}

void Twitter::receivePreviousPublicStatuses(int lastStatusId, int count) {
	QwitTools::log("Twitter::receivePublicStatuses()");
	
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
	receivePublicStatusesRequests[id] = tr("Updating public statuses: %1").arg(url.host() + url.path());
}

void Twitter::abort() {
	QwitTools::log("Twitter::abort()");

}

void Twitter::requestStarted(int id) {
	QwitTools::log("Twitter::requestStarted() " + QString::number(id));
	
	if (receiveFriendsStatusesRequests.find(id) != receiveFriendsStatusesRequests.end()) {
		QwitTools::log("Request started: " + receiveFriendsStatusesRequests[id]);
	} else if (receiveRepliesRequests.find(id) != receiveRepliesRequests.end()) {
		QwitTools::log("Request started: " + receiveRepliesRequests[id]);
	} else if (receivePublicStatusesRequests.find(id) != receivePublicStatusesRequests.end()) {
		QwitTools::log("Request started: " + receivePublicStatusesRequests[id]);
	} else if (receiveLastStatusRequests.find(id) != receiveLastStatusRequests.end()) {
		QwitTools::log("Request started: " + receiveLastStatusRequests[id]);
	} else if (sendStatusRequests.find(id) != sendStatusRequests.end()) {
		QwitTools::log("Request started: " + sendStatusRequests[id]);
	}
}

void Twitter::requestFinished(int id, bool error) {
	if (!error) {
		QwitTools::log("Twitter::requestFinished() " + QString::number(id));

		if (receiveFriendsStatusesRequests.find(id) != receiveFriendsStatusesRequests.end()) {
			QwitTools::log("Request finished: " + receiveFriendsStatusesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit friendsStatusesReceived(buffer.data());
		} else if (receiveRepliesRequests.find(id) != receiveRepliesRequests.end()) {
			QwitTools::log("Request finished: " + receiveRepliesRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit repliesReceived(buffer.data());
		} else if (receivePublicStatusesRequests.find(id) != receivePublicStatusesRequests.end()) {
			QwitTools::log("Request finished: " + receivePublicStatusesRequests[id]);
			buffer.close();
			emit publicStatusesReceived(buffer.data());
		} else if (receiveLastStatusRequests.find(id) != receiveLastStatusRequests.end()) {
			QwitTools::log("Request finished: " + receiveLastStatusRequests[id]);
			buffer.close();
			QHttpResponseHeader response = http->lastResponse();
			QString remainingRequests = response.value("X-RateLimit-Remaining");
			account->setRemainingRequests(remainingRequests != "" ? remainingRequests.toInt() : -1);
			emit lastStatusReceived(buffer.data());
		} else if (sendStatusRequests.find(id) != sendStatusRequests.end()) {
			QwitTools::log("Request finished: " + sendStatusRequests[id]);
			buffer.close();
			emit statusSent(buffer.data());
		}
	} else {
		QwitTools::log("Twitter::requestFinished() " + QString::number(id) + " error");
	}
}

#endif
