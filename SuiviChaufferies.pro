#-------------------------------------------------
#
# Project created by QtCreator 2016-02-09T11:05:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = SuiviChaufferies
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    configurationdialog.cpp \
    qcustomplot/qcustomplot.cpp \
    addfueldeliverydialog.cpp \
    addmetersrecorddialog.cpp \
    showmetersrecordsdialog.cpp \
    showfueldeliveriesdialog.cpp

HEADERS  += mainwindow.h \
    configurationdialog.h \
    qcustomplot/qcustomplot.h \
    addfueldeliverydialog.h \
    addmetersrecorddialog.h \
    showmetersrecordsdialog.h \
    showfueldeliveriesdialog.h

FORMS    += mainwindow.ui \
    configurationdialog.ui \
    addfueldeliverydialog.ui \
    addmetersrecorddialog.ui \
    showmetersrecordsdialog.ui \
    showfueldeliveriesdialog.ui

QMAKE_CXXFLAGS += -std=c++11
