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

#ifndef FriendsManagementDialog_cpp
#define FriendsManagementDialog_cpp

#include <QScrollArea>
#include <QScrollBar>
#include <QtXml/QDomDocument>
#include <iostream>
#include<QDir>

#include "FriendsManagementDialog.h"

using namespace std;

FriendsManagementDialog::FriendsManagementDialog(QWidget *parent, Twitter *twitter, UserpicsDownloader *userpicsDownloader) : QDialog(parent)
{
        setupUi(this);
        this->twitter = twitter;
        this->userpicsDownloader = userpicsDownloader;

        // set up friends management tab
        QScrollArea *scrollArea = new QScrollArea(friendsTab);

        FriendsMgmtWidget *friendsMgmtWidget = new FriendsMgmtWidget(scrollArea);
        friendsMgmtWidget->setObjectName(QString::fromUtf8("friendsMgmtWidget"));
        friendsMgmtWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);

        scrollArea->setBackgroundRole(QPalette::Light);
        scrollArea->setWidget(friendsMgmtWidget);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        vBoxLayout2->addWidget(scrollArea);

        connect(friendsMgmtWidget, SIGNAL(unfollow(QString)), this, SLOT(unfollow(QString)));
        connect(friendsMgmtWidget, SIGNAL(block(QString)), this, SLOT(block(QString)));
        friendsMgmtTabs[FRIENDS_MGMT_TAB] = FriendsMgmtTab(scrollArea, friendsMgmtWidget);

        // set up followers management tab
        QGridLayout *gridLayout = new QGridLayout(followersTab);
        gridLayout->setObjectName(QString::fromUtf8("followersGridLayout"));

        scrollArea = new QScrollArea(followersTab);

        friendsMgmtWidget = new FriendsMgmtWidget(scrollArea);
        friendsMgmtWidget->setObjectName(QString::fromUtf8("followersMgmtWidget"));
        friendsMgmtWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);

        scrollArea->setBackgroundRole(QPalette::Light);
        scrollArea->setWidget(friendsMgmtWidget);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        gridLayout->addWidget(scrollArea, 0, 0, 1, 1);

        friendsMgmtTabs[FOLLOWERS_MGMT_TAB] = FriendsMgmtTab(scrollArea, friendsMgmtWidget);

        // bring friends tab to the front
        tabWidget->setCurrentIndex(0);

        connect(closePushButton, SIGNAL(pressed()), this, SLOT(closeFriendsManagement()));

        connect(twitter, SIGNAL(friendsUpdated(const QByteArray&, int)), this, SLOT(friendsUpdated(const QByteArray&, int)));
}


/*!
    \fn FriendsManagementDialog::closeFriendsManagement()
 */
void FriendsManagementDialog::closeFriendsManagement()
{
	hide();
}

void FriendsManagementDialog::addTestUser(QString name)
{
    friendsMgmtTabs[FRIENDS_MGMT_TAB].getFriendsMgmtWidget()->addItem(name, "", "test");
}

void FriendsManagementDialog::resizeEvent(QResizeEvent *event)
{
    for(int i=0; i<MGMT_TABS; ++i)
        friendsMgmtTabs[i].getFriendsMgmtWidget()->resize(friendsMgmtTabs[i].getScrollArea()->width() - friendsMgmtTabs[i].getScrollArea()->verticalScrollBar()->width() -5, 500);

    event->accept();
}

void FriendsManagementDialog::showEvent(QShowEvent *event)
{
    event->accept();
}

void FriendsManagementDialog::unfollow(const QString &url)
{
    cerr << url.toStdString() << endl;
}

void FriendsManagementDialog::block(const QString &url)
{
    cerr << url.toStdString() << endl;
}

