#ifndef MainWindow_cpp
#define MainWindow_cpp

#include "MainWindow.h"

#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent): QDialog(parent) {
	setupUi(this);
	
	TwitterWidget *twitterWidget = new TwitterWidget();
	twitterWidget->setObjectName(QString::fromUtf8("homeTwitterWidget"));
	twitterWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
	
	QGridLayout *gridLayout = new QGridLayout(homeTab);
	gridLayout->setObjectName(QString::fromUtf8("homeGridLayout"));
	
	QScrollArea *scrollArea = new QScrollArea(homeTab);
	scrollArea->setBackgroundRole(QPalette::Light);
	scrollArea->setWidget(twitterWidget);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	
	gridLayout->addWidget(scrollArea, 0, 0, 1, 1);
	
	twitterTabs[HOME_TWITTER_TAB] = TwitterTab(false, scrollArea, twitterWidget, 0);
	
	
	twitterWidget = new TwitterWidget();
	twitterWidget->setObjectName(QString::fromUtf8("publicTwitterWidget"));
	twitterWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
	
	gridLayout = new QGridLayout(publicTab);
	gridLayout->setObjectName(QString::fromUtf8("publicGridLayout"));
	
	scrollArea = new QScrollArea(publicTab);
	scrollArea->setBackgroundRole(QPalette::Light);
	scrollArea->setWidget(twitterWidget);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	
	gridLayout->addWidget(scrollArea, 0, 0, 1, 1);
	
	twitterTabs[PUBLIC_TWITTER_TAB] = TwitterTab(true, scrollArea, twitterWidget, 0);
	
	
	twitterWidget = new TwitterWidget();
	twitterWidget->setObjectName(QString::fromUtf8("repliesTwitterWidget"));
	twitterWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
	
	gridLayout = new QGridLayout(repliesTab);
	gridLayout->setObjectName(QString::fromUtf8("repliesGridLayout"));
	
	scrollArea = new QScrollArea(repliesTab);
	scrollArea->setBackgroundRole(QPalette::Light);
	scrollArea->setWidget(twitterWidget);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	
	gridLayout->addWidget(scrollArea, 0, 0, 1, 1);
	
	twitterTabs[REPLIES_TWITTER_TAB] = TwitterTab(false, scrollArea, twitterWidget, 0);
	
	
	twitterWidget = new TwitterWidget();
	twitterWidget->setObjectName(QString::fromUtf8("customTwitterWidget"));
	twitterWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
	
	scrollArea = new QScrollArea(customTab);
	scrollArea->setBackgroundRole(QPalette::Light);
	scrollArea->setWidget(twitterWidget);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	
	vboxLayout1->addWidget(scrollArea);
	
	twitterTabs[CUSTOM_TWITTER_TAB] = TwitterTab(true, scrollArea, twitterWidget, 1);
	
	
	statusTextEdit = new StatusTextEdit(this);
	statusTextEdit->setObjectName(QString::fromUtf8("statusTextEdit"));
	QFont font = statusTextEdit->document()->defaultFont();
	font.setFamily("Verdana");
	statusTextEdit->document()->setDefaultFont(font);
	
	charsLeftLabel->setText(QString::number(statusTextEdit->getMaxStatusCharacter()));
	charsLeftLabel->setForegroundRole(QPalette::Light);

	vboxLayout->insertWidget(1, statusTextEdit);
	
	optionsDialog = new OptionsDialog(this);
	optionsDialog->setModal(true);
	
	logsDialog = new LogsDialog(this);

	acceptClose = false;
	
	connect(statusTextEdit, SIGNAL(returnPressed()), this, SLOT(sendStatus()));
	connect(customUsernameLineEdit, SIGNAL(returnPressed()), this, SLOT(customUsernameChanged()));
	connect(optionsDialog->savePushButton, SIGNAL(pressed()), this, SLOT(saveSettings()));
	connect(optionsDialog->resetPushButton, SIGNAL(pressed()), this, SLOT(resetSettings()));
	
	connect(refreshPushButton, SIGNAL(pressed()), this, SLOT(updateTimeline()));
	connect(optionsPushButton, SIGNAL(pressed()), optionsDialog, SLOT(showNormal()));
	connect(logsPushButton, SIGNAL(pressed()), logsDialog, SLOT(showNormal()));
	connect(exitPushButton, SIGNAL(pressed()), this, SLOT(quit()));
	
	connect(&twitter, SIGNAL(updated(const QByteArray&, int)), this, SLOT(updated(const QByteArray&, int)));
	connect(&twitter, SIGNAL(statusUpdated()), this, SLOT(statusUpdated()));
	
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	
	for (int i = 0; i < TWITTER_TABS; ++i) {
		connect(twitterTabs[i].twitterWidget, SIGNAL(reply(const QString &)), statusTextEdit, SLOT(reply(const QString &)));
	}
	
	connect(statusTextEdit, SIGNAL(leftCharsNumberChanged(int)), this, SLOT(leftCharsNumberChanged(int)));
	
	connect(&twitter, SIGNAL(stateChanged(const QString&)), this, SLOT(updateState(const QString&)));
	connect(&userpicsDownloader, SIGNAL(stateChanged(const QString&)), this, SLOT(updateState(const QString&)));
	
	for (int i = 0; i < TWITTER_TABS; ++i) {
		connect(&userpicsDownloader, SIGNAL(userpicDownloaded(const QString&)), twitterTabs[i].twitterWidget, SLOT(reloadUserpic(const QString&)));
	}
	
	setupTrayIcon();

	timer2 = new QTimer(this);
	connect(timer2, SIGNAL(timeout()), this, SLOT(updateItems()));
	timer2->start(10000);
}

