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

PlatformInfoValueDescriptor::PlatformInfoValueDescriptor()
{
	this->name = 0;
	this->valueType = clValueType::cl_char;
	this->displayFunction = nullptr;
}

PlatformInfoValueDescriptor::PlatformInfoValueDescriptor(cl_platform_info name, clValueType valueType, PlatformInfoDisplayFn displayFunction)
{
	this->name = name;
	this->valueType = valueType;
	this->displayFunction = displayFunction;
}

PlatformInfoValueDetailValue::PlatformInfoValueDetailValue(QString name, QVariant value, PlatformInfoDisplayFn displayFunction)
{
	this->name = name;
	this->detail = "";
	this->value = value;
	this->displayFunction = displayFunction;
}

PlatformInfoValueDetailValue::PlatformInfoValueDetailValue(QString name, QString detail, QVariant value, PlatformInfoDisplayFn displayFunction)
{
	this->name = name;
	this->detail = detail;
	this->value = value;
	this->displayFunction = displayFunction;
}

QString PlatformInfoValueDetailValue::getDisplayValue()
{
	if (displayFunction) {
		return displayFunction(value);
	}
	else {
		return value.toString();
	}
}

PlatformInfoValue::PlatformInfoValue(cl_platform_info info, QVariant value, QString extension, PlatformInfoDisplayFn displayFunction)
{
	this->name = utils::platformInfoString(info);
	this->value = value;
	this->extension = extension;
	this->enumValue = info;
	this->displayFunction = displayFunction;
}

void PlatformInfoValue::addDetailValue(QString name, QVariant value, PlatformInfoDisplayFn displayFunction)
{
	detailValues.push_back(PlatformInfoValueDetailValue(name, value, displayFunction));
}

void PlatformInfoValue::addDetailValue(QString name, QString detail, QVariant value, PlatformInfoDisplayFn displayFunction)
{
	detailValues.push_back(PlatformInfoValueDetailValue(name, detail, value, displayFunction));
}

QString PlatformInfoValue::getDisplayValue()
{
	if (displayFunction) {
		return displayFunction(value);
	} else {
		return value.toString();
	}
}

void PlatformInfo::readOpenCLVersion()
{
	size_t valueSize;
    _clGetPlatformInfo(this->platformId, CL_PLATFORM_VERSION, 0, nullptr, &valueSize);
	std::string value;
	value.resize(valueSize);
    _clGetPlatformInfo(this->platformId, CL_PLATFORM_VERSION, valueSize, &value[0], nullptr);
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

void PlatformInfo::readPlatformInfoValue(PlatformInfoValueDescriptor descriptor, QString extension)
{
	qInfo() << "Reading platform info value for" << utils::platformInfoString(descriptor.name);
	switch (descriptor.valueType)
	{
	case clValueType::cl_char:
	{
		size_t valueSize;
        _clGetPlatformInfo(this->platformId, descriptor.name, 0, nullptr, &valueSize);
		char* value = new char[valueSize];
        _clGetPlatformInfo(this->platformId, descriptor.name, valueSize, &value[0], nullptr);
		platformInfo.push_back(PlatformInfoValue(descriptor.name, QString::fromUtf8(value), extension));
		delete[] value;
		break;
	}
	case clValueType::cl_ulong:
	{
		cl_ulong value;
        _clGetPlatformInfo(this->platformId, descriptor.name, sizeof(cl_ulong), &value, nullptr);
        platformInfo.push_back(PlatformInfoValue(descriptor.name, QVariant::fromValue(value), extension));
		break;
	}
	case clValueType::cl_version:
	{
		cl_version value;
        _clGetPlatformInfo(this->platformId, descriptor.name, sizeof(cl_version), &value, nullptr);
		platformInfo.push_back(PlatformInfoValue(descriptor.name, value, extension));
		break;
	}
		break;
	}
	default:
		qDebug("Unknown platform info type");
	}
}

void PlatformInfo::readExtensions()
{
	extensions.clear();
	if (clVersionMajor >= 3) {
		qInfo() << "Reading platform extension list with versions (CL >=3.0) for platform" << platformId;
		size_t extSize;
        _clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS_WITH_VERSION, 0, nullptr, &extSize);
		cl_name_version* extensions = new cl_name_version[4096];
        _clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS_WITH_VERSION, extSize, extensions, nullptr);
		for (size_t i = 0; i < extSize / sizeof(cl_name_version); i++) {
			PlatformExtension extension{};
			extension.name = extensions[i].name;
			extension.version = extensions[i].version;
			this->extensions.push_back(extension);
		}
		delete[] extensions;
	} else {
		qInfo() << "Reading platform extension list (CL <3.0) for platform" << platformId;
		size_t extStrSize;
        _clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS, 0, nullptr, &extStrSize);
		std::string extensionString;
		extensionString.resize(extStrSize);
        _clGetPlatformInfo(this->platformId, CL_PLATFORM_EXTENSIONS, extStrSize, &extensionString[0], nullptr);
		std::vector<std::string> extensions;
		extensions = utils::explode(extensionString, ' ');
		for (size_t i = 0; i < extensions.size(); i++) {
			PlatformExtension extension{};
			// Remove trailing zeros
			extensions[i].erase(std::find(extensions[i].begin(), extensions[i].end(), '\0'), extensions[i].end());
			extension.name = QString::fromStdString(extensions[i]);
			// Skip empty extension strings
			if (extensions[i].length() == 0) {
				continue;
			}
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
	platformInfo.clear();
	std::vector<PlatformInfoValueDescriptor> infoList = {
		{ CL_PLATFORM_PROFILE, clValueType::cl_char },
		{ CL_PLATFORM_VERSION, clValueType::cl_char },
		{ CL_PLATFORM_NAME, clValueType::cl_char },
		{ CL_PLATFORM_VENDOR, clValueType::cl_char },
	};
	for (auto &info : infoList)
	{
		readPlatformInfoValue(info);
	}

	readOpenCLVersion();
	readExtensions();
	readExtensionInfo();

	// Version dependent information
	if ((clVersionMajor == 2) && (clVersionMinor >= 1)) {
		readPlatformInfoValue(PlatformInfoValueDescriptor(CL_PLATFORM_HOST_TIMER_RESOLUTION, clValueType::cl_ulong));
	}
}

void PlatformInfo::readExtensionInfo()
{
	qInfo() << "Reading extension info values for platform" << platformId;
	// KHR
	if (extensionSupported("cl_khr_icd")) {
		std::vector<PlatformInfoValueDescriptor> infoList = {
			{ CL_PLATFORM_ICD_SUFFIX_KHR, clValueType::cl_char },
		};
		for (auto &info : infoList) {
			readPlatformInfoValue(info, "cl_khr_icd");
		}
	}
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
		// Optional details for the platform info
		if (info.detailValues.size() > 0) {
			QJsonArray jsonDetails;
			for (auto& detail : info.detailValues) {
				QJsonObject jsonNodeDetail;
				jsonNodeDetail["name"] = detail.name;
				if (detail.detail.isEmpty()) {
					jsonNodeDetail["detail"] = QJsonValue::Null;
				}
				else {
					jsonNodeDetail["detail"] = detail.detail;
				}
				jsonNodeDetail["value"] = detail.value.toJsonValue();
				jsonDetails.append(jsonNodeDetail);
			}
			jsonNode["details"] = jsonDetails;
		}
		jsonDeviceInfos.append(jsonNode);
	}
	jsonRoot["info"] = jsonDeviceInfos;

	return jsonRoot;
}
