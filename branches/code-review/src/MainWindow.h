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

#ifndef MainWindow_h
#define MainWindow_h

#include <QDialog>
#include <QTimer>
#include <QMessageBox>
#include <QBuffer>
#include <QtNetwork/QHttp>
#include <QUrl>
#include <QtXml/QDomDocument>
#include <QString>
#include <QFile>
#include <QTime>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QMenu>
#include <QCloseEvent>
#include <QSettings>
#include <QScrollArea>
#include <QScrollBar>
#include <QQueue>
#include <QPair>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDir>

#include "ui_MainWindow.h"

#include "StatusTextEdit.h"
#include "OptionsDialog.h"

class MainWindow: public QDialog, public Ui::MainWindow {
	Q_OBJECT

private:

	StatusTextEdit *statusTextEdit;
	OptionsDialog *optionsDialog;

	QVector<QPushButton*> accountsButtons;
	
	QHBoxLayout *accountsLayout;
	
	static MainWindow* instance;
	MainWindow(QWidget *parent = 0);
	
public:

	static MainWindow* getInstance();
	void addAccountButton(Account *account);
	void deleteAccountButton(Account *account);
	void updateAccountButton(Account *account);
	
public slots:
	void leftCharsNumberChanged(int length);
	void saveState();
	void loadState();
	void updateState();
	void saveOptions();
	void resetOptionsDialog();
};

#endif
