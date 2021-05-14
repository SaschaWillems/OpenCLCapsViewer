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

const QString MainWindow::version = "1.0";
const QString MainWindow::reportVersion = "1.0";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QString title = "OpenCL Hardware Capability Viewer " + version;
    setWindowTitle(title);
    ui->labelTitle->setText(title);

    // Models
    ui->treeViewDeviceInfo->setModel(&filterProxies.deviceinfo);
    filterProxies.deviceinfo.setSourceModel(&models.deviceinfo);
    connect(ui->filterLineEditDeviceInfo, SIGNAL(textChanged(QString)), this, SLOT(slotFilterDeviceInfo(QString)));
    
    ui->treeViewDeviceExtensions->setModel(&filterProxies.deviceExtensions);
    filterProxies.deviceExtensions.setSourceModel(&models.deviceExtensions);
    connect(ui->filterLineEditExtensions, SIGNAL(textChanged(QString)), this, SLOT(slotFilterDeviceExtensions(QString)));
    
    ui->treeViewPlatformExtensions->setModel(&filterProxies.platformExtensions);
    filterProxies.platformExtensions.setSourceModel(&models.platformExtensions);
    connect(ui->filterLineEditPlatformExtensions, SIGNAL(textChanged(QString)), this, SLOT(slotFilterPlatformExtensions(QString)));

    // Slots
    connect(ui->comboBoxDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(slotComboBoxDeviceChanged(int)));
    connect(ui->toolButtonSave, SIGNAL(pressed()), this, SLOT(slotSaveReport()));
    connect(ui->toolButtonAbout, SIGNAL(pressed()), this, SLOT(slotAbout()));
    connect(ui->toolButtonExit, SIGNAL(pressed()), this, SLOT(slotClose()));

    getOperatingSystem();
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

    std::vector<cl_platform_id> platformIds(numPlatforms);
    status = clGetPlatformIDs(numPlatforms, platformIds.data(), nullptr);
    if (status != CL_SUCCESS)
    {
        QMessageBox::warning(this, tr("Error"), "Could not read platforms!");
        return;
    }
    for (cl_platform_id platformId : platformIds)
    {
        // @todo: store platform information
        
        // @todo: Error checking
        cl_uint numDevices;
        status = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
        std::vector<cl_device_id> deviceIds(numDevices);
        status = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, numDevices, deviceIds.data(), nullptr);

        PlatformInfo platformInfo{};
        platformInfo.platformId = platformId;
        platformInfo.read();
        platforms.push_back(platformInfo);

        for (auto deviceId : deviceIds) 
        {
            DeviceInfo deviceInfo{};
            deviceInfo.deviceId = deviceId;
            deviceInfo.platform = &platforms.back();
            deviceInfo.read();
            devices.push_back(deviceInfo);
        }

    }

    ui->comboBoxDevice->clear();
    for (DeviceInfo device : devices)
    {
        QString deviceName = device.name;
        ui->comboBoxDevice->addItem(deviceName);
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
    selectedDeviceIndex = index;
    DeviceInfo& device = devices[index];
    displayDeviceExtensions(device);
    displayDeviceInfo(device);
    displayPlatformExtensions(*device.platform);
    displayOperatingSystem();
}

