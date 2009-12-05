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
    accountsTreeWidget->setHeaderLabels(QStringList(tr("accounts")));
    this->oldAccountId = -1;

    pages.push_back(friendshipsPage = new FriendshipsMgmtPage(this));
    pages.push_back(followersPage = new FollowersMgmtPage(this));
    pages.push_back(blocksPage = new BlocksMgmtPage(this));
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
    delete friendshipsPage;
    delete followersPage;
    delete blocksPage;
    delete statusBar;
}


void FriendsMgmtDialog::resizeEvent(QResizeEvent *event)
{
    qDebug() << ("FriendsMgmtDialog::resizeEvent()");

    for(int i=0; i<pages.size(); i++)
	pages[i]->updateSize();

    event->ignore();
}


void FriendsMgmtDialog::showEvent(QShowEvent *event)
{
    qDebug() << ("FriendsMgmtDialog::showEvent()");

    Configuration *config = Configuration::getInstance();
    mainWindowAccountId = config->currentAccountId;
    int itemCount = config->accounts.size();
    treeItems.clear();
    accountsTreeWidget->clear();
    QTreeWidgetItem *currentItem;
    for(int i=0; i<itemCount; i++)
    {
	Account *account = config->accounts[i];
	QTreeWidgetItem *accountItem = new QTreeWidgetItem(accountsTreeWidget, QStringList(account->username));
	accountItem->setIcon(0, QIcon(":/images/" + account->type + ".png"));
	accountItem->setData(0, Qt::UserRole, account->id);
	treeItems.append(accountItem);

	QTreeWidgetItem *friendsItem = new QTreeWidgetItem(accountItem, QStringList(tr("friends")));
	friendsItem->setData(0, Qt::UserRole, 1001);
	treeItems.append(friendsItem);

	QTreeWidgetItem *followersItem = new QTreeWidgetItem(accountItem, QStringList(tr("followers")));
	followersItem->setData(0, Qt::UserRole, 1002);
	treeItems.append(followersItem);

	QTreeWidgetItem *blocksItem = new QTreeWidgetItem(accountItem, QStringList(tr("blocks")));
	blocksItem->setData(0, Qt::UserRole, 1003);
	blocksItem->setIcon(0, QIcon(":/images/block.png"));
	treeItems.append(blocksItem);

	// insert lists of the account
//	for(int i=0; i<list; i++) {}

	if(config->currentAccount() == account)
	    currentItem = friendsItem;
    }

    accountsTreeWidget->insertTopLevelItems(0, treeItems);

    accountsTreeWidget->expandAll();

    if(itemCount > 0)
    {
	accountsTreeWidget->setCurrentItem(currentItem);
	on_accountsTreeWidget_currentItemChanged(currentItem, (QTreeWidgetItem*) 0);
    }

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
    }

    connect(config->currentAccount(), SIGNAL(friendshipsUpdated(QVector<User>)), friendshipsPage, SLOT(updateItems(QVector<User>)));
    connect(config->currentAccount(), SIGNAL(followersUpdated(QVector<User>)), followersPage, SLOT(updateItems(QVector<User>)));
    connect(config->currentAccount(), SIGNAL(blocksUpdated(QVector<User>)), blocksPage, SLOT(updateItems(QVector<User>)));
    connect(config->currentAccount(), SIGNAL(friendshipAdded(User,uint)), this, SLOT(addFriend(User,uint)));
    connect(config->currentAccount(), SIGNAL(friendshipRemoved(User,uint)), this, SLOT(removeFriend(User,uint)));
    connect(config->currentAccount(), SIGNAL(blockAdded(User,uint)), this, SLOT(addBlock(User,uint)));
    connect(config->currentAccount(), SIGNAL(blockRemoved(User,uint)), this, SLOT(removeBlock(User,uint)));
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


void FriendsMgmtDialog::on_accountsTreeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    qDebug() << ("FriendsMgmtDialog::on_accountsTreeWidget_currentItemChanged()");
    if(!current)
	return;

    QVariant dataCurrent = current->data(0, Qt::UserRole);

    Configuration *config = Configuration::getInstance();
    int currentId = dataCurrent.toInt();

    if(currentId > 1000)
    {
	// user page or list selected
	QTreeWidgetItem *accountItem = current->parent();
	int accountId = accountItem->data(0, Qt::UserRole).toInt();

	if(accountId != config->currentAccountId || previous == 0)
	{
	    config->currentAccountId = accountId;
	    updateConnects();
	}

	switch(currentId)
	{
	case 1001:
	    config->currentAccount()->receiveFriendships();
	    currentPageWidget = friendshipsPage;
	    friendshipsPage->show();
	    followersPage->hide();
	    blocksPage->hide();
	    currentPageWidget->updateSize();
	    break;
	case 1002:
	    config->currentAccount()->receiveFollowers();
	    currentPageWidget = followersPage;
	    friendshipsPage->hide();
	    followersPage->show();
	    blocksPage->hide();
	    currentPageWidget->updateSize();
	    break;
	case 1003:
	    config->currentAccount()->receiveBlocks();
	    currentPageWidget = blocksPage;
	    friendshipsPage->hide();
	    followersPage->hide();
	    blocksPage->show();
	    currentPageWidget->updateSize();
	    break;
	default:
	    break;
	}
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
#endif