void MainWindow::setupTrayIcon() {
	showhideAction = new QAction(tr("&Show / Hide"), this);
	connect(showhideAction, SIGNAL(triggered()), this, SLOT(showhide()));
	quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(showhideAction);
	trayIconMenu->addAction(quitAction);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":/images/qwit.png"));
	trayIcon->show();
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}
	
void MainWindow::loadSettings() {
	QSettings settings("arti", "qwit");
	
	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(200, 600)).toSize());
	move(settings.value("pos", QPoint(200, 200)).toPoint());
	settings.endGroup();
	settings.beginGroup("Twitter");
	username = settings.value("username", "").toString();
	bool savePassword = settings.value("savePassword", false).toBool();
	password = settings.value("password", "").toString();
	interval = settings.value("interval", "300000").toInt();
	settings.endGroup();
	settings.beginGroup("Proxy");
	useProxy = settings.value("useProxy", "").toBool();
	proxyAddress = settings.value("proxyAddress", "").toString();
	proxyPort = settings.value("proxyPort", "").toInt();
	proxyUsername = settings.value("proxyUsername", "").toString();
	bool proxySavePassword = settings.value("proxySavePassword", false).toBool();
	proxyPassword = settings.value("proxyPassword", "").toString();
	settings.endGroup();
	settings.beginGroup("Other");
	customUsernameLineEdit->setText(settings.value("customUsername").toString());
	settings.endGroup();
	
	twitter.setUrl(CUSTOM_TWITTER_TAB, QString(CUSTOM_XML_URL) + customUsernameLineEdit->text() + ".xml");
	
	optionsDialog->usernameLineEdit->setText(username);
	optionsDialog->passwordLineEdit->setText(password);
	optionsDialog->savePasswordCheckBox->setCheckState(savePassword ? Qt::Checked : Qt::Unchecked);
	optionsDialog->intervalLineEdit->setText(QString::number(interval));
	optionsDialog->useProxyCheckBox->setCheckState(useProxy ? Qt::Checked : Qt::Unchecked);
	optionsDialog->proxyAddressLineEdit->setText(proxyAddress);
	optionsDialog->proxyPortLineEdit->setText(QString::number(proxyPort));
	optionsDialog->proxyUsernameLineEdit->setText(proxyUsername);
	optionsDialog->proxyPasswordLineEdit->setText(proxyPassword);
	optionsDialog->proxySavePasswordCheckBox->setCheckState(proxySavePassword ? Qt::Checked : Qt::Unchecked);
	
	if (useProxy) {
		twitter.useProxy(proxyAddress, proxyPort, proxyUsername, proxyPassword);
		userpicsDownloader.useProxy(proxyAddress, proxyPort, proxyUsername, proxyPassword);
	} else {
		twitter.dontUseProxy();
		userpicsDownloader.dontUseProxy();
	}
	
	if ((username == "") || (password == "")) {
		statusTextEdit->setDisabled(true);
	}
}
	
void MainWindow::sendStatus() {
	QString status = statusTextEdit->toPlainText().simplified();
	if (status == "") {
		return;
	}
	twitter.sendStatus(username, password, status);
	statusTextEdit->setText("");
	charsLeftLabel->setText(QString::number(statusTextEdit->getMaxStatusCharacter()));
	charsLeftLabel->setForegroundRole(QPalette::Light);
	twitterTabs[tabWidget->currentIndex()].twitterWidget->setFocus(Qt::OtherFocusReason);
}
	
void MainWindow::updateTimeline() {
	if (password == "") {
		return;
	}
	twitter.update(username, password, twitterTabs[tabWidget->currentIndex()].lastId, tabWidget->currentIndex());
}
	
