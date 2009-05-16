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

#ifndef MainWindowTab_cpp
#define MainWindowTab_cpp

#include "MainWindowTab.h"

#include "Configuration.h"

#include <iostream>

using namespace std;

MainWindowTab::MainWindowTab(QWidget *tabWidget) {
	this->tabWidget = tabWidget;
}

void MainWindowTab::setEnabled(bool enabled) {
	this->enabled = enabled;
}

bool MainWindowTab::isEnabled() {
	return enabled;
}

#endif
