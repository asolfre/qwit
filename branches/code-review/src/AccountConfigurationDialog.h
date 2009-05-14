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

#ifndef AccountConfigurationDialog_h
#define AccountConfigurationDialog_h

#include "ui_AccountConfigurationDialog.h"

const int ACTION_ADD = 0;
const int ACTION_EDIT = 1;

class AccountConfigurationDialog: public QDialog, public Ui::AccountConfigurationDialog {
	Q_OBJECT
	
public:
	int action;
	int accountId;
	int accountType;
	
	AccountConfigurationDialog(QWidget *parent);
};

#endif
