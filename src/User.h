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
 *  User class declaration
 */

#ifndef User_h
#define User_h

#include "QwitHeaders.h"
#include "Status.h"
#include "Account.h"

class User
{
private:
    QDateTime convertTimeString(QString timeString);
    QString getImageFilename(QString profileImageUrl);

public:
    quint64 id;
    QString name;
    QString screen_name;
    QString location;
    QString description;
    QString profile_image_url;
    QString url;
    bool isProtected;
    int followers_count;
    QString profile_background_color;
    QString profile_text_color;
    QString profile_link_color;
    QString profile_sidebar_fill_color;
    QString profile_sidebar_border_color;
    int friends_count;
    QDateTime created_at;
    int favourites_count;
    int utc_offset;
    QString time_zone;
    QString profile_background_image_url;
    QString profile_background_tile;
    int statuses_count;
    bool notifications;
    bool geo_enabled;
    bool verified;
    bool following;

    QString userpicFilename;
    Account *account;
    Status *status;

    User() {}
    User(quint64 id, const QString &name, const QString &screen_name, const QString &location, const QString &description, const QString &profile_image_url, const QString &url, bool isProtected, int followers_count, const QString &profile_background_color, const QString &profile_text_color, const QString &profile_link_color, const QString &profile_sidebar_fill_color, const QString profile_sidebar_border_color, int friends_count, const QString &created_at, int favourites_count, int utc_offset, const QString &time_zone, const QString &profile_background_image_url, const QString &profile_background_tile, int statuses_count, bool notifications, bool geo_enabled, bool verified, bool following, Account *account);
    void setStatus(Status *status);
    Status* getStatus();
};

#endif
