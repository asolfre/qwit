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
 *  OptionsDialog class implementation
 */

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
	accountConfigurationDialog->action = AccountConfigurationDialog::ActionAdd;
	accountConfigurationDialog->accountType = servicesComboBox->currentIndex();
	accountConfigurationDialog->accountUsernameLineEdit->setText("");
	accountConfigurationDialog->accountPasswordLineEdit->setText("");
	accountConfigurationDialog->accountUsernameLineEdit->setFocus();
	switch (servicesComboBox->currentIndex()) {
		case Configuration::AccountTwitter: {
				accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(false);
				accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(false);
			}
			break;
		case Configuration::AccountIdentica: {
				accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(false);
				accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(false);
			}
			break;
		case Configuration::AccountCustom: {
				accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(true);
				accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(true);
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
		accountsListWidget->takeItem(accountId);
		MainWindow *mainWindow = MainWindow::getInstance();
		mainWindow->deleteAccountButton(config->accounts[accountId]);
		config->deleteAccount(accountId);
	}
}

void OptionsDialog::editAccount() {
	int accountId = accountsListWidget->currentRow();
	if (accountId == -1) return;
	Configuration *config = Configuration::getInstance();
	accountConfigurationDialog->action = AccountConfigurationDialog::ActionEdit;
	accountConfigurationDialog->accountId = accountId;
	accountConfigurationDialog->accountUsernameLineEdit->setText((config->accounts[accountId])->username);
	accountConfigurationDialog->accountPasswordLineEdit->setText((config->accounts[accountId])->password);
	accountConfigurationDialog->accountUsernameLineEdit->setFocus();
	switch (Configuration::ServicesIds[config->accounts[accountId]->type]) {
		case Configuration::AccountTwitter: {
			accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(false);
			accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(false);
		}
		case Configuration::AccountIdentica: {
			accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(false);
			accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(false);
		}
		case Configuration::AccountCustom: {
			accountConfigurationDialog->serviceBaseURLLineEdit->setEnabled(true);
			accountConfigurationDialog->serviceAPIURLLineEdit->setEnabled(true);
		}
	}
	accountConfigurationDialog->showNormal();
}

void OptionsDialog::commitAccount() {
	Configuration *config = Configuration::getInstance();
	MainWindow *mainWindow = MainWindow::getInstance();
	switch (accountConfigurationDialog->action) {
		case AccountConfigurationDialog::ActionAdd: {
				Account *account = new Account(Configuration::Services[accountConfigurationDialog->accountType], accountConfigurationDialog->accountUsernameLineEdit->text(), accountConfigurationDialog->accountPasswordLineEdit->text());
				config->addAccount(account);
				accountsListWidget->addItem(Configuration::ServicesNames[account->type] + ": " + account->username);
				accountsListWidget->setCurrentRow(account->id);
				mainWindow->addAccountButton(account);
			}
			break;
		case AccountConfigurationDialog::ActionEdit: {
				Account *account = config->accounts[accountConfigurationDialog->accountId];
				account->username = accountConfigurationDialog->accountUsernameLineEdit->text();
				account->password = accountConfigurationDialog->accountPasswordLineEdit->text();
				accountsListWidget->takeItem(account->id);
				accountsListWidget->insertItem(account->id, Configuration::ServicesNames[account->type] + ": " + account->username);
				accountsListWidget->setCurrentRow(account->id);
				mainWindow->updateAccountButton(account);
			}
			break;
	}
}

#endif
