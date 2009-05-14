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

#include "Account.h"

#include <iostream>

using namespace std;

OptionsDialog::OptionsDialog(QWidget *parent): QDialog(parent) {
	setupUi(this);
	
	accountConfigurationDialog = new AccountConfigurationDialog(this);
	accountConfigurationDialog->setModal(true);
	
	connect(optionsGroupTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(changeOptionsGroup(QTreeWidgetItem*)));
	optionsStackedWidget->setCurrentWidget(accountsPage);
	connect(addAccountPushButton, SIGNAL(pressed()), this, SLOT(addAccount()));
	connect(deleteAccountPushButton, SIGNAL(pressed()), this, SLOT(deleteAccount()));
	connect(editAccountPushButton, SIGNAL(pressed()), this, SLOT(editAccount()));
	connect(accountConfigurationDialog, SIGNAL(accepted()), this, SLOT(commitAccount()));
	
	optionsStackedWidget->setCurrentWidget(accountsPage);
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
	accountConfigurationDialog->action = ACTION_ADD;
	accountConfigurationDialog->accountType = servicesComboBox->currentIndex();
	accountConfigurationDialog->accountUsernameLineEdit->setText("");
	accountConfigurationDialog->accountPasswordLineEdit->setText("");
	accountConfigurationDialog->accountUsernameLineEdit->setFocus();
	switch (servicesComboBox->currentIndex()) {
		case ACCOUNT_TWITTER: {
				accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(false);
				accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(false);
			}
			break;
		case ACCOUNT_IDENTICA: {
				accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(false);
				accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(false);
			}
			break;
		case ACCOUNT_CUSTOM: {
			}
			break;
	}
	accountConfigurationDialog->showNormal();
}

void OptionsDialog::deleteAccount() {
	int accountId = accountsListWidget->currentRow();
	if (accountId == -1) return;
	QMessageBox messageBox(this);
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
	accountConfigurationDialog->action = ACTION_EDIT;
	accountConfigurationDialog->accountId = accountId;
	accountConfigurationDialog->accountUsernameLineEdit->setText((config->accounts[accountId])->username);
	accountConfigurationDialog->accountPasswordLineEdit->setText((config->accounts[accountId])->password);
	accountConfigurationDialog->accountUsernameLineEdit->setFocus();
	switch (config->accounts[accountId]->type) {
		case ACCOUNT_TWITTER: {
				accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(false);
				accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(false);
			}
			break;
		case ACCOUNT_IDENTICA: {
				accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(false);
				accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(false);
			}
			break;
		case ACCOUNT_CUSTOM: {
			}
			break;
	}
	accountConfigurationDialog->showNormal();
}

void OptionsDialog::commitAccount() {
	Configuration *config = Configuration::getInstance();
	switch (accountConfigurationDialog->action) {
		case ACTION_ADD: {
				Account *account = new Account(accountConfigurationDialog->accountUsernameLineEdit->text(), accountConfigurationDialog->accountPasswordLineEdit->text());
				account->type = accountConfigurationDialog->accountType;
				config->addAccount(account);
				accountsListWidget->addItem(SERVICE_NAME[account->type] + ": " + account->username);
				accountsListWidget->setCurrentRow(account->id);
			}
			break;
		case ACTION_EDIT: {
				Account *account = config->accounts[accountConfigurationDialog->accountId];
				account->username = accountConfigurationDialog->accountUsernameLineEdit->text();
				account->password = accountConfigurationDialog->accountPasswordLineEdit->text();
				accountsListWidget->takeItem(account->id);
				accountsListWidget->insertItem(account->id, SERVICE_NAME[account->type] + ": " + account->username);
				accountsListWidget->setCurrentRow(account->id);
			}
			break;
	}
}

#endif
