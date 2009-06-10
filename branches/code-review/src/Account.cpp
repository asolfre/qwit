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

#include "QwitHeaders.h"

#include "Account.h"
#include "QwitTools.h"
#include "Configuration.h"
#include "Services.h"

Account::Account() {
	QwitTools::log("Account::Account()");
	remainingRequests = -1;
	twitter = new Twitter(this);
	connect(twitter, SIGNAL(friendsStatusesReceived(const QByteArray&)), this, SLOT(addFriendsStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(repliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(publicStatusesReceived(const QByteArray&)), this, SLOT(addPublicStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(lastStatusReceived(const QByteArray&)), this, SLOT(updateLastStatus(const QByteArray&)));
	connect(twitter, SIGNAL(statusSent(const QByteArray&)), this, SLOT(statusSent(const QByteArray&)));
}

Account::Account(const QString &type, const QString &username, const QString &password) {
	QwitTools::log("Account::Account()");
	remainingRequests = -1;
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
	QVector<Status> statuses = QwitTools::parseStatuses(data, this);
	if (statuses.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, friendsStatuses.size());
		uint lastId = (friendsStatuses.size() ? friendsStatuses[friendsStatuses.size() - 1].id : 0);
		uint maxId = lastId;
		for (int i = 0; i < statuses.size(); ++i) {
			if (statuses[i].id > maxId) maxId = statuses[i].id;
			friendsStatuses.push_back(statuses[i]);
		}
		qSort(friendsStatuses.begin(), friendsStatuses.end());
		QwitTools::makeStatusesUnique(friendsStatuses);
		if ((maxId > lastId) && (friendsStatuses.size() > size)) {
			friendsStatuses.resize(size);
		}
		emit friendsStatusesUpdated(friendsStatuses, this);
		emit newStatusesReceived(statuses, this);
	}
}

void Account::addReplies(const QByteArray &data) {
	QwitTools::log("Account::addReplies()");
	QVector<Status> statuses = QwitTools::parseStatuses(data, this);
	if (statuses.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, friendsStatuses.size());
		uint lastId = (replies.size() ? replies[replies.size() - 1].id : 0);
		uint maxId = lastId;
		for (int i = 0; i < statuses.size(); ++i) {
			if (statuses[i].id > maxId) maxId = statuses[i].id;
			replies.push_back(statuses[i]);
		}
		qSort(replies.begin(), replies.end());
		QwitTools::makeStatusesUnique(replies);
		if ((maxId > lastId) && (replies.size() > size)) {
			replies.resize(size);
		}
		emit repliesUpdated(replies, this);
		emit newStatusesReceived(statuses, this);
	}
}

void Account::addPublicStatuses(const QByteArray &data) {
	QwitTools::log("Account::addPublicStatuses()");
	QVector<Status> statuses = QwitTools::parseStatuses(data, this);
	if (statuses.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, friendsStatuses.size());
		uint lastId = (replies.size() ? replies[replies.size() - 1].id : 0);
		uint maxId = lastId;
		for (int i = 0; i < statuses.size(); ++i) {
			if (statuses[i].id > maxId) maxId = statuses[i].id;
			publicStatuses.push_back(statuses[i]);
		}
		qSort(publicStatuses.begin(), publicStatuses.end());
		QwitTools::makeStatusesUnique(publicStatuses);
		if ((maxId > lastId) && (publicStatuses.size() > size)) {
			publicStatuses.resize(size);
		}
		emit publicStatusesUpdated(publicStatuses, this);
		emit newStatusesReceived(statuses, this);
	}
}

void Account::updateLastStatus(const QByteArray &data) {
	QwitTools::log("Account::updateLastStatus()");
	lastStatus = QwitTools::parseUser(data, this);
}

void Account::updateLastStatus() {
	QwitTools::log("Account::updateLastStatus()");
	twitter->receiveLastStatus();
}

void Account::sendStatus(const QString &status, int inReplyToStatusId) {
	QwitTools::log("Account::sendStatus()");
	twitter->sendStatus(status, inReplyToStatusId);
}

void Account::statusSent(const QByteArray &data) {
	QwitTools::log("Account::statusSent()");
	lastStatus = QwitTools::parseStatus(data, this);
	emit lastStatusReceived(lastStatus.status, this);
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

void Account::removePreviousFriendsStatuses(int count) {
	QwitTools::log("Account::removePreviousFriendsStatuses()");
	int oldItemsCount = friendsStatuses.size();
	int newItemsCount = (oldItemsCount - 1) - (oldItemsCount - 1) % count;
	friendsStatuses.resize(newItemsCount);
	emit friendsStatusesUpdated(friendsStatuses, this);
}

void Account::receivePreviousReplies(int count) {
	QwitTools::log("Account::receivePreviousReplies()");
	twitter->receivePreviousReplies((replies.size() != 0 ? replies[replies.size() - 1].id : 0), count);
}

void Account::removePreviousReplies(int count) {
	QwitTools::log("Account::removePreviousReplies()");
	int oldItemsCount = replies.size();
	int newItemsCount = (oldItemsCount - 1) - (oldItemsCount - 1) % count;
	replies.resize(newItemsCount);
	emit repliesUpdated(replies, this);
}

void Account::saveMessages(QSettings &messagesCache) {
	Configuration *config = Configuration::getInstance();
	messagesCache.beginGroup("Status");
	lastStatus.save(messagesCache);
	messagesCache.endGroup();
	messagesCache.beginWriteArray("Friends");
	for (int i = 0; i < min(friendsStatuses.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		friendsStatuses[i].save(messagesCache);
	}
	messagesCache.endArray();
	messagesCache.beginWriteArray("Replies");
	for (int i = 0; i < min(replies.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		replies[i].save(messagesCache);
	}
	messagesCache.endArray();
	messagesCache.beginWriteArray("Public");
	for (int i = 0; i < min(publicStatuses.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		publicStatuses[i].save(messagesCache);
	}
	messagesCache.endArray();
}

void Account::loadMessages(QSettings &messagesCache) {
	messagesCache.beginGroup("Status");
	lastStatus = Status::load(messagesCache, this);
	messagesCache.endGroup();
	int n = messagesCache.beginReadArray("Friends");
	friendsStatuses.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		friendsStatuses.push_back(Status::load(messagesCache, this));
	}
	messagesCache.endArray();
	n = messagesCache.beginReadArray("Replies");
	replies.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		replies.push_back(Status::load(messagesCache, this));
	}
	messagesCache.endArray();
	n = messagesCache.beginReadArray("Public");
	publicStatuses.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		publicStatuses.push_back(Status::load(messagesCache, this));
	}
	messagesCache.endArray();
	emit friendsStatusesUpdated(friendsStatuses, this);
	emit repliesUpdated(replies, this);
	emit publicStatusesUpdated(publicStatuses, this);
	emit lastStatusReceived(lastStatus.status, this);
}

QString Account::serviceApiUrl() {
	return _serviceApiUrl == "" ? Services::options[type]["apiurl"] : _serviceApiUrl;
}

QString Account::serviceBaseUrl() {
	return _serviceBaseUrl == "" ? Services::options[type]["baseurl"] : _serviceBaseUrl;
}

void Account::setRemainingRequests(int remainingRequests) {
	this->remainingRequests = remainingRequests;
	emit remainingRequestsUpdated(remainingRequests, this);
}

#endif
