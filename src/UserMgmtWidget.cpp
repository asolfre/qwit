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
 *  UserMgmtWidget class implementation
 *
 */

#ifndef UserMgmtWidget_cpp
#define UserMgmtWidget_cpp

#include <QScrollBar>
#include <QMessageBox>

#include "UserMgmtWidget.h"

//UserMgmtWidget::UserMgmtWidget(QWidget *parent, QScrollArea *scrollArea, int widgetType) : QWidget(parent)
//{
//    qDebug() << ("UserMgmtWidget::UserMgmtWidget()");
//    this->scrollArea = scrollArea;
//    this->widgetType = widgetType;
//    nextItemIndex = 0;
//}

UserMgmtWidget::UserMgmtWidget(QWidget *parent) : QWidget(parent)
{
    qDebug() << ("UserMgmtWidget::UserMgmtWidget()");
//    nextItemIndex = 0;
}


//void UserMgmtWidget::addItem(QString username, QString userpic, Categories category, QString statusText, uint messageId, QDateTime time, uint replyStatusId)
//{
//    UserMgmtWidgetItem *item = new UserMgmtWidgetItem(this, username, userpic, category, statusText, messageId, time, replyStatusId, this->serviceBaseUrl);
//
//    item->loadIcon();
//
//    if(nextItemIndex > items.size())
//    nextItemIndex = items.size();
//
//    items.insert(nextItemIndex++, item);
//    cout << "added item" << endl;
//
//    item->show();
//
//    this->updateItems();
//}

void UserMgmtWidget::addItem(UserData userData)
{
    UserMgmtWidgetItem *item = new UserMgmtWidgetItem(userData, this, this->widgetType);

    items.push_back(item);
//    nextItemIndex++;
}

void UserMgmtWidget::updateItems()
{
    int height = 0;

    for (int i = 0; i < items.size(); ++i)
    {
	height += items[i]->update(height, (i & 1));
    }
    resize(width(), height);
}

void UserMgmtWidget::clear()
{
    qDebug() << ("UserMgmtWidget::clear()");

    if(items.size() > 0)
    {
	for(int i=items.size()-1; i>=0; i--)
	{
	    delete items[i];
	}
	this->items.clear();
    }
}

void UserMgmtWidget::paintEvent(QPaintEvent *event)
{
    // create local painter object
    QPainter painter(this);

    for(int i=0; i< items.size(); ++i)
    {
	items[i]->paint(painter, palette(), width());
    }
    event->accept();
}

void UserMgmtWidget::resizeEvent(QResizeEvent *event)
{
    if(event->oldSize() == event->size())
    {
	event->ignore();
	return;
    }
    updateItems();
    event->accept();
}

//void UserMgmtWidget::unfollowClicked(const QUrl &url)
//{
//    emit unfollow(url.path().remove(0, 1));
//}
//
//void UserMgmtWidget::followClicked(const QUrl &url)
//{
//    emit follow(url.path().remove(0, 1));
//}
//
//void UserMgmtWidget::blockClicked(const QUrl &url)
//{
//    QString screenName = url.path().remove(0, 1);
//
//    QMessageBox msgBox;
//    msgBox.setText(tr("Blocking will prevent %1 from following you. And you won't see their tweets in your timeline. Are you sure you want to block?").arg(screenName));
//    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
//    msgBox.setDefaultButton(QMessageBox::Cancel);
//    msgBox.setIcon(QMessageBox::Warning);
//    int ret = msgBox.exec();
//
//    if(ret == QMessageBox::Ok)
//    {
//	this->removeItem(screenName);
//	emit block(screenName);
//    }
//}
//
//void UserMgmtWidget::unblockClicked(const QUrl &url)
//{
//    emit unblock(url.path().remove(0, 1));
//}
//
//void UserMgmtWidget::removeItem(QString screenName)
//{
//    for(int i=0; i<items.size(); i++)
//    {
//	if(items[i]->getUsername() == screenName)
//	{
//	    delete items[i];
//	    this->items.remove(i);
//	    nextItemIndex--;
//	    updateItems();
//	    return;
//	}
//    }
//}
//
//int UserMgmtWidget::getItemCount()
//{
//    return nextItemIndex;
//}

//void UserMgmtWidget::resizeWidget()
//{
//    this->resize(scrollArea->width() - scrollArea->verticalScrollBar()->width() - 5, 500);
//}
#endif
