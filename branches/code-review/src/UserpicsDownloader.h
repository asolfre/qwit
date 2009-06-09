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
 *  UserpicsDownloader class declaration
 */

#ifndef UserpicsDownloader_h
#define UserpicsDownloader_h

#include "QwitHeaders.h"

class UserpicsDownloader: public QObject {
	Q_OBJECT
	
	QQueue<QPair<QString, QString> > queue;
	QFile file;
	QHttp http;
	QString proxyAddress;
	int proxyPort;
	QString proxyUsername;
	QString proxyPassword;
	static UserpicsDownloader* instance;
	UserpicsDownloader();
	
public:

	static UserpicsDownloader* getInstance();
	void startDownload();
	void download(const QString &url, const QString &filename);
	void useProxy(const QString &address, int port, const QString &username, const QString &password);
	void dontUseProxy();

public slots:

	void httpDone(bool error);
	
signals:
	
	void stateChanged(const QString &);
	void userpicDownloaded();
};

#endif
