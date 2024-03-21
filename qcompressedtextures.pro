QT       += core gui
QT += widgets
CONFIG += c++11

res.files = $$PWD/mtest.png $$PWD/mtest4x4.astc
res.base = $$PWD/
res.prefix = /

RESOURCES += res

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

DISTFILES += \
    README.md

