#-------------------------------------------------
#
# Project created by QtCreator 2017-01-22T22:56:18
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blocktrix_project
TEMPLATE = app


SOURCES += main.cpp \
    BlockTrixBoard.cpp \
    BlockTrixPiece.cpp \
    BlockTrixWindow.cpp \
    Player.cpp

HEADERS  += \
    BlockTrixBoard.h \
    BlockTrixPiece.h \
    BlockTrixWindow.h \
    Interfaces.h \
    Player.h
