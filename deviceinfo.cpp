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

QString DeviceInfo::getDeviceInfoString(cl_device_info info)
{
	size_t valueSize;
	clGetDeviceInfo(this->deviceId, info, 0, nullptr, &valueSize);
	std::string value;
	value.resize(valueSize);
	clGetDeviceInfo(this->deviceId, info, valueSize, &value[0], nullptr);
	return QString::fromStdString(value);
}

bool DeviceInfo::extensionSupported(const char* name)
{
	for (auto& ext : extensions) {
		if (ext.name == QLatin1String(name)) {
			return true;
		}
	}
	return false;
}

void DeviceInfo::readDeviceInfoValue(cl_device_info info, clValueType valueType, QString extension)
{
	// @todo: return instead of add to map?
	switch(valueType) 
	{
	case clValueType::cl_bool:
	{
		cl_bool value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_bool), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_char:
	{
		size_t valueSize;
		clGetDeviceInfo(this->deviceId, info, 0, nullptr, &valueSize);
		std::string value;
		value.resize(valueSize);
		clGetDeviceInfo(this->deviceId, info, valueSize, &value[0], nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, QString::fromStdString(value), extension));
		break;
	}
	case clValueType::cl_size_t:
	{
		size_t value;
		clGetDeviceInfo(this->deviceId, info, sizeof(size_t), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_uint:
	{
		cl_uint value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_uint), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_ulong:
	{
		cl_ulong value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_ulong), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_version:
	case clValueType::cl_version_khr:
	{
		cl_version value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_version), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	//
	case clValueType::cl_device_atomic_capabilities:
	{
		cl_device_atomic_capabilities value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_atomic_capabilities), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_device_device_enqueue_capabilities:
	{
		cl_device_device_enqueue_capabilities value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_device_enqueue_capabilities), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_device_local_mem_type:
	{
		cl_device_local_mem_type value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_local_mem_type), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_device_exec_capabilities:
	{
		cl_device_exec_capabilities value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_exec_capabilities), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_device_mem_cache_type:
	{
		cl_device_mem_cache_type value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_mem_cache_type), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_command_queue_properties:
	{
		cl_command_queue_properties value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_command_queue_properties), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_device_fp_config:
	{
		cl_device_fp_config value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_fp_config), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	case clValueType::cl_device_type:
	{
		cl_device_type value;
		clGetDeviceInfo(this->deviceId, info, sizeof(cl_device_type), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(info, value, extension));
		break;
	}
	/* Special cases */
	case clValueType::special:
	{
		switch (info)
		{
		case CL_DEVICE_MAX_WORK_ITEM_SIZES:
		{
			cl_uint dim;
			clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &dim, nullptr);
			std::vector<size_t> dimensions(dim);
			clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * dim, &dimensions[0], nullptr);
			QVariantList variantList;
			for (auto dimension : dimensions) {
				variantList << dimension;
			}
			deviceInfo.push_back(DeviceInfoValue(info, variantList, extension));
			break;
		}
		case CL_DEVICE_UUID_KHR:
		case CL_DRIVER_UUID_KHR:
		{
			cl_uchar uuid[CL_UUID_SIZE_KHR];
			clGetDeviceInfo(this->deviceId, info, sizeof(uuid), &uuid, nullptr);
			std::ostringstream os;
			os << std::hex << std::noshowbase << std::uppercase;
			for (uint32_t i = 0; i < CL_UUID_SIZE_KHR; i++) {
				os << std::right << std::setw(2) << std::setfill('0') << static_cast<unsigned short>(uuid[i]);
				if (i == 3 || i == 5 || i == 7 || i == 9) os << '-';
			}
			deviceInfo.push_back(DeviceInfoValue(info, QString::fromStdString(os.str()), extension));
			break;
		}
		case CL_DEVICE_LUID_KHR:
			cl_uchar uuid[CL_LUID_SIZE_KHR];
			clGetDeviceInfo(this->deviceId, info, sizeof(uuid), &uuid, nullptr);
			std::ostringstream os;
			os << std::hex << std::noshowbase << std::uppercase;
			for (uint32_t i = 0; i < CL_LUID_SIZE_KHR; i++) {
				os << std::right << std::setw(2) << std::setfill('0') << static_cast<unsigned short>(uuid[i]);
			}
			deviceInfo.push_back(DeviceInfoValue(info, QString::fromStdString(os.str()), extension));
			break;
		}
	}
	default:
		qDebug("Unknwon device info type");
	}
}

