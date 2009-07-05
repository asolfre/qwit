/*!
 *  @file
 *  @author Artem Iglikov <artem.iglikov@gmail.com>
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
 *  MainWindow class implementation
 */

#ifndef MainWindow_cpp
#define MainWindow_cpp

#include "QwitHeaders.h"

#include "TwitterWidget.cpp"
#include "MainWindow.h"
#include "UserpicsDownloader.h"
#include "Configuration.h"
#include "TwitPicDialog.h"

MainWindow* MainWindow::instance = 0;

MainWindow* MainWindow::getInstance() {
	if (!instance) {
		instance = new MainWindow();
	}
	return instance;
}

MainWindow::MainWindow(QWidget *parent): QDialog(parent) {
	qDebug() << ("MainWindow::MainWindow()");

	instance = this;

	setupUi(this);
	
	greetingMessageLabel = new QLabel(this);
	leftCharactersNumberLabel = new QLabel(this);

	messageTextEdit = new MessageTextEdit(this);
	messageTextEdit->setObjectName(QString::fromUtf8("messageTextEdit"));
	messageHorizontalLayout->insertWidget(0, messageTextEdit);
	connect(messageTextEdit, SIGNAL(leftCharsNumberChanged(int)), this, SLOT(leftCharsNumberChanged(int)));
	connect(this, SIGNAL(retweet(const Message &)), messageTextEdit, SLOT(retweet(const Message &)));
	connect(this, SIGNAL(reply(const Message &)), messageTextEdit, SLOT(reply(const Message &)));
	
	lastMessageLabel->setTextFormat(Qt::AutoText);
	lastMessageLabel->setOpenExternalLinks(true);
	
	optionsDialog = new OptionsDialog(this);
	connect(optionsDialog, SIGNAL(accepted()), this, SLOT(saveOptions()));
	connect(optionsDialog, SIGNAL(rejected()), this, SLOT(resetOptionsDialog()));
	
	aboutDialog = new AboutDialog(this);
	
	directMessageDialog = new DirectMessageDialog(this);
	connect(directMessageDialog , SIGNAL(accepted()), this, SLOT(sendDirectMessage()));

	connect(twitPicButton, SIGNAL(clicked()), this, SLOT(postTwitPic()));

	connect(refreshToolButton, SIGNAL(pressed()), this, SLOT(refresh()));
	connect(optionsToolButton, SIGNAL(pressed()), this, SLOT(showOptionsDialog()));
	connect(aboutToolButton, SIGNAL(pressed()), aboutDialog, SLOT(show()));
	connect(exitToolButton, SIGNAL(pressed()), this, SLOT(quit()));

	connect(&accountsButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(accountButtonClicked(int)));
	
	connect(mainTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	mainTabWidget->removeTab(0);
	
	accountsLayout = 0;
	
	redrawTimer = new QTimer(this);
	connect(redrawTimer, SIGNAL(timeout()), this, SLOT(redrawPages()));
	redrawTimer->start(10000);
	
	updateTimer = new QTimer(this);
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateAll()));

	setupTrayIcon();
	loadState();
	if (accountsButtons.size() > 0) {
		accountsButtons[0]->setChecked(true);
		accountsButtons[0]->click();
	}
	
	acceptClose = false;
	
	connect(UserpicsDownloader::getInstance(), SIGNAL(userpicDownloaded()), this, SLOT(reloadUserpics()));
	
	connect(UrlShortener::getInstance(), SIGNAL(urlShortened(const QString &)), messageTextEdit, SLOT(insertUrl(const QString &)));

	updateAll();

}

void MainWindow::leftCharsNumberChanged(int count) {
	leftCharactersNumberLabel->setText(QString::number(count));
}

void MainWindow::loadState() {
	qDebug() << ("MainWindow::loadState()");

	Configuration *config = Configuration::getInstance();
	
	config->load();

	updateState();
	
	for (int i = 0; i < config->accounts.size(); ++i) {
		addAccountButton(config->accounts[i]);
	}
	
	resetOptionsDialog();
}

