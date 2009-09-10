/*!
 *  $Id$
 *
 *  @file
 *  @author Thomas Salm <tosate@googlemail.com>
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
 *  UserMgmtWidgetItem class implementation
 *
 */

#ifndef UserMgmtWidgetItem_cpp
#define UserMgmtWidgetItem_cpp

#include <QDesktopServices>
#include <QScrollBar>

#include "UserMgmtWidgetItem.h"
#include "QwitTools.h"
//#include "TwitterWidget.h"

//UserMgmtWidgetItem::UserMgmtWidgetItem(QWidget *parent, const QString username, const QString iconFileName, const int category, const QString statusText, uint messageId, const QDateTime &time, const uint replyStatusId, const QString &serviceBaseUrl)
//{
//    this->parent = parent;
//    this->status = new QTextBrowser(parent);
//    this->status->setHtml(TwitterWidgetItem::prepare(statusText,replyStatusId, serviceBaseUrl));
//    this->status->setReadOnly(true);
//    this->status->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    this->status->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    this->status->setFrameShape(QFrame::NoFrame);
//    this->status->setOpenExternalLinks(true);
//
//    QFont font = status->document()->defaultFont();
//    font.setFamily("Verdana");
//    this->status->document()->setDefaultFont(font);
//
//    this->username = username;
//    this->category = category;
//    this->icon = new QLabel(parent);
//    this->messageId = messageId;
//    this->time = time;
//
//    this->sign = new QLabel(parent);
//    this->sign->setAlignment(Qt::AlignRight);
//    this->sign->setOpenExternalLinks(true);
//
//    this->ctrl = new QLabel(parent);
//    this->ctrl->setAlignment(Qt::AlignRight);
//    this->ctrl->setOpenExternalLinks(true);
//
//    this->iconFileName = iconFileName;
//
//    QDesktopServices::setUrlHandler("unfollow", parent, "unfollowClicked");
//    QDesktopServices::setUrlHandler("follow", parent, "followClicked");
//    QDesktopServices::setUrlHandler("block", parent, "blockClicked");
//    QDesktopServices::setUrlHandler("unblock", parent, "unblockClicked");
//}

UserMgmtWidgetItem::UserMgmtWidgetItem(UserData userData, QWidget *parent, int widgetType)
{
    qDebug() << ("UserMgmtWidgetItem::UserMgmtWidgetItem()");
    this->parent = parent;
    this->widgetType = widgetType;

    this->userData = userData;
    this->userDataTextBrowser = new QTextBrowser(parent);
    this->userDataTextBrowser->setHtml(QwitTools::prepareMessage(userData.lastMessage, userData.account));
    this->userDataTextBrowser->setReadOnly(true);
    this->userDataTextBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->userDataTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->userDataTextBrowser->setFrameShape(QFrame::NoFrame);
    this->userDataTextBrowser->setOpenExternalLinks(true);

    this->userpicLabel = new QLabel(parent);
    this->loadUserpic();
    this->signLabel = new QLabel(parent);
    this->signLabel->setAlignment(Qt::AlignRight);
    this->signLabel->setOpenExternalLinks(true);
    this->signLabel->setStyleSheet("a{text-decoration:none;}");

    // add Buttons

    this->userDataTextBrowser->show();
    this->userpicLabel->show();
    this->signLabel->show();
}

UserMgmtWidgetItem::~UserMgmtWidgetItem()
{
    qDebug() << ("UserMgmtWidgetItem::~UserMgmtWidgetItem()");
}

//void UserMgmtWidgetItem::show()
//{
//    status->show();
//    icon->show();
//    sign->show();
//    ctrl->show();
//}
//
//void UserMgmtWidgetItem::hide()
//{
//    status->hide();
//    icon->hide();
//    sign->hide();
//    ctrl->hide();
//}

void UserMgmtWidgetItem::loadUserpic()
{
	QPixmap pixmap(iconFileName);
	if (!pixmap.isNull()) {
		userpicLabel->setPixmap(pixmap.scaled(UserMgmtWidgetItem::IconSize, UserMgmtWidgetItem::IconSize));
	}
	userpicLabel->resize(UserMgmtWidgetItem::IconSize, UserMgmtWidgetItem::IconSize);
}

