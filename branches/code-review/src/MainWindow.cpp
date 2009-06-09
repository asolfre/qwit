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

MainWindow* MainWindow::instance = 0;

MainWindow* MainWindow::getInstance() {
	if (!instance) {
		instance = new MainWindow();
	}
	return instance;
}

MainWindow::MainWindow(QWidget *parent): QDialog(parent) {
	QwitTools::log("MainWindow::MainWindow()");

	instance = this;

	setupUi(this);

	statusTextEdit = new StatusTextEdit(this);
	statusTextEdit->setObjectName(QString::fromUtf8("statusTextEdit"));
	statusHorizontalLayout->insertWidget(0, statusTextEdit);
	connect(statusTextEdit, SIGNAL(leftCharsNumberChanged(int)), this, SLOT(leftCharsNumberChanged(int)));
	
	optionsDialog = new OptionsDialog(this);
	connect(optionsDialog, SIGNAL(accepted()), this, SLOT(saveOptions()));
	connect(optionsDialog, SIGNAL(rejected()), this, SLOT(resetOptionsDialog()));
	
	aboutDialog = new AboutDialog(this);

	connect(refreshToolButton, SIGNAL(pressed()), this, SLOT(refresh()));
	connect(optionsToolButton, SIGNAL(pressed()), this, SLOT(showOptionsDialog()));
	connect(aboutToolButton, SIGNAL(pressed()), aboutDialog, SLOT(show()));
	connect(exitToolButton, SIGNAL(pressed()), this, SLOT(quit()));

	connect(&accountsButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(accountButtonClicked(int)));
	
	connect(mainTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	mainTabWidget->removeTab(0);
	
	accountsLayout = 0;
	
	setupTrayIcon();
	loadState();
	if (accountsButtons.size() > 0) {
		accountsButtons[0]->setChecked(true);
		accountsButtons[0]->click();
	}
	
	acceptClose = false;
	
	connect(UserpicsDownloader::getInstance(), SIGNAL(userpicDownloaded()), this, SLOT(reloadUserpics()));

	redrawTimer = new QTimer(this);
	connect(redrawTimer, SIGNAL(timeout()), this, SLOT(redrawPages()));
	redrawTimer->start(10000);

	updateTimer = new QTimer(this);
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updatePages()));
	updateTimer->start(60000);
	
	updatePages();

}

void MainWindow::leftCharsNumberChanged(int count) {
	leftCharactersNumberLabel->setText(QString::number(count));
}

void MainWindow::loadState() {
	QwitTools::log("MainWindow::loadState()");

	Configuration *config = Configuration::getInstance();
	
	config->load();

	updateState();
	
	for (int i = 0; i < config->accounts.size(); ++i) {
		addAccountButton(config->accounts[i]);
	}
	
	resetOptionsDialog();
}

void MainWindow::saveState() {
	QwitTools::log("MainWindow::saveState()");

	Configuration *config = Configuration::getInstance();
	
	config->position = pos();
	config->size = size();
	
	config->save();
}

void MainWindow::saveOptions() {
	QwitTools::log("MainWindow::saveOptions()");

	Configuration *config = Configuration::getInstance();

	config->showGreetingMessage = (optionsDialog->showGreetingMessageCheckBox->checkState() == Qt::Checked);
	config->greetingMessage = optionsDialog->greetingMessageLineEdit->text();
	config->showLeftCharactersNumber = (optionsDialog->showLeftCharactersNumberCheckBox->checkState() == Qt::Checked);
	config->showLastStatus = (optionsDialog->showLastStatusCheckBox->checkState() == Qt::Checked);
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

	saveState();
	updateState();
}

