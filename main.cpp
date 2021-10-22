/*
*
* OpenCL hardware capability viewer
*
* Copyright (C) 2021 by Sascha Willems (www.saschawillems.de)
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

#include "mainwindow.h"

#include <QApplication>
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
        textStream << logMessage << endl;
    };
}

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication application(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption optionLogFile("log", "Write log messages to a text file for debugging (log.txt)");
    parser.setApplicationDescription("OpenCL Hardware Capability Viewer");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(optionLogFile);
    parser.process(application);
    if (parser.isSet(optionLogFile)) {
        qInstallMessageHandler(logMessageHandler);
    }
    qInfo() << "Application start";
    MainWindow w;
    w.show();
    return application.exec();
}
