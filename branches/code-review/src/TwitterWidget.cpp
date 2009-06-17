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
	QPixmap pixmap(status.userpicFilename);
	if (!pixmap.isNull()) {
		userpicLabel->setPixmap(pixmap.scaled(ICON_SIZE, ICON_SIZE));
	}
	userpicLabel->resize(ICON_SIZE, ICON_SIZE);
}

TwitterWidgetItem::~TwitterWidgetItem() {
	delete statusTextBrowser;
	delete userpicLabel;
	delete signLabel;
	delete replyButton;
	delete favorButton;
	delete retweetButton;
	delete unfollowButton;
	delete directMessageButton;
}

TwitterWidget::TwitterWidget(QWidget *parent): QWidget(parent) {
	qDebug() << ("TwitterWidget::TwitterWidget()");
	moreToolButton = 0;
	lessToolButton = 0;
	addMoreButton();
	connect(&retweetButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(retweetButtonClicked(int)));
	connect(&replyButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(replyButtonClicked(int)));
	connect(&directMessageButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(directMessageButtonClicked(int)));
	connect(&favorButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(favorButtonClicked(int)));
}

void TwitterWidget::clear() {
	qDebug() << ("TwitterWidget::clear()");

	for (int i = 0; i < items.size(); ++i) {
		delete items[i];
	}
	items.clear();
	updateItems();
}

void TwitterWidget::addItem(const Status &status) {
//	qDebug() << ("TwitterWidget::addItem()");

	TwitterWidgetItem *item = new TwitterWidgetItem();

	item->status = status;

	item->statusTextBrowser = new QTextBrowser(this);
	item->statusTextBrowser->setHtml(QwitTools::prepareStatus(status.status, status.account));
	item->statusTextBrowser->setReadOnly(true);
	item->statusTextBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	item->statusTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	item->statusTextBrowser->setFrameShape(QFrame::NoFrame);
	item->statusTextBrowser->setOpenExternalLinks(true);
	QFont font = item->statusTextBrowser->document()->defaultFont();
	font.setFamily("Verdana");
	item->statusTextBrowser->document()->setDefaultFont(font);

	item->userpicLabel = new QLabel(this);
	item->loadUserpic();
	item->signLabel = new QLabel(this);
	item->signLabel->setAlignment(Qt::AlignRight);
	item->signLabel->setOpenExternalLinks(true);
	item->signLabel->setStyleSheet("a{text-decoration:none;}");

	item->favorButton = new QToolButton(this);
	item->favorButton->setIcon(QwitTools::getToolButtonIcon(":/images/favor.png", status.favorited));
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

	item->statusTextBrowser->show();
	item->userpicLabel->show();
	item->signLabel->show();

//	updateItems();
}

