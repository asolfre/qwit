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
	QMap<int, QString> sendStatusRequests;
	QMap<int, QString> receiveFriendsStatusesRequests;
	QMap<int, QString> receiveRepliesRequests;
	QMap<int, QString> receivePublicStatusesRequests;
	QMap<int, QString> receiveFavoritesRequests;
	QMap<int, QString> receiveInboxMessagesRequests;
	QMap<int, QString> receiveLastStatusRequests;
	QMap<int, QString> receivePreviousFriendsStatusesRequests;
	QMap<int, QString> receivePreviousRepliesRequests;
	QMap<int, QString> receivePreviousPublicStatusesRequests;
	QMap<int, QString> receivePreviousFavoritesRequests;
	QMap<int, QString> receivePreviousInboxMessagesRequests;
	QMap<int, QString> sendDirectMessageRequests;
	QMap<int, QString> favorStatusRequests;
	QMap<int, QString> unfavorStatusRequests;
	void setupProxy();

public:

	Twitter(Account *account);
	void receiveFriendsStatuses(uint lastStatusId, int count);
	void receiveReplies(uint lastStatusId, int count);
	void receivePublicStatuses(uint lastStatusId, int count);
	void receiveInboxMessages(uint lastStatusId, int count);
	void receiveFavorites();
	void receiveLastStatus();
	void receivePreviousFriendsStatuses(uint lastStatusId, int count);
	void receivePreviousReplies(uint lastStatusId, int count);
	void receivePreviousPublicStatuses(uint lastStatusId, int count);
	void receivePreviousInboxMessages(uint lastStatusId, int count);
	void receivePreviousFavorites(int page);
	void sendStatus(const QString &status, uint inReplyToStatusId);
	void sendDirectMessage(const QString &username, const QString &message);
	void favorStatus(uint statusId);
	void unfavorStatus(uint statusId);
	void abort();

signals:

	void statusSent();
	void friendsStatusesReceived(const QByteArray &);
	void repliesReceived(const QByteArray &);
	void publicStatusesReceived(const QByteArray &);
	void lastStatusReceived(const QByteArray &);
	void favoritesReceived(const QByteArray &);
	void inboxMessagesReceived(const QByteArray &);
	void statusSent(const QByteArray &);
	void directMessageSent(const QByteArray &);
	void statusFavored(const QByteArray &);
	void statusUnfavored(const QByteArray &);
	void previousFriendsStatusesReceived(const QByteArray &);
	void previousRepliesReceived(const QByteArray &);
	void previousPublicStatusesReceived(const QByteArray &);
	void previousFavoritesReceived(const QByteArray &);
	void previousInboxMessagesReceived(const QByteArray &);
	
public slots:

	void requestStarted(int id);
	void requestFinished(int id, bool error);
	
};

#endif
