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
 *  Account class declaration
 */

#ifndef Account_h
#define Account_h

#include "QwitHeaders.h"

#include "Twitter.h"
#include "Status.h"

class Twitter;
class Status;

class Account: public QObject {
	Q_OBJECT
	
private:
	QString _serviceBaseUrl;
	QString _serviceApiUrl;
	
public:
	int id;
	QString type;
	QString username;
	QString password;
	Status lastStatus;
	Twitter *twitter;
	int remainingRequests;
	
	QVector<Status> friendsStatuses;
	QVector<Status> replies;
	QVector<Status> publicStatuses;
	QVector<Status> favorites;
	QVector<Status> inboxMessages;
	
	Account();
	Account(const QString &type, const QString &username, const QString &password);
	void saveMessages(QSettings &messagesCache);
	void loadMessages(QSettings &messagesCache);
	QString serviceApiUrl();
	QString serviceBaseUrl();
	void setRemainingRequests(int remainingRequests);

public slots:
	void addFriendsStatuses(const QByteArray &data);
	void addReplies(const QByteArray &data);
	void addPublicStatuses(const QByteArray &data);
	void addFavorites(const QByteArray &data);
	void addPreviousFavorites(const QByteArray &data);
	void addInboxMessages(const QByteArray &data);
	void updateLastStatus(const QByteArray &data);
	void updateLastStatus();
	void sendStatus(const QString &status, int inReplyToStatusId);
	void statusSent(const QByteArray &data);
	void directMessageSent(const QByteArray &data);
	void statusFavorChanged(const QByteArray &data);
	void receivePublicStatuses(int count);
	void receiveFriendsStatuses(int count);
	void receiveReplies(int count);
	void receiveFavorites();
	void receiveInboxMessages(int count);
	void receivePreviousPublicStatuses(int count);
	void receivePreviousFriendsStatuses(int count);
	void receivePreviousReplies(int count);
	void receivePreviousFavorites();
	void receivePreviousInboxMessages(int count);
	void removePreviousFriendsStatuses(int count);
	void removePreviousReplies(int count);
	void removePreviousFavorites();
	void removePreviousInboxMessages(int count);
	void sendDirectMessage(const QString &username, const QString &message);
	void favorStatus(const Status &status);
	void unfavorStatus(const Status &status);
	
signals:
	void friendsStatusesUpdated(const QVector<Status> &, Account *);
	void repliesUpdated(const QVector<Status> &, Account *);
	void publicStatusesUpdated(const QVector<Status> &, Account *);
	void favoritesUpdated(const QVector<Status> &, Account *);
	void lastStatusReceived(const QString &, Account *);
	void newStatusesReceived(const QVector<Status> &, Account *);
	void inboxMessagesUpdated(const QVector<Status> &, Account *);
	void remainingRequestsUpdated(int, Account *);
	void previousFriendsStatusesReceived();
	void previousRepliesReceived();
	void previousPublicStatusesReceived();
	void previousFavoritesReceived();
	void previousInboxMessagesReceived();
};

#endif
