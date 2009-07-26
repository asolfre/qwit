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

#ifndef FriendsMgmtDialog_cpp
#define FriendsMgmtDialog_cpp

#include <QScrollArea>
#include <QScrollBar>
#include <iostream>
#include <QDir>
#include <iostream>

#include "FriendsMgmtDialog.h"
#include "MainWindow.h"

using namespace std;

FriendsMgmtDialog::FriendsMgmtDialog(QWidget *parent, Twitter *twitter, UserpicsDownloader *userpicsDownloader) : QDialog(parent)
{
        setupUi(this);
        this->twitter = twitter;
        this->userpicsDownloader = userpicsDownloader;

        // set up friends management tab
        QScrollArea *scrollArea = new QScrollArea(friendsTab);

	FriendsMgmtWidget *friendsMgmtWidget = new FriendsMgmtWidget(scrollArea, twitter->getServiceBaseURL());
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

	friendsMgmtWidget = new FriendsMgmtWidget(scrollArea, twitter->getServiceBaseURL());
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

	connect(twitter, SIGNAL(friendshipsUpdated(const QByteArray&)), this, SLOT(friendshipsUpdated(const QByteArray&)));
	connect(twitter, SIGNAL(friendsMgmtEvent(QByteArray, int)), this, SLOT(friendsMgmtEvent(QByteArray, int)));
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}


void FriendsMgmtDialog::resizeEvent(QResizeEvent *event)
{
    for(int i=0; i<MGMT_TABS; ++i)
        friendsMgmtTabs[i].getFriendsMgmtWidget()->resize(friendsMgmtTabs[i].getScrollArea()->width() - friendsMgmtTabs[i].getScrollArea()->verticalScrollBar()->width() -5, 500);

    event->accept();
}

void FriendsMgmtDialog::showEvent(QShowEvent *event)
{
    MainWindow *mainWindow = MainWindow::getInstance();
    // update the displayed tab
    twitter->getFriendships(mainWindow->username, mainWindow->password, tabWidget->currentIndex());

    event->accept();
}

void FriendsMgmtDialog::unfollow(QString screenName)
{
    MainWindow *mainWindow = MainWindow::getInstance();

    twitter->destroyFriendship(screenName, mainWindow->username, mainWindow->password);

    cout << "Unfollowing: " << screenName.toStdString() << endl;
}

void FriendsMgmtDialog::block(const QString &url)
{
    cerr << url.toStdString() << endl;
}

void FriendsMgmtDialog::friendshipsUpdated(const QByteArray &friendshipsBuffer)
{
    QDomDocument *document = new QDomDocument();

    document->setContent(friendshipsBuffer);

    QDomElement *root = new QDomElement(document->documentElement());

    if(root->tagName() == "users")
    {
	QDomNode *node = new QDomNode(root->firstChild());

        friendsMgmtTabs[FRIENDS_MGMT_TAB].getFriendsMgmtWidget()->clear();
        friendsMgmtTabs[FOLLOWERS_MGMT_TAB].getFriendsMgmtWidget()->clear();

	while(!node->isNull())
        {
	    if(node->toElement().tagName() != "user")
		return;

	    processUserXmlStructure(new QDomNode(node->firstChild()), false);
	    node = new QDomNode(node->nextSibling());
        }
        this->saveState();
    }
}

void FriendsMgmtDialog::saveState()
{
    for(int i=0; i<MGMT_TABS; i++)
    {
        friendsMgmtTabs[i].getFriendsMgmtWidget()->resize(friendsMgmtTabs[i].getScrollArea()->width() - friendsMgmtTabs[i].getScrollArea()->verticalScrollBar()->width() -5, 500);
    }
}

void FriendsMgmtDialog::tabChanged(int index)
{
    if(index < MGMT_TABS)
    {
	int height = friendsMgmtTabs[index].getScrollArea()->width() - friendsMgmtTabs[index].getScrollArea()->verticalScrollBar()->width() - 5;
	if(friendsMgmtTabs[index].getFriendsMgmtWidget()->height() != height)
	{
	    friendsMgmtTabs[index].getFriendsMgmtWidget()->resize(height, 500);
	}
	MainWindow *mainWindow = MainWindow::getInstance();
	twitter->getFriendships(mainWindow->username, mainWindow->password, index);
    }
}
#endif


void FriendsMgmtDialog::friendsMgmtEvent(const QByteArray &friendsMgmtBuffer, int type)
{
    QDomDocument *document = new QDomDocument();

    document->setContent(friendsMgmtBuffer);

    QDomElement *root = new QDomElement(document->documentElement());

    if(root->tagName() == "user")
    {
	QDomNode *node = new QDomNode(root->firstChild());

	while(!node->isNull())
	{
	    if(node->toElement().tagName() != "user")
		return;

	    processUserXmlStructure(new QDomNode(node->firstChild()), (type==10 ? true : false));
	    node = new QDomNode(node->nextSibling());
	}
	this->saveState();
    }
    else if(root->tagName() == "hash")
    {
	QDomNode *node = new QDomNode(root->firstChild());

	while(!node->isNull())
	{
	    if(node->toElement().tagName() == "request"){}
	    else if(node->toElement().tagName() == "error")
	    {
		cout << node->toElement().text().toStdString() << endl;
	    }
	    node = new QDomNode(node->nextSibling());
	}
    }
}

