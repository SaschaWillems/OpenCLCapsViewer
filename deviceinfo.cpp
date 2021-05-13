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

#include "deviceinfo.h"

std::string DeviceInfo::getDeviceInfoString(cl_device_info info)
{
	size_t valueSize;
	clGetDeviceInfo(this->deviceId, info, 0, nullptr, &valueSize);
	std::string value;
	value.resize(valueSize);
	clGetDeviceInfo(this->deviceId, info, valueSize, &value[0], nullptr);
	return value;
}

bool DeviceInfo::extensionSupported(const char* name)
{
	for (auto& ext : extensions) {
		if (strcmp(ext.name.c_str(), name) == 0) {
			return true;
		}
	}
	return false;
}

void DeviceInfo::readDeviceInfoValue(cl_device_info info, clValueType valueType)
{
	// @todo: return instead of add to map
	switch(valueType) 
	{
	case clValueType::cl_bool:
	{
		cl_bool value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_bool), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_char:
	{
		size_t valueSize;
		clGetDeviceInfo(this->deviceId, info, 0, nullptr, &valueSize);
		std::string value;
		value.resize(valueSize);
		clGetDeviceInfo(this->deviceId, info, valueSize, &value[0], nullptr);
		deviceInfo[utils::deviceInfoString(info)] = QString::fromStdString(value);
		break;
	}
	case clValueType::cl_size_t:
	{
		size_t value;
		clGetDeviceInfo(this->deviceId, info, sizeof(size_t), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_uint:
	{
		cl_uint value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_uint), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_ulong:
	{
		cl_ulong value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_ulong), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_version:
	{
		cl_version value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_version), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	//
	case clValueType::cl_device_atomic_capabilities:
	{
		cl_device_atomic_capabilities value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_atomic_capabilities), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_device_device_enqueue_capabilities:
	{
		cl_device_device_enqueue_capabilities value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_device_enqueue_capabilities), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_device_local_mem_type:
	{
		cl_device_local_mem_type value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_local_mem_type), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_device_exec_capabilities:
	{
		cl_device_exec_capabilities value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_exec_capabilities), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_device_mem_cache_type:
	{
		cl_device_mem_cache_type value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_mem_cache_type), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_command_queue_properties:
	{
		cl_command_queue_properties value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_command_queue_properties), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_device_fp_config:
	{
		cl_device_fp_config value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_fp_config), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	case clValueType::cl_device_type:
	{
		cl_device_type value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_type), &value, nullptr);
		deviceInfo[utils::deviceInfoString(info)] = value;
		break;
	}
	/* Special cases */
	case clValueType::special:
	{
		if (info == CL_DEVICE_MAX_WORK_ITEM_SIZES) {
			cl_uint dim;
			clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &dim, nullptr);
			std::vector<size_t> dimensions(dim);
			clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * dim, &dimensions[0], nullptr);
			QVariantList variantList;
			for (auto dimension : dimensions) {
				variantList << dimension;
			}
			deviceInfo[utils::deviceInfoString(info)] = variantList;
		}
		break;
	}
	default:
		qDebug("Unknwon device info type");
	}
}

