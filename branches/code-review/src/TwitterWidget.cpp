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
 *  TwitterWidget class implementation
 */

#ifndef TwitterWidget_cpp
#define TwitterWidget_cpp

#include "QwitHeaders.h"

#include "TwitterWidget.h"
#include "QwitException.h"
#include "QwitTools.h"
#include "Configuration.h"

void TwitterWidgetItem::loadUserpic() {
	QPixmap pixmap(userpicFileName);
	if (!pixmap.isNull()) {
		userpic->setPixmap(pixmap.scaled(ICON_SIZE, ICON_SIZE));
	}
	userpic->resize(ICON_SIZE, ICON_SIZE);
}

TwitterWidgetItem::~TwitterWidgetItem() {
	delete status;
	delete userpic;
	delete sign;
	delete replyButton;
	delete favorButton;
	delete retweetButton;
	delete unfollowButton;
	delete directMessageButton;
}

TwitterWidget::TwitterWidget(QWidget *parent): QWidget(parent) {
	QwitTools::log("TwitterWidget::TwitterWidget()");
	moreToolButton = 0;
	lessToolButton = 0;
	enableMoreButton();
}

void TwitterWidget::clear() {
	QwitTools::log("TwitterWidget::clear()");

	for (int i = 0; i < items.size(); ++i) {
		delete items[i];
	}
	items.clear();
	updateItems();
}

void TwitterWidget::addItem(const Status &status) {
	QwitTools::log("TwitterWidget::addItem()");

	TwitterWidgetItem *item = new TwitterWidgetItem();

	item->time = status.time;
	item->username = status.username;
	item->messageId = status.id;

	item->rawStatus = status.status;

	item->status = new QTextBrowser(this);
	item->status->setHtml(QwitTools::prepareStatus(status.status, status.account));
	item->status->setReadOnly(true);
	item->status->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	item->status->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	item->status->setFrameShape(QFrame::NoFrame);
	item->status->setOpenExternalLinks(true);
	QFont font = item->status->document()->defaultFont();
	font.setFamily("Verdana");
	item->status->document()->setDefaultFont(font);

	item->userpic = new QLabel(this);
	item->userpicFileName = status.userpicFilename;
	item->loadUserpic();
	item->sign = new QLabel("<a href=\"http://twitter.com/" + status.username + "\" style=\"font-weight:bold;text-decoration:none\">" + status.username + "</a> - <a href=\"http://twitter.com/" + status.username + "/statuses/" + QString::number(status.id) + "\" style=\"font-size:70%;text-decoration:none\">" + QwitTools::formatDateTime(status.time) + "</a>", this);
	item->sign->setAlignment(Qt::AlignRight);
	item->sign->setOpenExternalLinks(true);

	item->favorButton = new QToolButton(this);
	item->favorButton->setIcon(QwitTools::getToolButtonIcon(":/images/favor.png"));
	item->favorButton->setText("");
	item->favorButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	item->favorButton->setAutoRaise(true);
	item->favorButton->show();

	item->replyButton = new QToolButton(this);
	item->replyButton->setIcon(QwitTools::getToolButtonIcon(":/images/reply.png"));
	item->replyButton->setText("");
	item->replyButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	item->replyButton->setAutoRaise(true);
	item->replyButton->show();

	item->retweetButton = new QToolButton(this);
	item->retweetButton->setIcon(QwitTools::getToolButtonIcon(":/images/retweet.png"));
	item->retweetButton->setText("");
	item->retweetButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	item->retweetButton->setAutoRaise(true);
	item->retweetButton->show();

	item->unfollowButton = new QToolButton(this);
	item->unfollowButton->setIcon(QwitTools::getToolButtonIcon(":/images/unfollow.png"));
	item->unfollowButton->setText("");
	item->unfollowButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	item->unfollowButton->setAutoRaise(true);
	item->unfollowButton->resize(18, 18);
	item->unfollowButton->show();

	item->directMessageButton = new QToolButton(this);
	item->directMessageButton->setIcon(QwitTools::getToolButtonIcon(":/images/directMessage.png"));
	item->directMessageButton->setText("");
	item->directMessageButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	item->directMessageButton->setAutoRaise(true);
	item->directMessageButton->resize(18, 18);
	item->directMessageButton->show();
	
	items.push_back(item);

	item->status->show();
	item->userpic->show();
	item->sign->show();

//	updateItems();
}

