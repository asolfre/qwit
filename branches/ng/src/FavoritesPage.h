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
 *  FavoritesPage class declaration
 */

#ifndef FavoritesPage_h
#define FavoritesPage_h

#include "QwitHeaders.h"

#include "AbstractPage.h"

class FavoritesPage: public AbstractPage {
	Q_OBJECT
	
public:
	FavoritesPage(QWidget* parent = 0);
	void updateSize();
	QString title();
	void update(Account *account = 0);
	
public slots:
	void updatePrevious();
	void removePrevious();
	bool updateAutomatically();
};

#endif
