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
//        this->userpicsDownloader = userpicsDownloader;

    oldAccountId = -1;
    firstRun = true;

    tabWidget->removeTab(0);
    tabWidget->removeTab(0);
    tabWidget->removeTab(0);
    pages.push_back(friendshipsPage = new FriendshipsMgmtPage(this));
    pages.push_back(followersPage = new FollowersMgmtPage(this));
    pages.push_back(blocksPage = new BlocksMgmtPage(this));

    for(int i=0; i<pages.size(); i++)
	tabWidget->addTab(pages[i], pages[i]->title());
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

    if(firstRun)
    {
	Configuration *config = Configuration::getInstance();
	oldAccountId = config->currentAccountId;

	if(config->currentAccountId != -1)
	{
	    connect(config->currentAccount(), SIGNAL(friendshipsUpdated(QVector<Message>)), friendshipsPage, SLOT(updateItems(QVector<Message>)));
	    connect(config->currentAccount(), SIGNAL(followersUpdated(QVector<Message>)), followersPage, SLOT(updateItems(QVector<Message>)));
	    connect(config->currentAccount(), SIGNAL(blocksUpdated(QVector<Message>)), blocksPage, SLOT(updateItems(QVector<Message>)));
	}
	firstRun = false;
    }
    else
	updateConnects();

    Configuration *config = Configuration::getInstance();
    config->currentAccount()->receiveFriendships();
    config->currentAccount()->receiveFollowers();
    config->currentAccount()->receiveBlocks();

    tabWidget->setCurrentIndex(0);

    event->accept();
}

void FriendsMgmtDialog::unfollow(const QString screenName)
{
    qDebug() << ("FriendsMgmtDialog::unfollow()");

    Configuration *config = Configuration::getInstance();

    config->currentAccount()->destroyFriendship(screenName);
}

void FriendsMgmtDialog::follow(const QString screenName)
{
    qDebug() << ("FriendsMgmtDialog::follow()");

    Configuration *config = Configuration::getInstance();

    config->currentAccount()->createFriendship(screenName);
}

void FriendsMgmtDialog::block(const QString screenName)
{
    qDebug() << ("FriendsMgmtDialog::block()");

    QMessageBox msgBox;
    msgBox.setText(tr("Blocking will prevent %1 from following you. And you won't see their tweets in your timeline. Are you sure you want to block?").arg(screenName));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Warning);
    int ret = msgBox.exec();

    if(ret == QMessageBox::Ok)
    {
	Configuration *config = Configuration::getInstance();

	config->currentAccount()->createBlock(screenName);
    }
}

void FriendsMgmtDialog::unblock(const QString screenName)
{
    qDebug() << ("FriendsMgmtDialog::unblock()");

    Configuration *config = Configuration::getInstance();

    config->currentAccount()->destroyBlock(screenName);
}

//void FriendsMgmtDialog::saveState()
//{
//    for(int i=0; i<MGMT_TABS; i++)
//    {
//        friendsMgmtTabs[i].getFriendsMgmtWidget()->resize(friendsMgmtTabs[i].getScrollArea()->width() - friendsMgmtTabs[i].getScrollArea()->verticalScrollBar()->width() -5, 500);
//    }
//}

void FriendsMgmtDialog::on_closePushButton_pressed()
{
    hide();
}

//void FriendsMgmtDialog::followImpl(QString screenName)
//{
//    for(int i=0; i<screenName.length(); i++)
//    {
//	if(!TwitterWidgetItem::isUsernameChar(screenName[i]))
//	{
//	    QString character = QString(screenName[i]);
//	    cout << "Screenname contains illegal character: " << character.toStdString() << endl;
//
//	    stateLabel->setText(tr("Screenname contains illegal character: %1").arg(character));
//
//	    return;
//	}
//    }
//
//    MainWindow *mainWindow = MainWindow::getInstance();
//
//    twitter->createFriendship(screenName, mainWindow->username, mainWindow->password);
//
//    newFriendLineEdit->setText("");
//
//    cout << "Following: " << screenName.toStdString() << endl;
//    this->stateLabel->setText("Following Request sent to " + screenName);
//
//    this->tabWidget->setCurrentIndex(FRIENDS);
//}

void FriendsMgmtDialog::updateConnects()
{
    qDebug() << ("FriendsMgmtDialog::updateConnects()");

    Configuration *config = Configuration::getInstance();

    if(config->currentAccountId == oldAccountId)
	return;

    oldAccountId = config->currentAccountId;

    disconnect(config->accounts[oldAccountId], SIGNAL(friendshipsUpdated(QVector<Message>)), 0, 0);
    connect(config->currentAccount(), SIGNAL(friendshipsUpdated(QVector<Message>)), friendshipsPage, SLOT(updateItems(QVector<Message>)));
    disconnect(config->accounts[oldAccountId], SIGNAL(followersUpdated(QVector<Message>)), 0, 0);
    connect(config->currentAccount(), SIGNAL(followersUpdated(QVector<Message>)), followersPage, SLOT(updateItems(QVector<Message>)));
    disconnect(config->accounts[oldAccountId], SIGNAL(blocksUpdated(QVector<Message>)), 0, 0);
    connect(config->currentAccount(), SIGNAL(blocksUpdated(QVector<Message>)), blocksPage, SLOT(updateItems(QVector<Message>)));
}

void FriendsMgmtDialog::on_tabWidget_currentChanged(int index)
{
    qDebug() << ("FriendsMgmtDialog::on_tabWidget_currentChanged()");

    if((index >= 0) && (index < pages.size()) && pages[index])
    {
	pages[index]->updateSize();
    }
}
#endif
