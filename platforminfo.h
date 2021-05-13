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
#include "openclutils.h"

#pragma once

struct PlatformExtension
{
    QString name;
    cl_version version;
};

struct PlatformInfoValue
{
    QString name;
    QVariant value;
    QString extension;
    // @todo: add display "translation" rule?
    PlatformInfoValue(cl_platform_info info, QVariant value, QString extension = "");
};

class PlatformInfo
{
private:
	void readPlatformInfoValue(cl_platform_info info, clValueType valueType, QString extension = "");
    void readPlatformExtensions();
public:
	cl_platform_id platformId;
    std::vector<PlatformInfoValue> platformInfo;
    std::vector<PlatformExtension> extensions;
	void read();
};

