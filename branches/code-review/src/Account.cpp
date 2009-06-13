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
	qDebug() << ("Account::Account()");
	remainingRequests = -1;
	twitter = new Twitter(this);
	connect(twitter, SIGNAL(friendsStatusesReceived(const QByteArray&)), this, SLOT(addFriendsStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(repliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(publicStatusesReceived(const QByteArray&)), this, SLOT(addPublicStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(favoritesReceived(const QByteArray&)), this, SLOT(addFavorites(const QByteArray&)));
	connect(twitter, SIGNAL(lastStatusReceived(const QByteArray&)), this, SLOT(updateLastStatus(const QByteArray&)));
	connect(twitter, SIGNAL(inboxMessagesReceived(const QByteArray&)), this, SLOT(addInboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(statusSent(const QByteArray&)), this, SLOT(statusSent(const QByteArray&)));
	connect(twitter, SIGNAL(previousFriendsStatusesReceived(const QByteArray&)), this, SLOT(addFriendsStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(previousRepliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(previousPublicStatusesReceived(const QByteArray&)), this, SLOT(addPublicStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(previousFavoritesReceived(const QByteArray&)), this, SLOT(addFavorites(const QByteArray&)));
	connect(twitter, SIGNAL(previousInboxMessagesReceived(const QByteArray&)), this, SLOT(addInboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousFriendsStatusesReceived(const QByteArray&)), this, SIGNAL(previousFriendsStatusesReceived()));
	connect(twitter, SIGNAL(previousRepliesReceived(const QByteArray&)), this, SIGNAL(previousRepliesReceived()));
	connect(twitter, SIGNAL(previousPublicStatusesReceived(const QByteArray&)), this, SIGNAL(previousPublicStatusesReceived()));
	connect(twitter, SIGNAL(previousFavoritesReceived(const QByteArray&)), this, SIGNAL(previousFavoritesReceived()));
	connect(twitter, SIGNAL(previousInboxMessagesReceived(const QByteArray&)), this, SIGNAL(previousInboxMessagesReceived()));
}

Account::Account(const QString &type, const QString &username, const QString &password) {
	qDebug() << ("Account::Account()");
	remainingRequests = -1;
	twitter = new Twitter(this);
	this->type = type;
	this->username = username;
	this->password = password;
	connect(twitter, SIGNAL(friendsStatusesReceived(const QByteArray&)), this, SLOT(addFriendsStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(repliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(publicStatusesReceived(const QByteArray&)), this, SLOT(addPublicStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(favoritesReceived(const QByteArray&)), this, SLOT(addFavorites(const QByteArray&)));
	connect(twitter, SIGNAL(lastStatusReceived(const QByteArray&)), this, SLOT(updateLastStatus(const QByteArray&)));
	connect(twitter, SIGNAL(inboxMessagesReceived(const QByteArray&)), this, SLOT(addInboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(statusSent(const QByteArray&)), this, SLOT(statusSent(const QByteArray&)));
	connect(twitter, SIGNAL(previousFriendsStatusesReceived(const QByteArray&)), this, SLOT(addFriendsStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(previousRepliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(previousPublicStatusesReceived(const QByteArray&)), this, SLOT(addPublicStatuses(const QByteArray&)));
	connect(twitter, SIGNAL(previousFavoritesReceived(const QByteArray&)), this, SLOT(addFavorites(const QByteArray&)));
	connect(twitter, SIGNAL(previousInboxMessagesReceived(const QByteArray&)), this, SLOT(addInboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousFriendsStatusesReceived(const QByteArray&)), this, SIGNAL(previousFriendsStatusesReceived()));
	connect(twitter, SIGNAL(previousRepliesReceived(const QByteArray&)), this, SIGNAL(previousRepliesReceived()));
	connect(twitter, SIGNAL(previousPublicStatusesReceived(const QByteArray&)), this, SIGNAL(previousPublicStatusesReceived()));
	connect(twitter, SIGNAL(previousFavoritesReceived(const QByteArray&)), this, SIGNAL(previousFavoritesReceived()));
	connect(twitter, SIGNAL(previousInboxMessagesReceived(const QByteArray&)), this, SIGNAL(previousInboxMessagesReceived()));
}

void Account::addFriendsStatuses(const QByteArray &data) {
	qDebug() << ("Account::addFriendsStatuses()");
	QVector<Status> statuses = QwitTools::parseStatuses(data, this);
	if (statuses.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, friendsStatuses.size());
		uint maxId = (friendsStatuses.size() ? friendsStatuses[0].id : 0);
		statuses = QwitTools::mergeStatuses(friendsStatuses, statuses);
		cerr << friendsStatuses.size() << endl;
		if ((friendsStatuses[0].id > maxId) && (friendsStatuses.size() > size)) {
			friendsStatuses.resize(size);
		}
		cerr << friendsStatuses.size() << endl;
		emit friendsStatusesUpdated(friendsStatuses, this);
		emit newStatusesReceived(statuses, this);
	}
}

void Account::addReplies(const QByteArray &data) {
	qDebug() << ("Account::addReplies()");
	QVector<Status> statuses = QwitTools::parseStatuses(data, this);
	if (statuses.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, replies.size());
		uint maxId = (replies.size() ? replies[0].id : 0);
		statuses = QwitTools::mergeStatuses(replies, statuses);
		if ((replies[0].id > maxId) && (replies.size() > size)) {
			replies.resize(size);
		}
		emit repliesUpdated(replies, this);
		emit newStatusesReceived(statuses, this);
	}
}

void Account::addPublicStatuses(const QByteArray &data) {
	qDebug() << ("Account::addPublicStatuses()");
	QVector<Status> statuses = QwitTools::parseStatuses(data, this);
	if (statuses.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, publicStatuses.size());
		uint maxId = (publicStatuses.size() ? publicStatuses[0].id : 0);
		statuses = QwitTools::mergeStatuses(publicStatuses, statuses);
		if ((publicStatuses[0].id > maxId) && (publicStatuses.size() > size)) {
			publicStatuses.resize(size);
		}
		emit publicStatusesUpdated(publicStatuses, this);
		emit newStatusesReceived(statuses, this);
	}
}

void Account::addInboxMessages(const QByteArray &data) {
	qDebug() << ("Account::addInboxMessages()");
	QVector<Status> messages = QwitTools::parseInboxMessages(data, this);
	if (messages.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, inboxMessages.size());
		uint maxId = (inboxMessages.size() ? inboxMessages[0].id : 0);
		messages = QwitTools::mergeStatuses(inboxMessages, messages);
		if ((inboxMessages[0].id > maxId) && (inboxMessages.size() > size)) {
			inboxMessages.resize(size);
		}
		emit inboxMessagesUpdated(inboxMessages, this);
		emit newStatusesReceived(messages, this);
	}
}

void Account::updateLastStatus(const QByteArray &data) {
	qDebug() << ("Account::updateLastStatus()");
	lastStatus = QwitTools::parseUser(data, this);
}

void Account::updateLastStatus() {
	qDebug() << ("Account::updateLastStatus()");
	twitter->receiveLastStatus();
}

void Account::sendStatus(const QString &status, int inReplyToStatusId) {
	qDebug() << ("Account::sendStatus()");
	twitter->sendStatus(status, inReplyToStatusId);
}

void Account::statusSent(const QByteArray &data) {
	qDebug() << ("Account::statusSent()");
	lastStatus = QwitTools::parseStatus(data, this);
	emit lastStatusReceived(lastStatus.status, this);
}

void Account::receivePublicStatuses(int count) {
	qDebug() << ("Account::receivePublicStatuses()");
	twitter->receivePublicStatuses((publicStatuses.size() != 0 ? publicStatuses[0].id : 0), count);
}

void Account::receiveFriendsStatuses(int count) {
	qDebug() << ("Account::receiveFriendsStatuses()");
	twitter->receiveFriendsStatuses((friendsStatuses.size() != 0 ? friendsStatuses[0].id : 0), count);
}

void Account::receiveReplies(int count) {
	qDebug() << ("Account::receiveReplies()");
	twitter->receiveReplies((replies.size() != 0 ? replies[0].id : 0), count);
}

void Account::receiveInboxMessages(int count) {
	qDebug() << ("Account::receiveInboxMessages()");
	twitter->receiveInboxMessages((inboxMessages.size() != 0 ? inboxMessages[0].id : 0), count);
}

void Account::receivePreviousPublicStatuses(int count) {
	qDebug() << ("Account::receivePreviousPublicStatuses()");
	twitter->receivePreviousPublicStatuses((publicStatuses.size() != 0 ? publicStatuses[publicStatuses.size() - 1].id : 0), count);
}

void Account::receivePreviousFriendsStatuses(int count) {
	qDebug() << ("Account::receivePreviousFriendsStatuses()");
	twitter->receivePreviousFriendsStatuses((friendsStatuses.size() != 0 ? friendsStatuses[friendsStatuses.size() - 1].id : 0), count);
}

void Account::removePreviousFriendsStatuses(int count) {
	qDebug() << ("Account::removePreviousFriendsStatuses()");
	int oldItemsCount = friendsStatuses.size();
	int newItemsCount = (oldItemsCount - 1) - (oldItemsCount - 1) % count;
	friendsStatuses.resize(newItemsCount);
	emit friendsStatusesUpdated(friendsStatuses, this);
}

void Account::receivePreviousReplies(int count) {
	qDebug() << ("Account::receivePreviousReplies()");
	twitter->receivePreviousReplies((replies.size() != 0 ? replies[replies.size() - 1].id : 0), count);
}

void Account::removePreviousReplies(int count) {
	qDebug() << ("Account::removePreviousReplies()");
	int oldItemsCount = replies.size();
	int newItemsCount = (oldItemsCount - 1) - (oldItemsCount - 1) % count;
	replies.resize(newItemsCount);
	emit repliesUpdated(replies, this);
}

void Account::receivePreviousInboxMessages(int count) {
	qDebug() << ("Account::receivePreviousInboxMessages()");
	twitter->receivePreviousInboxMessages((inboxMessages.size() != 0 ? inboxMessages[inboxMessages.size() - 1].id : 0), count);
}

void Account::removePreviousInboxMessages(int count) {
	qDebug() << ("Account::removePreviousInboxMessages()");
	int oldItemsCount = inboxMessages.size();
	int newItemsCount = (oldItemsCount - 1) - (oldItemsCount - 1) % count;
	inboxMessages.resize(newItemsCount);
	emit inboxMessagesUpdated(inboxMessages, this);
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
	messagesCache.beginWriteArray("Favorites");
	for (int i = 0; i < min(favorites.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		favorites[i].save(messagesCache);
	}
	messagesCache.endArray();
	messagesCache.beginWriteArray("Inbox");
	for (int i = 0; i < min(inboxMessages.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		inboxMessages[i].save(messagesCache);
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
	n = messagesCache.beginReadArray("Favorites");
	favorites.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		favorites.push_back(Status::load(messagesCache, this));
	}
	messagesCache.endArray();
	n = messagesCache.beginReadArray("Inbox");
	inboxMessages.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		inboxMessages.push_back(Status::load(messagesCache, this));
	}
	messagesCache.endArray();
	emit friendsStatusesUpdated(friendsStatuses, this);
	emit repliesUpdated(replies, this);
	emit publicStatusesUpdated(publicStatuses, this);
	emit favoritesUpdated(favorites, this);
	emit inboxMessagesUpdated(inboxMessages, this);
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

void Account::receiveFavorites() {
	qDebug() << ("Account::receiveFavorites()");
	twitter->receiveFavorites();
}

void Account::receivePreviousFavorites() {
	qDebug() << ("Account::receivePreviousFavorites()");
	twitter->receivePreviousFavorites(favorites.size() / 20 + 1);
}

void Account::removePreviousFavorites() {
	qDebug() << ("Account::removePreviousFavorites()");
	int oldItemsCount = favorites.size();
	int newItemsCount = (oldItemsCount - 1) - (oldItemsCount - 1) % 20;
	favorites.resize(newItemsCount);
	emit favoritesUpdated(favorites, this);
}

void Account::addFavorites(const QByteArray &data) {
	qDebug() << ("Account::addFavorites()");
	QVector<Status> statuses = QwitTools::parseStatuses(data, this);
	if (statuses.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, favorites.size());
		uint maxId = (favorites.size() ? favorites[0].id : 0);
		statuses = QwitTools::mergeStatuses(favorites, statuses);
		if ((favorites[0].id > maxId) && (favorites.size() > size)) {
			favorites.resize(size);
		}
		emit favoritesUpdated(favorites, this);
		emit newStatusesReceived(statuses, this);
	}
}

#endif
