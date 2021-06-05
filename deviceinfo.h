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

#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include "CL/cl.h"
#include "CL/cl_ext.h"
#include "openclutils.h"
#include "displayutils.h"
#include "platforminfo.h"
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <QVariantMap>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct DeviceExtension
{
    QString name;
    cl_version version;
};

typedef std::function<QString(QVariant)> DisplayFn;

struct DeviceInfoValueDescriptor
{
    cl_device_info name;
    clValueType valueType;
    DisplayFn displayFunction = nullptr;
    DeviceInfoValueDescriptor();
    DeviceInfoValueDescriptor(cl_device_info name, clValueType valueType, DisplayFn displayFunction = nullptr);
};

struct DeviceInfoValueDetailValue
{
    QString name;
    QVariant value;
    DisplayFn displayFunction = nullptr;
    DeviceInfoValueDetailValue(QString name, QVariant value, DisplayFn displayFunction = nullptr);
    QString getDisplayValue();
};

struct DeviceInfoValue
{
    QString name;
    QVariant value;
    QString extension;
    qint32 enumValue;
    DisplayFn displayFunction = nullptr;
    std::vector<DeviceInfoValueDetailValue> detailValues;
    // @todo: add display "translation" rule?
    DeviceInfoValue(cl_device_info info, QVariant value, QString extension, DisplayFn displayFunction = nullptr);
    void addDetailValue(QString name, QVariant value, DisplayFn displayFunction = nullptr);
    QString getDisplayValue();
};

struct DeviceImageChannelTypeInfo 
{
    cl_mem_flags memFlags = 0;
    void addFlag(cl_mem_flags flag) {
        memFlags |= flag;
    }
};

struct DeviceImageChannelOrderInfo
{
    std::unordered_map<cl_channel_type, DeviceImageChannelTypeInfo> channelTypes;
};

struct DeviceImageTypeInfo 
{
    std::unordered_map<cl_channel_order, DeviceImageChannelOrderInfo> channelOrders;
};

class DeviceInfo
{
private:
    QString getDeviceInfoString(cl_device_info info);
    bool extensionSupported(const char* name);
    void readDeviceInfoValue(DeviceInfoValueDescriptor descriptor, QString extension = "");
    void readDeviceInfo();
    void readOpenCLVersion();
    void readExtensions();
    void readExtensionInfo();
    void readSupportedImageFormats();
public:
    DeviceInfo();
    cl_device_id deviceId;
    PlatformInfo* platform;
    QString name;
    qint32 clVersionMajor;
    qint32 clVersionMinor;
    std::vector<DeviceInfoValue> deviceInfo;
    std::vector<DeviceExtension> extensions;
    std::unordered_map<cl_mem_object_type, DeviceImageTypeInfo> imageTypes;
    void read();
    QJsonObject toJson();
};

#endif // DEVICEINFO_H
