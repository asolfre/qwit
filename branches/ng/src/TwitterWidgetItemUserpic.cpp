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
 *  TwitterWidgetItemUserpic class implementation
 */

#ifndef TwitterWidgetItemUserpic_cpp
#define TwitterWidgetItemUserpic_cpp

#include "QwitHeaders.h"
#include "QwitTools.h"

#include "TwitterWidgetItemUserpic.h"

TwitterWidgetItemUserpic::TwitterWidgetItemUserpic(QWidget *parent): QLabel(parent) {
	setMouseTracking(true);
}

void TwitterWidgetItemUserpic::enterEvent(QEvent *event) {
	emit mouseEntered();
}

void TwitterWidgetItemUserpic::leaveEvent(QEvent *event) {
	QPoint pos = mapFromGlobal(QCursor::pos());
	if ((pos.x() < 0) || (pos.x() >= width()) || (pos.y() < 0) || (pos.y() >= height())) {
		emit mouseLeft();
	}
}


#endif
