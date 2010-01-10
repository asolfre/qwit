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
 *  FriendsMgmtDialog class implementation
 */

#ifndef FriendsMgmtDialog_cpp
#define FriendsMgmtDialog_cpp

#include <QScrollArea>
#include <QScrollBar>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

#include "FriendsMgmtDialog.h"
#include "Configuration.h"


FriendsMgmtDialog::FriendsMgmtDialog(QWidget *parent) : QDialog(parent)
{
    qDebug() << ("FriendsMgmtDialog::FriendsMgmtDialog()");
    setupUi(this);

    statusBar = new QStatusBar(this);
    statusBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    this->layout()->addWidget(statusBar);
    this->oldAccountId = -1;

    friendshipsPage = new FriendshipsMgmtPage(this);
    pages[1001] = friendshipsPage;
    currentPageWidget = friendshipsPage;
    followersPage = new FollowersMgmtPage(this);
    pages[1002] = followersPage;
    blocksPage = new BlocksMgmtPage(this);
    pages[1003] = blocksPage;
    pagesContainerLayout = new QVBoxLayout();
    pagesContainerLayout->setMargin(0);
    pagesWidget->setLayout(pagesContainerLayout);
    pagesContainerLayout->addWidget(friendshipsPage);
    pagesContainerLayout->addWidget(followersPage);
    pagesContainerLayout->addWidget(blocksPage);
    followersPage->hide();
    blocksPage->hide();

    connect(friendshipsPage, SIGNAL(follow(QString)), this, SLOT(friendshipsPage_follow(QString)));
    connect(friendshipsPage, SIGNAL(unfollow(QString,UserMgmtWidgetItem*)), this, SLOT(friendshipsPage_unfollow(QString,UserMgmtWidgetItem*)));
    connect(friendshipsPage, SIGNAL(block(QString,UserMgmtWidgetItem*)), this, SLOT(friendshipsPabe_block(QString,UserMgmtWidgetItem*)));
    connect(friendshipsPage, SIGNAL(stateChanged(QString)), this, SLOT(setState(QString)));
    connect(followersPage, SIGNAL(follow(QString,UserMgmtWidgetItem*)), this, SLOT(followersPage_follow(QString,UserMgmtWidgetItem*)));
    connect(followersPage, SIGNAL(unfollow(QString,UserMgmtWidgetItem*)), this, SLOT(followersPage_unfollow(QString,UserMgmtWidgetItem*)));
    connect(followersPage, SIGNAL(block(QString,UserMgmtWidgetItem*)), this, SLOT(followersPage_block(QString,UserMgmtWidgetItem*)));
    connect(followersPage, SIGNAL(stateChanged(QString)), this, SLOT(setState(QString)));
    connect(blocksPage, SIGNAL(unblock(QString,UserMgmtWidgetItem*)), this, SLOT(blocksPage_unblock(QString,UserMgmtWidgetItem*)));
    connect(blocksPage, SIGNAL(stateChanged(QString)), this, SLOT(setState(QString)));
    connect(UserpicsDownloader::getInstance(), SIGNAL(userpicDownloaded()), this, SLOT(reloadUserpics()));
    this->requestId = 1000;
}


FriendsMgmtDialog::~FriendsMgmtDialog()
{
    pages.clear();
    delete statusBar;
}


void FriendsMgmtDialog::resizeEvent(QResizeEvent *event)
{
    qDebug() << ("FriendsMgmtDialog::resizeEvent()");

    currentPageWidget->updateSize();

    event->ignore();
}


void FriendsMgmtDialog::showEvent(QShowEvent *event)
{
    qDebug() << ("FriendsMgmtDialog::showEvent()");

    Configuration *config = Configuration::getInstance();
    mainWindowAccountId = config->currentAccountId;
    int itemCount = config->accounts.size();
    for(int i=0; i<itemCount; i++) {
	Account *account = config->accounts[i];
	accountComboBox->addItem(QIcon(":/images/" + account->type + ".png"), account->username, QVariant(account->id));
    }

    updateConnects();
    updateTreeView();

    event->accept();
}