void MainWindow::saveState() {
	qDebug() << ("MainWindow::saveState()");

	Configuration *config = Configuration::getInstance();
	
	config->position = pos();
	config->size = size();
	
	config->save();
}

void MainWindow::saveOptions() {
	qDebug() << ("MainWindow::saveOptions()");

	Configuration *config = Configuration::getInstance();

	config->showGreetingMessage = (optionsDialog->showGreetingMessageCheckBox->checkState() == Qt::Checked);
	config->greetingMessage = optionsDialog->greetingMessageLineEdit->text();
	config->showLeftCharactersNumber = (optionsDialog->showLeftCharactersNumberCheckBox->checkState() == Qt::Checked);
	config->showLastMessage = (optionsDialog->showLastMessageCheckBox->checkState() == Qt::Checked);
	config->messagesPerPage = optionsDialog->messagesPerPageLineEdit->text().toInt();
	config->messagesInPopup = optionsDialog->messagesInPopupLineEdit->text().toInt();
	config->retweetTag = optionsDialog->retweetTagLineEdit->text();
	config->retweetTagAfterText = (optionsDialog->retweetTagAfterTextCheckBox->checkState() == Qt::Checked);
	config->placeControlsVertically = (optionsDialog->placeControlsVerticallyCheckBox->checkState() == Qt::Checked);
	config->placeTabsVertically = (optionsDialog->placeTabsVerticallyCheckBox->checkState() == Qt::Checked);
	config->showMessagesInTray = (optionsDialog->showMessagesInTrayCheckBox->checkState() == Qt::Checked);
	config->placeUsernameUnderAvatar = (optionsDialog->placeUsernameUnderAvatarCheckBox->checkState() == Qt::Checked);
	
	config->showHomeTab = (optionsDialog->homeTabCheckBox->checkState() == Qt::Checked);
	config->updateHomeTabAlways = (optionsDialog->homeTabUpdateAlwaysCheckBox->checkState() == Qt::Checked);
	config->showPublicTab = (optionsDialog->publicTabCheckBox->checkState() == Qt::Checked);
	config->updatePublicTabAlways = (optionsDialog->publicTabUpdateAlwaysCheckBox->checkState() == Qt::Checked);
	config->showRepliesTab = (optionsDialog->repliesTabCheckBox->checkState() == Qt::Checked);
	config->updateRepliesTabAlways = (optionsDialog->repliesTabUpdateAlwaysCheckBox->checkState() == Qt::Checked);
	config->showCustomTab = (optionsDialog->customTabCheckBox->checkState() == Qt::Checked);
	config->updateCustomTabAlways = (optionsDialog->customTabUpdateAlwaysCheckBox->checkState() == Qt::Checked);
	config->showInboxTab = (optionsDialog->inboxTabCheckBox->checkState() == Qt::Checked);
	config->updateInboxTabAlways = (optionsDialog->inboxTabUpdateAlwaysCheckBox->checkState() == Qt::Checked);
	config->showOutboxTab = (optionsDialog->outboxTabCheckBox->checkState() == Qt::Checked);
	config->updateOutboxTabAlways = (optionsDialog->outboxTabUpdateAlwaysCheckBox->checkState() == Qt::Checked);
	config->showSearchTab = (optionsDialog->searchTabCheckBox->checkState() == Qt::Checked);
	config->updateSearchTabAlways = (optionsDialog->searchTabUpdateAlwaysCheckBox->checkState() == Qt::Checked);
	config->showFavoritesTab = (optionsDialog->favoritesTabCheckBox->checkState() == Qt::Checked);
	config->updateFavoritesTabAlways = (optionsDialog->favoritesTabUpdateAlwaysCheckBox->checkState() == Qt::Checked);

	config->useProxy = (optionsDialog->useProxyCheckBox->checkState() == Qt::Checked);
	config->proxyAddress = optionsDialog->proxyAddressLineEdit->text();
	config->proxyPort = optionsDialog->proxyPortLineEdit->text().toInt();
	config->proxyUsername = optionsDialog->proxyUsernameLineEdit->text();
	config->proxyPassword = optionsDialog->proxyPasswordLineEdit->text();
	
	config->urlShortener = Configuration::UrlShorteners[optionsDialog->urlShortenersComboBox->currentIndex()];

	saveState();
	updateState();
}

