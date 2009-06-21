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
 *  TrImUrlShortener class declaration
 */

#ifndef TrImUrlShortener_h
#define TrImUrlShortener_h

#include "QwitHeaders.h"

#include "UrlShortener.h"

class TrImUrlShortener: public UrlShortener {
	Q_OBJECT

	QHttp *http;
	QString currentUrl;
	QBuffer buffer;
	int requestId;
	
public:
	TrImUrlShortener();
	
public slots:

	void shorten(const QString &url);
	void requestFinished(int id, bool error);
	
};

#endif
