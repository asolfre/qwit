#include "FriendsMgmtTab.h"

FriendsMgmtTab::FriendsMgmtTab()
{
}

FriendsMgmtTab::FriendsMgmtTab(QScrollArea *scrollArea, FriendsMgmtWidget *friendsMgmtWidget)
{
    this->scrollArea = scrollArea;
    this->friendsMgmtWidget = friendsMgmtWidget;
}

FriendsMgmtWidget* FriendsMgmtTab::getFriendsMgmtWidget()
{
    return this->friendsMgmtWidget;
}

QScrollArea* FriendsMgmtTab::getScrollArea()
{
    return this->scrollArea;
}