void MainWindow::updateState() {
	qDebug() << ("MainWindow::updateState()");

	Configuration *config = Configuration::getInstance();

	if ((greetingMessageLabel->isVisible() != config->showGreetingMessage) || (leftCharactersNumberLabel->isVisible() != config->showLeftCharactersNumber)) {
		if (greetingMessageLabel->isVisible()) {
			QLayout *layout = (QLayout*)verticalLayout->itemAt(0);
			verticalLayout->removeItem(verticalLayout->itemAt(0));
			delete layout;
		} else {
			toolbuttonsVerticalLayout->removeWidget(leftCharactersNumberLabel);
		}
		if (config->showGreetingMessage) {
			QHBoxLayout *layout = new QHBoxLayout(this);
			layout->addWidget(greetingMessageLabel);
			layout->addStretch();
			if (config->showLeftCharactersNumber) {
				layout->addWidget(leftCharactersNumberLabel);
			}
			verticalLayout->insertLayout(0, layout);
		} else {
			if (config->showLeftCharactersNumber) {
				toolbuttonsVerticalLayout->insertWidget(0, leftCharactersNumberLabel);
			}
		}
		greetingMessageLabel->setVisible(config->showGreetingMessage);
		leftCharactersNumberLabel->setVisible(config->showLeftCharactersNumber);
	}

	greetingMessageLabel->setText(config->greetingMessage);
	leftCharactersNumberLabel->setText(QString::number(MessageTextEdit::MaxMessageCharacters - messageTextEdit->toPlainText().length()));
	lastMessageLabel->setVisible(config->showLastMessage);

	if (config->placeTabsVertically) {
		mainTabWidget->setTabPosition(QTabWidget::West);
	} else {
		mainTabWidget->setTabPosition(QTabWidget::South);
	}
	
	for (int i = 0; i < pages.size(); ++i) {
		mainTabWidget->removeTab(i);
		delete pages[i];
		pages[i] = 0;
	}
	pages.clear();

	homePage = 0;
	repliesPage = 0;
	publicPage = 0;
	favoritesPage = 0;
	inboxPage = 0;
	outboxPage = 0;
	searchPage = 0;
	
	if (config->showHomeTab) {
		pages.push_back(homePage = new HomePage());
	}
	if (config->showPublicTab) {
		pages.push_back(publicPage = new PublicPage());
	}
	if (config->showRepliesTab) {
		pages.push_back(repliesPage = new RepliesPage());
	}
	if (config->showFavoritesTab) {
		pages.push_back(favoritesPage = new FavoritesPage());
	}
	if (config->showInboxTab) {
		pages.push_back(inboxPage = new InboxPage());
	}
	if (config->showOutboxTab) {
		pages.push_back(outboxPage = new OutboxPage());
	}
	if (config->showSearchTab) {
		pages.push_back(searchPage = new SearchPage());
	}

	for (int i = 0; i < pages.size(); ++i) {
		mainTabWidget->addTab(pages[i], pages[i]->title());
	}
	
	updateCurrentAccount(config->currentAccountId);
	
	move(config->position);
	resize(config->size);
	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->updateSize();
	}

	updateTimer->start(300000);
}

