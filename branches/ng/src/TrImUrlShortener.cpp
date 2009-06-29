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
 *  TrImUrlShortener class implementation
 */

#ifndef TrImUrlShortener_cpp
#define TrImUrlShortener_cpp

#include "QwitHeaders.h"

#include "TrImUrlShortener.h"
#include "Configuration.h"

TrImUrlShortener::TrImUrlShortener() {
	http = new QHttp(this);
	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
}

void TrImUrlShortener::shorten(const QString &url) {
	currentUrl = url;

	QUrl trImUrl("http://api.tr.im/api/trim_url.xml");

	Configuration *config = Configuration::getInstance();
	if (config->useProxy) {
		http->setProxy(config->proxyAddress, config->proxyPort, config->proxyUsername, config->proxyPassword);
	} else {
		http->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
	}

	if(trImUrl.toString().indexOf("https") == 0) {
	    http->setHost(trImUrl.host(), QHttp::ConnectionModeHttps, trImUrl.port(443));
    } else {
        http->setHost(trImUrl.host(), QHttp::ConnectionModeHttp, trImUrl.port(80));
    }

	buffer.open(QIODevice::WriteOnly);
	
	requestId = http->get(trImUrl.path() + "?url=" + QUrl::toPercentEncoding(url), &buffer);
}

void TrImUrlShortener::requestFinished(int id, bool error) {
	if (id == requestId) {
		QString shortenedUrl = currentUrl;
		if (!error && (http->lastResponse().statusCode() == 200)) {
			qDebug() << ("TrImUrlShortener::requestFinished() " + QString::number(id) + " " + currentUrl);
			QDomDocument document;
			document.setContent(buffer.data());

			QDomElement root = document.documentElement();
			
			if (root.tagName() == "trim") {
				QDomNode node = root.firstChild();
				while (!node.isNull()) {
					if (node.toElement().tagName() == "url") {
						shortenedUrl = node.toElement().text();
					}
					node = node.nextSibling();
				}
			}
		} else {
			qDebug() << ("TrImUrlShortener::requestFinished() " + QString::number(id) + " error " + QString::number(http->lastResponse().statusCode()) + " " + http->lastResponse().reasonPhrase());
		}
		emit urlShortened(shortenedUrl);
	}
}

#endif
