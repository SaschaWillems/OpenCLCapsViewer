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

#include "openclinfo.h"

std::vector<PlatformInfo> platforms;
std::vector<DeviceInfo> devices;

bool getOpenCLDevices(QString &error)
{
    cl_uint numPlatforms;
    cl_int status = _clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (status != CL_SUCCESS)
    {
        error = "Could not get platform count!";
        qCritical() << error;
        return false;
    }

    // Read platforms
    std::vector<cl_platform_id> platformIds(numPlatforms);
    status = _clGetPlatformIDs(numPlatforms, platformIds.data(), nullptr);
    if (status != CL_SUCCESS)
    {
        error = "Could not read platforms!";
        qCritical() << error;
        return false;
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
            error = "Could not read devices for the platform";
            qCritical() << error;
            return false;
        }
        std::vector<cl_device_id> deviceIds(numDevices);
        status = _clGetDeviceIDs(platform.platformId, CL_DEVICE_TYPE_ALL, numDevices, deviceIds.data(), nullptr);
        if (status != CL_SUCCESS) {
            error = "Could not read devices for the platform";
            qCritical() << error;
            return false;
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

    return true;
}