void MainWindow::resetOptionsDialog() {
	qDebug() << ("MainWindow::resetOptionsDialog()");

	Configuration *config = Configuration::getInstance();
	
// User interface
	optionsDialog->showGreetingMessageCheckBox->setCheckState(config->showGreetingMessage ? Qt::Checked : Qt::Unchecked);
	optionsDialog->greetingMessageLineEdit->setText(config->greetingMessage);
	optionsDialog->showLeftCharactersNumberCheckBox->setCheckState(config->showLeftCharactersNumber ? Qt::Checked : Qt::Unchecked);
	optionsDialog->showLastMessageCheckBox->setCheckState(config->showLastMessage ? Qt::Checked : Qt::Unchecked);
	optionsDialog->messagesPerPageLineEdit->setText(QString::number(config->messagesPerPage));
	optionsDialog->messagesInPopupLineEdit->setText(QString::number(config->messagesInPopup));
	optionsDialog->retweetTagAfterTextCheckBox->setCheckState(config->retweetTagAfterText ? Qt::Checked : Qt::Unchecked);
	optionsDialog->retweetTagLineEdit->setText(config->retweetTag);
	optionsDialog->placeControlsVerticallyCheckBox->setCheckState(config->placeControlsVertically ? Qt::Checked : Qt::Unchecked);
	optionsDialog->placeTabsVerticallyCheckBox->setCheckState(config->placeTabsVertically ? Qt::Checked : Qt::Unchecked);
	optionsDialog->showMessagesInTrayCheckBox->setCheckState(config->showMessagesInTray ? Qt::Checked : Qt::Unchecked);
	optionsDialog->placeUsernameUnderAvatarCheckBox->setCheckState(config->placeUsernameUnderAvatar ? Qt::Checked : Qt::Unchecked);
	
	optionsDialog->homeTabCheckBox->setCheckState(config->showHomeTab ? Qt::Checked : Qt::Unchecked);
	optionsDialog->homeTabUpdateAlwaysCheckBox->setCheckState(config->updateHomeTabAlways ? Qt::Checked : Qt::Unchecked);
	optionsDialog->publicTabCheckBox->setCheckState(config->showPublicTab ? Qt::Checked : Qt::Unchecked);
	optionsDialog->publicTabUpdateAlwaysCheckBox->setCheckState(config->updatePublicTabAlways ? Qt::Checked : Qt::Unchecked);
	optionsDialog->repliesTabCheckBox->setCheckState(config->showRepliesTab ? Qt::Checked : Qt::Unchecked);
	optionsDialog->repliesTabUpdateAlwaysCheckBox->setCheckState(config->updateRepliesTabAlways ? Qt::Checked : Qt::Unchecked);
	optionsDialog->customTabCheckBox->setCheckState(config->showCustomTab ? Qt::Checked : Qt::Unchecked);
	optionsDialog->customTabUpdateAlwaysCheckBox->setCheckState(config->updateCustomTabAlways ? Qt::Checked : Qt::Unchecked);
	optionsDialog->inboxTabCheckBox->setCheckState(config->showInboxTab ? Qt::Checked : Qt::Unchecked);
	optionsDialog->inboxTabUpdateAlwaysCheckBox->setCheckState(config->updateInboxTabAlways ? Qt::Checked : Qt::Unchecked);
	optionsDialog->outboxTabCheckBox->setCheckState(config->showOutboxTab ? Qt::Checked : Qt::Unchecked);
	optionsDialog->outboxTabUpdateAlwaysCheckBox->setCheckState(config->updateOutboxTabAlways ? Qt::Checked : Qt::Unchecked);
	optionsDialog->searchTabCheckBox->setCheckState(config->showSearchTab ? Qt::Checked : Qt::Unchecked);
	optionsDialog->searchTabUpdateAlwaysCheckBox->setCheckState(config->updateSearchTabAlways ? Qt::Checked : Qt::Unchecked);
	optionsDialog->favoritesTabCheckBox->setCheckState(config->showFavoritesTab ? Qt::Checked : Qt::Unchecked);
	optionsDialog->favoritesTabUpdateAlwaysCheckBox->setCheckState(config->updateFavoritesTabAlways ? Qt::Checked : Qt::Unchecked);
	
// Accounts
	optionsDialog->accountsListWidget->clear();
	for (int i = 0; i < config->accounts.size(); ++i) {
		optionsDialog->accountsListWidget->addItem(Configuration::ServicesNames[config->accounts[i]->type] + ": " + config->accounts[i]->username);
	}

// Connection
	optionsDialog->useProxyCheckBox->setCheckState(config->useProxy ? Qt::Checked : Qt::Unchecked);
	optionsDialog->proxyAddressLineEdit->setText(config->proxyAddress);
	optionsDialog->proxyPortLineEdit->setText(QString::number(config->proxyPort));
	optionsDialog->proxyUsernameLineEdit->setText(config->proxyUsername);
	optionsDialog->proxyPasswordLineEdit->setText(config->proxyPassword);

// UrlShortener
	optionsDialog->urlShortenersComboBox->setCurrentIndex(Configuration::UrlShortenersIds[config->urlShortener]);

}

