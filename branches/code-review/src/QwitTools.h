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

#include <QIcon>
#include <QString>
#include <QDateTime>
#include <QMap>

#include "Status.h"

class QwitTools: public QObject {
	Q_OBJECT
	
private:
	QMap<QString, int> monthes;
	static QwitTools *instance;
	QwitTools();
	static QwitTools* getInstance();
	QIcon _getToolButtonIcon(const QString &iconFileName);
	QString _formatDateTime(const QDateTime &time);
	QDateTime _dateFromString(QString date);
	QVector<Status> _parseStatuses(const QByteArray &data);
	Status _parseUser(const QByteArray &data);
	Status _parseStatus(const QByteArray &data);
	void _log(const QString &message);
	
public:
	static QDateTime dateFromString(QString date);
	static QIcon getToolButtonIcon(const QString &iconFileName);
	static QString formatDateTime(const QDateTime &time);
	static QVector<Status> parseStatuses(const QByteArray &data);
	static Status parseUser(const QByteArray &data);
	static Status parseStatus(const QByteArray &data);
	static void log(const QString &message);
};

#endif