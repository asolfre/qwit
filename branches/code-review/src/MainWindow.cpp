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

#ifndef MainWindow_cpp
#define MainWindow_cpp

#include "TwitterWidget.cpp"

#include "MainWindow.h"

#include "Configuration.h"
#include "HomePage.h"
#include "RepliesPage.h"
#include "PublicPage.h"

#include <iostream>

using namespace std;

MainWindow* MainWindow::instance = NULL;

MainWindow* MainWindow::getInstance() {
	if (!instance) instance = new MainWindow();
	return instance;
}

MainWindow::MainWindow(QWidget *parent): QDialog(parent) {
	setupUi(this);

	statusTextEdit = new StatusTextEdit(this);
	statusTextEdit->setObjectName(QString::fromUtf8("statusTextEdit"));
	statusHorizontalLayout->insertWidget(0, statusTextEdit);
	connect(statusTextEdit, SIGNAL(leftCharsNumberChanged(int)), this, SLOT(leftCharsNumberChanged(int)));
	
	optionsDialog = new OptionsDialog(this);
	optionsDialog->setModal(true);
	connect(optionsToolButton, SIGNAL(pressed()), this, SLOT(showOptionsDialog()));

	connect(optionsDialog, SIGNAL(accepted()), this, SLOT(saveOptions()));
	connect(optionsDialog, SIGNAL(rejected()), this, SLOT(resetOptionsDialog()));
	
	if (accountsButtons.size() > 0) {
		accountsButtons[0]->setChecked(true);
		accountButtonClicked(0);
	}
	
	connect(&accountsButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(accountButtonClicked(int)));
	
	mainTabWidget->removeTab(0);
	
	loadState();
}

void MainWindow::leftCharsNumberChanged(int count) {
	leftCharactersNumberLabel->setText(QString::number(count));
}

void MainWindow::loadState() {
	Configuration *config = Configuration::getInstance();
	
	config->load();

	updateState();
	
	for (int i = 0; i < config->accounts.size(); ++i) {
		addAccountButton(config->accounts[i]);
	}
	
	resetOptionsDialog();
}

void MainWindow::saveState() {
	Configuration *config = Configuration::getInstance();
	
	config->position = pos();
	config->size = size();
	
	config->save();
}

void MainWindow::saveOptions() {
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

	for (int i = 0; i < pages.size(); ++i) {
		mainTabWidget->removeTab(mainTabWidget->indexOf(pages[i]));
		delete pages[i];
	}
	pages.clear();

	if (config->showHomeTab) {
		pages.push_back(new HomePage());
	}
	if (config->showPublicTab) {
		pages.push_back(new PublicPage());
	}
	if (config->showRepliesTab) {
		pages.push_back(new RepliesPage());
	}

	for (int i = 0; i < pages.size(); ++i) {
		mainTabWidget->addTab(pages[i], pages[i]->title());
	}

	move(config->position);
	resize(config->size);
}

void MainWindow::resetOptionsDialog() {
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
		optionsDialog->accountsListWidget->addItem(Configuration::SERVICES_NAMES[config->accounts[i]->type] + ": " + config->accounts[i]->username);
	}
}

void MainWindow::addAccountButton(Account *account) {
	QToolButton *accountButton = new QToolButton(this);
	accountButton->setIcon(QIcon(":/images/" + account->type + ".png"));
	accountButton->setText(account->username);
	accountButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	accountButton->setAutoRaise(true);
	accountButton->setCheckable(true);
	accountsButtons.push_back(accountButton);
	accountsButtonGroup.addButton(accountButton, accountsButtons.size() - 1);
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
	accountsButtons[account->id]->setText(account->username);
}

void MainWindow::deleteAccountButton(Account *account) {
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
	accountsButtons[checkedId]->setChecked(true);
	accountButtonClicked(checkedId);
}

void MainWindow::showOptionsDialog() {
	optionsDialog->showNormal();
}

void MainWindow::accountButtonClicked(int id) {
	cout << id << endl;
	Configuration *config = Configuration::getInstance();
	config->currentAccountId = id;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
	pages[0]->updateSize();
}

void MainWindow::showEvent(QShowEvent *event) {
	Configuration *config = Configuration::getInstance();
	resize(config->size);
	move(config->position);

	pages[0]->updateSize();
	
	statusTextEdit->setFocus(Qt::OtherFocusReason);

	event->accept();
}

#endif
