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

#ifndef OptionsDialog_h
#define OptionsDialog_h

#include "ui_OptionsDialog.h"

#include "AccountConfigurationDialog.h"

#include "Account.h"

class OptionsDialog: public QDialog, public Ui::OptionsDialog {
	Q_OBJECT

public:
	AccountConfigurationDialog *accountConfigurationDialog;
	
	OptionsDialog(QWidget *parent);
	
public slots:

	void changeOptionsGroup(QTreeWidgetItem *item);
	void addAccount();
	void deleteAccount();
	void editAccount();
	void commitAccount();
};

#endif
