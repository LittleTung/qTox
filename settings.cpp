/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>
    
    This file is part of Tox Qt GUI.
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    
    See the COPYING file for more details.
*/

#include "settings.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

const QString Settings::FILENAME = "settings.ini";

Settings::Settings() :
    loaded(false)
{
    load();
}

Settings::~Settings()
{
    save();
}

Settings& Settings::getInstance()
{
    static Settings settings;
    return settings;
}

void Settings::load()
{
    if (loaded) {
        return;
    }

    QString filePath = getSettingsDirPath() + '/' + FILENAME;

    //if no settings file exist -- use the default one
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "No settings file found, using defaults";
        filePath = ":/conf/" + FILENAME;
    }

    QSettings s(filePath, QSettings::IniFormat);
    s.beginGroup("DHT Server");
        int serverListSize = s.beginReadArray("dhtServerList");
        for (int i = 0; i < serverListSize; i ++) {
            s.setArrayIndex(i);
            DhtServer server;
            server.name = s.value("name").toString();
            server.userId = s.value("userId").toString();
            server.address = s.value("address").toString();
            server.port = s.value("port").toInt();
            dhtServerList << server;
        }
        s.endArray();
    s.endGroup();

    //NOTE: uncomment when logging will be implemented
/*
    s.beginGroup("Logging");
       enableLogging = s.value("enableLogging", false).toBool();
       encryptLogs = s.value("encryptLogs", true).toBool();
    s.endGroup();
*/

    s.beginGroup("General");
        username = s.value("username", "My name").toString();
        statusMessage = s.value("statusMessage", "My status").toString();
    s.endGroup();

    s.beginGroup("Widgets");
        QList<QString> objectNames = s.childKeys();
        for (const QString& name : objectNames) {
            widgetSettings[name] = s.value(name).toByteArray();
        }
    s.endGroup();

    s.beginGroup("GUI");
        enableSmoothAnimation = s.value("smoothAnimation", true).toBool();
        smileyPack = s.value("smileyPack").toByteArray();
        customEmojiFont = s.value("customEmojiFont", true).toBool();
        emojiFontFamily = s.value("emojiFontFamily", "DejaVu Sans").toString();
        emojiFontPointSize = s.value("emojiFontPointSize", QApplication::font().pointSize()).toInt();
        firstColumnHandlePos = s.value("firstColumnHandlePos", 50).toInt();
        secondColumnHandlePosFromRight = s.value("secondColumnHandlePosFromRight", 50).toInt();
        timestampFormat = s.value("timestampFormat", "hh:mm").toString();
        minimizeOnClose = s.value("minimizeOnClose", false).toBool();
    s.endGroup();

    s.beginGroup("Privacy");
        typingNotification = s.value("typingNotification", false).toBool();
    s.endGroup();

    loaded = true;
}

void Settings::save()
{
    QString filePath = getSettingsDirPath() + '/' + FILENAME;

    QSettings s(filePath, QSettings::IniFormat);

    s.clear();

    s.beginGroup("DHT Server");
        s.beginWriteArray("dhtServerList", dhtServerList.size());
        for (int i = 0; i < dhtServerList.size(); i ++) {
            s.setArrayIndex(i);
            s.setValue("name", dhtServerList[i].name);
            s.setValue("userId", dhtServerList[i].userId);
            s.setValue("address", dhtServerList[i].address);
            s.setValue("port", dhtServerList[i].port);
        }
        s.endArray();
    s.endGroup();

    //NOTE: uncomment when logging will be implemented
/*
    s.beginGroup("Logging");
        s.setValue("storeLogs", enableLogging);
        s.setValue("encryptLogs", encryptLogs);
    s.endGroup();
*/

    s.beginGroup("General");
        s.setValue("username", username);
        s.setValue("statusMessage", statusMessage);
    s.endGroup();

    s.beginGroup("Widgets");
    const QList<QString> widgetNames = widgetSettings.keys();
    for (const QString& name : widgetNames) {
        s.setValue(name, widgetSettings.value(name));
    }
    s.endGroup();

    s.beginGroup("GUI");
        s.setValue("smoothAnimation", enableSmoothAnimation);
        s.setValue("smileyPack", smileyPack);
        s.setValue("customEmojiFont", customEmojiFont);
        s.setValue("emojiFontFamily", emojiFontFamily);
        s.setValue("emojiFontPointSize", emojiFontPointSize);
        s.setValue("firstColumnHandlePos", firstColumnHandlePos);
        s.setValue("secondColumnHandlePosFromRight", secondColumnHandlePosFromRight);
        s.setValue("timestampFormat", timestampFormat);
        s.setValue("minimizeOnClose", minimizeOnClose);
    s.endGroup();

    s.beginGroup("Privacy");
        s.setValue("typingNotification", typingNotification);
    s.endGroup();
}

