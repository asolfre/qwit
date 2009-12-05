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
 *  User class implementation
 */

#ifndef User_cpp
#define User_cpp

#include "QwitTools.h"
#include "Configuration.h"
#include "UserpicsDownloader.h"

#include "User.h"

User::User(quint64 id, const QString &name, const QString &screen_name, const QString &location, const QString &description, const QString &profile_image_url, const QString &url, bool isProtected, int followers_count, const QString &profile_background_color, const QString &profile_text_color, const QString &profile_link_color, const QString &profile_sidebar_fill_color, const QString profile_sidebar_border_color, int friends_count, const QString &created_at, int favourites_count, int utc_offset, const QString &time_zone, const QString &profile_background_image_url, const QString &profile_background_tile, int statuses_count, bool notifications, bool geo_enabled, bool verified, bool following, Account *account)
{
    this->id = id;
    this->name = name;
    this->screen_name = screen_name;
    this->location = location;
    this->description = description;
    this->profile_image_url = profile_image_url;
    this->url = url;
    this->isProtected = isProtected;
    this->followers_count = followers_count;
    this->profile_background_color = profile_background_color;
    this->profile_text_color = profile_text_color;
    this->profile_link_color = profile_link_color;
    this->profile_sidebar_fill_color = profile_sidebar_fill_color;
    this->profile_sidebar_border_color = profile_sidebar_border_color;
    this->friends_count = friends_count;
    this->created_at = convertTimeString(created_at);
    this->favourites_count = favourites_count;
    this->utc_offset = utc_offset;
    this->time_zone = time_zone;
    this->profile_background_image_url = profile_background_image_url;
    this->profile_background_tile = profile_background_tile;
    this->statuses_count = statuses_count;
    this->notifications = notifications;
    this->geo_enabled = geo_enabled;
    this->verified = verified;
    this->following = following;
    this->userpicFilename = getImageFilename(profile_image_url);
    this->account = account;
    this->status = (Status*) 0;
}

void User::setStatus(Status *status) {
    this->status = status;
}

Status* User::getStatus() {
    return this->status;
}

QDateTime User::convertTimeString(QString timeString) {
    QDateTime time = QwitTools::dateFromString(timeString);
    time = QDateTime(time.date(), time.time(), Qt::UTC);
    return time;
}

QString User::getImageFilename(QString profileImageUrl) {
    QByteArray hash = QCryptographicHash::hash(profileImageUrl.toAscii(), QCryptographicHash::Md5);
    QString imageFileName = "";
    for (int i = 0; i < hash.size(); ++i) {
	unsigned char c = hash[i];
	c >>= 4;
	if (c < 10) {
	    c += '0';
	} else {
	    c += 'A' - 10;
	}
	imageFileName += (char)c;
	c = hash[i];
	c &= 15;
	if (c < 10) {
	    c += '0';
	} else {
	    c += 'A' - 10;
	}
	imageFileName += (char)c;
    }
    imageFileName = Configuration::CacheDirectory + imageFileName;
    UserpicsDownloader::getInstance()->download(profileImageUrl, imageFileName);
    return imageFileName;
}
#endif