int UserMgmtWidgetItem::update(int top, bool odd)
{
    // calculate the size of the status element
    int messageItemWidth = parent->width() - (UserMgmtWidgetItem::IconSize + 5 * UserMgmtWidgetItem::Margin);
    QFontMetrics fontMetrics(userDataTextBrowser->font());
    int messageItemHeight = fontMetrics.boundingRect(0, 0, messageItemWidth, 1000, Qt::AlignTop | Qt::TextWordWrap, userDataTextBrowser->toPlainText()).height() + 5;

    if(messageItemHeight < UserMgmtWidgetItem::IconSize)
    {
	messageItemHeight = UserMgmtWidgetItem::IconSize;
    }

    // positioning and scaling of the status element
    this->topPosition = top;
    userDataTextBrowser->move(UserMgmtWidgetItem::IconSize + 2 * UserMgmtWidgetItem::Margin, top + UserMgmtWidgetItem::Margin);
    userDataTextBrowser->resize(messageItemWidth, messageItemHeight);
    messageItemHeight += userDataTextBrowser->verticalScrollBar()->maximum() - userDataTextBrowser->verticalScrollBar()->minimum();
    userDataTextBrowser->resize(messageItemWidth, messageItemHeight);
    // positioning of the icon
    userpicLabel->move(UserMgmtWidgetItem::Margin, top + UserMgmtWidgetItem::Margin);

    QString messageIdText(QString::number(userData.messageId));

    QString tablew = "";
    tablew.setNum(messageItemWidth + 70);
    signLabel->setText("<table border=\"0\" width=\"" + tablew + "\" cellpadding=\"0\" cellspacing=\"0\">"
			+ "<tr valign=\"top\"><td width=\"50%\">"
			+ "<a href=\"twitter://user/username\" style=\"font-weight:bold;text-decoration:none;font-size:small\">" + userData.screenName + "</a>"
			+ "</td>"
			+ "<td width=\"50%\">"
			+ "<p align=\"right\" style=\"UserMgmtWidgetItem::Margin-right:20px\">"
			+ "<a href=\"http://twitter.com/username/statuses/messageIdText\" style=\"font-size:small;text-decoration:none\">"
			+ "TwitterWidget::formatDateTime(this->time)</a>"
			+ "</p>"
			+ "</td>"
			+ "</tr></table>");

    signLabel->resize(parent->width() + (7 * UserMgmtWidgetItem::Margin), 16);
    signLabel->move(UserMgmtWidgetItem::Margin, top + messageItemHeight + UserMgmtWidgetItem::Margin);

//    switch(category)
//    {
//	case FRIENDS:
//	    ctrl->setText("<a href=\"unfollow://twitter.com/" + username + "\" style=\"text-decoration:none\"><img src=\":/images/unfollow.png\"/></a><br>"
//	    + "<a href=\"block://twitter.com/" + username + "\" style=\"text-decoration:none\"><img src=\":/images/block.png\"/></a>");
//	    break;
//	case FOLLOWERS:
//	    ctrl->setText("<a href=\"follow://twitter.com/" + username + "\" style=\"text-decoration:none\"><img src=\":/images/follow.png\"/></a><br>"
//	    + "<a href=\"block://twitter.com/" + username + "\" style=\"text-decoration:none\"><img src=\":/images/block.png\"/></a>");
//	    break;
//	case BLOCKED:
//	    ctrl->setText("<a href=\"unblock://twitter.com/" + username + "\" style=\"text-decoration:none\"><img src=\":/images/follow.png\"/></a>");
//	    break;
//	default:
//	    break;
//    }
//    // positioning of the ctrl element
//    ctrl->adjustSize();
//    ctrl->move(parent->width() - ctrl->width() - UserMgmtWidgetItem::Margin, top + UserMgmtWidgetItem::Margin);

    // set color of the FriendMgmtWidgetItem
    if(odd)
    {
	this->color.setRgb(230, 230, 230);
    }
    else
    {
	this->color.setRgb(255, 255, 255);
    }

    int itemHeight = signLabel->y() + signLabel->height();
    if(UserMgmtWidgetItem::IconSize > itemHeight)
    {
	itemHeight = UserMgmtWidgetItem::IconSize;
    }
    itemHeight += UserMgmtWidgetItem::Margin - top;
    height = itemHeight;

    return itemHeight;
}

void UserMgmtWidgetItem::paint(QPainter &painter, QPalette palette, int width)
{
    painter.fillRect(0, topPosition, width, height, QBrush(color));
    palette.setColor(QPalette::Active, QPalette::Base, color);
    palette.setColor(QPalette::Inactive, QPalette::Base, color);
    userDataTextBrowser->setPalette(palette);
}
#endif
