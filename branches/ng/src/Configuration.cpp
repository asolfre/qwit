/*!
 *  $Id$
 *
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
 *  Configuration class implementation
 */

#ifndef Configuration_cpp
#define Configuration_cpp

#include "QwitHeaders.h"

#include "MainWindow.h"
#include "Configuration.h"
#include "Services.h"

const QString Configuration::CompanyName = "arti";
const QString Configuration::ApplicationName = "qwit2";
const QString Configuration::CacheDirectory = QDir::homePath() + "/.qwit2/";
const QString Configuration::MessagesCacheFileName = Configuration::CacheDirectory + "/messages";

Configuration* Configuration::instance = NULL;
QSettings Configuration::settings(CompanyName, ApplicationName);
QSettings Configuration::messagesCache(Configuration::MessagesCacheFileName, QSettings::IniFormat);

QMap<QString, QString> Configuration::ServicesNames;
QMap<QString, int> Configuration::ServicesIds;
QVector<QString> Configuration::Services;
QMap<QString, QString> Configuration::UrlShortenersNames;
QMap<QString, int> Configuration::UrlShortenersIds;
QVector<QString> Configuration::UrlShorteners;

Configuration::Configuration() {
	for (QMap<QString, QMap<QString, QString> >::iterator it = Services::options.begin(); it != Services::options.end(); ++it) {
		ServicesNames[it.key()] = it.value()["title"];
		Services.push_back(it.key());
		ServicesIds[it.key()] = Services.size() - 1;
	}
	QFile file(settings.fileName());
	currentAccountId = -1;
	file.setPermissions(QFile::ReadUser | QFile::WriteUser);
	for (QMap<QString, QMap<QString, QString> >::iterator it = Services::urlShorteners.begin(); it != Services::urlShorteners.end(); ++it) {
		UrlShortenersNames[it.key()] = it.value()["title"];
		UrlShorteners.push_back(it.key());
		UrlShortenersIds[it.key()] = UrlShorteners.size() - 1;
	}
}

Configuration* Configuration::getInstance() {
	if (!instance) instance = new Configuration();
	return instance;
}

void Configuration::load() {
// State
	settings.beginGroup("State");
	position = settings.value("position", QPoint(100, 100)).toPoint();
	size = settings.value("size", QSize(300, 600)).toSize();
	currentAccountId = settings.value("currentAccountId", -1).toInt();
	searchQuery = settings.value("searchQuery", "").toString();
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
	placeTabsVertically = settings.value("placeTabsVertically", true).toBool();
	showLastMessage = settings.value("showLastMessage", true).toBool();
	settings.endGroup();
	
	settings.beginGroup("Tabs");
	showHomeTab = settings.value("showHomeTab", true).toBool();
	autoUpdateHomeTab = settings.value("autoUpdateHomeTab", true).toBool();
	showPublicTab = settings.value("showPublicTab", true).toBool();
	autoUpdatePublicTab = settings.value("autoUpdatePublicTab", false).toBool();
	showRepliesTab = settings.value("showRepliesTab", true).toBool();
	autoUpdateRepliesTab = settings.value("autoUpdateRepliesTab", true).toBool();
	showCustomTab = settings.value("showCustomTab", true).toBool();
	autoUpdateCustomTab = settings.value("autoUpdateCustomTab", false).toBool();
	showInboxTab = settings.value("showInboxTab", true).toBool();
	autoUpdateInboxTab = settings.value("autoUpdateInboxTab", true).toBool();
	showOutboxTab = settings.value("showOutboxTab", true).toBool();
	autoUpdateOutboxTab = settings.value("autoUpdateOutboxTab", false).toBool();
	showSearchTab = settings.value("showSearchTab", true).toBool();
	autoUpdateSearchTab = settings.value("autoUpdateSearchTab", false).toBool();
	showFavoritesTab = settings.value("showFavoritesTab", true).toBool();
	autoUpdateFavoritesTab = settings.value("autoUpdateFavoritesTab", false).toBool();
	updateInterval = settings.value("updateInterval", 300).toInt();
	settings.endGroup();
	
	settings.beginGroup("Colors");
	int color = settings.value("commonMessagesOddColor", (((180 << 8) + 180) << 8) + 180).toInt();
	commonMessagesOddColor.setRgb(color >> 16, (color >> 8) & 255, color & 255);
	color = settings.value("commonMessagesEvenColor", (((230 << 8) + 230) << 8) + 230).toInt();
	commonMessagesEvenColor.setRgb(color >> 16, (color >> 8) & 255, color & 255);
	color = settings.value("mentionsOddColor", (((128 << 8) + 255) << 8) + 128).toInt();
	mentionsOddColor.setRgb(color >> 16, (color >> 8) & 255, color & 255);
	color = settings.value("mentionsEvenColor", (((200 << 8) + 255) << 8) + 200).toInt();
	mentionsEvenColor.setRgb(color >> 16, (color >> 8) & 255, color & 255);
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
		bool useHttps = settings.value("useHttps", false).toBool();
		Account *account = new Account(type, username, password, useHttps);
		addAccount(account);
	}
	settings.endArray();
	settings.endGroup();
	
