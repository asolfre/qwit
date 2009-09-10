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
 *  UserMgmtWidgetItem class declaration
 *
 */

#ifndef UserMgmtWidgetITEM_H
#define UserMgmtWidgetITEM_H

#include <QLabel>
#include <QDateTime>
#include <QTextBrowser>
#include <QPushButton>
#include <QPainter>
#include <QDateTime>

#include "QwitHeaders.h"
#include "UserData.h"
//#include "Enumerations.h"

class UserMgmtWidgetItem
{
private:
    QWidget *parent;
    int widgetType;
    // object that displays all the information about a friend
    QTextBrowser *userDataTextBrowser;
    QToolButton *followButton;
    QToolButton *unfollowButton;
    QToolButton *blockButton;
    QToolButton *destroyBlockButton;
    QLabel *userpicLabel;
    QLabel *signLabel;
    QString iconFileName;
//    int category;
    int topPosition;
    int height;
    QColor color;
    UserData userData;

public:
    static const int IconSize = 48;
    static const int Margin = 5;
//    UserMgmtWidgetItem(QWidget *parent, const QString username, const QString iconFileName, const int category, const QString statusText, uint messageId, const QDateTime &time, const uint replyStatusId, const QString &serviceBaseUrl);
    UserMgmtWidgetItem(UserData userData, QWidget *parent, int widgetType);
    ~UserMgmtWidgetItem();
    void loadUserpic();
//    void show();
    int update(int top, bool odd);
    void paint(QPainter &painter, QPalette palette, int width);
//    void hide();

};

#endif // UserMgmtWidgetITEM_H
