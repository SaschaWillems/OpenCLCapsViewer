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

#pragma once

#include "CL/cl.h"
#include "CL/cl_ext.h"

#include <QString>
#include <QVariant>
#include <QList>
#include <unordered_map>

namespace utils
{
	QString implode(QList<QString>& items, char separator = ',');
	QString displayDefault(QVariant value);
	QString displayBool(QVariant value);
	QString displayNumberArray(QVariant value);
	QString displayHex(QVariant value);
	QString displayByteSize(QVariant value);
	QString displayText(QVariant value);
	QString displayDeviceType(QVariant value);
	QString displayVersion(QVariant value);
	QString displayNameVersionArray(QVariant value);
	QString displayAtomicCapabilities(QVariant value);
	QString displayMemCacheType(QVariant value);
	QString displayLocalMemType(QVariant value);
	QString displayFloatingPointConfig(QVariant value);
	QString displayExecCapabilities(QVariant value);
	QString displayEnqueueCapabilities(QVariant value);
	QString displayCommandQueueProperties(QVariant value);
	QString displayDeviceSvmCapabilities(QVariant value);
	QString displayDevicePartitionProperties(QVariant value);
	QString displayDeviceAffinityDomains(QVariant value);
	QString displayControlledTerminationCapabilitiesARM(QVariant value);
	QString displaySchedulingControlsCapabilitiesARM(QVariant value);
	QString displayQueueFamilyPropertiesIntel(QVariant value);
	QString displayCommandQueueCapabilitiesIntel(QVariant value);
	QString displayDeviceUsmCapabilitiesIntel(QVariant value);
	QString displayDetailValueArraySize(QVariant value);
	QString displayItegerDotProductCapabilities(QVariant value);
	QString displayExternalMemoryHandleTypes(QVariant value);
	QString displayExternalSemaphoreHandleTypes(QVariant value);
	QString displayCommandBufferCapabilities(QVariant value);
}

