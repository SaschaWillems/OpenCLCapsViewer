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
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Models
    ui->treeViewDeviceInfo->setModel(&filterProxies.deviceinfo);
    filterProxies.deviceinfo.setSourceModel(&models.deviceinfo);
    connect(ui->filterLineEditDeviceInfo, SIGNAL(textChanged(QString)), this, SLOT(slotFilterDeviceInfo(QString)));
    ui->treeViewDeviceExtensions->setModel(&filterProxies.extensions);
    filterProxies.extensions.setSourceModel(&models.extensions);
    connect(ui->filterLineEditExtensions, SIGNAL(textChanged(QString)), this, SLOT(slotFilterExtensions(QString)));

    // Slots
    connect(ui->toolButtonExit, SIGNAL(pressed()), this, SLOT(slotClose()));

    getDevices();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getDevices()
{
	cl_uint numPlatforms;
	cl_int status = clGetPlatformIDs(0, nullptr, &numPlatforms);
	if (status != CL_SUCCESS)
	{
        QMessageBox::warning(this, tr("Error"), "Could not get platform count!");
        return;
    }

    std::vector<cl_platform_id> platforms(numPlatforms);
    status = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    if (status != CL_SUCCESS)
    {
        QMessageBox::warning(this, tr("Error"), "Could not read platforms!");
        return;
    }
    for (cl_platform_id platform : platforms)
    {
        // @todo: store platform information
        
        // @todo: Error checking
        cl_uint numDevices;
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
        std::vector<cl_device_id> deviceIds(numDevices);
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, deviceIds.data(), nullptr);

        for (auto deviceId : deviceIds) 
        {
            DeviceInfo deviceInfo{};
            deviceInfo.deviceId = deviceId;
            deviceInfo.read();
            devices.push_back(deviceInfo);
        }

    }

    ui->comboBoxGPU->clear();
    for (DeviceInfo device : devices)
    {
        QString deviceName = QString::fromStdString("[GPU" + std::to_string(0) + "] " + device.name);
        ui->comboBoxGPU->addItem(deviceName);
    }

    if (devices.size() > 0)
    {
        displayDevice(0);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), "Could not find a device with OpenCL support!");
    }

}

void MainWindow::displayDevice(uint32_t index)
{
    DeviceInfo& device = devices[index];
    displayDeviceExtensions(device);
    displayDeviceInfo(device);
}

void MainWindow::displayDeviceInfo(DeviceInfo& device)
{
    models.deviceinfo.clear();
    QStandardItem* rootItem = models.deviceinfo.invisibleRootItem();
    for (auto info : device.deviceInfo) {
        if (info.extension.isEmpty()) {
            QList<QStandardItem*> extItem;
            extItem << new QStandardItem(info.name);
            extItem << new QStandardItem(info.value.toString());
            rootItem->appendRow(extItem);
        }
    }
    ui->treeViewDeviceInfo->expandAll();
    ui->treeViewDeviceInfo->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::displayDeviceExtensions(DeviceInfo& device)
{
    models.extensions.clear();
    QStandardItem* rootItem = models.extensions.invisibleRootItem();
    for (auto& extension : device.extensions) {
        QList<QStandardItem*> extItem;
        extItem << new QStandardItem(extension.name);
        extItem << new QStandardItem(utils::clVersionString(extension.version));
        // Append extension related device info
        for (auto info : device.deviceInfo) {
            if (extension.name == info.extension) {
                QList<QStandardItem*> extInfoItem;
                extInfoItem << new QStandardItem(info.name);
                extInfoItem << new QStandardItem(info.value.toString());
                extItem.first()->appendRow(extInfoItem);
            }
        }
        rootItem->appendRow(extItem);
    }
    ui->treeViewDeviceExtensions->expandAll();
    ui->treeViewDeviceExtensions->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void MainWindow::slotFilterDeviceInfo(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
    filterProxies.deviceinfo.setFilterRegExp(regExp);
}

void MainWindow::slotFilterExtensions(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
    filterProxies.extensions.setFilterRegExp(regExp);
}

void MainWindow::slotClose()
{
    close();
}