void MainWindow::getOperatingSystem()
{
    operatingSystem.name = QSysInfo::productType();
    operatingSystem.architecture = QSysInfo::buildCpuArchitecture();
    operatingSystem.version = QSysInfo::productVersion();
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
    models.deviceExtensions.clear();
    QStandardItem* rootItem = models.deviceExtensions.invisibleRootItem();
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

void MainWindow::displayPlatformExtensions(PlatformInfo& platform)
{
    models.platformExtensions.clear();
    QStandardItem* rootItem = models.platformExtensions.invisibleRootItem();
    for (auto& extension : platform.extensions) {
        QList<QStandardItem*> extItem;
        extItem << new QStandardItem(extension.name);
        extItem << new QStandardItem(utils::clVersionString(extension.version));
        // Append extension related device info
        for (auto info : platform.platformInfo) {
            if (extension.name == info.extension) {
                QList<QStandardItem*> extInfoItem;
                extInfoItem << new QStandardItem(info.name);
                extInfoItem << new QStandardItem(info.value.toString());
                extItem.first()->appendRow(extInfoItem);
            }
        }
        rootItem->appendRow(extItem);
    }
    ui->treeViewPlatformExtensions->expandAll();
    ui->treeViewPlatformExtensions->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::displayOperatingSystem()
{
    ui->treeWidgetOS->clear();
    std::unordered_map<std::string, std::string> osInfo;
    osInfo["Name"] = operatingSystem.name.toStdString();
    osInfo["Version"] = operatingSystem.version.toStdString();
    osInfo["Architecture"] = operatingSystem.architecture.toStdString();
    for (auto& info : osInfo) {
        QTreeWidgetItem* treeItem = new QTreeWidgetItem(ui->treeWidgetOS);
        treeItem->setText(0, QString::fromStdString(info.first));
        treeItem->setText(1, QString::fromStdString(info.second));
    }
    for (int i = 0; i < ui->treeWidgetOS->columnCount(); i++) {
        ui->treeWidgetOS->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
}

void MainWindow::saveReport(QString fileName, QString submitter, QString comment)
{
    DeviceInfo device = devices[selectedDeviceIndex];
    QJsonObject jsonReport;
    
    // Environment
    QJsonObject jsonEnv;
    jsonEnv["name"] = operatingSystem.name;
    jsonEnv["version"] = operatingSystem.version;
    jsonEnv["architecture"] = operatingSystem.architecture;
    jsonEnv["submitter"] = submitter;
    jsonEnv["comment"] = comment;
    jsonEnv["reportversion"] = reportVersion;
    jsonEnv["appversion"] = version;
    jsonReport["environment"] = jsonEnv;

    // Platform
    jsonReport["platform"] = device.platform->toJson();

    // Device
    jsonReport["device"] = device.toJson();

    QJsonDocument doc(jsonReport);
    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(doc.toJson(QJsonDocument::Indented));
}

void MainWindow::slotAbout()
{
    std::stringstream aboutText;
    aboutText << "<p>OpenCL Hardware Capability Viewer " << version.toStdString() << "<br/><br/>"
        "Copyright (c) 2021 by <a href='https://www.saschawillems.de'>Sascha Willems</a><br/><br/>"
        "This tool is <b>Free Open Source Software</b><br/><br/>"
        "For usage and distribution details refer to the readme<br/><br/>"
        "<a href='https://www.gpuinfo.org'>https://www.gpuinfo.org</a></p>";
    QMessageBox::about(this, tr("About the OpenCL Hardware Capability Viewer"), QString::fromStdString(aboutText.str()));
}

void MainWindow::slotSaveReport()
{
    DeviceInfo device = devices[selectedDeviceIndex];

#ifndef VK_USE_PLATFORM_IOS_MVK
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Report to disk"), device.name + ".json", tr("json (*.json)"));

    if (!fileName.isEmpty()) {
        saveReport(fileName, "", "");
    }

#else
    // On iOS, you cannot choose where to save the file, but we need some sort of
    // acknowledgement to the end user that the save has occured. It also seems
    // rather disingenuous to say it's saved if we don't actually check for an error.
    QString fileName = getWorkingFolderForiOS();
    fileName += "/";
    fileName += device.properties["deviceName"].toString();
    fileName += ".json";

    QMessageBox msgBox;
    if (-1 != saveReport(fileName.toStdString(), "", ""))
        msgBox.setText("Report saved to the iTunes file sharing folder.");
    else
        msgBox.setText("Error writing to iTunes file sharing folder.");
    msgBox.exec();

#endif
}

void MainWindow::slotFilterDeviceInfo(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
    filterProxies.deviceinfo.setFilterRegExp(regExp);
}

void MainWindow::slotFilterDeviceExtensions(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
    filterProxies.deviceExtensions.setFilterRegExp(regExp);
}

void MainWindow::slotFilterPlatformExtensions(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
    filterProxies.platformExtensions.setFilterRegExp(regExp);
}

void MainWindow::slotComboBoxDeviceChanged(int index)
{
    if (index != selectedDeviceIndex)
    {
        displayDevice(index);
    }
}

void MainWindow::slotClose()
{
    close();
}

