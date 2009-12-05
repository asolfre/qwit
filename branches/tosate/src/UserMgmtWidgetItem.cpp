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
#include "Configuration.h"
#include "UserMgmtWidget.h"

UserMgmtWidgetItem::UserMgmtWidgetItem(User user, QWidget *parent, int widgetType)
{
    qDebug() << ("UserMgmtWidgetItem::UserMgmtWidgetItem()");
    this->parent = parent;
    this->widgetType = widgetType;

    this->user = user;
    this->itemTextBrowser = new QTextBrowser(parent);
    if(user.status) {
	this->itemTextBrowser->setHtml(QwitTools::prepareMessage(user.status->text, user.account));
    }
    this->itemTextBrowser->setReadOnly(true);
    this->itemTextBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->itemTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->itemTextBrowser->setFrameShape(QFrame::NoFrame);
    this->itemTextBrowser->setOpenExternalLinks(true);

    this->userpicLabel = new QLabel(parent);
    this->loadUserpic();
    this->signLabel = new QLabel(parent);
    this->signLabel->setAlignment(Qt::AlignRight);
    this->signLabel->setOpenExternalLinks(true);
    this->signLabel->setStyleSheet("a{text-decoration:none;}");

    // setup Buttons
    followButton = new QToolButton(parent);
    followButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    followButton->setAutoRaise(true);
    QAction *action = new QAction(parent);
    action->setToolTip(tr("follow %1").arg(user.screen_name));
    followButton->setDefaultAction(action);
    followButton->setIcon(QwitTools::getToolButtonIcon(":/images/follow.png"));

    unfollowButton = new QToolButton(parent);
    unfollowButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    unfollowButton->setAutoRaise(true);
    action = new QAction(parent);
    action->setToolTip(tr("unfollow %1").arg(user.screen_name));
    unfollowButton->setDefaultAction(action);
    unfollowButton->setIcon(QwitTools::getToolButtonIcon(":/images/unfollow.png"));

    blockButton = new QToolButton(parent);
    blockButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    blockButton->setAutoRaise(true);
    action = new QAction(parent);
    action->setToolTip(tr("block %1").arg(user.screen_name));
    blockButton->setDefaultAction(action);
    blockButton->setIcon(QwitTools::getToolButtonIcon(":/images/block.png"));

    unblockButton = new QToolButton(parent);
    unblockButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    unblockButton->setAutoRaise(true);
    action = new QAction(parent);
    action->setToolTip(tr("unblock %1").arg(user.screen_name));
    unblockButton->setDefaultAction(action);
    unblockButton->setIcon(QwitTools::getToolButtonIcon(":/images/unblock.png"));

    connect(followButton, SIGNAL(pressed()), this, SLOT(on_followToolButton_pressed()));
    connect(unfollowButton, SIGNAL(pressed()), this, SLOT(on_unfollowToolButton_pressed()));
    connect(blockButton, SIGNAL(pressed()), this, SLOT(on_blockToolButton_pressed()));
    connect(unblockButton, SIGNAL(pressed()), this, SLOT(on_unblockToolButton_pressed()));

    this->itemTextBrowser->show();
    this->userpicLabel->show();
    this->signLabel->show();
}

UserMgmtWidgetItem::~UserMgmtWidgetItem()
{
    qDebug() << ("UserMgmtWidgetItem::~UserMgmtWidgetItem()");

    delete itemTextBrowser;
    delete userpicLabel;
    delete signLabel;
    delete followButton;
    delete unfollowButton;
    delete blockButton;
    delete unblockButton;
}

void UserMgmtWidgetItem::loadUserpic()
{
	QPixmap pixmap(user.userpicFilename);
	if (!pixmap.isNull()) {
		userpicLabel->setPixmap(pixmap.scaled(UserMgmtWidgetItem::IconSize, UserMgmtWidgetItem::IconSize));
	}
	userpicLabel->resize(UserMgmtWidgetItem::IconSize, UserMgmtWidgetItem::IconSize);
}

void UserMgmtWidgetItem::paint(QPainter &painter, QPalette palette, int width)
{
    painter.fillRect(0, topPosition, width, height, QBrush(color));
    palette.setColor(QPalette::Active, QPalette::Base, color);
    palette.setColor(QPalette::Inactive, QPalette::Base, color);
    itemTextBrowser->setPalette(palette);
}

int UserMgmtWidgetItem::update(int index, int currentHeight)
{
    return arrangeItem(index, currentHeight);
}

