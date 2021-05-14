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

#include <unordered_map>
#include "platforminfo.h"

void PlatformInfo::readPlatformInfoValue(cl_platform_info info, clValueType valueType, QString extension)
{
	switch (valueType)
	{
	case clValueType::cl_char:
	{
		size_t valueSize;
		clGetPlatformInfo(this->platformId, info, 0, nullptr, &valueSize);
		std::string value;
		value.resize(valueSize);
		clGetPlatformInfo(this->platformId, info, valueSize, &value[0], nullptr);
		platformInfo.push_back(PlatformInfoValue(info, QString::fromStdString(value), extension));
		break;
	}
	case clValueType::cl_ulong:
	{
		cl_ulong value;
		clGetPlatformInfo(this->platformId, info, sizeof(cl_ulong), &value, nullptr);
		platformInfo.push_back(PlatformInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_version:
	{
		cl_version value;
		clGetPlatformInfo(this->platformId, info, sizeof(cl_version), &value, nullptr);
		platformInfo.push_back(PlatformInfoValue(info, value, extension));
		break;
	}
	default:
		qDebug("Unknwon platform info type");
	}
}

void PlatformInfo::readPlatformExtensions()
{
	// @todo: 3.0 vs. older way of reading (no version)
	extensions.clear();
	cl_int status;
	size_t extSize;
	status = clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS_WITH_VERSION, 0, nullptr, &extSize);
	assert(status == CL_SUCCESS);
	cl_name_version* extensions = new cl_name_version[4096];
	clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS_WITH_VERSION, extSize, extensions, nullptr);
	for (size_t i = 0; i < extSize / sizeof(cl_name_version); i++) {
		PlatformExtension extension{};
		extension.name = extensions[i].name;
		extension.version = extensions[i].version;
		this->extensions.push_back(extension);
	}
	delete[] extensions;
}

void PlatformInfo::read()
{
	// @todo: move to function
	platformInfo.clear();

	std::unordered_map<cl_device_info, clValueType> infoList = {
		{ CL_PLATFORM_PROFILE, clValueType::cl_char },
		{ CL_PLATFORM_VERSION, clValueType::cl_char },
		{ CL_PLATFORM_NAME, clValueType::cl_char },
		{ CL_PLATFORM_VENDOR, clValueType::cl_char },
		//{ CL_PLATFORM_EXTENSIONS, clValueType::cl_char },
	};
	for (auto info : infoList)
	{
		readPlatformInfoValue(info.first, info.second);
	}

	// @todo: 2.1 and 2.2
	// CL_PLATFORM_HOST_TIMER_RESOLUTION

	// @todo: 3.0
	// CL_PLATFORM_EXTENSIONS_WITH_VERSION

	readPlatformExtensions();
}

QJsonObject PlatformInfo::toJson()
{
	QJsonObject jsonRoot;

	// Extensions
	QJsonArray jsonExtensions;
	for (auto& ext : extensions)
	{
		QJsonObject jsonNode;
		jsonNode["name"] = ext.name;
		jsonNode["version"] = int(ext.version);
		jsonExtensions.append(jsonNode);
	}
	jsonRoot["extensions"] = jsonExtensions;

	// Platform info
	QJsonArray jsonDeviceInfos;
	for (auto& info : platformInfo)
	{
		QJsonObject jsonNode;
		jsonNode["name"] = info.name;
		jsonNode["extension"] = info.extension;
		jsonNode["enumvalue"] = info.enumValue;
		if (info.value.canConvert(QMetaType::QVariantList)) {
			jsonNode["value"] = QJsonArray::fromVariantList(info.value.toList());
		}
		else {
			jsonNode["value"] = info.value.toString();
		}
		jsonDeviceInfos.append(jsonNode);
	}
	jsonRoot["info"] = jsonDeviceInfos;

	return jsonRoot;
}

PlatformInfoValue::PlatformInfoValue(cl_platform_info info, QVariant value, QString extension)
{
	this->name = utils::platformInfoString(info);
	this->value = value;
	this->extension = extension;
	this->enumValue = info;
}
