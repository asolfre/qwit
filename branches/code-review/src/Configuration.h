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
	
public:
	static const int ACCOUNT_TWITTER = 0;
	static const int ACCOUNT_IDENTICA = 1;
	static const int ACCOUNT_CUSTOM = 2;

	static QMap<QString, QString> SERVICES_NAMES;
	static QMap<QString, int> SERVICES_IDS;
	static QVector<QString> SERVICES;
	
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
	bool showLastStatus;
	QPoint position;
	QSize size;

// Accounts
	QVector<Account*> accounts;

	static Configuration* instance;
	
	int addAccount(Account *account);
	void deleteAccount(int accountId);
	void swapAccounts(int account1Id, int account2Id);
	
public:
	static Configuration* getInstance();
	
	void save();
	void load();
};

#endif
