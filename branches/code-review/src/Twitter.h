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
 *  Twitter class declaration
 */

#ifndef Twitter_h
#define Twitter_h

#include "QwitHeaders.h"

#include "Account.h"

class Account;

class Twitter: public QObject {
	Q_OBJECT
	
	QBuffer buffer;
	QHttp *http;
	Account *account;
	QMap<int, QString> sendMessageRequests;
	QMap<int, QString> receiveFriendsMessagesRequests;
	QMap<int, QString> receiveRepliesRequests;
	QMap<int, QString> receivePublicMessagesRequests;
	QMap<int, QString> receiveFavoritesRequests;
	QMap<int, QString> receiveInboxMessagesRequests;
	QMap<int, QString> receiveOutboxMessagesRequests;
	QMap<int, QString> receiveLastMessageRequests;
	QMap<int, QString> receivePreviousFriendsMessagesRequests;
	QMap<int, QString> receivePreviousRepliesRequests;
	QMap<int, QString> receivePreviousPublicMessagesRequests;
	QMap<int, QString> receivePreviousFavoritesRequests;
	QMap<int, QString> receivePreviousInboxMessagesRequests;
	QMap<int, QString> receivePreviousOutboxMessagesRequests;
	QMap<int, QString> sendDirectMessageRequests;
	QMap<int, QString> favorMessageRequests;
	QMap<int, QString> unfavorMessageRequests;
	QMap<int, QString> destroyMessageRequests;
	QMap<int, QString> destroyDirectMessageRequests;
	void setupProxy();

public:

	Twitter(Account *account);
	void receiveFriendsMessages(uint lastMessageId, int count);
	void receiveReplies(uint lastMessageId, int count);
	void receivePublicMessages(uint lastMessageId, int count);
	void receiveInboxMessages(uint lastMessageId, int count);
	void receiveOutboxMessages(uint lastMessageId, int count);
	void receiveFavorites();
	void receiveLastMessage();
	void receivePreviousFriendsMessages(uint lastMessageId, int count);
	void receivePreviousReplies(uint lastMessageId, int count);
	void receivePreviousPublicMessages(uint lastMessageId, int count);
	void receivePreviousInboxMessages(uint lastMessageId, int count);
	void receivePreviousOutboxMessages(uint lastMessageId, int count);
	void receivePreviousFavorites(int page);
	void sendMessage(const QString &message, uint inReplyToMessageId);
	void sendDirectMessage(const QString &username, const QString &message);
	void favorMessage(uint messageId);
	void unfavorMessage(uint messageId);
	void destroyMessage(uint messageId);
	void destroyDirectMessage(uint messageId);
	void abort();

signals:

	void messageSent();
	void friendsMessagesReceived(const QByteArray &);
	void repliesReceived(const QByteArray &);
	void publicMessagesReceived(const QByteArray &);
	void lastMessageReceived(const QByteArray &);
	void favoritesReceived(const QByteArray &);
	void inboxMessagesReceived(const QByteArray &);
	void outboxMessagesReceived(const QByteArray &);
	void messageSent(const QByteArray &);
	void directMessageSent(const QByteArray &);
	void messageFavored(const QByteArray &);
	void messageUnfavored(const QByteArray &);
	void messageDestroyed(const QByteArray &);
	void directMessageDestroyed(const QByteArray &);
	void previousFriendsMessagesReceived(const QByteArray &);
	void previousRepliesReceived(const QByteArray &);
	void previousPublicMessagesReceived(const QByteArray &);
	void previousFavoritesReceived(const QByteArray &);
	void previousInboxMessagesReceived(const QByteArray &);
	void previousOutboxMessagesReceived(const QByteArray &);
	
public slots:

	void requestStarted(int id);
	void requestFinished(int id, bool error);
	
};

#endif