int UserMgmtWidgetItem::arrangeItem(int index, int currentHeight)
{
    QFontMetrics fontMetrics(itemTextBrowser->font());
    int messageItemWidth = parent->width() - (UserMgmtWidgetItem::IconSize + 4 * UserMgmtWidgetItem::Margin + followButton->width());
    int messageItemHeight = fontMetrics.boundingRect(0, 0, messageItemWidth, 1000, Qt::AlignTop | Qt::TextWordWrap, itemTextBrowser->toPlainText()).height() + UserMgmtWidgetItem::Margin;
    if(messageItemHeight < UserMgmtWidgetItem::IconSize)
	messageItemHeight = UserMgmtWidgetItem::IconSize;

    itemTextBrowser->move(UserMgmtWidgetItem::IconSize + 2 * UserMgmtWidgetItem::Margin, currentHeight + UserMgmtWidgetItem::Margin);
    itemTextBrowser->resize(messageItemWidth, messageItemHeight);

    messageItemHeight += itemTextBrowser->verticalScrollBar()->maximum() - itemTextBrowser->verticalScrollBar()->minimum();
    itemTextBrowser->resize(messageItemWidth, messageItemHeight);

    userpicLabel->move(UserMgmtWidgetItem::Margin, currentHeight + UserMgmtWidgetItem::Margin);

    // arrange buttons on the right
    switch(widgetType)
    {
    case 0:
	followButton->hide();
	unfollowButton->show();
	blockButton->show();
	unblockButton->hide();
	break;
    case 1:
	if(user.following)
	{
	    followButton->hide();
	    unfollowButton->show();
	}
	else
	{
	    followButton->show();
	    unfollowButton->hide();
	}
	blockButton->show();
	unblockButton->hide();
	break;
    case 2:
	followButton->show();
	followButton->setDisabled(true);
	unfollowButton->hide();
	blockButton->hide();
	unblockButton->show();
	break;
    default:
	break;
    }
    followButton->move(parent->width() - UserMgmtWidgetItem::Margin - followButton->width(), currentHeight);
    unfollowButton->move(parent->width() - UserMgmtWidgetItem::Margin - unfollowButton->width(), currentHeight);
    blockButton->move(parent->width() - UserMgmtWidgetItem::Margin - blockButton->width(), currentHeight + blockButton->height());
    unblockButton->move(parent->width() - UserMgmtWidgetItem::Margin - unblockButton->width(), currentHeight + unblockButton->height());

    QString sign = "<style>a{text-decoration:none;}</style><div style=\"font-size:small\"><a href=\"" +
		   user.account->serviceBaseUrl() + "/" + user.screen_name + "\" style=\"font-weight:bold\">" + user.screen_name + "</a>";
    if(user.status) {
	QString messageUrl = this->user.account->singleMessageUrl().replace("%username", this->user.screen_name).replace("%messageid", QString::number(this->user.status->id));
	sign += " - <a href=\"" + messageUrl + "\">" + QwitTools::formatDateTime(user.status->created_at) + "</a>";
	if(user.status->source != "")
	{
	    sign += " from " + user.status->source;
	}
	if(user.status->in_reply_to_status_id)
	{
	    QString inReplyToMessageUrl = user.account->singleMessageUrl().replace("%username", this->user.status->in_reply_to_screen_name).replace("%messageid", QString::number(this->user.status->in_reply_to_status_id));
	    sign += " - <a href=\"" + inReplyToMessageUrl + "\"> in reply to " + user.status->in_reply_to_screen_name + "</a>";
	}
    }
    sign += "</div>";
    this->signLabel->setText(sign);
    this->signLabel->adjustSize();
//    this->signLabel->setToolTip(message.time.toString(Qt::DefaultLocaleLongDate));
    int signY = 0, signX = 0;
    // positioning
    signY = blockButton->pos().y() + blockButton->height() - signLabel->height();
    signY = max(signY, itemTextBrowser->pos().y() + itemTextBrowser->height() + UserMgmtWidgetItem::Margin);
    signX = parent->width() - signLabel->width() - UserMgmtWidgetItem::Margin - blockButton->width();
    signLabel->move(signX, signY);

    Configuration *config = Configuration::getInstance();
    if(index & 1)
	color = config->commonMessagesEvenColor;
    else
	color = config->commonMessagesOddColor;

    int itemHeight = messageItemHeight + signLabel->height() + UserMgmtWidgetItem::Margin;
    itemHeight = max(blockButton->y() + blockButton->height(), signLabel->y() + signLabel->height()) + UserMgmtWidgetItem::Margin - currentHeight;
    this->topPosition = currentHeight;
    this->height = itemHeight;

    return itemHeight;
}

void UserMgmtWidgetItem::on_followToolButton_pressed()
{
    UserMgmtWidget *userMgmtWidget = (UserMgmtWidget*) parent;
    userMgmtWidget->follow(user.screen_name, this);
}

void UserMgmtWidgetItem::on_unfollowToolButton_pressed()
{
    UserMgmtWidget *userMgmtWidget = (UserMgmtWidget*) parent;
    userMgmtWidget->unfollow(user.screen_name, this);
}

void UserMgmtWidgetItem::on_blockToolButton_pressed()
{
    UserMgmtWidget *userMgmtWidget = (UserMgmtWidget*) parent;
    userMgmtWidget->block(user.screen_name, this);
}

void UserMgmtWidgetItem::on_unblockToolButton_pressed()
{
    UserMgmtWidget *userMgmtWidget = (UserMgmtWidget*) parent;
    userMgmtWidget->unblock(user.screen_name, this);
}

//bool UserMgmtWidgetItem::operator ==(const UserMgmtWidgetItem &other)
//{
//    qDebug() << ("UserMgmtWidgetItem::operator ==()");
//
//    if(this->user.screen_name == other.user.screen_name)
//	return true;
//
//    return false;
//}
#endif
