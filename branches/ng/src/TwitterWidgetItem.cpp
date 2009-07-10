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
 *  TwitterWidgetItem class implementation
 */

#ifndef TwitterWidgetItem_cpp
#define TwitterWidgetItem_cpp

#include "QwitHeaders.h"
#include "QwitTools.h"

#include "TwitterWidgetItem.h"

void TwitterWidgetItem::loadUserpic() {
	QPixmap pixmap(message.userpicFilename);
	if (!pixmap.isNull()) {
		userpicLabel->setPixmap(pixmap.scaled(ICON_SIZE, ICON_SIZE));
	}
	userpicLabel->resize(ICON_SIZE, ICON_SIZE);
}

TwitterWidgetItem::TwitterWidgetItem() {
	messageTextBrowser = 0;
	userpicLabel = 0;
	signLabel = 0;
	replyButton = 0;
	favorButton = 0;
	destroyButton = 0;
	retweetButton = 0;
	unfollowButton = 0;
	followButton = 0;
	directMessageButton = 0;
}

TwitterWidgetItem::~TwitterWidgetItem() {
	delete messageTextBrowser;
	delete userpicLabel;
	delete signLabel;
	delete replyButton;
	delete favorButton;
	delete unfollowButton;
	delete followButton;
	delete directMessageButton;
	if (retweetButton) {
		delete retweetButton;
	}
	if (destroyButton) {
		delete destroyButton;
	}
}

#endif