/*!
 *  $Id$
 *
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
QMap<QString, QMap<QString, QString> > Services::urlShorteners;

void Services::initialize() {
	QMap<QString, QString> twitterOptions;
	twitterOptions["title"] = "Twitter";
	twitterOptions["apiurl"] = "http://twitter.com";
	twitterOptions["baseurl"] = "http://twitter.com";
	twitterOptions["singlemessageurl"] = "http://twitter.com/%username/statuses/%messageid";
	twitterOptions["searchbaseurl"] = "http://search.twitter.com/search?q=";
	twitterOptions["friends"] = "/statuses/friends_timeline";
	twitterOptions["public"] = "/statuses/public_timeline";
	twitterOptions["replies"] = "/statuses/mentions";
	twitterOptions["favorites"] = "/favorites";
	twitterOptions["favor"] = "/favorites/create/";
	twitterOptions["unfavor"] = "/favorites/destroy/";
	twitterOptions["user"] = "/statuses/user_timeline/";
	twitterOptions["last"] = "/users/show/";
	twitterOptions["update"] = "/statuses/update";
	twitterOptions["destroy"] = "/statuses/destroy/";
	twitterOptions["inbox"] = "/direct_messages";
	twitterOptions["outbox"] = "/direct_messages/sent";
	twitterOptions["send"] = "/direct_messages/new";
	twitterOptions["destroydirectmessage"] = "/direct_messages/destroy/";
	twitterOptions["searchapiurl"] = "http://search.twitter.com";
	twitterOptions["search"] = "/search";
	options["twitter"] = twitterOptions;

	QMap<QString, QString> identicaOptions;
	identicaOptions["title"] = "Identi.ca";
	identicaOptions["apiurl"] = "http://identi.ca/api";
	identicaOptions["baseurl"] = "http://identi.ca";
	identicaOptions["singlemessageurl"] = "http://identi.ca/notice/%messageid";
	identicaOptions["searchbaseurl"] = "http://identi.ca/search/notice?q=";
	identicaOptions["friends"] = "/statuses/friends_timeline";
	identicaOptions["public"] = "/statuses/public_timeline";
	identicaOptions["replies"] = "/statuses/mentions";
	identicaOptions["favorites"] = "/favorites";
	identicaOptions["favor"] = "/favorites/create/";
	identicaOptions["unfavor"] = "/favorites/destroy/";
	identicaOptions["user"] = "/statuses/user_timeline/";
	identicaOptions["last"] = "/users/show/";
	identicaOptions["update"] = "/statuses/update";
	identicaOptions["destroy"] = "/statuses/destroy/";
	identicaOptions["inbox"] = "/direct_messages";
	identicaOptions["outbox"] = "/direct_messages/sent";
	identicaOptions["send"] = "/direct_messages/new";
	identicaOptions["destroydirectmessage"] = "/direct_messages/destroy/";
	options["identica"] = identicaOptions;

	QMap<QString, QString> customOptions;
	customOptions["title"] = "Custom";
	options["custom"] = customOptions;

	QMap<QString, QString> trimOptions;
	trimOptions["title"] = "tr.im";
	trimOptions["apiurl"] = "http://api.tr.im/api/trim_url.xml";
	trimOptions["requesttemplate"] = "?url=%url";
	trimOptions["responseregexp"] = "http://tr.im/\\w+";
	urlShorteners["trim"] = trimOptions;

	QMap<QString, QString> murlkzOptions;
	murlkzOptions["title"] = "murl.kz";
	murlkzOptions["apiurl"] = "http://api.murl.kz/basic";
	murlkzOptions["requesttemplate"] = "?api_key=26e4ab94e13d99a20771d4d2a73d202e&format=xml&url=%url";
	murlkzOptions["responseregexp"] = "http://murl.kz/\\w+";
	urlShorteners["murlkz"] = murlkzOptions;

	QMap<QString, QString> unuOptions;
	unuOptions["title"] = "u.nu";
	unuOptions["apiurl"] = "http://u.nu/unu-api-simple";
	unuOptions["requesttemplate"] = "?url=%url";
	unuOptions["responseregexp"] = "http://u.nu/\\w+";
	urlShorteners["unu"] = unuOptions;

	QMap<QString, QString> clckruOptions;
	unuOptions["title"] = "clck.ru";
	unuOptions["apiurl"] = "http://clck.ru/--";
	unuOptions["requesttemplate"] = "?url=%url";
	unuOptions["responseregexp"] = "http://clck.ru/\\w+";
	urlShorteners["clckru"] = unuOptions;

	QMap<QString, QString> uiopmeOptions;
	uiopmeOptions["title"] = "uiop.me";
	uiopmeOptions["apiurl"] = "http://uiop.me/api/newlink.php";
	uiopmeOptions["requesttemplate"] = "?u=%url";
	uiopmeOptions["responseregexp"] = "http://uiop.me/\\w+";
	urlShorteners["uiopme"] = uiopmeOptions;
}

#endif