void MainWindow::saveSettings() {
	optionsDialog->hide();
	
	QSettings settings("arti", "qwit");
	
	username = optionsDialog->usernameLineEdit->text();
	password = optionsDialog->passwordLineEdit->text();
	bool savePassword = optionsDialog->savePasswordCheckBox->checkState() == Qt::Checked;
	interval = optionsDialog->intervalLineEdit->text().toInt();
	
	useProxy = optionsDialog->useProxyCheckBox->checkState() == Qt::Checked;
	proxyAddress = optionsDialog->proxyAddressLineEdit->text();
	proxyPort = optionsDialog->proxyPortLineEdit->text().toInt();
	proxyUsername = optionsDialog->proxyUsernameLineEdit->text();
	proxyPassword = optionsDialog->proxyPasswordLineEdit->text();
	bool proxySavePassword = optionsDialog->proxySavePasswordCheckBox->checkState() == Qt::Checked;
	
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.endGroup();
	settings.beginGroup("Twitter");
	settings.setValue("username", username);
	settings.setValue("savePassword", savePassword);
	if (savePassword) {
		settings.setValue("password", password);
	} else {
		settings.setValue("password", "");
	}
	settings.setValue("interval", interval);
	settings.endGroup();
	settings.beginGroup("Proxy");
	settings.setValue("useProxy", useProxy);
	settings.setValue("proxyAddress", proxyAddress);
	settings.setValue("proxyPort", proxyPort);
	settings.setValue("proxyUsername", proxyUsername);
	settings.setValue("proxySavePassword", proxySavePassword);
	if (proxySavePassword) {
		settings.setValue("proxyPassword", proxyPassword);
	} else {
		settings.setValue("proxyPassword", "");
	}
	settings.endGroup();
	
	if (useProxy) {
		twitter.useProxy(proxyAddress, proxyPort, proxyUsername, proxyPassword);
		userpicsDownloader.useProxy(proxyAddress, proxyPort, proxyUsername, proxyPassword);
	} else {
		twitter.dontUseProxy();
		userpicsDownloader.dontUseProxy();
	}
	
	if ((username == "") || (password == "")) {
		statusTextEdit->setDisabled(true);
	} else {
		statusTextEdit->setDisabled(false);
	}
	
	timer->stop();
	timer->start(interval * 1000);
	
	updateTimeline();
}
	
void MainWindow::resetSettings() {
	optionsDialog->usernameLineEdit->setText(username);
	optionsDialog->passwordLineEdit->setText(password);
	optionsDialog->intervalLineEdit->setText(QString::number(interval));
}
	
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		showhide();
	}
}
	
void MainWindow::quit() {
	acceptClose = true;
	close();
}

void MainWindow::closeEvent(QCloseEvent *event) {
	QSettings settings("arti", "qwit");
	if (acceptClose) {
		logsDialog->close();
		twitter.abort();
		settings.beginGroup("MainWindow");
		settings.setValue("size", size());
		settings.setValue("pos", pos());
		settings.endGroup();
		event->accept();
	} else {
		hide();
		event->ignore();
	}
}

void MainWindow::hideEvent(QHideEvent *event) {
	QSettings settings("arti", "qwit");
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.endGroup();
	event->accept();
}

void MainWindow::showEvent(QShowEvent *event) {
	static bool firstTime = true;
	if (firstTime) {
		loadSettings();
		updateTimeline();
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeline()));
		timer->start(interval * 1000);
		firstTime = false;
	}
	
	QSettings settings("arti", "qwit");
	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(200, 600)).toSize());
	move(settings.value("pos", QPoint(200, 200)).toPoint());
	settings.endGroup();
	
	statusTextEdit->setFocus(Qt::OtherFocusReason);
	
	event->accept();
}
	
void MainWindow::resizeEvent(QResizeEvent *event) {
	for (int i = 0; i < TWITTER_TABS; ++i) {
		twitterTabs[i].twitterWidget->resize(twitterTabs[i].scrollArea->width() - twitterTabs[i].scrollArea->verticalScrollBar()->width() - 5, 500);
	}
	event->accept();
}

