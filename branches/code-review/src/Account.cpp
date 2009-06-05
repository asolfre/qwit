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
 *  Account class implementation
 */

#ifndef Account_cpp
#define Account_cpp

#include "Account.h"
#include "QwitTools.h"

#include <iostream>

using namespace std;

Account::Account() {
	QwitTools::log("Account::Account()");
	twitter = new Twitter(this);
	connect(twitter, SIGNAL(friendsStatusesReceived(const QByteArray&)), this, SLOT(addFriendsStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(repliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(publicStatusesReceived(const QByteArray&)), this, SLOT(addPublicStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(lastStatusReceived(const QByteArray&)), this, SLOT(updateLastStatus(const QByteArray&)));
	connect(twitter, SIGNAL(statusSent(const QByteArray&)), this, SLOT(statusSent(const QByteArray&)));
}

Account::Account(const QString &type, const QString &username, const QString &password) {
	QwitTools::log("Account::Account()");
	twitter = new Twitter(this);
	this->type = type;
	this->username = username;
	this->password = password;
	connect(twitter, SIGNAL(friendsStatusesReceived(const QByteArray&)), this, SLOT(addFriendsStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(repliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(publicStatusesReceived(const QByteArray&)), this, SLOT(addPublicStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(lastStatusReceived(const QByteArray&)), this, SLOT(updateLastStatus(const QByteArray&)));
	connect(twitter, SIGNAL(statusSent(const QByteArray&)), this, SLOT(statusSent(const QByteArray&)));
}

void Account::addFriendsStatuses(const QByteArray &data) {
	QwitTools::log("Account::addFriendsStatuses()");
	QString trayMessage = "";
	QVector<Status> statuses = QwitTools::parseStatuses(data);
	int lastId = (friendsStatuses.size() ? friendsStatuses[friendsStatuses.size() - 1].id : 0);
	int maxId = lastId;
	for (int i = 0; i < statuses.size(); ++i) {
		if (statuses[i].id > maxId) maxId = statuses[i].id;
		if (statuses[i].id > lastId) {
			if (trayMessage.length()) {
				trayMessage += "----------------------------\n";
			}
			trayMessage += statuses[i].username + ": " + statuses[i].status + " /" + QwitTools::formatDateTime(statuses[i].time.toLocalTime()) + "\n";
		}
		friendsStatuses.push_back(statuses[i]);
	}
	qSort(friendsStatuses.begin(), friendsStatuses.end());
	QwitTools::makeStatusesUnique(friendsStatuses);
	emit friendsStatusesReceived(friendsStatuses);
}

void Account::addReplies(const QByteArray &data) {
	QwitTools::log("Account::addReplies()");
	QString trayMessage = "";
	QVector<Status> statuses = QwitTools::parseStatuses(data);
	int lastId = (replies.size() ? replies[replies.size() - 1].id : 0);
	int maxId = lastId;
	for (int i = 0; i < statuses.size(); ++i) {
		if (statuses[i].id > maxId) maxId = statuses[i].id;
		if (statuses[i].id > lastId) {
			if (trayMessage.length()) {
				trayMessage += "----------------------------\n";
			}
			trayMessage += statuses[i].username + ": " + statuses[i].status + " /" + QwitTools::formatDateTime(statuses[i].time.toLocalTime()) + "\n";
		}
		replies.push_back(statuses[i]);
	}
	qSort(replies.begin(), replies.end());
	QwitTools::makeStatusesUnique(replies);
	emit repliesReceived(replies);
}

void Account::addPublicStatuses(const QByteArray &data) {
	QwitTools::log("Account::addPublicStatuses()");
	QString trayMessage = "";
	QVector<Status> statuses = QwitTools::parseStatuses(data);
	int lastId = (replies.size() ? replies[replies.size() - 1].id : 0);
	int maxId = lastId;
	for (int i = 0; i < statuses.size(); ++i) {
		if (statuses[i].id > maxId) maxId = statuses[i].id;
		if (statuses[i].id > lastId) {
			if (trayMessage.length()) {
				trayMessage += "----------------------------\n";
			}
			trayMessage += statuses[i].username + ": " + statuses[i].status + " /" + QwitTools::formatDateTime(statuses[i].time.toLocalTime()) + "\n";
		}
		publicStatuses.push_back(statuses[i]);
	}
	qSort(publicStatuses.begin(), publicStatuses.end());
	QwitTools::makeStatusesUnique(publicStatuses);
	emit publicStatusesReceived(publicStatuses);
}

void Account::updateLastStatus(const QByteArray &data) {
	QwitTools::log("Account::updateLastStatus()");
	lastStatus = QwitTools::parseUser(data);
	emit lastStatusReceived(lastStatus.status);
}

void Account::updateLastStatus() {
	QwitTools::log("Account::updateLastStatus()");
	twitter->receiveLastStatus();
}

void Account::sendStatus(const QString &status) {
	QwitTools::log("Account::sendStatus()");
	twitter->sendStatus(status);
}

void Account::statusSent(const QByteArray &data) {
	QwitTools::log("Account::statusSent()");
	lastStatus = QwitTools::parseStatus(data);
	emit lastStatusReceived(lastStatus.status);
}

void Account::receivePublicStatuses(int count) {
	QwitTools::log("Account::receivePublicStatuses()");
	twitter->receivePublicStatuses((publicStatuses.size() != 0 ? publicStatuses[0].id : 0), count);
}

void Account::receiveFriendsStatuses(int count) {
	QwitTools::log("Account::receiveFriendsStatuses()");
	twitter->receiveFriendsStatuses((friendsStatuses.size() != 0 ? friendsStatuses[0].id : 0), count);
}

void Account::receiveReplies(int count) {
	QwitTools::log("Account::receiveReplies()");
	twitter->receiveReplies((replies.size() != 0 ? replies[0].id : 0), count);
}

void Account::receivePreviousPublicStatuses(int count) {
	QwitTools::log("Account::receivePreviousPublicStatuses()");
	twitter->receivePreviousPublicStatuses((publicStatuses.size() != 0 ? publicStatuses[publicStatuses.size() - 1].id : 0), count);
}

void Account::receivePreviousFriendsStatuses(int count) {
	QwitTools::log("Account::receivePreviousFriendsStatuses()");
	twitter->receivePreviousFriendsStatuses((friendsStatuses.size() != 0 ? friendsStatuses[friendsStatuses.size() - 1].id : 0), count);
}

void Account::receivePreviousReplies(int count) {
	QwitTools::log("Account::receivePreviousReplies()");
	twitter->receivePreviousReplies((replies.size() != 0 ? replies[replies.size() - 1].id : 0), count);
}

#endif
