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

#ifndef Twitter_cpp
#define Twitter_cpp

#include <QUrl>
#include <QHttpRequestHeader>
#include <QByteArray>
#include <QDateTime>
#include <QNetworkProxy>

#include "Twitter.h"

#include "Services.h"

#include <iostream>

using namespace std;

Twitter::Twitter(Account *account) {
	this->account = account;
	http = new QHttp();
	connect(http, SIGNAL(requestStarted(int)), this, SLOT(requestStarted(int)));
	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
}

void Twitter::sendStatus(QString status, QString replyId) {
	QUrl url((account->serviceAPIURL == "" ? Services::options[account->type]["apiurl"] : account->serviceAPIURL) + Services::options[account->type]["friends"] + ".xml");
	
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
	if (replyId != "") {
		data += "&in_reply_to_status_id=";
		data += replyId;
	}
	data += "&source=qwit";
	int id = http->request(header, data);
	sendStatusRequests[id] = tr("Sending status: %1").arg(url.host() + url.path());
}

void Twitter::receiveFriendsStatuses(int lastStatusId, int count) {
	QUrl url((account->serviceAPIURL == "" ? Services::options[account->type]["apiurl"] : account->serviceAPIURL) + Services::options[account->type]["friends"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&since_id=" + QString::number(lastStatusId) : ""), &buffer);
	receiveFriendsStatusesRequests[id] = tr("Updating timeline: %1").arg(url.host() + url.path());
}

void Twitter::receiveReplies(int lastStatusId, int count) {
	QUrl url((account->serviceAPIURL == "" ? Services::options[account->type]["apiurl"] : account->serviceAPIURL) + Services::options[account->type]["replies"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&since_id=" + QString::number(lastStatusId) : ""), &buffer);
	receiveRepliesRequests[id] = tr("Updating timeline: %1").arg(url.host() + url.path());
}

void Twitter::receivePublicStatuses(int lastStatusId, int count) {
	QUrl url((account->serviceAPIURL == "" ? Services::options[account->type]["apiurl"] : account->serviceAPIURL) + Services::options[account->type]["public"] + ".xml");

	if(url.toString().indexOf("https") == 0) {
	    http->setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	http->setUser(account->username, account->password);

	buffer.open(QIODevice::WriteOnly);

	int id = http->get(url.path() + "?count=" + QString::number(count) + (lastStatusId ? "&since_id=" + QString::number(lastStatusId) : ""), &buffer);
	receivePublicStatusesRequests[id] = tr("Updating timeline: %1").arg(url.host() + url.path());
}

void Twitter::abort() {
}

void Twitter::requestStarted(int id) {
	if (receiveFriendsStatusesRequests.find(id) != receiveFriendsStatusesRequests.end()) {
		cout << "Request started: " << qPrintable(receiveFriendsStatusesRequests[id]) << endl;
	} else if (receiveRepliesRequests.find(id) != receiveRepliesRequests.end()) {
		cout << "Request started: " << qPrintable(receiveRepliesRequests[id]) << endl;
	} else if (receivePublicStatusesRequests.find(id) != receivePublicStatusesRequests.end()) {
		cout << "Request started: " << qPrintable(receivePublicStatusesRequests[id]) << endl;
	}
}

void Twitter::requestFinished(int id, bool error) {
	if (receiveFriendsStatusesRequests.find(id) != receiveFriendsStatusesRequests.end()) {
		cout << "Request finished: " << qPrintable(receiveFriendsStatusesRequests[id]) << endl;
		buffer.close();
		emit friendsStatusesReceived(buffer.data());
	} else if (receiveRepliesRequests.find(id) != receiveRepliesRequests.end()) {
		cout << "Request finished: " << qPrintable(receiveRepliesRequests[id]) << endl;
		buffer.close();
		emit repliesReceived(buffer.data());
	} else if (receivePublicStatusesRequests.find(id) != receivePublicStatusesRequests.end()) {
		cout << "Request finished: " << qPrintable(receivePublicStatusesRequests[id]) << endl;
		buffer.close();
		emit publicStatusesReceived(buffer.data());
	}
}

#endif
