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
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <QVariantMap>
#include <QDebug>

struct DeviceExtension
{
    std::string name;
    cl_version version;
};

enum class clValueType {
    cl_bool,
    cl_char,
    cl_command_queue_properties,
    cl_device_atomic_capabilities,
    cl_device_device_enqueue_capabilities,
    cl_device_exec_capabilities,
    cl_device_fp_config,
    cl_device_local_mem_type,
    cl_device_mem_cache_type,
    cl_device_type,
    cl_size_t,
    cl_uint,
    cl_ulong,
    cl_version,
    special
};

class DeviceInfo
{
private:
    std::string getDeviceInfoString(cl_device_info info);
    bool extensionSupported(const char* name);
    void readDeviceInfoValue(cl_device_info info);
    void readDeviceInfoValue(cl_device_info info, clValueType valueType);
    void readDeviceInfo();
    void readDeviceExtensions();
    void readDeviceInfoExtensions();
public:
    DeviceInfo();
    cl_device_id deviceId;
    std::string name;
    QVariantMap deviceInfo;
    std::vector<DeviceExtension> extensions;
    void read();
};

#endif // DEVICEINFO_H
