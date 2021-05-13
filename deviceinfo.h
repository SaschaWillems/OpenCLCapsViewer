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
#include "platforminfo.h"
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
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

struct DeviceInfoValue
{
    QString name;
    QVariant value;
    QString extension;
    // @todo: add display "translation" rule?
    DeviceInfoValue(cl_device_info info, QVariant value, QString extension = "");
};

class DeviceInfo
{
private:
    QString getDeviceInfoString(cl_device_info info);
    bool extensionSupported(const char* name);
    void readDeviceInfoValue(cl_device_info info, clValueType valueType, QString extension = "");
    void readDeviceInfo();
    void readOpenCLVersion();
    void readDeviceExtensions();
    void readDeviceInfoExtensions();
public:
    DeviceInfo();
    cl_device_id deviceId;
    PlatformInfo* platform;
    QString name;
    uint32_t clVersionMajor;
    uint32_t clVersionMinor;
    std::vector<DeviceInfoValue> deviceInfo;
    std::vector<DeviceExtension> extensions;
    void read();
    QJsonObject toJson(QString submitter, QString comment);
};

#endif // DEVICEINFO_H
