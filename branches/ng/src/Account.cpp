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
	connect(twitter, SIGNAL(friendsMessagesReceived(const QByteArray&)), this, SLOT(addFriendsMessages(const QByteArray&)));
	connect(twitter, SIGNAL(repliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(publicMessagesReceived(const QByteArray&)), this, SLOT(addPublicMessages(const QByteArray&)));
	connect(twitter, SIGNAL(favoritesReceived(const QByteArray&)), this, SLOT(addFavorites(const QByteArray&)));
	connect(twitter, SIGNAL(lastMessageReceived(const QByteArray&)), this, SLOT(updateLastMessage(const QByteArray&)));
	connect(twitter, SIGNAL(inboxMessagesReceived(const QByteArray&)), this, SLOT(addInboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(outboxMessagesReceived(const QByteArray&)), this, SLOT(addOutboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(messageSent(const QByteArray&)), this, SLOT(messageSent(const QByteArray&)));
	connect(twitter, SIGNAL(directMessageSent(const QByteArray&)), this, SLOT(directMessageSent(const QByteArray&)));
	connect(twitter, SIGNAL(messageFavored(const QByteArray&)), this, SLOT(messageFavored(const QByteArray&)));
	connect(twitter, SIGNAL(messageUnfavored(const QByteArray&)), this, SLOT(messageUnfavored(const QByteArray&)));
	connect(twitter, SIGNAL(messageDestroyed(const QByteArray&)), this, SLOT(messageDestroyed(const QByteArray&)));
	connect(twitter, SIGNAL(directMessageDestroyed(const QByteArray&)), this, SLOT(directMessageDestroyed(const QByteArray&)));
	connect(twitter, SIGNAL(previousFriendsMessagesReceived(const QByteArray&)), this, SLOT(addFriendsMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousRepliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(previousPublicMessagesReceived(const QByteArray&)), this, SLOT(addPublicMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousFavoritesReceived(const QByteArray&)), this, SLOT(addPreviousFavorites(const QByteArray&)));
	connect(twitter, SIGNAL(previousInboxMessagesReceived(const QByteArray&)), this, SLOT(addInboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousOutboxMessagesReceived(const QByteArray&)), this, SLOT(addOutboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousFriendsMessagesReceived(const QByteArray&)), this, SIGNAL(previousFriendsMessagesReceived()));
	connect(twitter, SIGNAL(previousRepliesReceived(const QByteArray&)), this, SIGNAL(previousRepliesReceived()));
	connect(twitter, SIGNAL(previousPublicMessagesReceived(const QByteArray&)), this, SIGNAL(previousPublicMessagesReceived()));
	connect(twitter, SIGNAL(previousFavoritesReceived(const QByteArray&)), this, SIGNAL(previousFavoritesReceived()));
	connect(twitter, SIGNAL(previousInboxMessagesReceived(const QByteArray&)), this, SIGNAL(previousInboxMessagesReceived()));
	connect(twitter, SIGNAL(previousOutboxMessagesReceived(const QByteArray&)), this, SIGNAL(previousOutboxMessagesReceived()));
	sendingMessage = false;
}

Account::Account(const QString &type, const QString &username, const QString &password, bool useHttps) {
	qDebug() << ("Account::Account()");
	remainingRequests = -1;
	twitter = new Twitter(this);
	this->type = type;
	this->username = username;
	this->password = password;
	this->useHttps = useHttps;
	connect(twitter, SIGNAL(friendsMessagesReceived(const QByteArray&)), this, SLOT(addFriendsMessages(const QByteArray&)));
	connect(twitter, SIGNAL(repliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(publicMessagesReceived(const QByteArray&)), this, SLOT(addPublicMessages(const QByteArray&)));
	connect(twitter, SIGNAL(favoritesReceived(const QByteArray&)), this, SLOT(addFavorites(const QByteArray&)));
	connect(twitter, SIGNAL(lastMessageReceived(const QByteArray&)), this, SLOT(updateLastMessage(const QByteArray&)));
	connect(twitter, SIGNAL(inboxMessagesReceived(const QByteArray&)), this, SLOT(addInboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(outboxMessagesReceived(const QByteArray&)), this, SLOT(addOutboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(messageSent(const QByteArray&)), this, SLOT(messageSent(const QByteArray&)));
	connect(twitter, SIGNAL(directMessageSent(const QByteArray&)), this, SLOT(directMessageSent(const QByteArray&)));
	connect(twitter, SIGNAL(messageFavored(const QByteArray&)), this, SLOT(messageFavored(const QByteArray&)));
	connect(twitter, SIGNAL(messageUnfavored(const QByteArray&)), this, SLOT(messageUnfavored(const QByteArray&)));
	connect(twitter, SIGNAL(messageDestroyed(const QByteArray&)), this, SLOT(messageDestroyed(const QByteArray&)));
	connect(twitter, SIGNAL(directMessageDestroyed(const QByteArray&)), this, SLOT(directMessageDestroyed(const QByteArray&)));
	connect(twitter, SIGNAL(previousFriendsMessagesReceived(const QByteArray&)), this, SLOT(addFriendsMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousRepliesReceived(const QByteArray&)), this, SLOT(addReplies(const QByteArray&)));
	connect(twitter, SIGNAL(previousPublicMessagesReceived(const QByteArray&)), this, SLOT(addPublicMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousFavoritesReceived(const QByteArray&)), this, SLOT(addPreviousFavorites(const QByteArray&)));
	connect(twitter, SIGNAL(previousInboxMessagesReceived(const QByteArray&)), this, SLOT(addInboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousOutboxMessagesReceived(const QByteArray&)), this, SLOT(addOutboxMessages(const QByteArray&)));
	connect(twitter, SIGNAL(previousFriendsMessagesReceived(const QByteArray&)), this, SIGNAL(previousFriendsMessagesReceived()));
	connect(twitter, SIGNAL(previousRepliesReceived(const QByteArray&)), this, SIGNAL(previousRepliesReceived()));
	connect(twitter, SIGNAL(previousPublicMessagesReceived(const QByteArray&)), this, SIGNAL(previousPublicMessagesReceived()));
	connect(twitter, SIGNAL(previousFavoritesReceived(const QByteArray&)), this, SIGNAL(previousFavoritesReceived()));
	connect(twitter, SIGNAL(previousInboxMessagesReceived(const QByteArray&)), this, SIGNAL(previousInboxMessagesReceived()));
	connect(twitter, SIGNAL(previousOutboxMessagesReceived(const QByteArray&)), this, SIGNAL(previousOutboxMessagesReceived()));
	sendingMessage = false;
}

void Account::addFriendsMessages(const QByteArray &data) {
	qDebug() << ("Account::addFriendsMessages()");
	QVector<Message> messages = QwitTools::parseMessages(data, this);
	if (messages.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, friendsMessages.size());
		uint maxId = (friendsMessages.size() ? friendsMessages[0].id : 0);
		messages = QwitTools::mergeMessages(friendsMessages, messages);
		if ((friendsMessages[0].id > maxId) && (friendsMessages.size() > size)) {
			friendsMessages.resize(size);
		}
		emit friendsMessagesUpdated(friendsMessages, this);
		emit newMessagesReceived(messages, this);
	}
}

void Account::addReplies(const QByteArray &data) {
	qDebug() << ("Account::addReplies()");
	QVector<Message> messages = QwitTools::parseMessages(data, this);
	if (messages.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, replies.size());
		uint maxId = (replies.size() ? replies[0].id : 0);
		messages = QwitTools::mergeMessages(replies, messages);
		if ((replies[0].id > maxId) && (replies.size() > size)) {
			replies.resize(size);
		}
		emit repliesUpdated(replies, this);
		emit newMessagesReceived(messages, this);
	}
}

void Account::addPublicMessages(const QByteArray &data) {
	qDebug() << ("Account::addPublicMessages()");
	QVector<Message> messages = QwitTools::parseMessages(data, this);
	if (messages.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, publicMessages.size());
		uint maxId = (publicMessages.size() ? publicMessages[0].id : 0);
		messages = QwitTools::mergeMessages(publicMessages, messages);
		if ((publicMessages[0].id > maxId) && (publicMessages.size() > size)) {
			publicMessages.resize(size);
		}
		emit publicMessagesUpdated(publicMessages, this);
		emit newMessagesReceived(messages, this);
	}
}

void Account::addInboxMessages(const QByteArray &data) {
	qDebug() << ("Account::addInboxMessages()");
	QVector<Message> messages = QwitTools::parseInboxMessages(data, this);
	if (messages.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, inboxMessages.size());
		uint maxId = (inboxMessages.size() ? inboxMessages[0].id : 0);
		messages = QwitTools::mergeMessages(inboxMessages, messages);
		if ((inboxMessages[0].id > maxId) && (inboxMessages.size() > size)) {
			inboxMessages.resize(size);
		}
		emit inboxMessagesUpdated(inboxMessages, this);
		emit newMessagesReceived(messages, this);
	}
}

void Account::addOutboxMessages(const QByteArray &data) {
	qDebug() << ("Account::addOutboxMessages()");
	QVector<Message> messages = QwitTools::parseOutboxMessages(data, this);
	if (messages.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, outboxMessages.size());
		uint maxId = (outboxMessages.size() ? outboxMessages[0].id : 0);
		messages = QwitTools::mergeMessages(outboxMessages, messages);
		if ((outboxMessages[0].id > maxId) && (outboxMessages.size() > size)) {
			outboxMessages.resize(size);
		}
		emit outboxMessagesUpdated(outboxMessages, this);
		emit newMessagesReceived(messages, this);
	}
}

void Account::updateLastMessage(const QByteArray &data) {
	qDebug() << ("Account::updateLastMessage()");
	lastMessage = QwitTools::parseUser(data, this);
}

void Account::updateLastMessage() {
	qDebug() << ("Account::updateLastMessage()");
	twitter->receiveLastMessage();
}

void Account::sendMessage(const QString &message, int inReplyToMessageId) {
	qDebug() << ("Account::sendMessage()");
	sendingMessage = true;
	messageBeingSent = message;
	twitter->sendMessage(message, inReplyToMessageId);
}

void Account::messageSent(const QByteArray &data) {
	qDebug() << ("Account::messageSent()");
	sendingMessage = false;
	lastMessage = QwitTools::parseMessage(data, this);
	emit lastMessageReceived(lastMessage.text, this);
	emit messageSent(lastMessage.text, this);
}

void Account::receivePublicMessages(int count) {
	qDebug() << ("Account::receivePublicMessages()");
	twitter->receivePublicMessages((publicMessages.size() != 0 ? publicMessages[0].id : 0), count);
}

void Account::receiveFriendsMessages(int count) {
	qDebug() << ("Account::receiveFriendsMessages()");
	twitter->receiveFriendsMessages((friendsMessages.size() != 0 ? friendsMessages[0].id : 0), count);
}

void Account::receiveReplies(int count) {
	qDebug() << ("Account::receiveReplies()");
	twitter->receiveReplies((replies.size() != 0 ? replies[0].id : 0), count);
}

void Account::receiveInboxMessages(int count) {
	qDebug() << ("Account::receiveInboxMessages()");
	twitter->receiveInboxMessages((inboxMessages.size() != 0 ? inboxMessages[0].id : 0), count);
}

void Account::receiveOutboxMessages(int count) {
	qDebug() << ("Account::receiveOutboxMessages()");
	twitter->receiveOutboxMessages((outboxMessages.size() != 0 ? outboxMessages[0].id : 0), count);
}

void Account::receivePreviousPublicMessages(int count) {
	qDebug() << ("Account::receivePreviousPublicMessages()");
	twitter->receivePreviousPublicMessages((publicMessages.size() != 0 ? publicMessages[publicMessages.size() - 1].id : 0), count);
}

void Account::receivePreviousFriendsMessages(int count) {
	qDebug() << ("Account::receivePreviousFriendsMessages()");
	twitter->receivePreviousFriendsMessages((friendsMessages.size() != 0 ? friendsMessages[friendsMessages.size() - 1].id : 0), count);
}

void Account::removePreviousFriendsMessages(int count) {
	qDebug() << ("Account::removePreviousFriendsMessages()");
	int oldItemsCount = friendsMessages.size();
	int newItemsCount = (oldItemsCount - 1) - (oldItemsCount - 1) % count;
	friendsMessages.resize(newItemsCount);
	emit friendsMessagesUpdated(friendsMessages, this);
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

void Account::receivePreviousOutboxMessages(int count) {
	qDebug() << ("Account::receivePreviousOutboxMessages()");
	twitter->receivePreviousOutboxMessages((outboxMessages.size() != 0 ? outboxMessages[outboxMessages.size() - 1].id : 0), count);
}

void Account::removePreviousOutboxMessages(int count) {
	qDebug() << ("Account::removePreviousOutboxMessages()");
	int oldItemsCount = outboxMessages.size();
	int newItemsCount = (oldItemsCount - 1) - (oldItemsCount - 1) % count;
	outboxMessages.resize(newItemsCount);
	emit outboxMessagesUpdated(outboxMessages, this);
}

void Account::saveMessages(QSettings &messagesCache) {
	Configuration *config = Configuration::getInstance();
	messagesCache.beginGroup("Message");
	lastMessage.save(messagesCache);
	messagesCache.endGroup();
	messagesCache.beginWriteArray("Friends");
	for (int i = 0; i < min(friendsMessages.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		friendsMessages[i].save(messagesCache);
	}
	messagesCache.endArray();
	messagesCache.beginWriteArray("Replies");
	for (int i = 0; i < min(replies.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		replies[i].save(messagesCache);
	}
	messagesCache.endArray();
	messagesCache.beginWriteArray("Public");
	for (int i = 0; i < min(publicMessages.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		publicMessages[i].save(messagesCache);
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
	messagesCache.beginWriteArray("Outbox");
	for (int i = 0; i < min(outboxMessages.size(), config->messagesPerPage); ++i) {
		messagesCache.setArrayIndex(i);
		outboxMessages[i].save(messagesCache);
	}
	messagesCache.endArray();
}

void Account::loadMessages(QSettings &messagesCache) {
	messagesCache.beginGroup("Message");
	lastMessage = Message::load(messagesCache, this);
	messagesCache.endGroup();
	int n = messagesCache.beginReadArray("Friends");
	friendsMessages.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		friendsMessages.push_back(Message::load(messagesCache, this));
	}
	messagesCache.endArray();
	n = messagesCache.beginReadArray("Replies");
	replies.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		replies.push_back(Message::load(messagesCache, this));
	}
	messagesCache.endArray();
	n = messagesCache.beginReadArray("Public");
	publicMessages.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		publicMessages.push_back(Message::load(messagesCache, this));
	}
	messagesCache.endArray();
	n = messagesCache.beginReadArray("Favorites");
	favorites.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		favorites.push_back(Message::load(messagesCache, this));
	}
	messagesCache.endArray();
	n = messagesCache.beginReadArray("Inbox");
	inboxMessages.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		inboxMessages.push_back(Message::load(messagesCache, this));
	}
	messagesCache.endArray();
	n = messagesCache.beginReadArray("Outbox");
	outboxMessages.clear();
	for (int i = 0; i < n; ++i) {
		messagesCache.setArrayIndex(i);
		outboxMessages.push_back(Message::load(messagesCache, this));
	}
	messagesCache.endArray();
	emit friendsMessagesUpdated(friendsMessages, this);
	emit repliesUpdated(replies, this);
	emit publicMessagesUpdated(publicMessages, this);
	emit favoritesUpdated(favorites, this);
	emit inboxMessagesUpdated(inboxMessages, this);
	emit outboxMessagesUpdated(inboxMessages, this);
	emit lastMessageReceived(lastMessage.text, this);
}

QString Account::serviceApiUrl() {
	QString url = (_serviceApiUrl == "" ? Services::options[type]["apiurl"] : _serviceApiUrl);
	if (useHttps) {
		if (url.startsWith("http://")) {
			url = "https://" + url.mid(7);
		}
	}
	return url;
}

QString Account::serviceBaseUrl() {
	QString url = (_serviceBaseUrl == "" ? Services::options[type]["baseurl"] : _serviceBaseUrl);
	if (useHttps) {
		if (url.startsWith("http://")) {
			url = "https://" + url.mid(7);
		}
	}
	return url;
}

QString Account::searchBaseUrl() {
	QString url = (_searchBaseUrl == "" ? Services::options[type]["searchbaseurl"] : _searchBaseUrl);
	if (useHttps) {
		if (url.startsWith("http://")) {
			url = "https://" + url.mid(7);
		}
	}
	return url;
}

QString Account::singleMessageUrl() {
	QString url = (_singleMessageUrl == "" ? Services::options[type]["singlemessageurl"] : _singleMessageUrl);
	if (useHttps) {
		if (url.startsWith("http://")) {
			url = "https://" + url.mid(7);
		}
	}
	return url;
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
	QVector<Message> messages = QwitTools::parseMessages(data, this);
	Configuration *config = Configuration::getInstance();
	int size = max(config->messagesPerPage, favorites.size());
	uint maxId = (favorites.size() ? favorites[0].id : 0);
	QVector<Message> newMessages = QwitTools::mergeMessages(favorites, messages);
	favorites = messages;
	if ((favorites.size() > size) && (favorites[0].id > maxId)) {
		favorites.resize(size);
	}
	emit favoritesUpdated(favorites, this);
	emit newMessagesReceived(newMessages, this);
}

void Account::addPreviousFavorites(const QByteArray &data) {
	qDebug() << ("Account::addPreviousFavorites()");
	QVector<Message> messages = QwitTools::parseMessages(data, this);
	if (messages.size()) {
		Configuration *config = Configuration::getInstance();
		int size = max(config->messagesPerPage, favorites.size());
		uint maxId = (favorites.size() ? favorites[0].id : 0);
		messages = QwitTools::mergeMessages(favorites, messages);
		if ((favorites[0].id > maxId) && (favorites.size() > size)) {
			favorites.resize(size);
		}
		emit favoritesUpdated(favorites, this);
		emit newMessagesReceived(messages, this);
	}
}

void Account::sendDirectMessage(const QString &username, const QString &message) {
	qDebug() << ("Account::sendDirectMessage()");
	twitter->sendDirectMessage(username, message);
}

void Account::favorMessage(const Message &message) {
	qDebug() << ("Account::favorMessage()");
	twitter->favorMessage(message.id);
}

void Account::unfavorMessage(const Message &message) {
	qDebug() << ("Account::unfavorMessage()");
	twitter->unfavorMessage(message.id);
}

void Account::destroyMessage(const Message &message) {
	qDebug() << ("Account::destroyMessage()");
	if (!message.directMessage) {
		twitter->destroyMessage(message.id);
	} else {
		twitter->destroyDirectMessage(message.id);
	}
}

void Account::directMessageSent(const QByteArray &data) {
}

void Account::messageFavored(const QByteArray &data) {
	qDebug() << ("Account::messageFavored()");
	QString errorRequest = QwitTools::parseError(data);
	if (errorRequest != "") {
		qDebug() << "Error favoring message: " << errorRequest;
	}
	Message message = QwitTools::parseMessage(data, this);
	message.favorited = true;
	uint messageId = message.id;
	for (int i = 0; i < friendsMessages.size(); ++i) {
		if (friendsMessages[i].id == messageId) {
			friendsMessages[i].favorited = true;
		}
	}
	for (int i = 0; i < replies.size(); ++i) {
		if (replies[i].id == messageId) {
			replies[i].favorited = true;
		}
	}
	for (int i = 0; i < publicMessages.size(); ++i) {
		if (publicMessages[i].id == messageId) {
			publicMessages[i].favorited = true;
		}
	}
	int index = -1;
	for (int i = 0; i < favorites.size(); ++i) {
		if (favorites[i].id == messageId) {
			favorites[i].favorited = true;
			index = i;
		}
	}
	if (index == -1) {
		favorites.push_front(message);
	}
	emit friendsMessagesUpdated(friendsMessages, this);
	emit repliesUpdated(replies, this);
	emit publicMessagesUpdated(publicMessages, this);
	emit favoritesUpdated(favorites, this);
}

void Account::messageUnfavored(const QByteArray &data) {
	qDebug() << ("Account::messageUnfavored()");
	QString errorRequest = QwitTools::parseError(data);
	if (errorRequest != "") {
		qDebug() << "Error unfavoring message: " << errorRequest;
	}
	Message message = QwitTools::parseMessage(data, this);
	uint messageId = message.id;
	message.favorited = false;
	for (int i = 0; i < friendsMessages.size(); ++i) {
		if (friendsMessages[i].id == messageId) {
			friendsMessages[i].favorited = false;
		}
	}
	for (int i = 0; i < replies.size(); ++i) {
		if (replies[i].id == messageId) {
			replies[i].favorited = false;
		}
	}
	for (int i = 0; i < publicMessages.size(); ++i) {
		if (publicMessages[i].id == messageId) {
			publicMessages[i].favorited = false;
		}
	}
	int index = -1;
	for (int i = 0; i < favorites.size(); ++i) {
		if (favorites[i].id == messageId) {
			favorites[i].favorited = false;
			index = i;
		}
	}
	if (index != -1) {
		favorites.erase(favorites.begin() + index);
	}
	emit friendsMessagesUpdated(friendsMessages, this);
	emit repliesUpdated(replies, this);
	emit publicMessagesUpdated(publicMessages, this);
	emit favoritesUpdated(favorites, this);
}

void Account::messageDestroyed(const QByteArray &data) {
	qDebug() << ("Account::messageDestroyed()");
	QString errorRequest = QwitTools::parseError(data);
	uint messageId = 0;
	if (errorRequest == "") {
		Message message = QwitTools::parseMessage(data, this);
		messageId = message.id;
	} else {
		return;
	}
	for (int i = 0; i < friendsMessages.size(); ++i) {
		if (friendsMessages[i].id == messageId) {
			friendsMessages.erase(friendsMessages.begin() + i);
		}
	}
	for (int i = 0; i < replies.size(); ++i) {
		if (replies[i].id == messageId) {
			replies.erase(replies.begin() + i);
		}
	}
	for (int i = 0; i < publicMessages.size(); ++i) {
		if (publicMessages[i].id == messageId) {
			publicMessages.erase(publicMessages.begin() + i);
		}
	}
	int index = -1;
	for (int i = 0; i < favorites.size(); ++i) {
		if (favorites[i].id == messageId) {
			favorites.erase(favorites.begin() + i);
		}
	}
	emit friendsMessagesUpdated(friendsMessages, this);
	emit repliesUpdated(replies, this);
	emit publicMessagesUpdated(publicMessages, this);
	emit favoritesUpdated(favorites, this);
}

void Account::directMessageDestroyed(const QByteArray &data) {
	qDebug() << ("Account::directMessageDestroyed()");
	QString errorRequest = QwitTools::parseError(data);
	uint messageId = 0;
	if (errorRequest == "") {
		Message message = QwitTools::parseDirectMessage(data, this);
		messageId = message.id;
	} else {
		return;
	}
	for (int i = 0; i < inboxMessages.size(); ++i) {
		if (inboxMessages[i].id == messageId) {
			inboxMessages.erase(inboxMessages.begin() + i);
		}
	}
	for (int i = 0; i < outboxMessages.size(); ++i) {
		if (outboxMessages[i].id == messageId) {
			outboxMessages.erase(outboxMessages.begin() + i);
		}
	}
	emit inboxMessagesUpdated(inboxMessages, this);
	emit outboxMessagesUpdated(outboxMessages, this);
}

#endif