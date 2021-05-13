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

#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QKeyEvent>
#include <QWindow>
#include <treeproxyfilter.h>

#include <vector>
#include "deviceinfo.h"
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

    std::vector<DeviceInfo> devices;
    void getDevices();
    void displayDevice(uint32_t index);

private:
    Ui::MainWindow *ui;

    struct FilterProxies {
        TreeProxyFilter deviceinfo;
        TreeProxyFilter extensions;
    } filterProxies;
    struct Models {
        QStandardItemModel deviceinfo;
        QStandardItemModel extensions;
    } models;

    void displayDeviceInfo(DeviceInfo &device);
    void displayDeviceExtensions(DeviceInfo &device);

private Q_SLOTS:
    void slotClose();
    void slotFilterDeviceInfo(QString text);
    void slotFilterExtensions(QString text);
};
#endif // MAINWINDOW_H
