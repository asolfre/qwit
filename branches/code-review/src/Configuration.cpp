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

#ifndef Configuration_cpp
#define Configuration_cpp

#include "Configuration.h"

const char *COMPANY_NAME = "arti";
const char *APPLICATION_NAME = "qwit2";

Configuration* Configuration::instance = NULL;
QSettings Configuration::settings(COMPANY_NAME, APPLICATION_NAME);

Configuration::Configuration() {
}

Configuration* Configuration::getInstance() {
	if (instance == NULL) instance = new Configuration();
	return instance;
}

void Configuration::load() {
	settings.beginGroup("User interface");
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
	position = settings.value("position", QPoint(100, 100)).toPoint();
	size = settings.value("size", QSize(300, 600)).toSize();
	settings.endGroup();
}

void Configuration::save() {
	settings.beginGroup("User interface");
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
	settings.setValue("position", position);
	settings.setValue("size", size);
	settings.endGroup();
}

int Configuration::addAccount(AbstractAccount *account) {
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
	AbstractAccount *temp = accounts[account1Id];
	accounts[account1Id] = accounts[account2Id];
	accounts[account2Id] = temp;
}

#endif
