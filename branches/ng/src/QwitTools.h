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

#include "Message.h"

class QwitTools: public QObject {
	Q_OBJECT
	
private:
	static QRegExp urlRegExp;
	static QRegExp usernameRegExp;
	static QRegExp hashtagRegExp;
	QMap<QString, int> monthes;
	static QwitTools *instance;
	QwitTools();
	static QwitTools* getInstance();
	QIcon _getToolButtonIcon(const QString &iconFileName, bool active);
	QString _formatDateTime(const QDateTime &time);
	QDateTime _dateFromString(QString date);
	QVector<Message> _parseMessages(const QByteArray &data, Account *account);
	QVector<Message> _parseInboxMessages(const QByteArray &data, Account *account);
	QVector<Message> _parseOutboxMessages(const QByteArray &data, Account *account);
	Message _parseUser(const QByteArray &data, Account *account);
	Message _parseMessage(const QByteArray &data, Account *account);
	Message _parseDirectMessage(const QByteArray &data, Account *account);
	QString _parseError(const QByteArray &data);
	void _makeMessagesUnique(QVector<Message> &v);
	QString _prepareMessage(const QString &text, Account *account);
	QVector<Message> _mergeMessages(QVector<Message> &messages, QVector<Message> &receivedMessages);
	bool _isUrl(const QString &s);
	
public:
	static QDateTime dateFromString(QString date);
	static QIcon getToolButtonIcon(const QString &iconFileName, bool active = false);
	static QString formatDateTime(const QDateTime &time);
	static QVector<Message> parseMessages(const QByteArray &data, Account *account);
	static QVector<Message> parseInboxMessages(const QByteArray &data, Account *account);
	static QVector<Message> parseOutboxMessages(const QByteArray &data, Account *account);
	static Message parseUser(const QByteArray &data, Account *account);
	static Message parseMessage(const QByteArray &data, Account *account);
	static Message parseDirectMessage(const QByteArray &data, Account *account);
	static QString parseError(const QByteArray &data);
	static void makeMessagesUnique(QVector<Message> &messages);
	static QString prepareMessage(const QString &text, Account *account);
	static QVector<Message> mergeMessages(QVector<Message> &messages, QVector<Message> &receivedMessages);
	static bool isUrl(const QString &s);
};

void handleMessage(QtMsgType type, const char *msg);

#endif