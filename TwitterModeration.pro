#-------------------------------------------------
#
# Project created by QtCreator 2015-03-25T12:41:41
#
#-------------------------------------------------

QT       += core gui network webview
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TwitterModeration
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    OAuth2Qt/src/o1.cpp \
    OAuth2Qt/src/o1requestor.cpp \
    OAuth2Qt/src/o2.cpp \
    OAuth2Qt/src/o2facebook.cpp \
    OAuth2Qt/src/o2gft.cpp \
    OAuth2Qt/src/o2hubic.cpp \
    OAuth2Qt/src/o2reply.cpp \
    OAuth2Qt/src/o2replyserver.cpp \
    OAuth2Qt/src/o2requestor.cpp \
    OAuth2Qt/src/o2settingsstore.cpp \
    OAuth2Qt/src/o2skydrive.cpp \
    OAuth2Qt/src/oxtwitter.cpp \
    OAuth2Qt/src/simplecrypt.cpp \
    twittermoderationmodel.cpp \
    twittermoderationitem.cpp \
    jsondelegate.cpp \
    progressdelegate.cpp \
    jsonparser.cpp \
    twitterwalldialog.cpp

HEADERS  += mainwindow.h \
    OAuth2Qt/src/o1.h \
    OAuth2Qt/src/o1dropbox.h \
    OAuth2Qt/src/o1flickr.h \
    OAuth2Qt/src/o1freshbooks.h \
    OAuth2Qt/src/o1requestor.h \
    OAuth2Qt/src/o1twitter.h \
    OAuth2Qt/src/o2.h \
    OAuth2Qt/src/o2abstractstore.h \
    OAuth2Qt/src/o2facebook.h \
    OAuth2Qt/src/o2gft.h \
    OAuth2Qt/src/o2globals.h \
    OAuth2Qt/src/o2hubic.h \
    OAuth2Qt/src/o2reply.h \
    OAuth2Qt/src/o2replyserver.h \
    OAuth2Qt/src/o2requestor.h \
    OAuth2Qt/src/o2settingsstore.h \
    OAuth2Qt/src/o2skydrive.h \
    OAuth2Qt/src/oxtwitter.h \
    OAuth2Qt/src/simplecrypt.h \
    twittermoderationmodel.h \
    twittermoderationitem.h \
    jsondelegate.h \
    progressdelegate.h \
    jsonparser.h \
    twitterwalldialog.h

FORMS    += mainwindow.ui \
    help.ui \
    twitterwalldialog.ui

DISTFILES +=