void FriendsMgmtDialog::updateConnects()
{
    qDebug() << ("FriendsMgmtDialog::updateConnects()");

    Configuration *config = Configuration::getInstance();

    if(config->currentAccountId == oldAccountId)
	return;

    qDebug() << (QString("old account id: %1").arg(oldAccountId));

    oldAccountId = config->currentAccountId;

    qDebug() << (QString("old account id: %1").arg(oldAccountId));

    if(oldAccountId != -1)
    {
	disconnect(config->accounts[oldAccountId], SIGNAL(friendshipsUpdated(QVector<User>)), 0, 0);
	disconnect(config->accounts[oldAccountId], SIGNAL(followersUpdated(QVector<User>)), 0, 0);
	disconnect(config->accounts[oldAccountId], SIGNAL(blocksUpdated(QVector<User>)), 0, 0);
	disconnect(config->accounts[oldAccountId], SIGNAL(friendshipAdded(User,uint)), 0, 0);
	disconnect(config->accounts[oldAccountId], SIGNAL(friendshipRemoved(User,uint)), 0, 0);
	disconnect(config->accounts[oldAccountId], SIGNAL(blockAdded(User,uint)), 0 , 0);
	disconnect(config->accounts[oldAccountId], SIGNAL(blockRemoved(User,uint)), 0, 0);
	disconnect(config->accounts[oldAccountId], SIGNAL(userListsUpdated(QVector<List>)), 0, 0);
	disconnect(config->accounts[oldAccountId], SIGNAL(listMembersUpdated(QVector<User>,quint64)), 0, 0);
    }

    connect(config->currentAccount(), SIGNAL(friendshipsUpdated(QVector<User>)), friendshipsPage, SLOT(updateItems(QVector<User>)));
    connect(config->currentAccount(), SIGNAL(followersUpdated(QVector<User>)), followersPage, SLOT(updateItems(QVector<User>)));
    connect(config->currentAccount(), SIGNAL(blocksUpdated(QVector<User>)), blocksPage, SLOT(updateItems(QVector<User>)));
    connect(config->currentAccount(), SIGNAL(friendshipAdded(User,uint)), this, SLOT(addFriend(User,uint)));
    connect(config->currentAccount(), SIGNAL(friendshipRemoved(User,uint)), this, SLOT(removeFriend(User,uint)));
    connect(config->currentAccount(), SIGNAL(blockAdded(User,uint)), this, SLOT(addBlock(User,uint)));
    connect(config->currentAccount(), SIGNAL(blockRemoved(User,uint)), this, SLOT(removeBlock(User,uint)));
    connect(config->currentAccount(), SIGNAL(userListsUpdated(QVector<List>)), this, SLOT(updateUserLists(QVector<List>)));
    connect(config->currentAccount(), SIGNAL(listMembersUpdated(QVector<User>,quint64)), this, SLOT(updateListMembers(QVector<User>,quint64)));
}


void FriendsMgmtDialog::friendshipsPage_follow(QString screenName)
{
    qDebug() << ("FriendsMgmtDialog::on_add_friend()");

    requestsFromFrienshipsPage.insert(requestId, 0);

    Configuration *config = Configuration::getInstance();
    config->currentAccount()->createFriendship(screenName, requestId++);
}

void FriendsMgmtDialog::friendshipsPage_unfollow(QString screenName, UserMgmtWidgetItem *item)
{
   qDebug() << ("FriendsMgmtDialog::on_unfollow_friend()");

   requestsFromFrienshipsPage.insert(requestId, item);

   Configuration *config = Configuration::getInstance();
   config->currentAccount()->destroyFriendship(screenName, requestId++);
}

void FriendsMgmtDialog::friendshipsPabe_block(QString screenName, UserMgmtWidgetItem *item)
{
    qDebug() << ("FriendsMgmtDialog::on_block_friend()");

    QMessageBox msgBox;
    msgBox.setText(tr("Blocking will prevent %1 from following you. And you won't see their tweets in your timeline. Are you sure you want to block?").arg(screenName));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Warning);
    int ret = msgBox.exec();

    if(ret == QMessageBox::Ok)
    {
	requestsFromFrienshipsPage.insert(requestId, item);

	Configuration *config = Configuration::getInstance();
	config->currentAccount()->createBlock(screenName, requestId++);
    }
}

void FriendsMgmtDialog::followersPage_follow(QString screenName, UserMgmtWidgetItem *item)
{
    qDebug() << ("FriendsMgmtDialog::on_follow_follower()");

    requestsFromFollowersPage.insert(requestId, item);

    Configuration *config = Configuration::getInstance();
    config->currentAccount()->createFriendship(screenName, requestId++);
}

void FriendsMgmtDialog::followersPage_unfollow(QString screenName, UserMgmtWidgetItem *item)
{
   qDebug() << ("FriendsMgmtDialog::on_unfollow_follower()");

   requestsFromFollowersPage.insert(requestId, item);

   Configuration *config = Configuration::getInstance();
   config->currentAccount()->destroyFriendship(screenName, requestId++);
}

