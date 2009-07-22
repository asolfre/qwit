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

#ifndef FRIENDSMGMTWIDGET_H
#define FRIENDSMGMTWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QDateTime>
#include <QPainter>
#include <QPaintEvent>

#include "FriendsMgmtWidgetItem.h"

class FriendsMgmtWidget : public QWidget
{
    Q_OBJECT

private:
    QScrollArea *scrollArea;
    QVector<FriendsMgmtWidgetItem*> items;
    int currItemIndex;

public:
    FriendsMgmtWidget(QScrollArea *scrollArea);
    void clear();
    void addItem(QString username, QString userpic, QString statusText);
    void updateItems();
    const FriendsMgmtWidgetItem getItem(int index);
    int getItemsCount();
    void setMessagesPerPage(int value);
    void setUsernameUnderAvatar(bool value);
    void setVerticalAlignControl(bool value);
    void saveScrollPosition();
    void restoreScrollPosition();

    static QString formatDateTime(const QDateTime &time);

protected:
    // overwrite QWidget::paintEvent() method
    void paintEvent(QPaintEvent *event);
    // overwrite QWidget::resizeEvent() method
    void resizeEvent(QResizeEvent *event);

public slots:
    void unfollowClicked(const QUrl &url);
    void blockClicked(const QUrl &url);

signals:
    void unfollow(const QString &);
    void block(const QString &);
};

#endif // FRIENDSMGMTWIDGET_H
