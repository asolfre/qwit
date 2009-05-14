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

#ifndef MainWindow_cpp
#define MainWindow_cpp

#include "MainWindow.h"

#include "Configuration.h"

#include <iostream>

using namespace std;

MainWindow* MainWindow::instance = NULL;

MainWindow* MainWindow::getInstance() {
	if (!instance) instance = new MainWindow();
	return instance;
}

MainWindow::MainWindow(QWidget *parent): QDialog(parent) {
	setupUi(this);
	
	statusTextEdit = new StatusTextEdit(this);
	statusTextEdit->setObjectName(QString::fromUtf8("statusTextEdit"));
	verticalLayout->insertWidget(1, statusTextEdit);
	connect(statusTextEdit, SIGNAL(leftCharsNumberChanged(int)), this, SLOT(leftCharsNumberChanged(int)));

	optionsDialog = new OptionsDialog(this);
	optionsDialog->setModal(true);
	connect(optionsPushButton, SIGNAL(pressed()), optionsDialog, SLOT(showNormal()));

	connect(optionsDialog, SIGNAL(accepted()), this, SLOT(saveOptions()));
	connect(optionsDialog, SIGNAL(rejected()), this, SLOT(resetOptionsDialog()));
	
	loadState();
}

void MainWindow::leftCharsNumberChanged(int count) {
	leftCharactersNumberLabel->setText(QString::number(count));
}

void MainWindow::loadState() {
	Configuration *config = Configuration::getInstance();
	
	config->load();

	updateState();
	
	resetOptionsDialog();
}

void MainWindow::saveState() {
	Configuration *config = Configuration::getInstance();
	
	config->position = pos();
	config->size = size();
	
	config->save();
}

void MainWindow::saveOptions() {
	Configuration *config = Configuration::getInstance();

	config->showGreetingMessage = (optionsDialog->showGreetingMessageCheckBox->checkState() == Qt::Checked);
	config->greetingMessage = optionsDialog->greetingMessageLineEdit->text();
	config->showLeftCharactersNumber = (optionsDialog->showLeftCharactersNumberCheckBox->checkState() == Qt::Checked);
	config->showLastStatus = (optionsDialog->showLastStatusCheckBox->checkState() == Qt::Checked);
	config->messagesPerPage = optionsDialog->messagesPerPageLineEdit->text().toInt();
	config->messagesInPopup = optionsDialog->messagesInPopupLineEdit->text().toInt();
	config->retweetTag = optionsDialog->retweetTagLineEdit->text();
	config->retweetTagAfterText = (optionsDialog->retweetTagAfterTextCheckBox->checkState() == Qt::Checked);
	config->placeControlsVertically = (optionsDialog->placeControlsVerticallyCheckBox->checkState() == Qt::Checked);
	config->showMessagesInTray = (optionsDialog->showMessagesInTrayCheckBox->checkState() == Qt::Checked);
	config->placeUsernameUnderAvatar = (optionsDialog->placeUsernameUnderAvatarCheckBox->checkState() == Qt::Checked);

	saveState();
	updateState();
}

void MainWindow::updateState() {
	Configuration *config = Configuration::getInstance();

	if (leftCharactersNumberLabel->isHidden() && greetingMessageLabel->isHidden()) {
		if (config->showLeftCharactersNumber || config->showGreetingMessage) {
			QHBoxLayout *layout = new QHBoxLayout();
			layout->addWidget(greetingMessageLabel);
			layout->addWidget(leftCharactersNumberLabel);
			verticalLayout->insertLayout(0, layout);
		}
	} else {
		if (!config->showLeftCharactersNumber && !config->showGreetingMessage) {
			QLayout *layout = (QLayout*)verticalLayout->itemAt(0);
			verticalLayout->removeItem(verticalLayout->itemAt(0));
			delete layout;
		}
	}

	greetingMessageLabel->setText(config->greetingMessage);
	greetingMessageLabel->setVisible(config->showGreetingMessage);
	leftCharactersNumberLabel->setVisible(config->showLeftCharactersNumber);
	lastStatusLabel->setVisible(config->showLastStatus);

	move(config->position);
	resize(config->size);
}

void MainWindow::resetOptionsDialog() {
	Configuration *config = Configuration::getInstance();
	optionsDialog->showGreetingMessageCheckBox->setCheckState(config->showGreetingMessage ? Qt::Checked : Qt::Unchecked);
	optionsDialog->greetingMessageLineEdit->setText(config->greetingMessage);
	optionsDialog->showLeftCharactersNumberCheckBox->setCheckState(config->showLeftCharactersNumber ? Qt::Checked : Qt::Unchecked);
	optionsDialog->showLastStatusCheckBox->setCheckState(config->showLastStatus ? Qt::Checked : Qt::Unchecked);
	optionsDialog->messagesPerPageLineEdit->setText(QString::number(config->messagesPerPage));
	optionsDialog->messagesInPopupLineEdit->setText(QString::number(config->messagesInPopup));
	optionsDialog->retweetTagAfterTextCheckBox->setCheckState(config->retweetTagAfterText ? Qt::Checked : Qt::Unchecked);
	optionsDialog->retweetTagLineEdit->setText(config->retweetTag);
	optionsDialog->placeControlsVerticallyCheckBox->setCheckState(config->placeControlsVertically ? Qt::Checked : Qt::Unchecked);
	optionsDialog->showMessagesInTrayCheckBox->setCheckState(config->showMessagesInTray ? Qt::Checked : Qt::Unchecked);
	optionsDialog->placeUsernameUnderAvatarCheckBox->setCheckState(config->placeUsernameUnderAvatar ? Qt::Checked : Qt::Unchecked);
}

#endif