void MainWindow::addAccountButton(Account *account) {
	qDebug() << ("MainWindow::addAccountButton()");

	QToolButton *accountButton = new QToolButton(this);
	accountButton->setIcon(QIcon(":/images/" + account->type + ".png"));
	accountButton->setText(account->username);
	accountButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	accountButton->setAutoRaise(true);
	accountButton->setCheckable(true);
	accountsButtons.push_back(accountButton);
	accountsButtonGroup.addButton(accountButton, accountsButtons.size() - 1);
	if (accountsButtons.size() == 1) {
		accountsButtons[0]->setVisible(false);
	}
	if (accountsButtons.size() == 2) {
		accountsLayout = new QHBoxLayout();
		accountsLayout->addWidget(accountsButtons[0]);
		accountsButtons[0]->setVisible(true);
		verticalLayout->insertLayout(verticalLayout->count() - 1, accountsLayout);
		accountsLayout->addStretch();
	}
	if (accountsButtons.size() >= 2) {
		accountsLayout->insertWidget(accountsLayout->count() - 1, accountButton);
	}
}

void MainWindow::updateAccountButton(Account *account) {
	qDebug() << ("MainWindow::updateAccountButton()");

	accountsButtons[account->id]->setText(account->username);
}

void MainWindow::deleteAccountButton(Account *account) {
	qDebug() << ("MainWindow::deleteAccountButton()");

	if (accountsLayout) {
		accountsLayout->removeWidget(accountsButtons[account->id]);
	}
	int checkedId = accountsButtonGroup.checkedId();
	accountsButtonGroup.removeButton(accountsButtons[account->id]);
	delete accountsButtons[account->id];
	accountsButtons.erase(accountsButtons.begin() + account->id);
	if (accountsButtons.size() <= 1) {
		if (accountsButtons.size() == 1) {
			accountsLayout->removeWidget(accountsButtons[0]);
			accountsButtons[0]->setVisible(false);
		}
		verticalLayout->removeItem(accountsLayout);
		delete accountsLayout;
		accountsLayout = 0;
	}
	for (int i = 0; i < accountsButtons.size(); ++i) {
		accountsButtonGroup.removeButton(accountsButtons[i]);
	}
	for (int i = 0; i < accountsButtons.size(); ++i) {
		accountsButtonGroup.addButton(accountsButtons[i], i);
	}
	if (checkedId == account->id) {
		checkedId = min(accountsButtons.size() - 1, checkedId);
	} else if (checkedId > account->id) {
		--checkedId;
	}
	if (checkedId != -1) {
		accountsButtons[checkedId]->setChecked(true);
		accountButtonClicked(checkedId);
	}
}

void MainWindow::showOptionsDialog() {
	qDebug() << ("MainWindow::showOptionsDialog()");

	resetOptionsDialog();
	optionsDialog->showNormal();
}

void MainWindow::accountButtonClicked(int id) {
	qDebug() << ("MainWindow::accountButtonClicked()");

	updateCurrentAccount(id);
}

