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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if defined(__linux__)
#include <dlfcn.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QKeyEvent>
#include <QWindow>
#include <QFileDialog>
#include <QDesktopServices>
#include <QStyleFactory>
#if defined(ANDROID)
#include <QScroller>
#endif
#include <treeproxyfilter.h>

#include <vector>
#include <settings.h>
#include <settingsdialog.h>
#include "deviceinfo.h"
#include "platforminfo.h"
#include "database.h"
#include "submitdialog.h"
#include "CL/cl.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static const QString version;
    static const QString reportVersion;
    
    struct OperatingSystem
    {
        QString name;
        QString version;
        QString architecture;
        int type;
    } operatingSystem;

    std::vector<PlatformInfo> platforms;
    std::vector<DeviceInfo> devices;

    ReportState reportState = ReportState::unknown;
    
    void getDevices();
    void displayDevice(uint32_t index);
    void getOperatingSystem();

private:
    Ui::MainWindow *ui;
    Settings appSettings;

    int selectedDeviceIndex = 0;

    struct FilterProxies {
        TreeProxyFilter deviceinfo;
        TreeProxyFilter deviceExtensions;
        TreeProxyFilter deviceImageFormats;
        TreeProxyFilter platformInfo;
        TreeProxyFilter platformExtensions;
    } filterProxies;
    struct Models {
        QStandardItemModel deviceinfo;
        QStandardItemModel deviceExtensions;
        QStandardItemModel deviceImageFormats;
        QStandardItemModel platformInfo;
        QStandardItemModel platformExtensions;
    } models;

    Database database;

    bool checkOpenCLAvailability(QString &error);

    void connectFilterAndModel(QStandardItemModel& model, TreeProxyFilter& filter);

    void displayDeviceInfo(DeviceInfo &device);
    void displayDeviceExtensions(DeviceInfo &device);
    void displayDeviceImageFormats(DeviceInfo &device);
    void displayPlatformInfo(PlatformInfo& platform);
    void displayPlatformExtensions(PlatformInfo& platform);
    void displayOperatingSystem();

    void setReportState(ReportState state);
    void checkReportDatabaseState();

    void reportToJson(DeviceInfo& device, QString submitter, QString comment, QJsonObject& jsonObject);
    void saveReport(QString fileName, QString submitter, QString comment);

#if defined(ANDROID)
    void setTouchProps(QWidget *widget);
#endif
private Q_SLOTS:
    void slotComboBoxDeviceChanged(int index);
    void slotClose();
    void slotAbout();
    void slotSettings();
    void slotDisplayOnlineReport();
    void slotBrowseDatabase();
    void slotSaveReport();
    void slotUploadReport();
    void slotFilterDeviceInfo(QString text);
    void slotFilterDeviceExtensions(QString text);
    void slotFilterDeviceImageFormats(QString text);
    void slotFilterPlatformInfo(QString text);
    void slotFilterPlatformExtensions(QString text);
};
#endif // MAINWINDOW_H