void FriendsManagementDialog::friendsUpdated(const QByteArray &buffer, int type)
{
    QDomDocument document;

    document.setContent(buffer);

    QDomElement root = document.documentElement();

    if(root.tagName() == "users")
    {
        QDomNode node = root.firstChild();

        friendsMgmtTabs[FRIENDS_MGMT_TAB].getFriendsMgmtWidget()->clear();
        friendsMgmtTabs[FOLLOWERS_MGMT_TAB].getFriendsMgmtWidget()->clear();

        while(!node.isNull())
        {
            if(node.toElement().tagName() != "user")
                return;

            QDomNode node2 = node.firstChild();

            QString screenName;
            bool following;
            QString statusText;
            QString image;

            while(!node2.isNull())
            {
                if(node2.toElement().tagName() == "id"){}
                else if(node2.toElement().tagName() == "name"){}
                else if(node2.toElement().tagName() == "screen_name")
                {
                    screenName = node2.toElement().text();
                }
                else if(node2.toElement().tagName() == "location"){}
//                else if(node2.toElement().tagName() == "description"){}
                else if(node2.toElement().tagName() == "profile_image_url")
                {
                    image = node2.toElement().text();
                }
//                else if(node2.toElement().tagName() == "url"){}
//                else if(node2.toElement().tagName() == "protected"){}
//                else if(node2.toElement().tagName() == "followers_count"){}
//                else if(node2.toElement().tagName() == "profile_background_color"){}
//                else if(node2.toElement().tagName() == "profile_text_color"){}
//                else if(node2.toElement().tagName() == "profile_link_color"){}
//                else if(node2.toElement().tagName() == "profile_sidebar_fill_color"){}
//                else if(node2.toElement().tagName() == "profile_sidebar_border_color"){}
//                else if(node2.toElement().tagName() == "friends_count"){}
//                else if(node2.toElement().tagName() == "created_at"){}
//                else if(node2.toElement().tagName() == "favourites_count"){}
//                else if(node2.toElement().tagName() == "utc_offset"){}
//                else if(node2.toElement().tagName() == "time_zone"){}
//                else if(node2.toElement().tagName() == "profile_background_image_url"){}
//                else if(node2.toElement().tagName() == "profile_background_tile"){}
//                else if(node2.toElement().tagName() == "statuses_count"){}
//                else if(node2.toElement().tagName() == "notifications"){}
//                else if(node2.toElement().tagName() == "verified"){}
                else if(node2.toElement().tagName() == "following")
                {
                    if(node2.toElement().text() == "true")
                        following = true;
                    else
                        following = false;
                }
                else if(node2.toElement().tagName() == "status")
                {
                    QDomNode node3 = node2.firstChild();
                    while(!node3.isNull())
                    {
                        if(node3.toElement().tagName() == "created_at"){}
//                        else if(node3.toElement().tagName() == "id"){}
                        else if(node3.toElement().tagName() == "text")
                        {
                            statusText = node3.toElement().text();
                        }
//                        else if(node3.toElement().tagName() == "source"){}
//                        else if(node3.toElement().tagName() == "truncated"){}
//                        else if(node3.toElement().tagName() == "in_reply_to_status_id"){}
//                        else if(node3.toElement().tagName() == "in_reply_to_user_id"){}
//                        else if(node3.toElement().tagName() == "favorited"){}
//                        else if(node3.toElement().tagName() == "in_reply_to_screen_name"){}
                        node3 = node3.nextSibling();
                    }
                }
                node2 = node2.nextSibling();
            }

            // process user entry

            // start with the image file
            QByteArray hash = QCryptographicHash::hash(image.toAscii(), QCryptographicHash::Md5);
            QString imageFileName = "";
            for (int i = 0; i < hash.size(); ++i){
                unsigned char c = hash[i];
                c >>= 4;
                if (c < 10) {
                    c += '0';
                } else {
                    c += 'A' - 10;
                }
                imageFileName += (char)c;
                c = hash[i];
                c &= 15;
                if (c < 10) {
                    c += '0';
                } else {
                    c += 'A' - 10;
                }
                imageFileName += (char)c;
            }
            //imageFileName += "." + QFileInfo(QUrl(image).path()).suffix();
            QDir dir(QDir::homePath());
            dir.mkdir(".qwit");
            imageFileName = dir.absolutePath() + "/.qwit/" + imageFileName;
            userpicsDownloader->download(image, imageFileName);

            if(following)
            {
                friendsMgmtTabs[FRIENDS_MGMT_TAB].getFriendsMgmtWidget()->addItem(screenName, imageFileName, statusText);
            }
            else
            {
                friendsMgmtTabs[FOLLOWERS_MGMT_TAB].getFriendsMgmtWidget()->addItem(screenName, imageFileName, statusText);
            }
            node = node.nextSibling();
        }
        this->saveState();
    }
}

void FriendsManagementDialog::saveState()
{
    for(int i=0; i<MGMT_TABS; ++i)
        friendsMgmtTabs[i].getFriendsMgmtWidget()->resize(friendsMgmtTabs[i].getScrollArea()->width() - friendsMgmtTabs[i].getScrollArea()->verticalScrollBar()->width() -5, 500);
}
#endif
