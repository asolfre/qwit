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
 *  Services class implementation
 */

#ifndef Services_cpp
#define Services_cpp

#include "QwitHeaders.h"

#include "Services.h"

QMap<QString, QMap<QString, QString> > Services::options;

void Services::initialize() {
	QMap<QString, QString> twitterOptions;
	twitterOptions["apiurl"] = "http://twitter.com";
	twitterOptions["baseurl"] = "http://twitter.com";
	twitterOptions["friends"] = "/statuses/friends_timeline";
	twitterOptions["public"] = "/statuses/public_timeline";
	twitterOptions["replies"] = "/statuses/mentions";
	twitterOptions["favorites"] = "/favorites";
	twitterOptions["user"] = "/statuses/user_timeline/";
	twitterOptions["last"] = "/users/show";
	twitterOptions["update"] = "/statuses/update";
	twitterOptions["inbox"] = "/direct_messages";
	twitterOptions["outbox"] = "/direct_messages/sent";
	options["twitter"] = twitterOptions;

	QMap<QString, QString> identicaOptions;
	identicaOptions["apiurl"] = "http://identi.ca/api";
	identicaOptions["baseurl"] = "http://identi.ca";
	identicaOptions["friends"] = "/statuses/friends_timeline";
	identicaOptions["public"] = "/statuses/public_timeline";
	identicaOptions["replies"] = "/statuses/mentions";
	identicaOptions["favorites"] = "/favorites";
	identicaOptions["user"] = "/statuses/user_timeline/";
	identicaOptions["last"] = "/users/show";
	identicaOptions["update"] = "/statuses/update";
	identicaOptions["inbox"] = "/direct_messages";
	identicaOptions["outbox"] = "/direct_messages/sent";
	options["identica"] = identicaOptions;
}

#endif