void MainWindow::updateCurrentAccount(int id) {
	qDebug() << ("MainWindow::updateCurrentAccount()");

	Configuration *config = Configuration::getInstance();
	if (config->currentAccountId == -1) {
		return;
	}
	
	int oldAccountId = config->currentAccountId;
	config->currentAccountId = id;
	if (homePage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(friendsMessagesUpdated(const QVector<Message> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(friendsMessagesUpdated(const QVector<Message> &, Account *)), homePage, SLOT(updateItems(const QVector<Message> &, Account *)));
		homePage->updateItems(config->currentAccount()->friendsMessages, config->currentAccount());
		disconnect(config->accounts[oldAccountId], SIGNAL(previousFriendsMessagesReceived()), 0, 0);
		connect(config->currentAccount(), SIGNAL(previousFriendsMessagesReceived()), homePage->twitterWidget, SLOT(enableMoreButton()));
	}
	if (repliesPage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(repliesUpdated(const QVector<Message> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(repliesUpdated(const QVector<Message> &, Account *)), repliesPage, SLOT(updateItems(const QVector<Message> &, Account *)));
		repliesPage->updateItems(config->currentAccount()->replies, config->currentAccount());
		disconnect(config->accounts[oldAccountId], SIGNAL(previousRepliesReceived()), 0, 0);
		connect(config->currentAccount(), SIGNAL(previousRepliesReceived()), repliesPage->twitterWidget, SLOT(enableMoreButton()));
	}
	if (publicPage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(publicMessagesUpdated(const QVector<Message> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(publicMessagesUpdated(const QVector<Message> &, Account *)), publicPage, SLOT(updateItems(const QVector<Message> &, Account *)));
		publicPage->updateItems(config->currentAccount()->publicMessages, config->currentAccount());
		disconnect(config->accounts[oldAccountId], SIGNAL(previousPublicMessagesReceived()), 0, 0);
		connect(config->currentAccount(), SIGNAL(previousPublicMessagesReceived()), publicPage->twitterWidget, SLOT(enableMoreButton()));
	}
	if (favoritesPage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(favoritesUpdated(const QVector<Message> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(favoritesUpdated(const QVector<Message> &, Account *)), favoritesPage, SLOT(updateItems(const QVector<Message> &, Account *)));
		favoritesPage->updateItems(config->currentAccount()->favorites, config->currentAccount());
		disconnect(config->accounts[oldAccountId], SIGNAL(previousFavoritesReceived()), 0, 0);
		connect(config->currentAccount(), SIGNAL(previousFavoritesReceived()), favoritesPage->twitterWidget, SLOT(enableMoreButton()));
	}
	if (inboxPage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(inboxMessagesUpdated(const QVector<Message> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(inboxMessagesUpdated(const QVector<Message> &, Account *)), inboxPage, SLOT(updateItems(const QVector<Message> &, Account *)));
		inboxPage->updateItems(config->currentAccount()->inboxMessages, config->currentAccount());
		disconnect(config->accounts[oldAccountId], SIGNAL(previousInboxMessagesReceived()), 0, 0);
		connect(config->currentAccount(), SIGNAL(previousInboxMessagesReceived()), inboxPage->twitterWidget, SLOT(enableMoreButton()));
	}
	if (outboxPage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(outboxMessagesUpdated(const QVector<Message> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(outboxMessagesUpdated(const QVector<Message> &, Account *)), outboxPage, SLOT(updateItems(const QVector<Message> &, Account *)));
		outboxPage->updateItems(config->currentAccount()->outboxMessages, config->currentAccount());
		disconnect(config->accounts[oldAccountId], SIGNAL(previousOutboxMessagesReceived()), 0, 0);
		connect(config->currentAccount(), SIGNAL(previousOutboxMessagesReceived()), outboxPage->twitterWidget, SLOT(enableMoreButton()));
	}
	if (searchPage) {
//		disconnect(config->accounts[oldAccountId], SIGNAL(inboxMessagesUpdated(const QVector<Message> &, Account *)), 0, 0);
//		connect(config->currentAccount(), SIGNAL(inboxMessagesUpdated(const QVector<Message> &, Account *)), inboxPage, SLOT(updateItems(const QVector<Message> &, Account *)));
//		inboxPage->updateItems(config->currentAccount()->inboxMessages, config->currentAccount());
//		disconnect(config->accounts[oldAccountId], SIGNAL(previousInboxMessagesReceived()), 0, 0);
//		connect(config->currentAccount(), SIGNAL(previousInboxMessagesReceived()), inboxPage->twitterWidget, SLOT(enableMoreButton()));
	}
	disconnect(config->accounts[oldAccountId], SIGNAL(lastMessageReceived(const QString &, Account *)), 0, 0);
	connect(config->currentAccount(), SIGNAL(lastMessageReceived(const QString &, Account *)), this, SLOT(updateLastMessage(const QString &, Account *)));
	disconnect(config->accounts[oldAccountId], SIGNAL(remainingRequestsUpdated(int, Account *)), 0, 0);
	connect(config->currentAccount(), SIGNAL(remainingRequestsUpdated(int, Account *)), this, SLOT(updateRemainingRequests(int, Account *)));
	disconnect(messageTextEdit, SIGNAL(messageEntered(const QString &, int)), 0, 0);
	connect(messageTextEdit, SIGNAL(messageEntered(const QString &, int)), config->currentAccount(), SLOT(sendMessage(const QString &, int)));
	messageTextEdit->setEnabled(!config->currentAccount()->sendingMessage);
	connect(config->currentAccount(), SIGNAL(messageSent(const QString &, Account *)), this, SLOT(messageSent(const QString &, Account *)));
	updateLastMessage(config->currentAccount()->lastMessage.text, config->currentAccount());
	updateRemainingRequests(config->currentAccount()->remainingRequests, config->currentAccount());
}

void MainWindow::resizeEvent(QResizeEvent *event) {
	qDebug() << ("MainWindow::resizeEvent()");

	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->updateSize();
	}
}

void MainWindow::showEvent(QShowEvent *event) {
	qDebug() << ("MainWindow::showEvent()");

	Configuration *config = Configuration::getInstance();
	resize(config->size);
	move(config->position);

	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->updateSize();
	}
	
	messageTextEdit->setFocus(Qt::OtherFocusReason);

	event->accept();
}

void MainWindow::hideEvent(QHideEvent *event) {
	qDebug() << ("MainWindow::hideEvent()");

	saveState();
	event->accept();
}

void MainWindow::setupTrayIcon() {
	qDebug() << ("MainWindow::setupTrayIcon()");

	trayShowhideAction = new QAction(tr("&Show / Hide"), this);
	connect(trayShowhideAction, SIGNAL(triggered()), this, SLOT(showhide()));
	trayQuitAction = new QAction(tr("&Quit"), this);
	connect(trayQuitAction, SIGNAL(triggered()), this, SLOT(quit()));
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(trayShowhideAction);
	trayIconMenu->addAction(trayQuitAction);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":/images/qwit.png"));
	trayIcon->show();
//	connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(makeActive()));
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
	qDebug() << ("MainWindow::iconActivated()");

	if (reason == QSystemTrayIcon::Trigger) {
		showhide();
	}
}

void MainWindow::showhide() {
	qDebug() << ("MainWindow::showhide()");

	if (isVisible()) {
		hide();
	} else {
		show();
		activateWindow();
//		for (int i = 0; i < TWITTER_TABS; ++i) {
//			twitterTabs[i].twitterWidget->updateItems();
//		}
	}
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
	qDebug() << ("MainWindow::keyPressEvent()");

	if (event->key() == Qt::Key_Escape) {
		showhide();
	} else if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_Q)) {
		acceptClose = true;
		quit();
	} else {
		QDialog::keyPressEvent(event);
	}
}

