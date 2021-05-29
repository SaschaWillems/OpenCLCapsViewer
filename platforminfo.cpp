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

void PlatformInfo::readOpenCLVersion()
{
	size_t valueSize;
	clGetPlatformInfo(this->platformId, CL_PLATFORM_VERSION, 0, nullptr, &valueSize);
	std::string value;
	value.resize(valueSize);
	clGetPlatformInfo(this->platformId, CL_PLATFORM_VERSION, valueSize, &value[0], nullptr);
	// OpenCL<space><major_version.minor_version><space>
	size_t versStart = value.find(' ', 0);
	size_t versSplit = value.find('.', versStart + 1);
	size_t versEnd = value.find(' ', versStart + 1);
	std::string major, minor;
	major = value.substr(versStart, versSplit - versStart);
	minor = value.substr(versSplit + 1, versEnd - versSplit - 1);
	clVersionMajor = std::stoi(major);
	clVersionMinor = std::stoi(minor);
}

void PlatformInfo::readPlatformInfoValue(cl_platform_info info, clValueType valueType, QString extension)
{
	switch (valueType)
	{
	case clValueType::cl_char:
	{
		size_t valueSize;
		clGetPlatformInfo(this->platformId, info, 0, nullptr, &valueSize);
		char* value = new char[valueSize];
		clGetPlatformInfo(this->platformId, info, valueSize, &value[0], nullptr);
		platformInfo.push_back(PlatformInfoValue(info, QString::fromUtf8(value), extension));
		delete[] value;
		break;
	}
	case clValueType::cl_ulong:
	{
		cl_ulong value;
		clGetPlatformInfo(this->platformId, info, sizeof(cl_ulong), &value, nullptr);
        platformInfo.push_back(PlatformInfoValue(info, QVariant::fromValue(value), extension));
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

void PlatformInfo::readExtensions()
{
	extensions.clear();
	if (clVersionMajor >= 3) {
		size_t extSize;
		clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS_WITH_VERSION, 0, nullptr, &extSize);
		cl_name_version* extensions = new cl_name_version[4096];
		clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS_WITH_VERSION, extSize, extensions, nullptr);
		for (size_t i = 0; i < extSize / sizeof(cl_name_version); i++) {
			PlatformExtension extension{};
			extension.name = extensions[i].name;
			extension.version = extensions[i].version;
			this->extensions.push_back(extension);
		}
		delete[] extensions;
	} else {
		size_t extStrSize;
		clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS, 0, nullptr, &extStrSize);
		std::string extensionString;
		extensionString.resize(extStrSize);
		clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS, extStrSize, &extensionString[0], nullptr);
		std::vector<std::string> extensions;
		extensions = utils::explode(extensionString, ' ');
		for (size_t i = 0; i < extensions.size(); i++) {
			PlatformExtension extension{};
			extension.name = QString::fromStdString(extensions[i]);
			// @todo
			extension.version = 0;
			this->extensions.push_back(extension);
		}
	}
}

bool PlatformInfo::extensionSupported(const char* name)
{
	for (auto& ext : extensions) {
		if (ext.name == QLatin1String(name)) {
			return true;
		}
	}
	return false;
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
	};
	for (auto info : infoList)
	{
		readPlatformInfoValue(info.first, info.second);
	}

	// @todo: 2.1 and 2.2
	// CL_PLATFORM_HOST_TIMER_RESOLUTION

	// @todo: 3.0
	// CL_PLATFORM_NUMERIC_VERSION
	// CL_PLATFORM_EXTENSIONS_WITH_VERSION

	readOpenCLVersion();
	readExtensions();
	readExtensionInfo();
}

void PlatformInfo::readExtensionInfo()
{
	// KHR
	if (extensionSupported("cl_khr_icd")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_PLATFORM_ICD_SUFFIX_KHR, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readPlatformInfoValue(info.first, info.second, "cl_khr_icd");
		}
	}
	// @todo
	// cl_khr_extended_versioning
	// CL_PLATFORM_NUMERIC_VERSION_KHR
	// CL_PLATFORM_EXTENSIONS_WITH_VERSION_KHR

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
		jsonNode["value"] = info.value.toJsonValue();
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