QString Settings::getSettingsDirPath()
{
    // workaround for https://bugreports.qt-project.org/browse/QTBUG-38845
#ifdef Q_OS_WIN
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + '/' + qApp->organizationName() + '/' + qApp->applicationName();
#endif
}

const QList<Settings::DhtServer>& Settings::getDhtServerList() const
{
    return dhtServerList;
}

void Settings::setDhtServerList(const QList<DhtServer>& newDhtServerList)
{
    dhtServerList = newDhtServerList;
    emit dhtServerListChanged();
}

QString Settings::getUsername() const
{
    return username;
}

void Settings::setUsername(const QString& newUsername)
{
    username = newUsername;
}

QString Settings::getStatusMessage() const
{
    return statusMessage;
}

void Settings::setStatusMessage(const QString& newMessage)
{
    statusMessage = newMessage;
}

bool Settings::getEnableLogging() const
{
    return enableLogging;
}

void Settings::setEnableLogging(bool newValue)
{
    enableLogging = newValue;
}

bool Settings::getEncryptLogs() const
{
    return encryptLogs;
}

void Settings::setEncryptLogs(bool newValue)
{
    encryptLogs = newValue;
}

void Settings::setWidgetData(const QString& uniqueName, const QByteArray& data)
{
    widgetSettings[uniqueName] = data;
}

QByteArray Settings::getWidgetData(const QString& uniqueName) const
{
    return widgetSettings.value(uniqueName);
}

bool Settings::isAnimationEnabled() const
{
    return enableSmoothAnimation;
}

void Settings::setAnimationEnabled(bool newValue)
{
    enableSmoothAnimation = newValue;
}

QByteArray Settings::getSmileyPack() const
{
    return smileyPack;
}

void Settings::setSmileyPack(const QByteArray &value)
{
    smileyPack = value;
    emit smileyPackChanged();
}

bool Settings::isCurstomEmojiFont() const
{
    return customEmojiFont;
}

void Settings::setCurstomEmojiFont(bool value)
{
    customEmojiFont = value;
    emit emojiFontChanged();
}

int Settings::getEmojiFontPointSize() const
{
    return emojiFontPointSize;
}

void Settings::setEmojiFontPointSize(int value)
{
    emojiFontPointSize = value;
    emit emojiFontChanged();
}

int Settings::getFirstColumnHandlePos() const
{
    return firstColumnHandlePos;
}

void Settings::setFirstColumnHandlePos(const int pos)
{
    firstColumnHandlePos = pos;
}

int Settings::getSecondColumnHandlePosFromRight() const
{
    return secondColumnHandlePosFromRight;
}

void Settings::setSecondColumnHandlePosFromRight(const int pos)
{
    secondColumnHandlePosFromRight = pos;
}

const QString &Settings::getTimestampFormat() const
{
    return timestampFormat;
}

void Settings::setTimestampFormat(const QString &format)
{
    timestampFormat = format;
    emit timestampFormatChanged();
}

QString Settings::getEmojiFontFamily() const
{
    return emojiFontFamily;
}

void Settings::setEmojiFontFamily(const QString &value)
{
    emojiFontFamily = value;
    emit emojiFontChanged();
}

bool Settings::isMinimizeOnCloseEnabled() const
{
    return minimizeOnClose;
}

void Settings::setMinimizeOnClose(bool newValue)
{
    minimizeOnClose = newValue;
}

bool Settings::isTypingNotificationEnabled() const
{
    return typingNotification;
}

void Settings::setTypingNotification(bool enabled)
{
    typingNotification = enabled;
}
