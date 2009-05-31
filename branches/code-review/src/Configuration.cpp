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

#ifndef Configuration_cpp
#define Configuration_cpp

#include "Configuration.h"

#include <iostream>

using namespace std;

const char *COMPANY_NAME = "arti";
const char *APPLICATION_NAME = "qwit2";

Configuration* Configuration::instance = NULL;
QSettings Configuration::settings(COMPANY_NAME, APPLICATION_NAME);

QMap<QString, QString> Configuration::SERVICES_NAMES;
QMap<QString, int> Configuration::SERVICES_IDS;
QVector<QString> Configuration::SERVICES;

Configuration::Configuration() {
	SERVICES_NAMES["twitter"] = "Twitter";
	SERVICES_NAMES["identica"] = "Identica";
	SERVICES_NAMES["custom"] = "Custom";
	SERVICES_IDS["twitter"] = 0;
	SERVICES_IDS["identica"] = 1;
	SERVICES_IDS["custom"] = 2;
	SERVICES.push_back("twitter");
	SERVICES.push_back("identica");
	SERVICES.push_back("custom");
}

Configuration* Configuration::getInstance() {
	if (instance == NULL) instance = new Configuration();
	return instance;
}

void Configuration::load() {
// State
	settings.beginGroup("State");
	position = settings.value("position", QPoint(100, 100)).toPoint();
	size = settings.value("size", QSize(300, 600)).toSize();
	currentAccountId = settings.value("currentAccountId", 0).toInt();
	settings.endGroup();
	
// User interface
	settings.beginGroup("User interface");
	
	settings.beginGroup("Misc");
	showGreetingMessage = settings.value("showGreetingMessage", true).toBool();
	greetingMessage = settings.value("greetingMessage", "What's up?").toString();
	showLeftCharactersNumber = settings.value("showLeftCharactersNumber", true).toBool();
	messagesPerPage = settings.value("messagesPerPage", 20).toInt();
	messagesInPopup = settings.value("messagesInPopup", 10).toInt();
	retweetTag = settings.value("retweetTag", "RT @:").toString();
	retweetTagAfterText = settings.value("retweetTagAfterText", false).toBool();
	placeUsernameUnderAvatar = settings.value("placeUsernameUnderAvatar", false).toBool();
	showMessagesInTray = settings.value("showMessagesInTray", true).toBool();
	placeControlsVertically = settings.value("placeControlsVertically", true).toBool();
	showLastStatus = settings.value("showLastStatus", true).toBool();
	settings.endGroup();
	
	settings.beginGroup("Tabs");
	showHomeTab = settings.value("showHomeTab", true).toBool();
	updateHomeTabAlways = settings.value("updateHomeTabAlways", true).toBool();
	showPublicTab = settings.value("showPublicTab", true).toBool();
	updatePublicTabAlways = settings.value("updatePublicTabAlways", false).toBool();
	showRepliesTab = settings.value("showRepliesTab", true).toBool();
	updateRepliesTabAlways = settings.value("updateRepliesTabAlways", true).toBool();
	showCustomTab = settings.value("showCustomTab", true).toBool();
	updateCustomTabAlways = settings.value("updateCustomTabAlways", false).toBool();
	showInboxTab = settings.value("showInboxTab", true).toBool();
	updateInboxTabAlways = settings.value("updateInboxTabAlways", true).toBool();
	showOutboxTab = settings.value("showOutboxTab", true).toBool();
	updateOutboxTabAlways = settings.value("updateOutboxTabAlways", false).toBool();
	showSearchTab = settings.value("showSearchTab", true).toBool();
	updateSearchTabAlways = settings.value("updateSearchTabAlways", false).toBool();
	settings.endGroup();
	
	settings.endGroup();

// Accounts
	settings.beginGroup("Accounts");
	int accountsNumber = settings.beginReadArray("Accounts");
	accounts.clear();
	for (int i = 0; i < accountsNumber; ++i) {
		settings.setArrayIndex(i);
		QString username = settings.value("username", "").toString();
		QString password = settings.value("password", "").toString();
		QString type = settings.value("type", "").toString();
		Account *account = new Account(type, username, password);
		addAccount(account);
	}
	settings.endArray();
	settings.endGroup();
}

void Configuration::save() {
// State
	settings.beginGroup("State");
	settings.setValue("position", position);
	settings.setValue("size", size);
	settings.setValue("currentAccountId", currentAccountId);
	settings.endGroup();

// User interface
	settings.beginGroup("User interface");
	
	settings.beginGroup("Misc");
	settings.setValue("showGreetingMessage", showGreetingMessage);
	settings.setValue("greetingMessage", greetingMessage);
	settings.setValue("showLeftCharactersNumber", showLeftCharactersNumber);
	settings.setValue("messagesPerPage", messagesPerPage);
	settings.setValue("messagesInPopup", messagesInPopup);
	settings.setValue("retweetTag", retweetTag);
	settings.setValue("retweetTagAfterText", retweetTagAfterText);
	settings.setValue("placeUsernameUnderAvatar", placeUsernameUnderAvatar);
	settings.setValue("showMessagesInTray", showMessagesInTray);
	settings.setValue("placeControlsVertically", placeControlsVertically);
	settings.setValue("showLastStatus", showLastStatus);
	settings.endGroup();

	settings.beginGroup("Tabs");
	settings.setValue("showHomeTab", showHomeTab);
	settings.setValue("updateHomeTabAlways", updateHomeTabAlways);
	settings.setValue("showPublicTab", showPublicTab);
	settings.setValue("updatePublicTabAlways", updatePublicTabAlways);
	settings.setValue("showRepliesTab", showRepliesTab);
	settings.setValue("updateRepliesTabAlways", updateRepliesTabAlways);
	settings.setValue("showCustomTab", showCustomTab);
	settings.setValue("updateCustomTabAlways", updateCustomTabAlways);
	settings.setValue("showInboxTab", showInboxTab);
	settings.setValue("updateInboxTabAlways", updateInboxTabAlways);
	settings.setValue("showOutboxTab", showOutboxTab);
	settings.setValue("updateOutboxTabAlways", updateOutboxTabAlways);
	settings.setValue("showSearchTab", showSearchTab);
	settings.setValue("updateSearchTabAlways", updateSearchTabAlways);
	settings.endGroup();

	settings.endGroup();

// Accounts
	settings.beginGroup("Accounts");
	settings.beginWriteArray("Accounts");
	for (int i = 0; i < accounts.size(); ++i) {
		settings.setArrayIndex(i);
		settings.setValue("username", accounts[i]->username);
		settings.setValue("password", accounts[i]->password);
		settings.setValue("type", accounts[i]->type);
	}
	settings.endArray();
	settings.endGroup();
}

int Configuration::addAccount(Account *account) {
	accounts.push_back(account);
	account->id = accounts.size() - 1;
	return account->id;
}

void Configuration::deleteAccount(int accountId) {
	delete accounts[accountId];
	accounts.erase(accounts.begin() + accountId);
	for (int i = 0; i < accounts.size(); ++i) {
		accounts[i]->id = i;
	}
}

void Configuration::swapAccounts(int account1Id, int account2Id) {
	Account *temp = accounts[account1Id];
	accounts[account1Id] = accounts[account2Id];
	accounts[account2Id] = temp;
}

Account* Configuration::currentAccount() {
	return accounts[currentAccountId];
}

#endif
