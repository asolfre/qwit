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
 *  FriendsMgmtDialog class declaration
 *
 */

#ifndef FriendsMgmtDialog_h
#define FriendsMgmtDialog_h

//const int MGMT_TABS = 3;

#include "ui_FriendsMgmtDialog.h"

#include "QwitHeaders.h"

#include <QtXml/QDomDocument>
#include "UserpicsDownloader.h"
//#include "FriendsMgmtWidget.h"
#include "FriendshipsMgmtPage.h"
#include "FollowersMgmtPage.h"
#include "BlocksMgmtPage.h"
//#include "Enumerations.h"

class FriendsMgmtDialog : public QDialog, public Ui::FriendsMgmtDialog
{
        Q_OBJECT

private:
    QVector<AbstractUserMgmtPage*> pages;
    FriendshipsMgmtPage *friendshipsPage;
    FollowersMgmtPage *followersPage;
    BlocksMgmtPage *blocksPage;
//    FriendsMgmtWidget *widgets[MGMT_TABS];
    UserpicsDownloader *userpicsDownloader;

//    void followImpl(QString screenName);
    void updateConnects();
    int oldAccountId;
    bool firstRun;

public:
    FriendsMgmtDialog(QWidget *parent);

public slots:
//    void unfollow(const QString screenName);
//    void follow(const QString screenName);
//    void block(const QString screenName);
//    void unblock(const QString screenName);
//    void friendshipsUpdated(const QByteArray &friendshipsBuffer, int type);
//    void saveState();
//    void tabChanged(int index);
//    void friendsMgmtEvent(const QByteArray &friendsMgmtBuffer, int type);

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

private:
//	void processUserXmlStructure(QDomNode *currentNode, Categories category, Actions action);

private slots:
    void on_tabWidget_currentChanged(int index);
    void on_newFriendLineEdit_textEdited(QString );
    void on_closePushButton_pressed();
    void on_addFriendPushButton_pressed();
};

#endif