void DeviceInfo::readDeviceInfo()
{
	deviceInfo.clear();

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
	for (auto info : infoList)
	{
		readDeviceInfoValue(info.first, info.second);
	}

	// OpenCL 3.0
	if (clVersionMajor == 3) 
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

void DeviceInfo::readOpenCLVersion()
{
	size_t valueSize;
	clGetDeviceInfo(this->deviceId, CL_DEVICE_VERSION, 0, nullptr, &valueSize);
	std::string value;
	value.resize(valueSize);
	clGetDeviceInfo(this->deviceId, CL_DEVICE_VERSION, valueSize, &value[0], nullptr);
	// OpenCL<space><major_version.minor_version><space>
	size_t versStart = value.find(' ', 0);
	size_t versSplit = value.find('.', versStart+1);
	size_t versEnd = value.find(' ', versStart+1);
	std::string major, minor;
	major = value.substr(versStart, versSplit - versStart);
	minor = value.substr(versSplit + 1, versEnd - versSplit - 1);
	clVersionMajor = std::stoi(major);
	clVersionMinor = std::stoi(minor);
}

void DeviceInfo::readExtensionInfo()
{
	// KHR
	if (extensionSupported("cl_khr_fp64")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_DOUBLE_FP_CONFIG, clValueType::cl_device_fp_config },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_fp64");
		}
	}
	if (extensionSupported("cl_khr_fp16")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_HALF_FP_CONFIG, clValueType::cl_device_fp_config },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_fp16");
		}
	}
	if (extensionSupported("cl_khr_il_program")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_IL_VERSION_KHR, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_il_program");
		}
	}
	if (extensionSupported("cl_khr_image2D_from_buffer")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_IMAGE_PITCH_ALIGNMENT_KHR, clValueType::cl_uint },
			{ CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT_KHR, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_image2D_from_buffer");
		}
	}	
	if (extensionSupported("cl_khr_terminate_context")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			// @todo: bitfield
			//{ CL_DEVICE_TERMINATE_CAPABILITY_KHR, clValueType::cl_device_terminate_capability_khr },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_terminate_context");
		}
	}
	if (extensionSupported("cl_khr_spir")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_SPIR_VERSIONS, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_spir");
		}
	}
	if (extensionSupported("cl_khr_subgroup_named_barrier")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_MAX_NAMED_BARRIER_COUNT_KHR, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_spir");
		}
	}
	if (extensionSupported("cl_khr_device_uuid")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_UUID_KHR, clValueType::special },
			{ CL_DRIVER_UUID_KHR, clValueType::special },
			{ CL_DEVICE_LUID_VALID_KHR, clValueType::cl_bool },
			{ CL_DEVICE_LUID_KHR, clValueType::special },
			{ CL_DEVICE_NODE_MASK_KHR, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_device_uuid");
		}
	}
	if (extensionSupported("cl_khr_extended_versioning")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_NUMERIC_VERSION_KHR, clValueType::cl_version_khr },
			{ CL_DEVICE_OPENCL_C_NUMERIC_VERSION_KHR, clValueType::cl_version_khr },
			// @todo
			//{ CL_DEVICE_EXTENSIONS_WITH_VERSION_KHR, clValueType::cl_name_version_khr[] },
			//{ CL_DEVICE_ILS_WITH_VERSION_KHR, clValueType::cl_name_version_khr[] },
			//{ CL_DEVICE_BUILT_IN_KERNELS_WITH_VERSION_KHR, clValueType::cl_name_version_khr[] },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_extended_versioning");
		}
	}
	if (extensionSupported("cl_khr_pci_bus_info")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			// @todo
			//{ CL_DEVICE_PCI_BUS_INFO_KHR, clValueType::cl_device_pci_bus_info_khr },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_khr_pci_bus_info");
		}
	}
	
	// EXT
	// @todo: types
	if (extensionSupported("cl_ext_device_fission")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_PARENT_DEVICE_EXT, clValueType::cl_char },
			{ CL_DEVICE_PARTITION_TYPES_EXT, clValueType::cl_char },
			{ CL_DEVICE_AFFINITY_DOMAINS_EXT, clValueType::cl_char },
			{ CL_DEVICE_REFERENCE_COUNT_EXT, clValueType::cl_char },
			{ CL_DEVICE_PARTITION_STYLE_EXT, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_ext_device_fission");
		}
	}
	if (extensionSupported("cl_ext_cxx_for_opencl")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_CXX_FOR_OPENCL_NUMERIC_VERSION_EXT, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_ext_cxx_for_opencl");
		}
	}

	// ARM
	if (extensionSupported("cl_arm_shared_virtual_memory")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_SVM_CAPABILITIES_ARM, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_arm_shared_virtual_memory");
		}
	}
	if (extensionSupported("cl_arm_get_core_id")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_COMPUTE_UNITS_BITFIELD_ARM, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_arm_get_core_id");
		}
	}
	if (extensionSupported("cl_arm_controlled_kernel_termination")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_CONTROLLED_TERMINATION_CAPABILITIES_ARM, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_arm_controlled_kernel_termination");
		}
	}
	if (extensionSupported("cl_arm_scheduling_controls")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			//@todo: bitfield
			//{ CL_DEVICE_SCHEDULING_CONTROLS_CAPABILITIES_ARM, clValueType::cl_device_scheduling_controls_capabilities_arm },
			//@todo: int[]
			//{ CL_DEVICE_SUPPORTED_REGISTER_ALLOCATIONS_ARM, clValueType::cl_int[] },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_arm_scheduling_controls");
		}
	}

	// INTEL
	if (extensionSupported("cl_intel_advanced_motion_estimation")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_ME_VERSION_INTEL, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_intel_advanced_motion_estimation");
		}
	}
	if (extensionSupported("cl_intel_simultaneous_sharing")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			// @todo
			// { CL_DEVICE_SIMULTANEOUS_INTEROPS_INTEL, clValueType::cl_uint[] }, 
			{ CL_DEVICE_NUM_SIMULTANEOUS_INTEROPS_INTEL, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_intel_simultaneous_sharing");
		}
	}
	if (extensionSupported("cl_intel_required_subgroup_size")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			// @todo
			// { CL_DEVICE_SUB_GROUP_SIZES_INTEL, clValueType::size_t[] },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_intel_required_subgroup_size");
		}
	}
	if (extensionSupported("cl_intel_planar_yuv")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_PLANAR_YUV_MAX_WIDTH_INTEL, clValueType::cl_size_t },
			{ CL_DEVICE_PLANAR_YUV_MAX_HEIGHT_INTEL, clValueType::cl_size_t },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_intel_planar_yuv");
		}
	}
	if (extensionSupported("cl_intel_device_side_avc_motion_estimation")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_AVC_ME_VERSION_INTEL, clValueType::cl_uint },
			{ CL_DEVICE_AVC_ME_SUPPORTS_TEXTURE_SAMPLER_USE_INTEL, clValueType::cl_bool },
			{ CL_DEVICE_AVC_ME_SUPPORTS_PREEMPTION_INTEL, clValueType::cl_bool },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_intel_device_side_avc_motion_estimation");
		}
	}
	/* @todo: undocumented?
	if (extensionSupported("cl_intel_unified_shared_memory")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_HOST_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
			{ CL_DEVICE_DEVICE_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
			{ CL_DEVICE_SINGLE_DEVICE_SHARED_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
			{ CL_DEVICE_CROSS_DEVICE_SHARED_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
			{ CL_DEVICE_SHARED_SYSTEM_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_intel_unified_shared_memory");
		}
	}
	*/
	if (extensionSupported("cl_intel_command_queue_families")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			// @todo
			//{ CL_DEVICE_QUEUE_FAMILY_PROPERTIES_INTEL, clValueType::cl_queue_family_properties_intel },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_intel_command_queue_families");
		}
	}

	// QCOM
	if (extensionSupported("cl_qcom_ext_host_ptr")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_EXT_MEM_PADDING_IN_BYTES_QCOM, clValueType::cl_char },
			{ CL_DEVICE_PAGE_SIZE_QCOM, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_qcom_ext_host_ptr");
		}
	}

	// AMD
	// @todo: types
	if (extensionSupported("cl_amd_device_attribute_query")) {
		std::unordered_map<cl_device_info, clValueType> infoList = {
			{ CL_DEVICE_PROFILING_TIMER_OFFSET_AMD, clValueType::cl_char },
			{ CL_DEVICE_TOPOLOGY_AMD, clValueType::cl_char },
			{ CL_DEVICE_BOARD_NAME_AMD, clValueType::cl_char },
			{ CL_DEVICE_GLOBAL_FREE_MEMORY_AMD, clValueType::cl_char },
			{ CL_DEVICE_SIMD_PER_COMPUTE_UNIT_AMD, clValueType::cl_char },
			{ CL_DEVICE_SIMD_WIDTH_AMD, clValueType::cl_char },
			{ CL_DEVICE_SIMD_INSTRUCTION_WIDTH_AMD, clValueType::cl_char },
			{ CL_DEVICE_WAVEFRONT_WIDTH_AMD, clValueType::cl_char },
			{ CL_DEVICE_GLOBAL_MEM_CHANNELS_AMD, clValueType::cl_char },
			{ CL_DEVICE_GLOBAL_MEM_CHANNEL_BANKS_AMD, clValueType::cl_char },
			{ CL_DEVICE_GLOBAL_MEM_CHANNEL_BANK_WIDTH_AMD, clValueType::cl_char },
			{ CL_DEVICE_LOCAL_MEM_SIZE_PER_COMPUTE_UNIT_AMD, clValueType::cl_char },
			{ CL_DEVICE_LOCAL_MEM_BANKS_AMD, clValueType::cl_char },
			{ CL_DEVICE_THREAD_TRACE_SUPPORTED_AMD, clValueType::cl_char },
			{ CL_DEVICE_GFXIP_MAJOR_AMD, clValueType::cl_char },
			{ CL_DEVICE_GFXIP_MINOR_AMD, clValueType::cl_char },
			{ CL_DEVICE_AVAILABLE_ASYNC_QUEUES_AMD, clValueType::cl_char },
			{ CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_AMD, clValueType::cl_char },
			{ CL_DEVICE_MAX_WORK_GROUP_SIZE_AMD, clValueType::cl_char },
			{ CL_DEVICE_PREFERRED_CONSTANT_BUFFER_SIZE_AMD, clValueType::cl_char },
			{ CL_DEVICE_PCIE_ID_AMD, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info.first, info.second, "cl_amd_device_attribute_query");
		}
	}

	// NV
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
			readDeviceInfoValue(info.first, info.second, "cl_nv_device_attribute_query");
		}
	}
}

