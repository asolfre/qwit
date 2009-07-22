# #####################################################################
# Automatically generated by qmake (2.01a) Tue Dec 16 18:48:51 2008
# #####################################################################
TEMPLATE = app
TARGET = 
DEPENDPATH += src \
    images
INCLUDEPATH += src
OBJECTS_DIR = var
MOC_DIR = var
UI_SOURCES_DIR = var
UI_HEADERS_DIR = var
RCC_DIR = var
unix:# Prefix: base instalation directory
isEmpty( PREFIX ):PREFIX = /usr/local

# Input
HEADERS += src/LogsDialog.h \
    src/MainWindow.h \
    src/OptionsDialog.h \
    src/StatusTextEdit.h \
    src/Twitter.h \
    src/TwitterWidget.h \
    src/UserpicsDownloader.h \
    src/TwitPicDialog.h \
    src/TwitterWidgetItem.h
FORMS += src/LogsDialog.ui \
    src/MainWindow.ui \
    src/OptionsDialog.ui
SOURCES += src/LogsDialog.cpp \
    src/MainWindow.cpp \
    src/OptionsDialog.cpp \
    src/qwit.cpp \
    src/StatusTextEdit.cpp \
    src/Twitter.cpp \
    src/TwitterWidget.cpp \
    src/UserpicsDownloader.cpp \
    src/TwitPicDialog.cpp \
    src/TwitterWidgetItem.cpp
TRANSLATIONS += translations/qwit_en_US.ts \
    translations/qwit_es_ES.ts \
    translations/qwit_it_IT.ts \
    translations/qwit_pt_BR.ts \
    translations/qwit_ru_RU.ts \
    translations/qwit_tr_TR.ts \
    translations/qwit_de_DE.ts \
    translations/qwit_kk_KZ.ts \
    translations/qwit_zh_CN.ts
QT += network \
    xml
RESOURCES = qwit.qrc

# Windows icons
RC_FILE = qwit-win.rc

# Mac OS X icon
ICON = images/qwit.icns
target.path = $${PREFIX}/bin
desktop.path = $${PREFIX}/share/applications
desktop.files += qwit.desktop
icon.path = $${PREFIX}/share/icons
icon.files += images/qwit.png
INSTALLS = target \
    desktop \
    icon
CONFIG += debug \
    x86 \
    ppc \
    x86_64 \
    ppc64
OTHER_FILES += qwit-win.rc