void TwitterWidget::updateItems() {
	QwitTools::log("TwitterWidget::updateItems()");

	int height = 0;
	for (int i = 0; i < items.size(); ++i) {
		TwitterWidgetItem *item = items[i];
		QFontMetrics fontMetrics(item->status->font());
		int statusItemWidth = width() - (ICON_SIZE + 4 * MARGIN + item->favorButton->width());
		int statusItemHeight = fontMetrics.boundingRect(0, 0, statusItemWidth, 1000, Qt::AlignTop | Qt::TextWordWrap, item->status->toPlainText()).height() + MARGIN;
		if (statusItemHeight < ICON_SIZE) {
			statusItemHeight = ICON_SIZE;
		}
		item->status->move(ICON_SIZE + 2 * MARGIN, height + MARGIN);
		item->status->resize(statusItemWidth, statusItemHeight);
		statusItemHeight += item->status->verticalScrollBar()->maximum() - item->status->verticalScrollBar()->minimum();
		item->status->resize(statusItemWidth, statusItemHeight);
		item->userpic->move(MARGIN, height + MARGIN);

		item->sign->setText("<a href=\"http://twitter.com/" + item->username + "\" style=\"font-weight:bold;text-decoration:none;font-size:small\">" + item->username + "</a> - <a href=\"http://twitter.com/" + item->username + "/statuses/" + QString::number(item->messageId) + "\" style=\"font-size:small;text-decoration:none\">" + QwitTools::formatDateTime(item->time) + "</a> ");
		item->sign->adjustSize();
		item->sign->move(width() - item->sign->width() - MARGIN - item->favorButton->width(), height + statusItemHeight + MARGIN);
		
		item->favorButton->move(width() - MARGIN - item->favorButton->width(), height);
		item->favorButton->show();
		item->replyButton->move(width() - MARGIN - item->replyButton->width(), height + item->favorButton->height());
		item->replyButton->show();
		item->retweetButton->move(width() - MARGIN - item->retweetButton->width(), height + item->favorButton->height() + item->retweetButton->height());
		item->retweetButton->show();
		item->directMessageButton->move(MARGIN + ICON_SIZE / 2 - MARGIN / 2 - item->directMessageButton->width(), height + ICON_SIZE + 2 * MARGIN);
		item->directMessageButton->show();
		item->unfollowButton->move(MARGIN + ICON_SIZE / 2 + MARGIN / 2, height + ICON_SIZE + 2 * MARGIN);
		item->unfollowButton->show();
		
		if (i & 1) {
			item->color = QColor(230, 230, 230);
		} else {
			item->color = QColor(180, 180, 180);
		}

		int itemHeight = statusItemHeight + item->sign->height() + MARGIN;
		itemHeight = max(item->unfollowButton->y() + item->unfollowButton->height(), item->sign->y() + item->sign->height()) + MARGIN - height;
		item->top = height;
		item->height = itemHeight;
		height += itemHeight;
	}
	Configuration *config = Configuration::getInstance();
	if (moreToolButton) {
		moreToolButton->move((width() - moreToolButton->width()) / 2, height + MARGIN);
		moreToolButton->show();
	}
	if (moreToolButton && (config->messagesPerPage < items.size())) {
		enableLessButton();
	} else {
		disableLessButton();
	}
	if (lessToolButton) {
		moreToolButton->move((width() - (moreToolButton->width() + lessToolButton->width() + MARGIN)) / 2, height + MARGIN);
		moreToolButton->show();
		lessToolButton->move((width() - (moreToolButton->width() + lessToolButton->width() + MARGIN)) / 2 + moreToolButton->width() + MARGIN, height + MARGIN);
		lessToolButton->show();
	}
	if (moreToolButton) {
		height += moreToolButton->height() + 2 * MARGIN;
	}
	resize(width(), height);
}

void TwitterWidget::resizeEvent(QResizeEvent *event) {
	QwitTools::log("TwitterWidget::resizeEvent()");

	if (event->oldSize() == event->size()) {
		event->ignore();
		return;
	}
	updateItems();
	event->accept();
}

void TwitterWidget::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	for (int i = 0; i < items.size(); ++i) {
		TwitterWidgetItem *item = items[i];
		painter.fillRect(0, item->top, width(), item->height, QBrush(item->color));
		QPalette p = palette();
		p.setColor(QPalette::Active, QPalette::Base, item->color);
		p.setColor(QPalette::Inactive, QPalette::Base, item->color);
		item->status->setPalette(p);
	}
	event->accept();
}

void TwitterWidget::reloadUserpics() {
	QwitTools::log("TwitterWidget::reloadUserpics()");

	for (int i = 0; i < items.size(); ++i) {
		items[i]->loadUserpic();
	}
}

void TwitterWidget::disableMoreButton() {
	if (moreToolButton) {
		QwitTools::log("TwitterWidget::disableMoreButton()");
		delete moreToolButton;
		moreToolButton = 0;
	}
}

void TwitterWidget::disableLessButton() {
	if (lessToolButton) {
		QwitTools::log("TwitterWidget::disableLessButton()");
		delete lessToolButton;
		lessToolButton = 0;
	}
}

void TwitterWidget::enableMoreButton() {
	if (!moreToolButton) {
		QwitTools::log("TwitterWidget::enableMoreButton()");
		moreToolButton = new QToolButton(this);
		moreToolButton->setText(tr("more"));
		connect(moreToolButton, SIGNAL(clicked()), this, SIGNAL(moreButtonClicked()));
	}
}

void TwitterWidget::enableLessButton() {
	if (!lessToolButton) {
		QwitTools::log("TwitterWidget::enableLessButton()");
		lessToolButton = new QToolButton(this);
		lessToolButton->setText(tr("less"));
		connect(lessToolButton, SIGNAL(clicked()), this, SIGNAL(lessButtonClicked()));
	}
}

#endif
