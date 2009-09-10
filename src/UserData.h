/*!
 *  $Id$
 *
 *  @file
 *  @author Thomas Salm <tosate@googlemail.com>
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
 *  UserData class declaration
 *
 */

#ifndef USERDATA_H
#define USERDATA_H

#include <QDateTime>

#include "QwitHeaders.h"

class Account;

class UserData
{
public:
    Account *account;
    QString screenName;
    QString time;
    quint64 messageId;
    QString lastMessage;
    quint64 inReplyToMessageId;
    QString inReplyToUsername;
    bool favorited;
//public:
//    UserData(Account *account, QString screenName, QDateTime time, quint64 messageId, QString lastMessage, quint64 inReplyToMessageId, QString inReplyToUsername, bool favorited);
};

#endif // USERDATA_H
