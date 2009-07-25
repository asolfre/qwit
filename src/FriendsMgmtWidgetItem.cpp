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

const int ICON_SIZE = 48;
const int MARGIN = 5;

#include <QDesktopServices>
#include "FriendsMgmtWidgetItem.h"

using namespace std;

FriendsMgmtWidgetItem::FriendsMgmtWidgetItem(QWidget *parent, QString username, QString iconFileName, bool following)
{
    this->parent = parent;
    this->status = new QTextBrowser(parent);
    this->username = username;
    this->following = following;
    this->icon = new QLabel(parent);

    this->sign = new QLabel(parent);
    this->sign->setAlignment(Qt::AlignRight);
    this->sign->setOpenExternalLinks(true);

    this->ctrl = new QLabel(parent);
    this->ctrl->setAlignment(Qt::AlignRight);
    this->ctrl->setOpenExternalLinks(true);

    this->iconFileName = iconFileName;

    QDesktopServices::setUrlHandler("unfollow", parent, "unfollowClicked");
    QDesktopServices::setUrlHandler("block", parent, "blockClicked");
}

FriendsMgmtWidgetItem::~FriendsMgmtWidgetItem()
{
    delete status;
    delete icon;
    delete sign;
    delete ctrl;
}

int FriendsMgmtWidgetItem::getTopPosition()
{
    return this->topPos;
}

int FriendsMgmtWidgetItem::getHeight()
{
    return this->height;
}

QColor FriendsMgmtWidgetItem::getColor()
{
    return this->color;
}

QTextBrowser* FriendsMgmtWidgetItem::getStatus()
{
    return this->status;
}

//void FriendsMgmtWidgetItem::setIcon(QLabel *icon)
//{
//    this->icon = icon;
//}

void FriendsMgmtWidgetItem::show()
{
    status->show();
    icon->show();
    sign->show();
    ctrl->show();
}

QLabel* FriendsMgmtWidgetItem::getIcon()
{
    return icon;
}

//void FriendsMgmtWidgetItem::setSignText(QString text)
//{
//    this->sign->setText(text);
//}

//QLabel* FriendsMgmtWidgetItem::getSign()
//{
//    return this->sign;
//}

//void FriendsMgmtWidgetItem::setTopPosition(int top)
//{
//    this->topPos = top;
//}

void FriendsMgmtWidgetItem::setHeight(int height)
{
    this->height = height;
}

QString FriendsMgmtWidgetItem::getUsername()
{
    return this->username;
}

void FriendsMgmtWidgetItem::setColor(QColor color)
{
    this->color = color;
}

void FriendsMgmtWidgetItem::loadIcon()
{
        QPixmap pixmap(iconFileName);
        if (!pixmap.isNull()) {
                icon->setPixmap(pixmap.scaled(ICON_SIZE, ICON_SIZE));
        }
        icon->resize(ICON_SIZE, ICON_SIZE);
}

int FriendsMgmtWidgetItem::update(int height, int itemCount)
{    
    // potitioning of the icon
    icon->move(MARGIN, height + MARGIN);
    
    // calculate the size of the status element
    int statusItemWidth = parent->width() - (ICON_SIZE + 5 * MARGIN);
    QFontMetrics fontMetrics(status->font());
    int statusItemHeight = fontMetrics.boundingRect(0, 0, statusItemWidth, 1000, Qt::AlignTop | Qt::TextWordWrap, status->toPlainText()).height() + 5;
    
    if(statusItemHeight < ICON_SIZE)
    statusItemHeight = ICON_SIZE;
    
    // positioning and scaling of the status element
    status->move(ICON_SIZE + 2 * MARGIN, height + MARGIN);
    status->resize(statusItemWidth, statusItemHeight);
    
    QString tablew = "";
    tablew.setNum(statusItemWidth + 70);
    sign->setText("<table border=\"0\" width=\"" + tablew + "\" cellpadding=\"0\" cellspacing=\"0\">"
                      + "<tr valign=\"top\"><td width=\"50%\">"
                      + "<a href=\"twitter://user/" + username + "\" style=\"font-weight:bold;text-decoration:none;font-size:small\">" + username + "</a>"
                      + "</td></tr></table>");
    sign->resize(parent->width() + (7 * MARGIN), 16);
    sign->move(MARGIN, height + statusItemHeight + MARGIN);

    if(following)
    {
ctrl->setText("<a href=\"unfollow://" + username + "\" style=\"text-decoration:none\"><img src=\":/images/unfollow.png\"/></a><br><a href=\"block://" + username + "\" style=\"text-decoration:none\"><img src=\":/images/block.png\"/></a>");
    }
    else
    {
ctrl->setText("");
    }
    // positioning of the ctrl element
    ctrl->adjustSize();
    ctrl->move(parent->width() - ctrl->width() - MARGIN, height + MARGIN);
    
    // set color of the FriendMgmtWidgetItem
    if(itemCount & 1)
    {
        this->setColor(QColor(230, 230, 230));
    }
    else
    {
        this->setColor(QColor("white"));
    }
    
    int itemHeight = statusItemHeight + sign->height() + MARGIN;
    itemHeight = max(ICON_SIZE, sign->y() + sign->height()) + MARGIN - height;
    this->topPos = height;
    this->setHeight(itemHeight);
    height += itemHeight;

    return height;
}