void MainWindow::quit() {
	qDebug() << ("MainWindow::quit()");

	acceptClose = true;
	close();
}

void MainWindow::closeEvent(QCloseEvent *event) {
	qDebug() << ("MainWindow::closeEvent()");

	if (acceptClose) {
		saveState();
		event->accept();
	} else {
		hide();
		event->ignore();
	}
}

void MainWindow::refresh() {
	qDebug() << ("MainWindow::refresh()");

	pages[mainTabWidget->currentIndex()]->update();
	Configuration *config = Configuration::getInstance();
	config->currentAccount()->updateLastMessage();
}

void MainWindow::tabChanged(int tabIndex) {
	qDebug() << ("MainWindow::tabChanged()");
	if ((tabIndex >= 0) && (tabIndex < pages.size()) && pages[tabIndex]) {
		pages[tabIndex]->updateSize();
	}
}

void MainWindow::reloadUserpics() {
	qDebug() << ("MainWindow::reloadUserpics()");

	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->reloadUserpics();
	}
}

void MainWindow::updateLastMessage(const QString &message, Account *account) {
	qDebug() << ("MainWindow::updateLastMessage()");
	lastMessageLabel->setText(QwitTools::prepareMessage(message, account));
}

void MainWindow::messageSent(const QString &message, Account *account) {
	qDebug() << ("MainWindow::messageSent()");
	messageTextEdit->setEnabled(true);
	messageTextEdit->clear();
	updateLastMessage(message, account);
}