void TwitterWidget::updateItems() {
	qDebug() << ("TwitterWidget::updateItems()");

	retweetButtonGroup.buttons().clear();
	replyButtonGroup.buttons().clear();
	directMessageButtonGroup.buttons().clear();
	favorButtonGroup.buttons().clear();
	
	int height = 0;
	for (int i = 0; i < items.size(); ++i) {
		TwitterWidgetItem *item = items[i];
		retweetButtonGroup.addButton(item->retweetButton, i);
		replyButtonGroup.addButton(item->replyButton, i);
		directMessageButtonGroup.addButton(item->directMessageButton, i);
		favorButtonGroup.addButton(item->favorButton, i);
		QFontMetrics fontMetrics(item->statusTextBrowser->font());
		int statusItemWidth = width() - (ICON_SIZE + 4 * MARGIN + item->favorButton->width());
		int statusItemHeight = fontMetrics.boundingRect(0, 0, statusItemWidth, 1000, Qt::AlignTop | Qt::TextWordWrap, item->statusTextBrowser->toPlainText()).height() + MARGIN;
		if (statusItemHeight < ICON_SIZE) {
			statusItemHeight = ICON_SIZE;
		}
		item->statusTextBrowser->move(ICON_SIZE + 2 * MARGIN, height + MARGIN);
		item->statusTextBrowser->resize(statusItemWidth, statusItemHeight);
		statusItemHeight += item->statusTextBrowser->verticalScrollBar()->maximum() - item->statusTextBrowser->verticalScrollBar()->minimum();
		item->statusTextBrowser->resize(statusItemWidth, statusItemHeight);
		item->userpicLabel->move(MARGIN, height + MARGIN);

		item->signLabel->setText("<style>a{text-decoration:none;}</style><div style=\"font-size:small\"><a href=\"http://twitter.com/" + item->status.username + "\" style=\"font-weight:bold\">" + item->status.username + "</a> - <a href=\"http://twitter.com/" + item->status.username + "/statuses/" + QString::number(item->status.id) + "\">" + QwitTools::formatDateTime(item->status.time) + "</a> - from " + item->status.source + "</div>");
		item->signLabel->adjustSize();
		item->signLabel->move(width() - item->signLabel->width() - MARGIN - item->favorButton->width(), height + statusItemHeight + MARGIN);
		
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

		int itemHeight = statusItemHeight + item->signLabel->height() + MARGIN;
		itemHeight = max(item->unfollowButton->y() + item->unfollowButton->height(), item->signLabel->y() + item->signLabel->height()) + MARGIN - height;
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
		addLessButton();
	} else {
		removeLessButton();
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
	qDebug() << ("TwitterWidget::resizeEvent()");

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
		item->statusTextBrowser->setPalette(p);
	}
	event->accept();
}

void TwitterWidget::reloadUserpics() {
//	qDebug() << ("TwitterWidget::reloadUserpics()");

	for (int i = 0; i < items.size(); ++i) {
		items[i]->loadUserpic();
	}
}

void TwitterWidget::removeMoreButton() {
	if (moreToolButton) {
		qDebug() << ("TwitterWidget::removeMoreButton()");
		delete moreToolButton;
		moreToolButton = 0;
	}
}

void TwitterWidget::removeLessButton() {
	if (lessToolButton) {
		qDebug() << ("TwitterWidget::removeLessButton()");
		delete lessToolButton;
		lessToolButton = 0;
	}
}

void TwitterWidget::addMoreButton() {
	if (!moreToolButton) {
		qDebug() << ("TwitterWidget::addMoreButton()");
		moreToolButton = new QToolButton(this);
		moreToolButton->setText(tr("more"));
		moreToolButton->setCheckable(true);
		connect(moreToolButton, SIGNAL(clicked()), this, SIGNAL(moreButtonClicked()));
	}
}

void TwitterWidget::addLessButton() {
	if (!lessToolButton) {
		qDebug() << ("TwitterWidget::addLessButton()");
		lessToolButton = new QToolButton(this);
		lessToolButton->setText(tr("less"));
		lessToolButton->setCheckable(true);
		connect(lessToolButton, SIGNAL(clicked()), this, SIGNAL(lessButtonClicked()));
	}
}

void TwitterWidget::retweetButtonClicked(int id) {
	emit retweet(items[id]->status);
}

void TwitterWidget::replyButtonClicked(int id) {
	emit reply(items[id]->status);
}

void TwitterWidget::directMessageButtonClicked(int id) {
	emit directMessage(items[id]->status);
}

void TwitterWidget::favorButtonClicked(int id) {
	if (items[id]->status.favorited) {
		emit unfavor(items[id]->status);
	} else {
		emit favor(items[id]->status);
	}
}

void TwitterWidget::enableMoreButton() {
	if (moreToolButton) {
		moreToolButton->setEnabled(true);
		moreToolButton->setChecked(false);
	}
}

void TwitterWidget::enableLessButton() {
	if (lessToolButton) {
		lessToolButton->setEnabled(true);
		moreToolButton->setChecked(false);
	}
}

void TwitterWidget::disableMoreButton() {
	if (moreToolButton) {
		moreToolButton->setEnabled(false);
	}
}

void TwitterWidget::disableLessButton() {
	if (lessToolButton) {
		lessToolButton->setEnabled(false);
	}
}

#endif