void MainWindow::updateState() {
	QwitTools::log("MainWindow::updateState()");

	Configuration *config = Configuration::getInstance();

	if (leftCharactersNumberLabel->isHidden() && greetingMessageLabel->isHidden()) {
		if (config->showLeftCharactersNumber || config->showGreetingMessage) {
			QHBoxLayout *layout = new QHBoxLayout();
			layout->addWidget(greetingMessageLabel);
			layout->addWidget(leftCharactersNumberLabel);
			verticalLayout->insertLayout(0, layout);
		}
	} else {
		if (!config->showLeftCharactersNumber && !config->showGreetingMessage) {
			QLayout *layout = (QLayout*)verticalLayout->itemAt(0);
			verticalLayout->removeItem(verticalLayout->itemAt(0));
			delete layout;
		}
	}

	greetingMessageLabel->setText(config->greetingMessage);
	greetingMessageLabel->setVisible(config->showGreetingMessage);
	leftCharactersNumberLabel->setVisible(config->showLeftCharactersNumber);
	lastStatusLabel->setVisible(config->showLastStatus);

	if (config->placeTabsVertically) {
		mainTabWidget->setTabPosition(QTabWidget::West);
	} else {
		mainTabWidget->setTabPosition(QTabWidget::South);
	}
	
	for (int i = 0; i < pages.size(); ++i) {
		mainTabWidget->removeTab(i);
		delete pages[i];
	}
	pages.clear();

	homePage = 0;
	repliesPage = 0;
	publicPage = 0;
	
	if (config->showHomeTab) {
		pages.push_back(homePage = new HomePage());
	}
	if (config->showPublicTab) {
		pages.push_back(publicPage = new PublicPage());
	}
	if (config->showRepliesTab) {
		pages.push_back(repliesPage = new RepliesPage());
	}

	for (int i = 0; i < pages.size(); ++i) {
		mainTabWidget->addTab(pages[i], pages[i]->title());
	}
	
	updateCurrentAccount(config->currentAccountId);
	
	move(config->position);
	resize(config->size);
}

void MainWindow::resetOptionsDialog() {
	QwitTools::log("MainWindow::resetOptionsDialog()");

	Configuration *config = Configuration::getInstance();
	
// User interface
	optionsDialog->showGreetingMessageCheckBox->setCheckState(config->showGreetingMessage ? Qt::Checked : Qt::Unchecked);
	optionsDialog->greetingMessageLineEdit->setText(config->greetingMessage);
	optionsDialog->showLeftCharactersNumberCheckBox->setCheckState(config->showLeftCharactersNumber ? Qt::Checked : Qt::Unchecked);
	optionsDialog->showLastStatusCheckBox->setCheckState(config->showLastStatus ? Qt::Checked : Qt::Unchecked);
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
	
// Accounts
	optionsDialog->accountsListWidget->clear();
	for (int i = 0; i < config->accounts.size(); ++i) {
		optionsDialog->accountsListWidget->addItem(Configuration::ServicesNames[config->accounts[i]->type] + ": " + config->accounts[i]->username);
	}
}

void MainWindow::addAccountButton(Account *account) {
	QwitTools::log("MainWindow::addAccountButton()");

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
	QwitTools::log("MainWindow::updateAccountButton()");

	accountsButtons[account->id]->setText(account->username);
}

void MainWindow::deleteAccountButton(Account *account) {
	QwitTools::log("MainWindow::deleteAccountButton()");

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
	QwitTools::log("MainWindow::showOptionsDialog()");

	resetOptionsDialog();
	optionsDialog->showNormal();
}

void MainWindow::accountButtonClicked(int id) {
	QwitTools::log("MainWindow::accountButtonClicked()");

	updateCurrentAccount(id);
}