void DeviceInfo::readDeviceInfo()
{
	std::unordered_map<cl_device_info, clValueType> infoList = {
		{ CL_DEVICE_TYPE, clValueType::cl_device_type },
		{ CL_DEVICE_VENDOR_ID, clValueType::cl_uint },
		{ CL_DEVICE_MAX_COMPUTE_UNITS, clValueType::cl_uint },
		{ CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, clValueType::cl_uint },
		{ CL_DEVICE_MAX_WORK_GROUP_SIZE, clValueType::cl_size_t },
		{ CL_DEVICE_MAX_WORK_ITEM_SIZES, clValueType::special },
		{ CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, clValueType::cl_uint },
		{ CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, clValueType::cl_uint },
		{ CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, clValueType::cl_uint },
		{ CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, clValueType::cl_uint },
		{ CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, clValueType::cl_uint },
		{ CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, clValueType::cl_uint },
		{ CL_DEVICE_MAX_CLOCK_FREQUENCY, clValueType::cl_uint },
		{ CL_DEVICE_ADDRESS_BITS, clValueType::cl_uint },
		{ CL_DEVICE_MAX_READ_IMAGE_ARGS, clValueType::cl_uint },
		{ CL_DEVICE_MAX_WRITE_IMAGE_ARGS, clValueType::cl_uint },
		{ CL_DEVICE_MAX_MEM_ALLOC_SIZE, clValueType::cl_ulong },
		{ CL_DEVICE_IMAGE2D_MAX_WIDTH, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE2D_MAX_HEIGHT, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE3D_MAX_WIDTH, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE3D_MAX_HEIGHT, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE3D_MAX_DEPTH, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE_SUPPORT, clValueType::cl_bool },
		{ CL_DEVICE_MAX_PARAMETER_SIZE, clValueType::cl_size_t },
		{ CL_DEVICE_MAX_SAMPLERS, clValueType::cl_uint },
		{ CL_DEVICE_MEM_BASE_ADDR_ALIGN, clValueType::cl_uint },
		{ CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, clValueType::cl_uint}, // @todo: deprecated in 1.2
		{ CL_DEVICE_SINGLE_FP_CONFIG, clValueType::cl_device_fp_config },
		{ CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, clValueType::cl_device_mem_cache_type },
		{ CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, clValueType::cl_uint },
		{ CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, clValueType::cl_ulong },
		{ CL_DEVICE_GLOBAL_MEM_SIZE, clValueType::cl_ulong },
		{ CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, clValueType::cl_ulong },
		{ CL_DEVICE_MAX_CONSTANT_ARGS, clValueType::cl_uint },
		{ CL_DEVICE_LOCAL_MEM_TYPE, clValueType::cl_device_local_mem_type },
		{ CL_DEVICE_LOCAL_MEM_SIZE, clValueType::cl_ulong },
		{ CL_DEVICE_ERROR_CORRECTION_SUPPORT, clValueType::cl_bool },
		{ CL_DEVICE_PROFILING_TIMER_RESOLUTION, clValueType::cl_size_t },
		{ CL_DEVICE_ENDIAN_LITTLE, clValueType::cl_bool },
		//{ CL_DEVICE_AVAILABLE, clValueType:: },
		{ CL_DEVICE_COMPILER_AVAILABLE, clValueType::cl_bool },
		{ CL_DEVICE_EXECUTION_CAPABILITIES, clValueType::cl_device_exec_capabilities },
		// { CL_DEVICE_QUEUE_PROPERTIES, clValueType:: }, @todo
		{ CL_DEVICE_NAME, clValueType::cl_char },
		{ CL_DEVICE_VENDOR, clValueType::cl_char },
		{ CL_DRIVER_VERSION, clValueType::cl_char },
		{ CL_DEVICE_PROFILE, clValueType::cl_char },
		{ CL_DEVICE_VERSION, clValueType::cl_char },
		//{ CL_DEVICE_EXTENSIONS, clValueType:: },
		//{ CL_DEVICE_PLATFOR, clValueType:: },
	};

	deviceInfo.clear();
	for (auto info : infoList)
	{
		readDeviceInfoValue(info.first, info.second);
	}

	// OpenCL 3.0
	// @todo: check version support
	if (false) 
	{
		std::unordered_map<cl_device_info, clValueType> infoListCL30 = {
			{ CL_DEVICE_NUMERIC_VERSION, clValueType::cl_version },
			// { CL_DEVICE_ILS_WITH_VERSION, clValueType::cl_device_type }, array of descriptors
			// { CL_DEVICE_BUILT_IN_KERNELS_WITH_VERSION, clValueType::cl_device_type }, array of descriptors
			{ CL_DEVICE_ATOMIC_MEMORY_CAPABILITIES, clValueType::cl_device_atomic_capabilities },
			{ CL_DEVICE_ATOMIC_FENCE_CAPABILITIES, clValueType::cl_device_atomic_capabilities },
			{ CL_DEVICE_NON_UNIFORM_WORK_GROUP_SUPPORT, clValueType::cl_bool },
			// { CL_DEVICE_OPENCL_C_ALL_VERSIONS, clValueType::cl_device_type },  array of descriptors
			{ CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, clValueType::cl_size_t },
			{ CL_DEVICE_WORK_GROUP_COLLECTIVE_FUNCTIONS_SUPPORT, clValueType::cl_bool },
			{ CL_DEVICE_GENERIC_ADDRESS_SPACE_SUPPORT, clValueType::cl_bool },
			//{ CL_DEVICE_OPENCL_C_FEATURES, clValueType:: }, array of descriptors
			{ CL_DEVICE_DEVICE_ENQUEUE_CAPABILITIES, clValueType::cl_device_device_enqueue_capabilities },
			{ CL_DEVICE_PIPE_SUPPORT, clValueType::cl_bool },
			{ CL_DEVICE_LATEST_CONFORMANCE_VERSION_PASSED, clValueType::cl_char },
		};
		for (auto info : infoListCL30)
		{
			readDeviceInfoValue(info.first, info.second);
		}

		// @todo: OpenCL CL_DEVICE_OPENCL_C_FEATURES
		size_t featuresSize;
		clGetDeviceInfo(this->deviceId, CL_DEVICE_OPENCL_C_FEATURES, 0, nullptr, &featuresSize);
		cl_name_version* features = new cl_name_version[4096];
		clGetDeviceInfo(this->deviceId, CL_DEVICE_OPENCL_C_FEATURES, featuresSize, features, nullptr);
		for (size_t i = 0; i < featuresSize / sizeof(cl_name_version); i++) {
			qDebug(features[i].name);
		}
		delete[] features;
	}
}

void DeviceInfo::readDeviceInfoExtensions()
{
	if (extensionSupported("cl_nv_device_attribute_query")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, clValueType::cl_uint },
			{ CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, clValueType::cl_uint },
			{ CL_DEVICE_REGISTERS_PER_BLOCK_NV, clValueType::cl_uint },
			{ CL_DEVICE_WARP_SIZE_NV, clValueType::cl_uint },
			{ CL_DEVICE_GPU_OVERLAP_NV, clValueType::cl_bool },
			{ CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV, clValueType::cl_bool },
			{ CL_DEVICE_INTEGRATED_MEMORY_NV, clValueType::cl_bool },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second);
		}
	}
}

DeviceInfo::DeviceInfo()
{
}

void DeviceInfo::read()
{
	name = getDeviceInfoString(CL_DEVICE_NAME);
	readDeviceInfo();
	readDeviceExtensions();
	readDeviceInfoExtensions();
}

void DeviceInfo::readDeviceExtensions()
{
	// @todo: 3.0 vs. older way of reading (no version)
	extensions.clear();
	size_t extSize;
	clGetDeviceInfo(this->deviceId, CL_DEVICE_EXTENSIONS_WITH_VERSION, 0, nullptr, &extSize);
	cl_name_version* extensions = new cl_name_version[4096];
	clGetDeviceInfo(this->deviceId, CL_DEVICE_EXTENSIONS_WITH_VERSION, extSize, extensions, nullptr);
	for (size_t i = 0; i < extSize / sizeof(cl_name_version); i++) {
		DeviceExtension deviceExtension{};
		deviceExtension.name = extensions[i].name;
		deviceExtension.version = extensions[i].version;
		this->extensions.push_back(deviceExtension);
	}
	delete[] extensions;
}
