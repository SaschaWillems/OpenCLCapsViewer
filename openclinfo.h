/*
*
* OpenCL hardware capability viewer
*
* Copyright (C) 2021-2022 by Sascha Willems (www.saschawillems.de)
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

#ifndef OPENCLINFO_H
#define OPENCLINFO_H

#include <QString>
#include <QDebug>
#include <vector>

#include <CL/cl.h>
#if defined(__linux__)
#include <dlfcn.h>
#elif defined(_WIN32)
#include "windows.h"
#endif

#include "deviceinfo.h"
#include "platforminfo.h"

extern std::vector<PlatformInfo> platforms;
extern std::vector<DeviceInfo> devices;

bool getOpenCLDevices(QString& error);

#endif