void MainWindow::showNewMessages(const QVector<Message> &messages, Account *account) {
	qDebug() << ("MainWindow::showNewMessages()");
	Configuration *config = Configuration::getInstance();
	QString trayMessage = "";
	for (int i = 0; i < min(messages.size(), config->messagesInPopup); ++i) {
		if (trayMessage.length()) {
			trayMessage += "----------------------------\n";
		}
		trayMessage += messages[i].username + ": " + messages[i].text + " /" + QwitTools::formatDateTime(messages[i].time.toLocalTime()) + "\n";
	}
	if ((trayMessage != "") && config->showMessagesInTray) {
		trayIcon->showMessage(tr("Qwit: new messages receieved"), trayMessage);
	}
}

void MainWindow::redrawPages() {
	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->redraw();
	}
}

void MainWindow::updateAccount(Account *account) {
	for (int i = 0; i < pages.size(); ++i) {
		if (pages[i]->updateAutomatically()) {
			pages[i]->update(account);
		}
	}
	account->updateLastMessage();
}

void MainWindow::updateAll() {
	Configuration *config = Configuration::getInstance();
	for (int i = 0; i < config->accounts.size(); ++i) {
		updateAccount(config->accounts[i]);
	}
}

void MainWindow::updateRemainingRequests(int remainingRequests, Account *account) {
	if (remainingRequests == -1) {
		stateLabel->setText("");
	} else if (remainingRequests == 0) {
		stateLabel->setText("Rate limit exceeded");
	} else {
		stateLabel->setText(QString::number(remainingRequests) + " requests left");
	}
}

void MainWindow::directMessage(const Message &message) {
	Configuration *config = Configuration::getInstance();
	directMessageDialog->accountsComboBox->clear();
	for (int i = 0; i < config->accounts.size(); ++i) {
		directMessageDialog->accountsComboBox->addItem(QIcon(":/images/" + config->accounts[i]->type + ".png"), config->accounts[i]->username);
	}
	directMessageDialog->accountsComboBox->setCurrentIndex(message.account->id);
	directMessageDialog->usernameLineEdit->setText(message.username);
	directMessageDialog->messagePlainTextEdit->clear();
	directMessageDialog->messagePlainTextEdit->setFocus();
	directMessageDialog->showNormal();
}

void MainWindow::sendDirectMessage() {
	Configuration *config = Configuration::getInstance();
	config->accounts[directMessageDialog->accountsComboBox->currentIndex()]->sendDirectMessage(directMessageDialog->usernameLineEdit->text(), directMessageDialog->messagePlainTextEdit->toPlainText());
}

void MainWindow::favor(const Message &message) {
	message.account->favorMessage(message);
}

void MainWindow::unfavor(const Message &message) {
	message.account->unfavorMessage(message);
}

void MainWindow::destroy(const Message &message) {
	message.account->destroyMessage(message);
}

void MainWindow::postTwitPic() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Pick an image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
	if (fileName.isEmpty()) {
		return;
	}
	TwitPicDialog dialog(fileName, this);
	Configuration *config = Configuration::getInstance();
	dialog.setUser(config->currentAccount()->username, config->currentAccount()->password);
	if (dialog.exec() == QDialog::Accepted) {
		messageTextEdit->append(dialog.twitPickedUrlString());
	}
}

#endif