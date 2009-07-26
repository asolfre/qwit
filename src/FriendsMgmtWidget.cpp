/*  This file is part of Qwit.

    Copyright (C) 2008, 2009 Artem Iglikov

    Qwit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Qwit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Qwit.  If not, see <http://www.gnu.org/licenses/>. */


#include <QScrollBar>
#include <iostream>

#include "FriendsMgmtWidget.h"
#include "TwitterWidgetItem.h"

using namespace std;

FriendsMgmtWidget::FriendsMgmtWidget(QScrollArea *scrollArea, const QString &serviceBaseURL) : QWidget()
{
    this->serviceBaseUrl = serviceBaseURL;
    this->scrollArea = scrollArea;
    currItemIndex = 0;
}


void FriendsMgmtWidget::addItem(QString username, QString userpic, bool following, QString statusText, uint replyStatusId)
{
    // FIXME duplicates check?
    FriendsMgmtWidgetItem *item = new FriendsMgmtWidgetItem(this, username, userpic, following);
    QTextBrowser *status = item->getStatus();
    status->setHtml(TwitterWidgetItem::prepare(statusText,replyStatusId, serviceBaseUrl));
    status->setReadOnly(true);
    status->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    status->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    status->setFrameShape(QFrame::NoFrame);
    status->setOpenExternalLinks(true);

    QFont font = status->document()->defaultFont();
    font.setFamily("Verdana");
    status->document()->setDefaultFont(font);

    item->loadIcon();

    if(currItemIndex > items.size())
    currItemIndex = items.size();

    items.insert(currItemIndex++, item);
    cout << "added item" << endl;

    item->show();

    this->updateItems();
}


void FriendsMgmtWidget::updateItems()
{
    int height = 0;

    for (int i = 0; i < items.size(); ++i)
    {
        height = items[i]->update(height, i);
    }
    resize(width(), height);
}

void FriendsMgmtWidget::clear()
{
    currItemIndex = 0;
    this->items.clear();
}

void FriendsMgmtWidget::paintEvent(QPaintEvent *event)
{
    // create local painter object
    QPainter painter(this);

    for(int i=0; i< items.size(); ++i)
    {
        FriendsMgmtWidgetItem *item = items[i];

        painter.fillRect(0, item->getTopPosition() ,width() ,item->getHeight(), QBrush(item->getColor()));
        // get QPalette object from inherited palette() method
        QPalette p = this->palette();
        /*
         * set the brush in the specified color group, used for the given
         * color role to the specified color
         * setColor(colorGroup, colorRole, color);
         */
        p.setColor(QPalette::Active, QPalette::Base, item->getColor());
        p.setColor(QPalette::Inactive, QPalette::Base, item->getColor());
	item->getStatus()->setPalette(p);
    }
    event->accept();
}

void FriendsMgmtWidget::resizeEvent(QResizeEvent *event)
{
    if(event->oldSize() == event->size())
    {
        event->ignore();
        return;
    }
    updateItems();
    event->accept();
}

void FriendsMgmtWidget::unfollowClicked(const QUrl &url)
{
    emit unfollow(url.path().remove(0, 1));
}

void FriendsMgmtWidget::blockClicked(const QUrl &url)
{
    emit block("Received: " + url.toString());
}

void FriendsMgmtWidget::removeItem(QString screenName)
{
    for(int i=0; i<items.size(); i++)
    {
	if(items[i]->getUsername() == screenName)
	{
	    this->items.remove(i);
	    currItemIndex--;
	    updateItems();
	    return;
	}
    }
}