// Connection
	settings.beginGroup("Connection");
	useProxy = settings.value("useProxy", false).toBool();
	proxyAddress = settings.value("proxyAddress", "").toString();
	proxyPort = settings.value("proxyPort", "").toInt();
	proxyUsername = settings.value("proxyUsername", "").toString();
	proxyPassword = settings.value("proxyPassword", "").toString();
	settings.endGroup();

// UrlShortener
	settings.beginGroup("UrlShortener");
	urlShortener = settings.value("urlShortener", "trim").toString();
	settings.endGroup();

	loadMessages();
}

void Configuration::save() {
// State
	settings.beginGroup("State");
	settings.setValue("position", position);
	settings.setValue("size", size);
	settings.setValue("currentAccountId", currentAccountId);
	settings.setValue("searchQuery", searchQuery);
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
	settings.setValue("placeTabsVertically", placeTabsVertically);
	settings.setValue("showLastMessage", showLastMessage);
	settings.endGroup();

	settings.beginGroup("Tabs");
	settings.setValue("showHomeTab", showHomeTab);
	settings.setValue("autoUpdateHomeTab", autoUpdateHomeTab);
	settings.setValue("showPublicTab", showPublicTab);
	settings.setValue("autoUpdatePublicTab", autoUpdatePublicTab);
	settings.setValue("showRepliesTab", showRepliesTab);
	settings.setValue("autoUpdateRepliesTab", autoUpdateRepliesTab);
	settings.setValue("showCustomTab", showCustomTab);
	settings.setValue("autoUpdateCustomTab", autoUpdateCustomTab);
	settings.setValue("showInboxTab", showInboxTab);
	settings.setValue("autoUpdateInboxTab", autoUpdateInboxTab);
	settings.setValue("showOutboxTab", showOutboxTab);
	settings.setValue("autoUpdateOutboxTab", autoUpdateOutboxTab);
	settings.setValue("showSearchTab", showSearchTab);
	settings.setValue("autoUpdateSearchTab", autoUpdateSearchTab);
	settings.setValue("showFavoritesTab", showFavoritesTab);
	settings.setValue("autoUpdateFavoritesTab", autoUpdateFavoritesTab);
	settings.setValue("updateInterval", updateInterval);
	settings.endGroup();

	settings.beginGroup("Colors");
	settings.setValue("commonMessagesOddColor", (((commonMessagesOddColor.red() << 8) + commonMessagesOddColor.green()) << 8) + commonMessagesOddColor.blue());
	settings.setValue("commonMessagesEvenColor", (((commonMessagesEvenColor.red() << 8) + commonMessagesEvenColor.green()) << 8) + commonMessagesEvenColor.blue());
	settings.setValue("mentionsOddColor", (((mentionsOddColor.red() << 8) + mentionsOddColor.green()) << 8) + mentionsOddColor.blue());
	settings.setValue("mentionsEvenColor", (((mentionsEvenColor.red() << 8) + mentionsEvenColor.green()) << 8) + mentionsEvenColor.blue());
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
		settings.setValue("useHttps", accounts[i]->useHttps);
	}
	settings.endArray();
	settings.endGroup();
	
// Connection
	settings.beginGroup("Connection");
	settings.setValue("useProxy", useProxy);
	settings.setValue("proxyAddress", proxyAddress);
	settings.setValue("proxyPort", proxyPort);
	settings.setValue("proxyUsername", proxyUsername);
	settings.setValue("proxyPassword", proxyPassword);
	settings.endGroup();
	
// UrlShortener
	settings.beginGroup("UrlShortener");
	settings.setValue("urlShortener", urlShortener);
	settings.endGroup();

	saveMessages();
}

int Configuration::addAccount(Account *account) {
	accounts.push_back(account);
	account->id = accounts.size() - 1;
	QObject::connect(account, SIGNAL(newMessagesReceived(const QVector<Message>&, Account *)), MainWindow::getInstance(), SLOT(showNewMessages(const QVector<Message>&, Account *)));
	if (currentAccountId == -1) {
		currentAccountId = account->id;
	}
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

void Configuration::saveMessages() {
	messagesCache.beginWriteArray("Accounts");
	for (int i = 0; i < accounts.size(); ++i) {
		messagesCache.setArrayIndex(i);
		accounts[i]->saveMessages(messagesCache);
	}
	messagesCache.endArray();
}

void Configuration::loadMessages() {
	int n = messagesCache.beginReadArray("Accounts");
	for (int i = 0; i < min(n, accounts.size()); ++i) {
		messagesCache.setArrayIndex(i);
		accounts[i]->loadMessages(messagesCache);
	}
	messagesCache.endArray();
}

#endif
