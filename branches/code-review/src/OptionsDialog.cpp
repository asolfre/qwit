/*  This file is part of Qwit.

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

#ifndef OptionsDialog_cpp
#define OptionsDialog_cpp

#include "MainWindow.h"
#include "OptionsDialog.h"
#include "Configuration.h"

#include "TwitterAccount.h"
#include "IdenticaAccount.h"
#include "CustomAccount.h"

#include <iostream>

using namespace std;

OptionsDialog::OptionsDialog(QWidget *parent): QDialog(parent) {
	setupUi(this);
	
	twitterAccountDialog = new TwitterAccountDialog(this);
	twitterAccountDialog->setModal(true);
	
	connect(optionsGroupTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(changeOptionsGroup(QTreeWidgetItem*)));
	optionsStackedWidget->setCurrentWidget(accountsPage);
	connect(addAccountPushButton, SIGNAL(pressed()), this, SLOT(addAccount()));
	connect(deleteAccountPushButton, SIGNAL(pressed()), this, SLOT(deleteAccount()));
	connect(editAccountPushButton, SIGNAL(pressed()), this, SLOT(editAccount()));
	connect(twitterAccountDialog, SIGNAL(accepted()), this, SLOT(commitTwitterAccount()));
}

void OptionsDialog::changeOptionsGroup(QTreeWidgetItem *item) {
	if (item->text(0) == tr("Accounts")) {
		optionsStackedWidget->setCurrentWidget(accountsPage);
	} else if (item->text(0) == tr("URL shortener")) {
		optionsStackedWidget->setCurrentWidget(urlShortenerPage);
	} else if (item->text(0) == tr("Connection")) {
		optionsStackedWidget->setCurrentWidget(connectionPage);
	} else if (item->text(0) == tr("User interface")) {
		optionsStackedWidget->setCurrentWidget(userInterfacePage);
	}
}

void OptionsDialog::addAccount() {
	twitterAccountDialog->action = ACTION_ADD;
	twitterAccountDialog->showNormal();
	twitterAccountDialog->twitterUsernameLineEdit->setText("");
	twitterAccountDialog->twitterPasswordLineEdit->setText("");
	twitterAccountDialog->twitterUsernameLineEdit->setFocus();
}

void OptionsDialog::deleteAccount() {
	int accountId = accountsListWidget->currentRow();
	if (accountId == -1) return;
	QMessageBox messageBox;
	messageBox.setText("Do you really want to delete account \"" + accountsListWidget->currentItem()->text() + "\"?");
	messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	messageBox.setDefaultButton(QMessageBox::Ok);
	if (messageBox.exec() == QMessageBox::Ok) {
		Configuration *config = Configuration::getInstance();
		config->deleteAccount(accountId);
		accountsListWidget->takeItem(accountId);
	}
}

void OptionsDialog::editAccount() {
	int accountId = accountsListWidget->currentRow();
	if (accountId == -1) return;
	Configuration *config = Configuration::getInstance();
	switch (config->accounts[accountId]->type) {
		case ACCOUNT_TWITTER: {
				twitterAccountDialog->action = ACTION_EDIT;
				twitterAccountDialog->showNormal();
				twitterAccountDialog->twitterUsernameLineEdit->setText(((TwitterAccount*)config->accounts[accountId])->username);
				twitterAccountDialog->twitterPasswordLineEdit->setText(((TwitterAccount*)config->accounts[accountId])->password);
				twitterAccountDialog->twitterUsernameLineEdit->setFocus();
			}
			break;
	}
}

void OptionsDialog::commitTwitterAccount() {
	Configuration *config = Configuration::getInstance();
	switch (twitterAccountDialog->action) {
		case ACTION_ADD: {
				TwitterAccount *account = new TwitterAccount(twitterAccountDialog->twitterUsernameLineEdit->text(), twitterAccountDialog->twitterPasswordLineEdit->text());
				int accountId = config->addAccount(account);
				accountsListWidget->addItem("Twitter: " + account->username);
			}
			break;
		case ACTION_EDIT:
			break;
	}
}

#endif
