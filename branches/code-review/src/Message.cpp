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
 *  Message class implementation
 */

#ifndef Message_cpp
#define Message_cpp

#include "QwitHeaders.h"

#include "Message.h"

Message::Message(uint id, const QString &text, const QString &username, const QString &userpicFilename, const QDateTime &time, bool favorited, Account *account, const QString &source, uint inReplyToMessageId, const QString &inReplyToUsername, bool directMessage) {
	this->id = id;
	this->text = text;
	this->username = username;
	this->userpicFilename = userpicFilename;
	this->time = time;
	this->account = account;
	this->favorited = favorited;
	this->source = source;
	this->inReplyToMessageId = inReplyToMessageId;
	this->inReplyToUsername = inReplyToUsername;
	this->directMessage = directMessage;
}

bool Message::operator<(const Message &x) const {
	return id > x.id;
}

bool Message::operator==(const Message &x) const {
	return id == x.id;
}

bool Message::operator!=(const Message &x) const {
	return id != x.id;
}

void Message::save(QSettings &messagesCache) {
	messagesCache.setValue("id", id);
	messagesCache.setValue("text", text);
	messagesCache.setValue("username", username);
	messagesCache.setValue("userpicFilename", userpicFilename);
	messagesCache.setValue("time", time);
	messagesCache.setValue("favorited", favorited);
	messagesCache.setValue("source", source);
	messagesCache.setValue("inReplyToMessageId", inReplyToMessageId);
	messagesCache.setValue("inReplyToUsername", inReplyToUsername);
	messagesCache.setValue("directMessage", directMessage);
}

Message Message::load(QSettings &messagesCache, Account *account) {
	return Message(
		messagesCache.value("id").toInt(),
		messagesCache.value("text").toString(),
		messagesCache.value("username").toString(),
		messagesCache.value("userpicFilename").toString(),
		messagesCache.value("time").toDateTime(),
		messagesCache.value("favorited").toBool(),
		account,
		messagesCache.value("source").toString(),
		messagesCache.value("inReplyToMessageId").toUInt(),
		messagesCache.value("inReplyToUsername").toString(),
		messagesCache.value("directMessage").toBool()
	);
}

#endif
