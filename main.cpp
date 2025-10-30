/*
*
* OpenCL hardware capability viewer
*
* Copyright (C) 2021-2025 by Sascha Willems (www.saschawillems.de)
*
* This code is free software, you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License version 3 as published by the Free Software Foundation.
*
* Please review the following information to ensure the GNU Lesser
* General Public License version 3 requirements will be met:
* http://opensource.org/licenses/lgpl-3.0.html
*
* The code is distributed WITHOUT ANY WARRANTY; without even the
* implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU LGPL 3.0 for more details.
*
*/

#ifdef GUI_BUILD
#include <QApplication>
#include "mainwindow.h"
#else
#include <QCoreApplication>
#endif

#include "database.h"
#include "openclinfo.h"
#include "openclfunctions.h"
#include "operatingsystem.h"
#include "report.h"
#include "settings.h"
#include <stdio.h>
#include <iostream>
#include <QCommandLineParser>

void logMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString msgType;
    switch (type) {
    case QtInfoMsg:
        msgType = "Info";
        break;
    case QtDebugMsg:
        msgType = "Debug";
        break;
    case QtWarningMsg:
        msgType = "Warning";
        break;
    case QtCriticalMsg:
        msgType = "Critical";
        break;
    case QtFatalMsg:
        msgType = "Fatal";
        break;
    }
    QDateTime timeStamp = QDateTime::currentDateTime();
    QString logMessage = QString("%1: %2: %3").arg(timeStamp.toString("yyyy-MM-dd hh:mm:ss.zzz")).arg(msgType).arg(msg);
    QFile logFile("log.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream textStream(&logFile);
        textStream << logMessage << Qt::endl;
    }
    // Suppress compiler warning
    (void)context;
}

#ifndef GUI
void logMessageHandlerCli(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // Suppress compiler warning
    (void)type;
    (void)context;
    (void)msg;
    return;
}
#endif

int main(int argc, char *argv[])
{    

#ifdef GUI_BUILD
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    #ifdef Q_OS_WIN
        // Disable dark mode on Winodows as it doesn't look very good
        qputenv("QT_QPA_PLATFORM", "windows::darkmode=0");
    #endif
#endif
    QApplication::setApplicationVersion(appVersion);
    QApplication application(argc, argv);
#else
    QCoreApplication application(argc, argv);
    QCoreApplication::setApplicationVersion(appVersion);
    qInstallMessageHandler(logMessageHandlerCli);
#endif

    QCommandLineParser parser;
    QCommandLineOption optionLogFile("log", "Write log messages to a text file for debugging (log.txt)");
    QCommandLineOption optionDisableProxy("noproxy", "Run withouth proxy (overrides setting)");
    QCommandLineOption optionSaveReport("save", "Save report to file without starting the GUI", "savereport", "");
    QCommandLineOption optionUploadReport("upload", "Upload report for device with given index to the database without visual interaction");
    QCommandLineOption optionUploadReportDeviceIndex("deviceindex", "Set device index for report upload", "0");
    QCommandLineOption optionListDevices("devices", "List available devices");
    QCommandLineOption optionUploadReportSubmitter("submitter", "Set optional submitter name for report upload", "submitter", "");
    QCommandLineOption optionUploadReportComment("comment", "Set optional comment for report upload", "comment", "");

    parser.setApplicationDescription("OpenCL Hardware Capability Viewer");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(optionLogFile);
    parser.addOption(optionDisableProxy);
    parser.addOption(optionSaveReport);
    parser.addOption(optionUploadReport);
    parser.addOption(optionUploadReportDeviceIndex);
    parser.addOption(optionUploadReportSubmitter);
    parser.addOption(optionUploadReportComment);
    parser.addOption(optionListDevices);
    parser.process(application);
    if (parser.isSet(optionLogFile)) {
        qInstallMessageHandler(logMessageHandler);
    }       
    qInfo() << "Application start";
    settings.restore();
    if (parser.isSet(optionDisableProxy)) {
        settings.proxyEnabled = false;
        settings.applyProxySettings();
    }
#ifdef GUI_BUILD
    MainWindow w;
#endif

    QString error;
    if (!checkOpenCLAvailability(error))
    {
#ifdef GUI_BUILD
        QMessageBox::warning(&w, "Error", "OpenCL does not seem to be supported on this platform:\n" + error);
#else
#endif
        exit(EXIT_FAILURE);
    }
    if (!getOpenCLDevices(error)) {
#ifdef GUI_BUILD
        QMessageBox::critical(&w, "Error", error);
#else
#endif
        exit(EXIT_FAILURE);
    }
    getOperatingSystem();

    if (parser.isSet(optionListDevices))
    {
        for (size_t i = 0; i < devices.size(); i++) {
            std::cout << "[" << i << "] " << devices[i].identifier.name.toStdString() << "\n";
        }
        return 0;
    }

    if (parser.isSet(optionSaveReport))
    {
        int deviceIndex = 0;
        QString submitter = "";
        QString comment = "";
        QString filename = parser.value(optionSaveReport);
        if (parser.isSet(optionUploadReportDeviceIndex)) {
            deviceIndex = parser.value(optionUploadReportDeviceIndex).toInt();
        }
        if (deviceIndex > devices.size()) {
            std::cerr << "Device index out of range\n";
        } else {
            Report report;
            report.saveToFile(devices[deviceIndex], filename, submitter, comment);
        }
        return 0;
    }

    if (parser.isSet(optionUploadReport))
    {
        int deviceIndex = 0;
        QString submitter = "";
        QString comment = "";
        if (parser.isSet(optionUploadReportDeviceIndex)) {
            deviceIndex = parser.value(optionUploadReportDeviceIndex).toInt();
        }
        if (parser.isSet(optionUploadReportSubmitter)) {
            submitter = parser.value(optionUploadReportSubmitter);
        }
        if (parser.isSet(optionUploadReportComment)) {
            comment = parser.value(optionUploadReportComment);
        }
        if (deviceIndex > devices.size()) {
            std::cerr << "Device index out of range\n";
            return 0;
        } else {
            Report report;
            int res = report.uploadNonVisual(devices[deviceIndex], submitter, comment);
            return res;
        }
    }
    
#ifdef GUI_BUILD
    w.updateDeviceList();
    w.show();
    return application.exec();
#else
    return 0;
#endif
}