DeviceInfo::DeviceInfo()
{
}

void DeviceInfo::read()
{
	name = getDeviceInfoString(CL_DEVICE_NAME);
	readOpenCLVersion();
	readDeviceInfo();
	readExtensions();
	readExtensionInfo();
}

QJsonObject DeviceInfo::toJson()
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

	// Device info
	QJsonArray jsonDeviceInfos;
	for (auto& info : deviceInfo)
	{
		QJsonObject jsonNode;
		jsonNode["name"] = info.name;
		jsonNode["extension"] = info.extension;
		jsonNode["enumvalue"] = info.enumValue;
		if (info.value.canConvert(QMetaType::QVariantList)) {
			jsonNode["value"] = QJsonArray::fromVariantList(info.value.toList());
		} else {
			jsonNode["value"] = info.value.toString();
		}
		jsonDeviceInfos.append(jsonNode);
	}
	jsonRoot["info"] = jsonDeviceInfos;

	// Additional OpenCL info
	QJsonObject jsonDeviceInfosOpenCL;
	jsonDeviceInfosOpenCL["versionmajor"] = clVersionMajor;
	jsonDeviceInfosOpenCL["versionminor"] = clVersionMinor;
	jsonRoot["opencl"] = jsonDeviceInfosOpenCL;

	return jsonRoot;
}

void DeviceInfo::readExtensions()
{
	// @todo: 3.0 vs. older way of reading (no version)
	extensions.clear();
	size_t extSize;
	clGetDeviceInfo(this->deviceId, CL_DEVICE_EXTENSIONS_WITH_VERSION, 0, nullptr, &extSize);
	cl_name_version* extensions = new cl_name_version[4096];
	clGetDeviceInfo(this->deviceId, CL_DEVICE_EXTENSIONS_WITH_VERSION, extSize, extensions, nullptr);
	for (size_t i = 0; i < extSize / sizeof(cl_name_version); i++) {
		DeviceExtension extension{};
		extension.name = extensions[i].name;
		extension.version = extensions[i].version;
		this->extensions.push_back(extension);
	}
	delete[] extensions;
}

DeviceInfoValue::DeviceInfoValue(cl_device_info info, QVariant value, QString extension)
{
	this->name = utils::deviceInfoString(info);
	this->value = value;
	this->extension = extension;
	this->enumValue = info;
}
