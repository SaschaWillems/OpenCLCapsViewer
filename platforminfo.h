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

#include "CL/cl.h"
#include <QVariant>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "openclutils.h"
#include "openclfunctions.h"
#include "displayutils.h"

#pragma once

struct PlatformExtension
{
    QString name;
    cl_version version;
};

typedef std::function<QString(QVariant)> PlatformInfoDisplayFn;

struct PlatformInfoValueDescriptor
{
    cl_device_info name;
    clValueType valueType;
    PlatformInfoDisplayFn displayFunction = nullptr;
    PlatformInfoValueDescriptor();
    PlatformInfoValueDescriptor(cl_platform_info name, clValueType valueType, PlatformInfoDisplayFn displayFunction = nullptr);
};

struct PlatformInfoValueDetailValue
{
    QString name;
    QString detail;
    QVariant value;
    PlatformInfoDisplayFn displayFunction = nullptr;
    PlatformInfoValueDetailValue(QString name, QVariant value, PlatformInfoDisplayFn displayFunction = nullptr);
    PlatformInfoValueDetailValue(QString name, QString detail, QVariant value, PlatformInfoDisplayFn displayFunction = nullptr);
    QString getDisplayValue();
};

struct PlatformInfoValue
{
    QString name;
    QVariant value;
    QString extension;
    qint32 enumValue;
    PlatformInfoDisplayFn displayFunction = nullptr;
    std::vector<PlatformInfoValueDetailValue> detailValues;
    PlatformInfoValue(cl_platform_info info, QVariant value, QString extension = "", PlatformInfoDisplayFn displayFunction = nullptr);
    void addDetailValue(QString name, QVariant value, PlatformInfoDisplayFn displayFunction = nullptr);
    void addDetailValue(QString name, QString detail, QVariant value, PlatformInfoDisplayFn displayFunction = nullptr);
    QString getDisplayValue();
};

class PlatformInfo
{
private:
    void readOpenCLVersion();
	void readPlatformInfoValue(PlatformInfoValueDescriptor descriptor, QString extension = "");
    void readExtensions();
    bool extensionSupported(const char* name);
public:
	cl_platform_id platformId;
    std::vector<PlatformInfoValue> platformInfo;
    std::vector<PlatformExtension> extensions;
    qint32 clVersionMajor;
    qint32 clVersionMinor;
	void read();
    void readExtensionInfo();
    QJsonObject toJson();
};