void MainWindow::updated(const QByteArray &buffer, int type) {
	if (twitterTabs[type].clear) {
		twitterTabs[type].twitterWidget->clear();
	}
	
	QDomDocument document;
	
	document.setContent(buffer);
	
	QDomElement root = document.documentElement();
	if (root.tagName() != "statuses") {
		return;
	}
	QDomNode node = root.firstChild();
	QString html = "";
	QString trayMessage = "";
	int maxId = twitterTabs[type].lastId;
	int j = 0;
	while (!node.isNull()) {
		if (node.toElement().tagName() != "status") {
			return;
		}
		QDomNode node2 = node.firstChild();
		QString message = "", timeStr = "", user = "", image = "";
		int id = 0, replyUserID = 0, replyStatusId = 0;
		while (!node2.isNull()) {
			if (node2.toElement().tagName() == "created_at") {
				timeStr = node2.toElement().text();
			} else if (node2.toElement().tagName() == "text") {
				message = node2.toElement().text();
			} else if (node2.toElement().tagName() == "id") {
				id = node2.toElement().text().toInt();
			} else if (node2.toElement().tagName() == "in_reply_to_status_id") {
				replyStatusId = node2.toElement().text().toInt();
			} else if (node2.toElement().tagName() == "in_reply_to_user_id") {
				replyUserID = node2.toElement().text().toInt();
			} else if (node2.toElement().tagName() == "user") {
				QDomNode node3 = node2.firstChild();
				while (!node3.isNull()) {
					if (node3.toElement().tagName() == "screen_name") {
						user = node3.toElement().text();
					} else if (node3.toElement().tagName() == "profile_image_url") {
						image = node3.toElement().text();
					}
					node3 = node3.nextSibling();
				}
			}
			node2 = node2.nextSibling();
		}
		if (id) {
			if (id > maxId) maxId = id;
			QDateTime time = QDateTime::fromString(timeStr, "ddd MMM dd hh:mm:ss +0000 yyyy");
			time = QDateTime(time.date(), time.time(), Qt::UTC);
			if (id > twitterTabs[type].lastId) {
				trayMessage += user + ": " + message + " " + TwitterWidget::formatDateTime(time.toLocalTime()) + "\n";
			}
			QByteArray hash = QCryptographicHash::hash(image.toAscii(), QCryptographicHash::Md5);
			QString imageFileName = "";
			for (int i = 0; i < hash.size(); ++i) {
				unsigned char c = hash[i];
				c >>= 4;
				if (c < 10) {
					c += '0';
				} else {
					c += 'A' - 10;
				}
				imageFileName += (char)c;
				c = hash[i];
				c &= 15;
				if (c < 10) {
					c += '0';
				} else {
					c += 'A' - 10;
				}
				imageFileName += (char)c;
			}
			imageFileName += "." + QFileInfo(QUrl(image).path()).suffix();
			QDir dir(QDir::homePath());
			dir.mkdir(".qwit");
			imageFileName = dir.absolutePath() + "/.qwit/" + imageFileName;
			userpicsDownloader.download(image, imageFileName);
			
			twitterTabs[type].twitterWidget->addItem(imageFileName, user, message.simplified(), time.toLocalTime(), id, replyStatusId, j++);
		}
		node = node.nextSibling();
	}
	twitterTabs[type].lastId = maxId;
	twitterTabs[type].lastUpdateTime = QDateTime::currentDateTime().toTime_t();
	if (trayMessage != "") {
		trayIcon->showMessage("Qwit updates", trayMessage);
	}
}

void MainWindow::statusUpdated() {
	updateTimeline();
}

void MainWindow::leftCharsNumberChanged(int count) {
	if (count >= 0) charsLeftLabel->setForegroundRole(QPalette::Light);
	else charsLeftLabel->setForegroundRole(QPalette::Dark);
	charsLeftLabel->setText(QString::number(count));
}

void MainWindow::showhide() {
	if (isVisible()) {
		logsDialog->hide();
		hide();
	} else {
		show();
		for (int i = 0; i < TWITTER_TABS; ++i) {
			twitterTabs[i].twitterWidget->updateItems();
		}
	}
}

void MainWindow::updateState(const QString &state) {
	stateLabel->setText(state);
	stateLabel->setToolTip(state);
	logsDialog->logsTextEdit->setPlainText(logsDialog->logsTextEdit->toPlainText() + QDateTime::currentDateTime().toString("hh:mm:ss") + " - " + state + "\n");
}

void MainWindow::tabChanged(int index) {
	if (index < TWITTER_TABS) {
		int height = twitterTabs[index].scrollArea->width() - twitterTabs[index].scrollArea->verticalScrollBar()->width() - 5;
		if (twitterTabs[index].twitterWidget->height() != height) {
			twitterTabs[index].twitterWidget->resize(height, 500);
		}
		int time = QDateTime::currentDateTime().toTime_t();
		if (twitterTabs[index].lastUpdateTime + interval <= time) {
			twitter.update(username, password, twitterTabs[index].lastId, index);
		}
	}
}

void MainWindow::updateItems() {
	for (int i = 0; i < TWITTER_TABS; ++i) {
		twitterTabs[i].twitterWidget->updateItems();
	}
}

void MainWindow::customUsernameChanged() {
	twitter.setUrl(CUSTOM_TWITTER_TAB, QString(CUSTOM_XML_URL) + customUsernameLineEdit->text() + ".xml");
	
	QSettings settings("arti", "qwit");
	
	settings.beginGroup("Other");
	settings.setValue("customUsername", customUsernameLineEdit->text());
	settings.endGroup();
	
	updateTimeline();
}

#endif
