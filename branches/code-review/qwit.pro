######################################################################
# Automatically generated by qmake (2.01a) Tue Dec 16 18:48:51 2008
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += src images
INCLUDEPATH += src
OBJECTS_DIR = var
MOC_DIR = var
UI_SOURCES_DIR = var
UI_HEADERS_DIR = var
RCC_DIR = var

unix {
    # Prefix: base instalation directory
    isEmpty( PREFIX ) {
      PREFIX = /usr/local
    }
}
# Input
HEADERS += \
	   src/Configuration.h \
           src/MainWindow.h \
           src/StatusTextEdit.h \
           src/OptionsDialog.h \
           src/AbstractAccount.h \
           src/TwitterAccount.h \
           src/IdenticaAccount.h \
           src/CustomAccount.h \
           src/TwitterAccountDialog.h
FORMS += \
           src/MainWindow.ui \
           src/OptionsDialog.ui \
           src/TwitterAccountDialog.ui
SOURCES += \
           src/qwit.cpp \
	   src/Configuration.cpp \
           src/MainWindow.cpp \
           src/StatusTextEdit.cpp \
           src/OptionsDialog.cpp \
           src/TwitterAccountDialog.cpp
TRANSLATIONS += \
           translations/qwit_en_US.ts \
           translations/qwit_es_ES.ts \
           translations/qwit_it_IT.ts \
           translations/qwit_pt_BR.ts \
           translations/qwit_ru_RU.ts \
           translations/qwit_tr_TR.ts
QT += network xml
RESOURCES = qwit.qrc
target.path = $${PREFIX}/bin
desktop.path = $${PREFIX}/share/applications
desktop.files += qwit.desktop
icon.path = $${PREFIX}/share/icons
icon.files += images/qwit.png
INSTALLS = target desktop icon
CONFIG += debug x86 ppc x86_64 ppc64
