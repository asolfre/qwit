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
 *  Status class implementation
 */

#ifndef Status_cpp
#define Status_cpp

#include "QwitTools.h"

#include "Status.h"

Status::Status(quint64 id, const QString &created_at, const QString &text, const QString &source, bool truncated, quint64 in_reply_to_status_id, quint64 in_reply_to_user_id, bool favorited, const QString &in_reply_to_screen_name)
{
    this->id = id;
    this->created_at = convertTimeString(created_at);
    this->text = text;
    this->source = source;
    this->truncated = truncated;
    this->in_reply_to_status_id = in_reply_to_status_id;
    this->in_reply_to_user_id = in_reply_to_user_id;
    this->favorited = favorited;
    this->in_reply_to_screen_name = in_reply_to_screen_name;
}

QDateTime Status::convertTimeString(QString timeString) {
    QDateTime time = QwitTools::dateFromString(timeString);
    time = QDateTime(time.date(), time.time(), Qt::UTC);
    return time;
}
#endif
