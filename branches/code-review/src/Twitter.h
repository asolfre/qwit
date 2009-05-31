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

#ifndef Twitter_h
#define Twitter_h

#include <QObject>
#include <QHttp>
#include <QString>
#include <QBuffer>
#include <QSslError>

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

public:

	Twitter(Account *account);
	void sendStatus(QString status, QString replyId);
	void receiveFriendsStatuses(int lastStatusId, int count);
	void receiveReplies(int lastStatusId, int count);
	void receivePublicStatuses(int lastStatusId, int count);
	void abort();

signals:

	void statusSent();
	void friendsStatusesReceived(const QByteArray &);
	void repliesReceived(const QByteArray &);
	void publicStatusesReceived(const QByteArray &);
	
public slots:

	void requestStarted(int id);
	void requestFinished(int id, bool error);
	
};

#endif
