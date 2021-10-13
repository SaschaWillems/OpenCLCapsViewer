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
