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

#ifndef FriendsMgmtDialog_H
#define FriendsMgmtDialog_H

const int MGMT_TABS = 2;
const int FRIENDS_MGMT_TAB = 0;
const int FOLLOWERS_MGMT_TAB = 1;

#include <QtXml/QDomDocument>

#include "ui_FriendsMgmtDialog.h"
#include "FriendsMgmtTab.h"
#include "UserpicsDownloader.h"
#include "Twitter.h"

/**
	@author tosate <tosate@googlemail.com>
*/
class FriendsMgmtDialog : public QDialog, public Ui::FriendsMgmtDialog
{
        Q_OBJECT

private:
    FriendsMgmtTab friendsMgmtTabs[MGMT_TABS];
    UserpicsDownloader *userpicsDownloader;
    Twitter *twitter;

public:

        FriendsMgmtDialog(QWidget *parent, Twitter *twitter, UserpicsDownloader *userpicsDownloader);

public slots:
        void unfollow(const QString &url);
        void block(const QString &url);
	void friendshipsUpdated(const QByteArray &friendshipsBuffer);
        void saveState();
	void tabChanged(int index);
	void friendsMgmtEvent(const QByteArray &friendsMgmtBuffer);

protected:

        void resizeEvent(QResizeEvent *event);
        void showEvent(QShowEvent *event);

private:
	void processUserXmlStructure(QDomNode *currentNode);

private slots:
    void on_newFriendLineEdit_textEdited(QString );
    void on_closePushButton_pressed();
    void on_addFriendPushButton_pressed();
};

#endif
