#-------------------------------------------------
#
# Project created by QtCreator 2021-07-09T12:31:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XMLEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    controller.cpp \
    attribute.cpp \
    Tokens.cpp \
    helper.cpp

HEADERS  += mainwindow.h \
    controller.h \
    helper.h \
    stack.h \
    attribute.h \
    Tokens.h

FORMS    += mainwindow.ui
