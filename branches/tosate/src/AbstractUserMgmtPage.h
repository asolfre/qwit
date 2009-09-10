/*!
 *  $Id$
 *
 *  @file
 *  @author Thomas Salm <tosate@googlemail.com>
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
 *  AbstractUserMgmtPage class declaration
 *
 */

#ifndef ABSTRACTUSERMGMTPAGE_H
#define ABSTRACTUSERMGMTPAGE_H

#include "QwitHeaders.h"

#include "UserMgmtWidget.h"
#include "Configuration.h"

class AbstractUserMgmtPage : public QWidget
{
    Q_OBJECT

protected:
    QScrollArea *scrollArea;
    UserMgmtWidget *userMgmtWidget;

public:
    AbstractUserMgmtPage(QWidget *parent = 0);
    virtual void update(Account *accout = 0) = 0;
    virtual void redraw();
    virtual void addItem(UserData userData);
    virtual void updateSize() = 0;
    virtual QString title() = 0;
    virtual void reloadUserpics();
    void clear();

public slots:
    virtual void updateItems(const QVector<UserData> &items);
};

#endif // ABSTRACTUSERMGMTPAGE_H
