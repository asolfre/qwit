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

#include "ui_FriendsMgmtDialog.h"

#include "QwitHeaders.h"

#include <QtXml/QDomDocument>
#include "UserpicsDownloader.h"
#include "FriendshipsMgmtPage.h"
#include "FollowersMgmtPage.h"
#include "BlocksMgmtPage.h"
#include "UserMgmtWidgetItem.h"

class FriendsMgmtDialog : public QDialog, private Ui::FriendsMgmtDialog
{
        Q_OBJECT

private:
    QVector<AbstractUserMgmtPage*> pages;
    FriendshipsMgmtPage *friendshipsPage;
    FollowersMgmtPage *followersPage;
    BlocksMgmtPage *blocksPage;
    QMap<uint, UserMgmtWidgetItem*> requestsFromFrienshipsPage;
    QMap<uint, UserMgmtWidgetItem*> requestsFromFollowersPage;
    QMap<uint, UserMgmtWidgetItem*> requestsFromBlocksPage;
    uint requestId;
    QStatusBar *statusBar;
    QList<QTreeWidgetItem *> treeItems;

    void updateConnects();
    int oldAccountId;
    int mainWindowAccountId;
    bool firstRun;

public:
    FriendsMgmtDialog(QWidget *parent);
    ~FriendsMgmtDialog();

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    bool event(QEvent *e);

private slots:
    void on_accountsTreeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void on_splitter_splitterMoved(int pos, int index);
    void on_tabWidget_currentChanged(int index);
    void friendshipsPage_follow(QString screenName);
    void friendshipsPage_unfollow(QString screenName, UserMgmtWidgetItem *item);
    void friendshipsPabe_block(QString screenName, UserMgmtWidgetItem *item);
    void followersPage_follow(QString screenName, UserMgmtWidgetItem *item);
    void followersPage_unfollow(QString screenName, UserMgmtWidgetItem *item);
    void followersPage_block(QString screenName, UserMgmtWidgetItem *item);
    void blocksPage_unblock(QString screenName, UserMgmtWidgetItem *item);
    void addFriend(Message message, uint requestId);
    void removeFriend(Message Message, uint requestId);
    void addBlock(Message message, uint requestId);
    void removeBlock(Message message, uint requestId);
    void setState(QString state);
    void on_closeButtonBox_rejected();
};

#endif
