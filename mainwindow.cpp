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
#include "openclfunctions.h"

const QString MainWindow::version = "1.0";
const QString MainWindow::reportVersion = "1.0";

bool MainWindow::checkOpenCLAvailability(QString &error)
{
    // Check if OpenCL is supported by trying to load the OpenCL library and getting a valid function pointer
    bool openCLAvailable = false;
    error = "";

#if defined(__ANDROID__)
    // Try to find the OepenCL library on one of the following paths
    static const char *libraryPaths[] = {
        // Generic
        "/system/vendor/lib64/libOpenCL.so",
        "/system/lib64/libOpenCL.so",
        "/system/vendor/lib/libOpenCL.so",
        "/system/lib/libOpenCL.so",
        // ARM Mali
        "/system/vendor/lib64/egl/libGLES_mali.so",
        "/system/lib64/egl/libGLES_mali.so",
        "/system/vendor/lib/egl/libGLES_mali.so",
        "/system/lib/egl/libGLES_mali.so",
        // PowerVR
        "/system/vendor/lib64/libPVROCL.so",
        "/system/lib64/libPVROCL.so",
        "/system/vendor/lib/libPVROCL.so",
        "/system/lib/libPVROCL.so"
    };
    void *libOpenCL = nullptr;
    for (auto libraryPath : libraryPaths) {
        qInfo() << "Trying to load library from" << libraryPath;
        libOpenCL = dlopen(libraryPath, RTLD_LAZY);
        if (libOpenCL) {
            qInfo() << "Found library in" << libraryPath;
            break;
        }
    }
    if (libOpenCL) {
        // OpenCl library loaded, now try to get a function pointer to check if it works
        PFN_clGetPlatformIDs test_fn = reinterpret_cast<PFN_clGetPlatformIDs>(dlsym(libOpenCL, "clGetPlatformIDs"));
        if (test_fn) {
            qInfo() << "Got valid function pointer for clGetPlatformIDs";
            openCLAvailable = true;
            loadFunctionPointers(libOpenCL);
        } else {
            error = "Could not get a valid function pointer";
        }
    } else {
       error = "Could not find a OpenCL library";
    }
#elif defined(__linux__)
    // Try to find the OepenCL library on one of the following paths
    static const char *libraryPaths[] = {
        "libOpenCL.so",
        "/usr/lib/libOpenCL.so",
        "/usr/local/lib/libOpenCL.so",
        "/usr/local/lib/libpocl.so",
        "/usr/lib64/libOpenCL.so",
        "/usr/lib32/libOpenCL.so"
    };
    void *libOpenCL = nullptr;
    for (auto libraryPath : libraryPaths) {
        qInfo() << "Trying to load library from" << libraryPath;
        libOpenCL = dlopen(libraryPath, RTLD_LAZY);
        if (libOpenCL) {
            qInfo() << "Found library in" << libraryPath;
            break;
        }
    }
    if (libOpenCL) {
        // OpenCl library loaded, now try to get a function pointer to check if it works
        PFN_clGetPlatformIDs test_fn = reinterpret_cast<PFN_clGetPlatformIDs>(dlsym(libOpenCL, "clGetPlatformIDs"));
        if (test_fn) {
            qInfo() << "Got valid function pointer for clGetPlatformIDs";
            openCLAvailable = true;
            loadFunctionPointers(libOpenCL);
        } else {
            error = "Could not get a valid function pointer";
        }
    } else {
       error = "Could not find a OpenCL library";
    }
#elif defined(_WIN32)
    HMODULE libOpenCL = LoadLibraryA("OpenCL.dll");
    if (libOpenCL) {
        char libPath[MAX_PATH] = { 0 };
        GetModuleFileNameA(libOpenCL, libPath, sizeof(libPath));
        qInfo() << "Found library in" << libPath;
        PFN_clGetPlatformIDs test_fn = reinterpret_cast<PFN_clGetPlatformIDs>(GetProcAddress((HMODULE)libOpenCL, "clGetPlatformIDs"));
        if (test_fn) {
            qInfo() << "Got valid function pointer for clGetPlatformIDs";
            openCLAvailable = true;
            loadFunctionPointers(libOpenCL);
        } else {
            error = "Could not get a valid function pointer";
        }
    } else {
        error = "Could not find a OpenCL library";
    }
#endif
    if (error != "") {
        qCritical() << error;
    }
    return openCLAvailable;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QString title = "OpenCL Hardware Capability Viewer " + version;
    setWindowTitle(title);
    ui->labelTitle->setText(title);
    qApp->setStyle(QStyleFactory::create("Fusion"));

    appSettings.restore();

    QString error;
    if (!checkOpenCLAvailability(error))
    {
        QMessageBox::warning(this, "Error", "OpenCL does not seem to be supported on this platform:\n" + error);
        exit(EXIT_FAILURE);
    }

    // Models
    ui->treeViewDeviceInfo->setModel(&filterProxies.deviceinfo);
    connectFilterAndModel(models.deviceinfo, filterProxies.deviceinfo);
    connect(ui->filterLineEditDeviceInfo, SIGNAL(textChanged(QString)), this, SLOT(slotFilterDeviceInfo(QString)));
    
    ui->treeViewDeviceExtensions->setModel(&filterProxies.deviceExtensions);
    connectFilterAndModel(models.deviceExtensions, filterProxies.deviceExtensions);
    connect(ui->filterLineEditExtensions, SIGNAL(textChanged(QString)), this, SLOT(slotFilterDeviceExtensions(QString)));

    ui->treeViewDeviceImageFormats->setModel(&filterProxies.deviceImageFormats);
    connectFilterAndModel(models.deviceImageFormats, filterProxies.deviceImageFormats);
    connect(ui->filterLineEditDeviceImageFormats, SIGNAL(textChanged(QString)), this, SLOT(slotFilterDeviceImageFormats(QString)));

    ui->treeViewPlatformExtensions->setModel(&filterProxies.platformExtensions);
    connectFilterAndModel(models.platformExtensions, filterProxies.platformExtensions);
    connect(ui->filterLineEditPlatformExtensions, SIGNAL(textChanged(QString)), this, SLOT(slotFilterPlatformExtensions(QString)));

    ui->treeViewPlatformInfo->setModel(&filterProxies.platformInfo);
    connectFilterAndModel(models.platformInfo, filterProxies.platformInfo);
    connect(ui->filterLineEditPlatformInfo, SIGNAL(textChanged(QString)), this, SLOT(slotFilterPlatformInfo(QString)));

    // Slots
    connect(ui->comboBoxDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(slotComboBoxDeviceChanged(int)));
    connect(ui->toolButtonSave, SIGNAL(pressed()), this, SLOT(slotSaveReport()));
    connect(ui->toolButtonOnlineDevice, SIGNAL(pressed()), this, SLOT(slotDisplayOnlineReport()));
    connect(ui->toolButtonOnlineDataBase, SIGNAL(pressed()), this, SLOT(slotBrowseDatabase()));
    connect(ui->toolButtonUpload, SIGNAL(pressed()), this, SLOT(slotUploadReport()));
    connect(ui->toolButtonAbout, SIGNAL(pressed()), this, SLOT(slotAbout()));
    connect(ui->toolButtonSettings, SIGNAL(pressed()), this, SLOT(slotSettings()));
    connect(ui->toolButtonExit, SIGNAL(pressed()), this, SLOT(slotClose()));

    // Optimize the UI for mobile platforms
#if defined(ANDROID)
    // Adjust toolbar to better fit mobile devices
    foreach (QToolButton *toolButton, findChildren<QToolButton *>()) {
        toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    // Touch scrolling
    foreach (QTreeView *widget, findChildren<QTreeWidget  *>()) {
        setTouchProps(widget);
        widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        widget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        widget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        widget->setSelectionMode(QAbstractItemView::NoSelection);
        widget->setFrameStyle(QFrame::NoFrame);
    }

    foreach (QTreeView *widget, findChildren<QTreeView *>()) {
        setTouchProps(widget);
        widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        widget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        widget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        widget->setSelectionMode(QAbstractItemView::NoSelection);
        widget->setFrameStyle(QFrame::NoFrame);
    }
    // No identation
    for (int i = 0; i < ui->tabWidgetDevice->count(); i++) {
        ui->tabWidgetDevice->widget(i)->layout()->setMargin(0);
    }
#endif

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
    cl_int status = _clGetPlatformIDs(0, nullptr, &numPlatforms);
	if (status != CL_SUCCESS)
	{
        qCritical() << "Could not get platform count!";
        QMessageBox::critical(this, tr("Error"), "Could not get platform count!");
        exit(EXIT_FAILURE);
    }

    // Read platforms
    std::vector<cl_platform_id> platformIds(numPlatforms);
    status = _clGetPlatformIDs(numPlatforms, platformIds.data(), nullptr);
    if (status != CL_SUCCESS)
    {
        qCritical() << "Could not read platforms!";
        QMessageBox::critical(this, tr("Error"), "Could not read platforms!");
        exit(EXIT_FAILURE);
    }
    qInfo() << "Found" << numPlatforms << "OpenCL platforms";
    for (cl_platform_id platformId : platformIds)
    {
        PlatformInfo platformInfo{};
        platformInfo.platformId = platformId;
        platformInfo.read();
        platforms.push_back(platformInfo);
    }

    // Read devices for platforms
    for (auto& platform : platforms)
    {
        qInfo() << "Reading devices for platform id" << platform.platformId;
        cl_uint numDevices;
        status = _clGetDeviceIDs(platform.platformId, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
        if (status != CL_SUCCESS) {
            qCritical() << "Could not read devices for the platform";
            QMessageBox::critical(this, tr("Error"), "Could not read devices for the current platform!");
            exit(EXIT_FAILURE);
        }
        std::vector<cl_device_id> deviceIds(numDevices);
        status = _clGetDeviceIDs(platform.platformId, CL_DEVICE_TYPE_ALL, numDevices, deviceIds.data(), nullptr);
        if (status != CL_SUCCESS) {
            qCritical() << "Could not read devices for the platform";
            QMessageBox::critical(this, tr("Error"), "Could not read devices for the current platform!");
            exit(EXIT_FAILURE);
        }
        qInfo() << "Found" << numDevices << "OpenCL device(s) for the current OpenCL platform";
        for (auto deviceId : deviceIds)
        {
            qInfo() << "Reading properties for device" << deviceId;
            DeviceInfo deviceInfo{};
            deviceInfo.deviceId = deviceId;
            deviceInfo.platform = &platform;
            deviceInfo.read();
            devices.push_back(deviceInfo);
        }
    }

    ui->comboBoxDevice->clear();
    for (DeviceInfo device : devices)
    {
        QString deviceName = device.identifier.name;
        ui->comboBoxDevice->addItem(deviceName);
    }

    if (devices.size() > 0)
    {
        displayDevice(0);
    }
    else
    {
        qCritical() << "Could not find a device with OpenCL support!";
        QMessageBox::critical(this, tr("Error"), "Could not find a device with OpenCL support!");
        exit(EXIT_FAILURE);
    }

}

void MainWindow::displayDevice(uint32_t index)
{
    selectedDeviceIndex = index;
    DeviceInfo& device = devices[index];
    qDebug() << "Displaying device" << device.identifier.name;
    displayDeviceExtensions(device);
    displayDeviceInfo(device);
    displayDeviceImageFormats(device);
    displayPlatformExtensions(*device.platform);
    displayPlatformInfo(*device.platform);
    displayOperatingSystem();
    checkReportDatabaseState();
}

void MainWindow::getOperatingSystem()
{
    operatingSystem.name = QSysInfo::productType();
    operatingSystem.architecture = QSysInfo::buildCpuArchitecture();
    operatingSystem.version = QSysInfo::productVersion();
    operatingSystem.type = -1;
#if defined(_WIN32)
    operatingSystem.type = 0;
#elif defined(__ANDROID__)
    operatingSystem.type = 2;
#elif defined(__linux__)
    operatingSystem.type = 1;
#elif __APPLE__
    // @todo: dinstinguish between macos and ios
#endif
}

void MainWindow::connectFilterAndModel(QStandardItemModel& model, TreeProxyFilter& filter)
{
    filter.setSourceModel(&model);
    filter.setFilterKeyColumn(-1);
    filter.setRecursiveFilteringEnabled(true);
}

void MainWindow::displayDeviceInfo(DeviceInfo& device)
{
    models.deviceinfo.clear();
    QStandardItem* rootItem = models.deviceinfo.invisibleRootItem();
    for (auto info : device.deviceInfo) {
        if (info.extension.isEmpty()) {
            QList<QStandardItem*> extItem;
            QString displayValue = info.getDisplayValue();
            extItem << new QStandardItem(info.name);
            extItem << new QStandardItem(displayValue);
            if (displayValue == "true") {
                extItem[1]->setForeground(QColor::fromRgb(0, 128, 0));
            }
            if (displayValue == "false") {
                extItem[1]->setForeground(QColor::fromRgb(255, 0, 0));
            }
            if (displayValue == "none") {
                extItem[1]->setForeground(QColor::fromRgb(128, 128, 128));
            }
            // Append additional device info detail values
            if (info.detailValues.size() > 0) {
                for (auto& detailItem : info.detailValues) {
                    QList<QStandardItem*> additionalInfoItem;
                    QString caption = detailItem.name;
                    if (!detailItem.detail.isEmpty()) {
                        caption += " - " + detailItem.detail;
                    }
                    additionalInfoItem << new QStandardItem(caption);
                    additionalInfoItem << new QStandardItem(detailItem.getDisplayValue());
                    extItem.first()->appendRow(additionalInfoItem);
                }
            }
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
        extItem << new QStandardItem(extension.version > 0 ? utils::clVersionString(extension.version) : "");
        // Append extension related device info
        for (auto info : device.deviceInfo) {
            if (extension.name == info.extension) {
                QList<QStandardItem*> extInfoItem;
                QString displayValue = info.getDisplayValue();
                extInfoItem << new QStandardItem(info.name);
                extInfoItem << new QStandardItem(displayValue);
                if (displayValue == "true") {
                    extInfoItem[1]->setForeground(QColor::fromRgb(0, 128, 0));
                }
                if (displayValue == "false") {
                    extInfoItem[1]->setForeground(QColor::fromRgb(255, 0, 0));
                }
                // Append additional device info detail values
                if (info.detailValues.size() > 0) {
                    for (auto& detailItem : info.detailValues) {
                        QList<QStandardItem*> additionalInfoItem;
                        QString caption = detailItem.name;
                        if (!detailItem.detail.isEmpty()) {
                            caption += " - " + detailItem.detail;
                        }
                        additionalInfoItem << new QStandardItem(caption);
                        additionalInfoItem << new QStandardItem(detailItem.getDisplayValue());
                        extInfoItem.first()->appendRow(additionalInfoItem);
                    }
                }
                extItem.first()->appendRow(extInfoItem);
            }
        }
        rootItem->appendRow(extItem);
    }
    ui->treeViewDeviceExtensions->expandAll();
    ui->treeViewDeviceExtensions->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::displayDeviceImageFormats(DeviceInfo& device)
{
    models.deviceImageFormats.clear();

    std::vector<cl_mem_flags> memFlagList = {
        CL_MEM_READ_WRITE,
        CL_MEM_READ_ONLY,
        CL_MEM_WRITE_ONLY,
        CL_MEM_KERNEL_READ_AND_WRITE
    };

    models.deviceImageFormats.setHorizontalHeaderLabels({"Format", "Order", "Type", "Read/Write", "Read only", "Write only", "Kernel Read/Write"});
    ui->treeViewDeviceImageFormats->setHeaderHidden(false);

    QStandardItem* rootItem = models.deviceImageFormats.invisibleRootItem();
    for (auto& imageType : device.imageTypes)
    {
        for (auto& channelOrder : imageType.second.channelOrders)
        {
            for (auto& channelType : channelOrder.second.channelTypes)
            {
                QList<QStandardItem*> imageTypeItem;
                imageTypeItem << new QStandardItem(utils::imageTypeString(imageType.first));
                imageTypeItem << new QStandardItem(utils::channelOrderString(channelOrder.first));
                imageTypeItem << new QStandardItem(utils::channelTypeString(channelType.first));
                // Display mem flags as columns
                int colIndex = 3;
                for (auto& memFlag : memFlagList)
                {
                    bool flagSupported = channelType.second.memFlags & memFlag;
                    if (flagSupported)
                    {
                        imageTypeItem << new QStandardItem("true");
                        imageTypeItem[colIndex]->setForeground(QColor::fromRgb(0, 128, 0));
                    }
                    else {
                        imageTypeItem << new QStandardItem("false");
                        imageTypeItem[colIndex]->setForeground(QColor::fromRgb(255, 0, 0));
                    }
                    colIndex++;
                }
                rootItem->appendRow(imageTypeItem);
            }
        }
    }

    ui->treeViewDeviceImageFormats->expandAll();
    ui->treeViewDeviceImageFormats->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeViewDeviceImageFormats->header()->setStretchLastSection(true);
}

void MainWindow::displayPlatformInfo(PlatformInfo& platform)
{
    models.platformInfo.clear();
    QStandardItem* rootItem = models.platformInfo.invisibleRootItem();
    for (auto info : platform.platformInfo) {
        if (info.extension.isEmpty()) {
            QList<QStandardItem*> extItem;
            extItem << new QStandardItem(info.name);
            extItem << new QStandardItem(info.value.toString());
            rootItem->appendRow(extItem);
        }
    }
    ui->treeViewPlatformInfo->expandAll();
    ui->treeViewPlatformInfo->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::displayPlatformExtensions(PlatformInfo& platform)
{
    models.platformExtensions.clear();
    QStandardItem* rootItem = models.platformExtensions.invisibleRootItem();
    for (auto& extension : platform.extensions) {
        QList<QStandardItem*> extItem;
        extItem << new QStandardItem(extension.name);
        extItem << new QStandardItem(extension.version > 0 ? utils::clVersionString(extension.version) : "");
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

void MainWindow::setReportState(ReportState state)
{
    reportState = state;
    switch (reportState) 
    {
    case ReportState::is_present:
        ui->toolButtonUpload->setEnabled(false);
        ui->toolButtonOnlineDevice->setEnabled(true);
        ui->toolButtonUpload->setText("Upload");
        ui->labelReportDatabaseState->setText("<font color='#00cc63'>Device is already present in the database</font>");
        break;
    case ReportState::not_present:
        ui->toolButtonUpload->setEnabled(true);
        ui->toolButtonOnlineDevice->setEnabled(false);
        ui->toolButtonUpload->setText("Upload");
        ui->labelReportDatabaseState->setText("<font color='#80b3ff'>Device can be uploaded to the database</font>");
        break;
    case ReportState::is_updatable:
        ui->toolButtonUpload->setEnabled(true);
        ui->toolButtonOnlineDevice->setEnabled(true);
        ui->toolButtonUpload->setText("Update");
        ui->labelReportDatabaseState->setText("<font color='#ffcc00'>Device is already present in the database, but can be updated</font>");
        break;
    default:
        ui->toolButtonUpload->setEnabled(false);
        ui->toolButtonOnlineDevice->setEnabled(false);
        ui->toolButtonUpload->setText("n.a.");
        ui->labelReportDatabaseState->setText("<font color='#ff0000'>Could not get report state from database</font>");
    }
}

void MainWindow::checkReportDatabaseState()
{
    qInfo() << "Checking report state against database for device" << devices[selectedDeviceIndex].identifier.name;
    ui->labelReportDatabaseState->setText("<font color='#000000'>Connecting to database...</font>");
    ui->toolButtonOnlineDevice->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString message;
    if (!database.checkServerConnection(message))
    {
        qInfo() << "Unable to reach server";
        ui->labelReportDatabaseState->setText("<font color='#FF0000'>Could not connect to the database!\n\nPlease check your internet connection and proxy settings!</font>");
        QApplication::restoreOverrideCursor();
        return;
    }
    QJsonObject jsonReport;
    reportToJson(devices[selectedDeviceIndex], "", "", jsonReport);
    ReportState state;
    if (database.getReportState(jsonReport, state))
    {
        qInfo() << "Got valid state from database";
        setReportState(state);
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::reportToJson(DeviceInfo& device, QString submitter, QString comment, QJsonObject& jsonObject)
{
    // Environment
    QJsonObject jsonEnv;
    jsonEnv["name"] = operatingSystem.name;
    jsonEnv["version"] = operatingSystem.version;
    jsonEnv["architecture"] = operatingSystem.architecture;
    jsonEnv["type"] = operatingSystem.type;
    jsonEnv["submitter"] = submitter;
    jsonEnv["comment"] = comment;
    jsonEnv["reportversion"] = reportVersion;
    jsonEnv["appversion"] = version;
    jsonObject["environment"] = jsonEnv;
    // Platform
    jsonObject["platform"] = device.platform->toJson();
    // Device
    jsonObject["device"] = device.toJson();
}

void MainWindow::saveReport(QString fileName, QString submitter, QString comment)
{
    QJsonObject jsonReport;
    reportToJson(devices[selectedDeviceIndex], submitter, comment, jsonReport);
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

void MainWindow::slotSettings()
{
    SettingsDialog dialog(appSettings);
    dialog.setModal(true);
    dialog.exec();
    appSettings.restore();
}

void MainWindow::slotDisplayOnlineReport()
{
    QJsonObject jsonReport;
    reportToJson(devices[selectedDeviceIndex], "", "", jsonReport);
    int reportId;
    if (database.getReportId(jsonReport, reportId)) {
        QUrl url(database.databaseUrl + "displayreport.php?id=" + QString::number(reportId));
        QDesktopServices::openUrl(url);
    } else {
        QMessageBox::warning(this, "Error", "Could not get the report id from the database");
    }
}

void MainWindow::slotBrowseDatabase()
{
    QDesktopServices::openUrl(QUrl("https://opencl.gpuinfo.org"));
}

void MainWindow::slotSaveReport()
{
    DeviceInfo& device = devices[selectedDeviceIndex];
#ifndef VK_USE_PLATFORM_IOS_MVK
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Report to disk"), device.identifier.name + ".json", tr("json (*.json)"));

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

void MainWindow::slotUploadReport()
{
    QString error;
    if (!database.checkServerConnection(error))
    {
        QMessageBox::warning(this, "Error", "Could not connect to database:<br>" + error);
        return;
    }
    // Upload new report
    if (reportState == ReportState::not_present) {
        SubmitDialog dialog(appSettings.submitterName,  "Submit new report");

        if (dialog.exec() == QDialog::Accepted) {
            QString message;
            QJsonObject reportJson;
            reportToJson(devices[selectedDeviceIndex], dialog.getSubmitter(), dialog.getComment(), reportJson);
            if (database.uploadReport(reportJson, message))
            {
                QMessageBox::information(this, "Report submitted", "Your report has been uploaded to the database!\n\nThank you for your contribution!");
                checkReportDatabaseState();
            }
            else
            {
                QMessageBox::warning(this, "Error", "The report could not be uploaded : \n" + message);
            }
        }
        return;
    }
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

void MainWindow::slotFilterDeviceImageFormats(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
    filterProxies.deviceImageFormats.setRecursiveFilteringEnabled(true);
    filterProxies.deviceImageFormats.setFilterRegExp(regExp);
}

void MainWindow::slotFilterPlatformInfo(QString text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
    filterProxies.platformInfo.setFilterRegExp(regExp);
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

#if defined(ANDROID)
void MainWindow::setTouchProps(QWidget *widget) {
    QScroller *scroller = QScroller::scroller(widget);
    QScrollerProperties properties = scroller->scrollerProperties();
    QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
    properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
    properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
    properties.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.6);
    properties.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
    properties.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.5);
    properties.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime, 0.4);
    properties.setScrollMetric(QScrollerProperties::AcceleratingFlickSpeedupFactor, 1.2);
    properties.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.2);
    properties.setScrollMetric(QScrollerProperties::MaximumClickThroughVelocity, 0);
    properties.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
    properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0.5);
    scroller->grabGesture(widget, QScroller::LeftMouseButtonGesture);
    scroller->setScrollerProperties(properties);
}
#endif