void FriendsMgmtDialog::followersPage_block(QString screenName, UserMgmtWidgetItem *item)
{
    qDebug() << ("FriendsMgmtDialog::on_block_follower()");

    QMessageBox msgBox;
    msgBox.setText(tr("Blocking will prevent %1 from following you. And you won't see their tweets in your timeline. Are you sure you want to block?").arg(screenName));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Warning);
    int ret = msgBox.exec();

    if(ret == QMessageBox::Ok)
    {
	requestsFromFollowersPage.insert(requestId, item);

	Configuration *config = Configuration::getInstance();
	config->currentAccount()->createBlock(screenName, requestId++);
    }
}

void FriendsMgmtDialog::blocksPage_unblock(QString screenName, UserMgmtWidgetItem *item)
{
    qDebug() << ("FriendsMgmtDialog::on_unblock_user()");

    requestsFromBlocksPage.insert(requestId, item);

    Configuration *config = Configuration::getInstance();
    config->currentAccount()->destroyBlock(screenName, requestId++);
}

void FriendsMgmtDialog::addFriend(User user, uint requestId)
{
    qDebug() << ("FriendsMgmtDialog::addFriend()");

    Configuration *config = Configuration::getInstance();
    if(requestsFromFrienshipsPage.contains(requestId)) {
	if(requestsFromFrienshipsPage[requestId] == 0) {
	    config->currentAccount()->receiveFriendships();
	} else {
	    config->currentAccount()->receiveFriendships();
	}
    }
    if(requestsFromFollowersPage.contains(requestId)) {
	config->currentAccount()->receiveFriendships();
    }
    this->setState(tr("Following request sent to %1").arg(user.screen_name));
}

void FriendsMgmtDialog::removeFriend(User user, uint requestId)
{
    qDebug() << ("FriendsMgmtDialog::removeFriend()");

    if(requestsFromFrienshipsPage.contains(requestId)) {
	UserMgmtWidgetItem *item = requestsFromFrienshipsPage[requestId];
	friendshipsPage->removeItem(item);
	friendshipsPage->updateSize();
	this->setState(tr("Unfollowing %1").arg(user.screen_name));
    }
}

void FriendsMgmtDialog::addBlock(User user, uint requestId)
{
    qDebug() << ("FriendsMgmtDialog::addBlock()");

    Configuration *config = Configuration::getInstance();
    if(requestsFromFrienshipsPage.contains(requestId)) {
	UserMgmtWidgetItem *item = requestsFromFrienshipsPage[requestId];
	friendshipsPage->removeItem(item);
	friendshipsPage->updateSize();
//	config->currentAccount()->receiveBlocks();
    }
    if(requestsFromFollowersPage.contains(requestId)) {
	UserMgmtWidgetItem *item = requestsFromFollowersPage[requestId];
	followersPage->removeItem(item);
	followersPage->updateSize();
//	config->currentAccount()->receiveBlocks();
    }
    this->setState(tr("%1 blocked").arg(user.screen_name));
}

void FriendsMgmtDialog::removeBlock(User user, uint requestId)
{
    qDebug() << ("FriendsMgmtDialog::removeBlock()");

    if(requestsFromBlocksPage.contains(requestId))
    {
	UserMgmtWidgetItem *item = requestsFromBlocksPage[requestId];
	blocksPage->removeItem(item);
	blocksPage->updateSize();
	this->setState(tr("%1 unblocked").arg(user.screen_name));
    }
}

void FriendsMgmtDialog::setState(QString state)
{
    statusBar->showMessage(state);
}

bool FriendsMgmtDialog::event(QEvent *e)
{
    if(e->type() == QEvent::StatusTip)
    {
	QStatusTipEvent *statusTipEvent = (QStatusTipEvent*) e;
	statusBar->showMessage(statusTipEvent->tip());
	return true;
    }
    return QDialog::event(e);
}

void FriendsMgmtDialog::on_splitter_splitterMoved(int pos, int index)
{
    qDebug() << ("FriendsMgmtDialog::on_splitter_splitterMoved()");

    currentPageWidget->updateSize();
}