void FriendsMgmtDialog::processUserXmlStructure(QDomNode *currentNode, bool remove)
{
    QString screenName;
    bool following;
    QString statusText;
    QString image;
    uint replyStatusId = 0;

    QDomNode *node = currentNode;

    while(!node->isNull())
    {
	if(node->toElement().tagName() == "id"){}
	else if(node->toElement().tagName() == "name"){}
	else if(node->toElement().tagName() == "screen_name")
	{
	    screenName = node->toElement().text();
	}
	else if(node->toElement().tagName() == "location"){}
//      else if(node->toElement().tagName() == "description"){}
	else if(node->toElement().tagName() == "profile_image_url")
	{
	    image = node->toElement().text();
	}
//	else if(node->toElement().tagName() == "url"){}
//	else if(node->toElement().tagName() == "protected"){}
//	else if(node->toElement().tagName() == "followers_count"){}
//	else if(node->toElement().tagName() == "profile_background_color"){}
//	else if(node->toElement().tagName() == "profile_text_color"){}
//	else if(node->toElement().tagName() == "profile_link_color"){}
//	else if(node->toElement().tagName() == "profile_sidebar_fill_color"){}
//	else if(node->toElement().tagName() == "profile_sidebar_border_color"){}
//	else if(node->toElement().tagName() == "friends_count"){}
//	else if(node->toElement().tagName() == "created_at"){}
//	else if(node->toElement().tagName() == "favourites_count"){}
//	else if(node->toElement().tagName() == "utc_offset"){}
//	else if(node->toElement().tagName() == "time_zone"){}
//	else if(node->toElement().tagName() == "profile_background_image_url"){}
//	else if(node->toElement().tagName() == "profile_background_tile"){}
//	else if(node->toElement().tagName() == "statuses_count"){}
//	else if(node->toElement().tagName() == "notifications"){}
//	else if(node->toElement().tagName() == "verified"){}
	else if(node->toElement().tagName() == "following")
	{
	    if(node->toElement().text() == "true")
		following = true;
	    else
		following = false;
	}
	else if(node->toElement().tagName() == "status")
	{
	    QDomNode *node2 = new QDomNode(node->firstChild());
	    while(!node2->isNull())
	    {
		if(node2->toElement().tagName() == "created_at"){}
//              else if(node2->toElement().tagName() == "id"){}
		else if(node2->toElement().tagName() == "text")
		{
		    statusText = node2->toElement().text();
		}
//              else if(node2->toElement().tagName() == "source"){}
//              else if(node2->toElement().tagName() == "truncated"){}
		else if(node2->toElement().tagName() == "in_reply_to_status_id")
		{
		    replyStatusId = node2->toElement().text().toUInt();
		}
//		else if(node2->toElement().tagName() == "in_reply_to_user_id"){}
//		else if(node2->toElement().tagName() == "favorited"){}
//		else if(node2->toElement().tagName() == "in_reply_to_screen_name"){}
		node2 = new QDomNode(node2->nextSibling());
	    }
	}
	node = new QDomNode(node->nextSibling());
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

    if(remove)
    {
    friendsMgmtTabs[FRIENDS_MGMT_TAB].getFriendsMgmtWidget()->removeItem(screenName);
    }
    else
    {
	if(following)
	{
	    friendsMgmtTabs[FRIENDS_MGMT_TAB].getFriendsMgmtWidget()->addItem(screenName, imageFileName, following, statusText, replyStatusId);
	}
	else
	{
	    friendsMgmtTabs[FOLLOWERS_MGMT_TAB].getFriendsMgmtWidget()->addItem(screenName, imageFileName, following, statusText, replyStatusId);
	}
    }
    return;
}

void FriendsMgmtDialog::on_addFriendPushButton_pressed()
{
    QString screenName = newFriendLineEdit->text();

    for(int i=0; i<screenName.length(); i++)
    {
	if(!TwitterWidgetItem::isUsernameChar(screenName[i]))
	{
	    cout << "Screenname contains illegal character: " << QString(screenName[i]).toStdString() << endl;
	    return;
	}
    }

    MainWindow *mainWindow = MainWindow::getInstance();

    twitter->createFriendship(screenName, mainWindow->username, mainWindow->password);
}

void FriendsMgmtDialog::on_closePushButton_pressed()
{
    hide();
}

void FriendsMgmtDialog::on_newFriendLineEdit_textEdited(QString )
{
    if(newFriendLineEdit->text().length() > 0)
    {
    addFriendPushButton->setEnabled(true);
    }
    else
    {
    addFriendPushButton->setEnabled(false);
    }
}
