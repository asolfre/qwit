#ifndef FRIENDSMGMTTAB_H
#define FRIENDSMGMTTAB_H

#include <QScrollArea>
#include "FriendsMgmtWidget.h"

class FriendsMgmtTab
{
private:
    FriendsMgmtWidget *friendsMgmtWidget;
    QScrollArea *scrollArea;

public:
    FriendsMgmtTab(QScrollArea *scrollArea, FriendsMgmtWidget *friendsMgmtWidget);
    FriendsMgmtTab();
    FriendsMgmtWidget* getFriendsMgmtWidget();
    QScrollArea* getScrollArea();
};

#endif // FRIENDSMGMTTAB_H