void FriendsMgmtDialog::on_groupsTreeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    qDebug() << ("FriendsMgmtDialog::on_groupsTreeWidget_currentItemChanged()");
    if(!current)
	return;

    QVariant dataCurrent = current->data(0, Qt::UserRole);

    Configuration *config = Configuration::getInstance();
    quint64 currentId = dataCurrent.toULongLong();

    if(currentId > 1000)
    {
	// user page or list selected
//	QTreeWidgetItem *accountItem = current->parent();
//	int accountId = accountItem->data(0, Qt::UserRole).toInt();
//
//	if(accountId != config->currentAccountId || previous == 0)
//	{
//	    config->currentAccountId = accountId;
//	    updateConnects();
//	}

	if(!pages.contains(currentId))
		return;

	currentPageWidget->hide();
	currentPageWidget->clear();
	switch(currentId)
	{
	case 1001:
	    currentPageWidget = friendshipsPage;
	    config->currentAccount()->receiveFriendships();
	    break;
	case 1002:
	    currentPageWidget = followersPage;
	    config->currentAccount()->receiveFollowers();
	    break;
	case 1003:
	    currentPageWidget = blocksPage;
	    config->currentAccount()->receiveBlocks();
	    break;
	default:
	    currentPageWidget = pages[currentId];
	    config->currentAccount()->receiveListMembers(currentId);
	    break;
	}
	currentPageWidget->show();
	currentPageWidget->updateSize();
    }
}

void FriendsMgmtDialog::on_closeButtonBox_rejected()
{
    qDebug() << ("FriendsMgmtDialog::on_close()");

    Configuration *config = Configuration::getInstance();

    if(mainWindowAccountId == config->currentAccountId)
	return;

    config->currentAccountId = mainWindowAccountId;

    updateConnects();
}

void FriendsMgmtDialog::reloadUserpics()
{
    qDebug() << ("FriendsMgmtDialog::reloadUserpics()");

    currentPageWidget->reloadUserpics();
}

void FriendsMgmtDialog::updateUserLists(const QVector<List> &lists)
{
    qDebug() << ("FriendsMgmtDialog::updateUserLists()");

    for(int i=0;i<lists.size();i++) {
	List list = lists[i];

//	QTreeWidgetItem *accountItem;
//	for(int j=0;j<treeItems.size();j++) {
//	    QTreeWidgetItem *currItem = treeItems[j];
//	    int accountId = currItem->data(0, Qt::UserRole).toInt();
//
//	    if(accountId == list.account->id) {
//		accountItem = currItem;
//		break;
//	    }
//	}
	QTreeWidgetItem *listItem = new QTreeWidgetItem(groupsTreeWidget, QStringList(list.name));
	listItem->setIcon(0, QIcon(":/images/list.png"));
	listItem->setData(0, Qt::UserRole, list.id);
	treeItems.append(listItem);

	if(!pages.contains(list.id)) {
	    pages[list.id] = new FriendshipsMgmtPage(this);
	    pages[list.id]->hide();
	    pagesContainerLayout->addWidget(pages[list.id]);
	}
    }
}

void FriendsMgmtDialog::updateTreeView()
{
    treeItems.clear();
    groupsTreeWidget->clear();

    QTreeWidgetItem *followingItem = new QTreeWidgetItem(groupsTreeWidget, QStringList(tr("following")));
    followingItem->setIcon(0, QIcon(":/images/following.png"));
    followingItem->setData(0, Qt::UserRole, 1001);
    treeItems.append(followingItem);

    QTreeWidgetItem *followersItem = new QTreeWidgetItem(groupsTreeWidget, QStringList(tr("followers")));
    followersItem->setIcon(0, QIcon(":/images/following.png"));
    followersItem->setData(0, Qt::UserRole, 1002);
    treeItems.append(followersItem);

    QTreeWidgetItem *blocksItem = new QTreeWidgetItem(groupsTreeWidget, QStringList(tr("blocks")));
    blocksItem->setData(0, Qt::UserRole, 1003);
    blocksItem->setIcon(0, QIcon(":/images/block.png"));
    treeItems.append(blocksItem);

    groupsTreeWidget->insertTopLevelItems(0, treeItems);
    groupsTreeWidget->expandAll();

    Configuration *config = Configuration::getInstance();
    config->currentAccount()->receiveUserLists();

    groupsTreeWidget->setCurrentItem(followingItem);
}

void FriendsMgmtDialog::updateListMembers(const QVector<User> &listMembers, quint64 listId)
{
    pages[listId]->updateItems(listMembers);
}
#endif

void FriendsMgmtDialog::on_accountComboBox_currentIndexChanged(int index)
{
    int accountId = accountComboBox->itemData(index).toInt();
    Configuration *config = Configuration::getInstance();
    if(accountId != config->currentAccountId)
    {
	config->currentAccountId = accountId;
	updateConnects();

	updateTreeView();
    }
}
