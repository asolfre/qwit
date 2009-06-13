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
 *  QwitTools class declaration
 */

#ifndef QwitTools_h
#define QwitTools_h

#include "QwitHeaders.h"

#include "Status.h"

class QwitTools: public QObject {
	Q_OBJECT
	
private:
	static QRegExp urlRegExp;
	static QRegExp usernameRegExp;
	QMap<QString, int> monthes;
	static QwitTools *instance;
	QwitTools();
	static QwitTools* getInstance();
	QIcon _getToolButtonIcon(const QString &iconFileName);
	QString _formatDateTime(const QDateTime &time);
	QDateTime _dateFromString(QString date);
	QVector<Status> _parseStatuses(const QByteArray &data, Account *account);
	QVector<Status> _parseInboxMessages(const QByteArray &data, Account *account);
	Status _parseUser(const QByteArray &data, Account *account);
	Status _parseStatus(const QByteArray &data, Account *account);
	void _makeStatusesUnique(QVector<Status> &v);
	QString _prepareStatus(const QString &text, Account *account);
	QVector<Status> _mergeStatuses(QVector<Status> &statuses, QVector<Status> &receivedStatuses);
	
public:
	static QDateTime dateFromString(QString date);
	static QIcon getToolButtonIcon(const QString &iconFileName);
	static QString formatDateTime(const QDateTime &time);
	static QVector<Status> parseStatuses(const QByteArray &data, Account *account);
	static QVector<Status> parseInboxMessages(const QByteArray &data, Account *account);
	static Status parseUser(const QByteArray &data, Account *account);
	static Status parseStatus(const QByteArray &data, Account *account);
	static void makeStatusesUnique(QVector<Status> &statuses);
	static QString prepareStatus(const QString &text, Account *account);
	static QVector<Status> mergeStatuses(QVector<Status> &statuses, QVector<Status> &receivedStatuses);
};

void handleMessage(QtMsgType type, const char *msg);

#endif