void MainWindow::updateCurrentAccount(int id) {
	QwitTools::log("MainWindow::updateCurrentAccount()");

	Configuration *config = Configuration::getInstance();
	int oldAccountId = config->currentAccountId;
	config->currentAccountId = id;
	if (homePage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(friendsStatusesUpdated(const QVector<Status> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(friendsStatusesUpdated(const QVector<Status> &, Account *)), homePage, SLOT(updateItems(const QVector<Status> &, Account *)));
		homePage->updateItems(config->currentAccount()->friendsStatuses, config->currentAccount());
	}
	if (repliesPage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(repliesUpdated(const QVector<Status> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(repliesUpdated(const QVector<Status> &, Account *)), repliesPage, SLOT(updateItems(const QVector<Status> &, Account *)));
		repliesPage->updateItems(config->currentAccount()->replies, config->currentAccount());
	}
	if (publicPage) {
		disconnect(config->accounts[oldAccountId], SIGNAL(publicStatusesUpdated(const QVector<Status> &, Account *)), 0, 0);
		connect(config->currentAccount(), SIGNAL(publicStatusesUpdated(const QVector<Status> &, Account *)), publicPage, SLOT(updateItems(const QVector<Status> &, Account *)));
		publicPage->updateItems(config->currentAccount()->publicStatuses, config->currentAccount());
	}
	disconnect(config->accounts[oldAccountId], SIGNAL(lastStatusReceived(const QString &, Account *)), 0, 0);
	connect(config->currentAccount(), SIGNAL(lastStatusReceived(const QString &, Account *)), this, SLOT(updateLastStatus(const QString &, Account *)));
	disconnect(config->currentAccount(), SIGNAL(remainingRequestsUpdated(int, Account *)), 0, 0);
	connect(config->currentAccount(), SIGNAL(remainingRequestsUpdated(int, Account *)), this, SLOT(updateRemainingRequests(int, Account *)));
	disconnect(statusTextEdit, SIGNAL(statusEntered(const QString &)), 0, 0);
	connect(statusTextEdit, SIGNAL(statusEntered(const QString &)), config->currentAccount(), SLOT(sendStatus(const QString &)));
	lastStatusLabel->setText(config->currentAccount()->lastStatus.status);
	updateRemainingRequests(config->currentAccount()->remainingRequests, config->currentAccount());
}

void MainWindow::resizeEvent(QResizeEvent *event) {
	QwitTools::log("MainWindow::resizeEvent()");

	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->updateSize();
	}
}

void MainWindow::showEvent(QShowEvent *event) {
	QwitTools::log("MainWindow::showEvent()");

	Configuration *config = Configuration::getInstance();
	resize(config->size);
	move(config->position);

	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->updateSize();
	}
	
	statusTextEdit->setFocus(Qt::OtherFocusReason);

	event->accept();
}

void MainWindow::hideEvent(QHideEvent *event) {
	QwitTools::log("MainWindow::hideEvent()");

	saveState();
	event->accept();
}

void MainWindow::setupTrayIcon() {
	QwitTools::log("MainWindow::setupTrayIcon()");

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
	QwitTools::log("MainWindow::iconActivated()");

	if (reason == QSystemTrayIcon::Trigger) {
		showhide();
	}
}

void MainWindow::showhide() {
	QwitTools::log("MainWindow::showhide()");

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
	QwitTools::log("MainWindow::keyPressEvent()");

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
	QwitTools::log("MainWindow::quit()");

	acceptClose = true;
	close();
}

void MainWindow::closeEvent(QCloseEvent *event) {
	QwitTools::log("MainWindow::closeEvent()");

	if (acceptClose) {
		saveState();
		event->accept();
	} else {
		hide();
		event->ignore();
	}
}

void MainWindow::refresh() {
	QwitTools::log("MainWindow::refresh()");

	pages[mainTabWidget->currentIndex()]->update();
	Configuration *config = Configuration::getInstance();
	config->currentAccount()->updateLastStatus();
}

void MainWindow::tabChanged(int tabIndex) {
	QwitTools::log("MainWindow::tabChanged()");

	if ((tabIndex >= 0) && (tabIndex < pages.size())) {
		pages[tabIndex]->updateSize();
	}
}

void MainWindow::reloadUserpics() {
	QwitTools::log("MainWindow::reloadUserpics()");

	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->reloadUserpics();
	}
}

void MainWindow::updateLastStatus(const QString &status, Account *account) {
	QwitTools::log("MainWindow::updateLastStatus()");

	lastStatusLabel->setText(status);
}

void MainWindow::showNewStatuses(const QVector<Status> &statuses, Account *account) {
	QwitTools::log("MainWindow::showNewStatuses()");
	Configuration *config = Configuration::getInstance();
	QString trayMessage = "";
	for (int i = 0; i < min(statuses.size(), config->messagesInPopup); ++i) {
		if (trayMessage.length()) {
			trayMessage += "----------------------------\n";
		}
		trayMessage += statuses[i].username + ": " + statuses[i].status + " /" + QwitTools::formatDateTime(statuses[i].time.toLocalTime()) + "\n";
	}
	if ((trayMessage != "") && config->showMessagesInTray) {
		trayIcon->showMessage(tr("Qwit: new statuses receieved"), trayMessage);
	}
}

void MainWindow::redrawPages() {
	for (int i = 0; i < pages.size(); ++i) {
		pages[i]->redraw();
	}
}

void MainWindow::updatePages() {
	Configuration *config = Configuration::getInstance();
	for (int i = 0; i < config->accounts.size(); ++i) {
		config->accounts[i]->receiveFriendsStatuses(config->messagesPerPage);
		config->accounts[i]->receiveReplies(config->messagesPerPage);
		config->accounts[i]->updateLastStatus();
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

#endif
