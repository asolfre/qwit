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
 *  Status class implementation
 */

#ifndef Status_cpp
#define Status_cpp

#include "QwitHeaders.h"

#include "Status.h"

Status::Status(uint id, const QString &status, const QString &username, const QString &userpicFilename, const QDateTime &time, Account *account) {
	this->id = id;
	this->status = status;
	this->username = username;
	this->userpicFilename = userpicFilename;
	this->time = time;
	this->account = account;
}

bool Status::operator<(const Status &x) const {
	return id > x.id;
}

bool Status::operator==(const Status &x) const {
	return id == x.id;
}

bool Status::operator!=(const Status &x) const {
	return id != x.id;
}

void Status::save(QSettings &messagesCache) {
	messagesCache.setValue("id", id);
	messagesCache.setValue("status", status);
	messagesCache.setValue("username", username);
	messagesCache.setValue("userpicFilename", userpicFilename);
	messagesCache.setValue("time", time);
}

Status Status::load(QSettings &messagesCache, Account *account) {
	return Status(
		messagesCache.value("id").toInt(),
		messagesCache.value("status").toString(),
		messagesCache.value("username").toString(),
		messagesCache.value("userpicFilename").toString(),
		messagesCache.value("time").toDateTime(),
		account
	);
}

#endif
