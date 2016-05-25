#-------------------------------------------------
#
# Project created by QtCreator 2016-02-09T11:05:03
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = SuiviChaufferies
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    configurationdialog.cpp \
    qcustomplot/qcustomplot.cpp \
    showmetersrecordsdialog.cpp \
    showfueldeliveriesdialog.cpp \
    delegates/comboboxdelegate.cpp \
    delegates/doublespinboxdelegate.cpp \
    dju.cpp \
    filedownloader.cpp \
    heatingsystem.cpp \
    energyconsumptionchart.cpp \
    tabstyle.cpp \
    addfueldelivery.cpp \
    addmetersrecord.cpp

HEADERS  += mainwindow.h \
    configurationdialog.h \
    qcustomplot/qcustomplot.h \
    showmetersrecordsdialog.h \
    showfueldeliveriesdialog.h \
    delegates/comboboxdelegate.h \
    delegates/doublespinboxdelegate.h \
    dju.h \
    filedownloader.h \
    heatingsystem.h \
    energyconsumptionchart.h \
    tabstyle.h \
    addfueldelivery.h \
    addmetersrecord.h

FORMS    += mainwindow.ui \
    configurationdialog.ui \
    showmetersrecordsdialog.ui \
    showfueldeliveriesdialog.ui \
    addfueldelivery.ui \
    addmetersrecord.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES = SuiviChaufferies.qrc
