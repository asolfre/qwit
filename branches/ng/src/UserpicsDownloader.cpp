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
 *  UserpicsDownloader class implementation
 */

#ifndef UserpicsDownloader_cpp
#define UserpicsDownloader_cpp

#include "QwitHeaders.h"

#include "UserpicsDownloader.h"

UserpicsDownloader* UserpicsDownloader::instance = 0;

UserpicsDownloader* UserpicsDownloader::getInstance() {
	if (!instance) {
		instance = new UserpicsDownloader();
	}
	return instance;
}

UserpicsDownloader::UserpicsDownloader() {
	connect(&http, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
}
	
void UserpicsDownloader::startDownload() {
	QPair<QString, QString> item = queue.front();
	file.setFileName(item.second);
	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}
	QUrl url(item.first);
	if (proxyAddress != "") {
		http.setProxy(proxyAddress, proxyPort, proxyUsername, proxyPassword);
	} else {
		http.setProxy("", 0);
	}
	http.setHost(url.host(), url.port(80));
	http.get(QUrl::toPercentEncoding(url.path(), "/"), &file);
	emit stateChanged(tr("Downloading: %1").arg(item.first));
}
	
void UserpicsDownloader::download(const QString &url, const QString &filename) {
	QFileInfo fi = QFileInfo(filename);
	if (fi.exists() && (fi.size() > 0)) return;
	queue.enqueue(qMakePair(url, filename));
	if (queue.size() == 1) {
		startDownload();
	}
}

void UserpicsDownloader::httpDone(bool error) {
	if (error) {
		emit stateChanged(tr("Error while downloading userpic: %1").arg(http.errorString()));
	}
	QPair<QString, QString> item = queue.dequeue();
	file.close();
	emit userpicDownloaded();
	emit stateChanged(tr("Downloaded: %1").arg(item.first));
	if (queue.size()) {
		startDownload();
	}
}

void UserpicsDownloader::useProxy(const QString &address, int port, const QString &username, const QString &password) {
	proxyAddress = address;
	proxyPort = port;
	proxyUsername = username;
	proxyPassword = password;
}

void UserpicsDownloader::dontUseProxy() {
	proxyAddress = "";
}

#endif