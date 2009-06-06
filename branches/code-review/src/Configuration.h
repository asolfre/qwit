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
 *  Configuration class declaration
 */

#ifndef Configuration_h
#define Configuration_h

#include <QSettings>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QVector>
#include <QMap>

#include "Account.h"

class Configuration {
	Configuration();
	static QSettings settings;
	static QSettings messagesCache;
	
public:
	static const int AccountTwitter = 0;
	static const int AccountIdentica = 1;
	static const int AccountCustom = 2;
	static const QString CompanyName;
	static const QString ApplicationName;
	static const QString CacheDirectory;
	static const QString MessagesCacheFileName;


	static QMap<QString, QString> ServicesNames;
	static QMap<QString, int> ServicesIds;
	static QVector<QString> Services;
	
// State
	QPoint position;
	QSize size;
	int currentAccountId;
	
// User interface options
	bool showGreetingMessage;
	QString greetingMessage;
	bool showLeftCharactersNumber;
	int messagesPerPage;
	int messagesInPopup;
	QString retweetTag;
	bool retweetTagAfterText;
	bool placeUsernameUnderAvatar;
	bool showMessagesInTray;
	bool placeControlsVertically;
	bool placeTabsVertically;
	bool showLastStatus;
	bool showHomeTab;
	bool updateHomeTabAlways;
	bool showPublicTab;
	bool updatePublicTabAlways;
	bool showRepliesTab;
	bool updateRepliesTabAlways;
	bool showCustomTab;
	bool updateCustomTabAlways;
	bool showInboxTab;
	bool updateInboxTabAlways;
	bool showOutboxTab;
	bool updateOutboxTabAlways;
	bool showSearchTab;
	bool updateSearchTabAlways;

// Accounts
	QVector<Account*> accounts;

	static Configuration* instance;
	
	int addAccount(Account *account);
	void deleteAccount(int accountId);
	void swapAccounts(int account1Id, int account2Id);
	Account* currentAccount();
	void saveMessages();
	void loadMessages();
	
public:
	static Configuration* getInstance();
	
	void save();
	void load();
};

#endif
