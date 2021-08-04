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
	char* value = new char[valueSize];
	clGetDeviceInfo(this->deviceId, info, valueSize, &value[0], nullptr);
	return QString::fromUtf8(value);
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

void DeviceInfo::readDeviceInfoValue(DeviceInfoValueDescriptor descriptor, QString extension)
{
	// @todo: return instead of add to map?
	switch(descriptor.valueType)
	{
	case clValueType::cl_bool:
	{
		cl_bool value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_bool), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, value, extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_char:
	{
		size_t valueSize;
		clGetDeviceInfo(this->deviceId, descriptor.name, 0, nullptr, &valueSize);
		char* value = new char[valueSize];
		clGetDeviceInfo(this->deviceId, descriptor.name, valueSize, &value[0], nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, QString::fromUtf8(value), extension, descriptor.displayFunction));
		delete[] value;
		break;
	}
	case clValueType::cl_size_t:
	{
		size_t value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(size_t), &value, nullptr);
        deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_size_t_array:
	{
		size_t valueSize;
		clGetDeviceInfo(this->deviceId, descriptor.name, 0, nullptr, &valueSize);
		std::vector<size_t> values;
		values.resize(valueSize/sizeof(size_t));
		clGetDeviceInfo(this->deviceId, descriptor.name, valueSize, &values[0], nullptr);
		QVariantList variantList;
		for (auto value : values) {
            variantList << QVariant::fromValue(value);
		}
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, variantList, extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_name_version_array:
	{
		size_t valueSize;
		clGetDeviceInfo(this->deviceId, descriptor.name, 0, nullptr, &valueSize);
		std::vector<cl_name_version> values;
		DeviceInfoValue infoValue(descriptor.name, 0, extension, descriptor.displayFunction);
		if (valueSize > 0) {
			values.resize(valueSize / sizeof(cl_name_version));
            infoValue.value = QVariant::fromValue(values.size());
			clGetDeviceInfo(this->deviceId, descriptor.name, valueSize, &values[0], nullptr);
			for (auto& value : values) {
				infoValue.addDetailValue(value.name, QVariant::fromValue(value.version), utils::displayVersion);
			}
		}
		deviceInfo.push_back(infoValue);
		break;
	}
	case clValueType::cl_device_partition_property_array:
	{
		size_t valueSize;
		clGetDeviceInfo(this->deviceId, descriptor.name, 0, nullptr, &valueSize);
		std::vector<cl_device_partition_property> values;
		DeviceInfoValue infoValue(descriptor.name, 0, extension, descriptor.displayFunction);
		infoValue.value = QVariant();
		if (valueSize > 0) {
			values.resize(valueSize / sizeof(cl_device_partition_property));
			// Instead of an empty array, an implementation may also return one element with a value of zero (as a terminator)
			if ((values.size() > 0) && (values[0] > 0)) {
				infoValue.value = QVariant::fromValue(values.size());
				clGetDeviceInfo(this->deviceId, descriptor.name, valueSize, &values[0], nullptr);
				for (auto& value : values) {
					infoValue.addDetailValue("", QVariant::fromValue(value) /*@todo*/);
				}
			}
		}
		deviceInfo.push_back(infoValue);
		break;
	}
	case clValueType::cl_uint:
	{
		cl_uint value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_uint), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, value, extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_ulong:
	{
		cl_ulong value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_ulong), &value, nullptr);
        deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_version:
	case clValueType::cl_version_khr:
	{
		cl_version value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_version), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, value, extension, descriptor.displayFunction));
		break;
	}
	//
	case clValueType::cl_device_atomic_capabilities:
	{
		cl_device_atomic_capabilities value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_atomic_capabilities), &value, nullptr);
        deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_device_enqueue_capabilities:
	{
		cl_device_device_enqueue_capabilities value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_device_enqueue_capabilities), &value, nullptr);
        deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_local_mem_type:
	{
		cl_device_local_mem_type value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_local_mem_type), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, value, extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_exec_capabilities:
	{
		cl_device_exec_capabilities value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_exec_capabilities), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_mem_cache_type:
	{
		cl_device_mem_cache_type value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_mem_cache_type), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, value, extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_command_queue_properties:
	{
		cl_command_queue_properties value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_command_queue_properties), &value, nullptr);
        deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_fp_config:
	{
		cl_device_fp_config value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_fp_config), &value, nullptr);
        deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_svm_capabilities:
	{
		cl_device_svm_capabilities value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_svm_capabilities), &value, nullptr);
        deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_type:
	{
		cl_device_type value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_type), &value, nullptr);
        deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_pci_bus_info_khr:
	{
		cl_device_pci_bus_info_khr value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_type), &value, nullptr);
		DeviceInfoValue infoValue(descriptor.name, QVariant(), extension, descriptor.displayFunction);
		// @todo: display functions
		infoValue.addDetailValue("pci_domain", value.pci_domain);
		infoValue.addDetailValue("pci_bus", value.pci_bus);
		infoValue.addDetailValue("pci_device", value.pci_device);
		infoValue.addDetailValue("pci_function", value.pci_function);
		deviceInfo.push_back(infoValue);
		break;
	}
	case clValueType::cl_device_affinity_domain:
	{
		cl_device_affinity_domain value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_affinity_domain), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	/* ARM */
	case clValueType::cl_device_controlled_termination_capabilities_arm:
	{
		cl_device_controlled_termination_capabilities_arm value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_controlled_termination_capabilities_arm), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	case clValueType::cl_device_scheduling_controls_capabilities_arm:
	{
		cl_device_scheduling_controls_capabilities_arm value;
		clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_scheduling_controls_capabilities_arm), &value, nullptr);
		deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
		break;
	}
	/* Special cases */
	case clValueType::special:
	{
		switch (descriptor.name)
		{
		case CL_DEVICE_MAX_WORK_ITEM_SIZES:
		{
			cl_uint dim;
			clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &dim, nullptr);
			std::vector<size_t> dimensions(dim);
			clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * dim, &dimensions[0], nullptr);
			QVariantList variantList;
			for (auto dimension : dimensions) {
                variantList << QVariant::fromValue(dimension);
			}
			deviceInfo.push_back(DeviceInfoValue(descriptor.name, variantList, extension, utils::displayNumberArray));
			break;
		}
		case CL_DEVICE_UUID_KHR:
		case CL_DRIVER_UUID_KHR:
		{
			cl_uchar uuid[CL_UUID_SIZE_KHR];
			clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(uuid), &uuid, nullptr);
			std::ostringstream os;
			os << std::hex << std::noshowbase << std::uppercase;
			for (uint32_t i = 0; i < CL_UUID_SIZE_KHR; i++) {
				os << std::right << std::setw(2) << std::setfill('0') << static_cast<unsigned short>(uuid[i]);
				if (i == 3 || i == 5 || i == 7 || i == 9) os << '-';
			}
			deviceInfo.push_back(DeviceInfoValue(descriptor.name, QString::fromStdString(os.str()), extension, descriptor.displayFunction));
			break;
		}
		case CL_DEVICE_LUID_KHR:
			cl_uchar uuid[CL_LUID_SIZE_KHR];
			clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(uuid), &uuid, nullptr);
			std::ostringstream os;
			os << std::hex << std::noshowbase << std::uppercase;
			for (uint32_t i = 0; i < CL_LUID_SIZE_KHR; i++) {
				os << std::right << std::setw(2) << std::setfill('0') << static_cast<unsigned short>(uuid[i]);
			}
			deviceInfo.push_back(DeviceInfoValue(descriptor.name, QString::fromStdString(os.str()), extension, descriptor.displayFunction));
			break;
		}
	}
	default:
		qDebug("Unknwon device info type");
	}
}

#if defined(__ANDROID__)
QString getSystemProperty(const char* propname)
{
	char prop[PROP_VALUE_MAX + 1];
	int len = __system_property_get(propname, prop);
	if (len > 0) {
		return QString(prop);
	}
	else {
		return "";
	}
}
#endif

void DeviceInfo::readDeviceIdentifier()
{
	// To distinguish android devices, we use the device name from the operating system as an identifier
	// CL_DEVICE_NAME only contains the GPU name, which may be the same for many different android devices
#if defined(__ANDROID__)
	QString productModel = getSystemProperty("ro.product.model");
	QString productManufacturer = getSystemProperty("ro.product.manufacturer");
	identifier.name = "";
	if (productManufacturer.trimmed() != "") {
		identifier.name = productManufacturer + " ";
	}
	identifier.name += productModel;
	identifier.gpuName = getDeviceInfoString(CL_DEVICE_NAME);
#else
	identifier.name = getDeviceInfoString(CL_DEVICE_NAME);
	identifier.gpuName = identifier.name;
#endif
	identifier.driverVersion = getDeviceInfoString(CL_DRIVER_VERSION);
	identifier.deviceVersion = getDeviceInfoString(CL_DEVICE_VERSION);
}

void DeviceInfo::readDeviceInfo()
{
	deviceInfo.clear();

	std::vector<DeviceInfoValueDescriptor> infoList = {
		{ CL_DEVICE_NAME, clValueType::cl_char },
		{ CL_DEVICE_TYPE, clValueType::cl_device_type, utils::displayDeviceType },
		{ CL_DEVICE_VENDOR_ID, clValueType::cl_uint },
		{ CL_DEVICE_VENDOR, clValueType::cl_char },
		{ CL_DRIVER_VERSION, clValueType::cl_char },
		{ CL_DEVICE_PROFILE, clValueType::cl_char },
		{ CL_DEVICE_VERSION, clValueType::cl_char },
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
		{ CL_DEVICE_MAX_MEM_ALLOC_SIZE, clValueType::cl_ulong, utils::displayByteSize },
		{ CL_DEVICE_IMAGE2D_MAX_WIDTH, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE2D_MAX_HEIGHT, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE3D_MAX_WIDTH, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE3D_MAX_HEIGHT, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE3D_MAX_DEPTH, clValueType::cl_size_t },
		{ CL_DEVICE_IMAGE_SUPPORT, clValueType::cl_bool, utils::displayBool },
		{ CL_DEVICE_MAX_PARAMETER_SIZE, clValueType::cl_size_t, utils::displayByteSize },
		{ CL_DEVICE_MAX_SAMPLERS, clValueType::cl_uint },
		{ CL_DEVICE_MEM_BASE_ADDR_ALIGN, clValueType::cl_uint },
		{ CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, clValueType::cl_uint, utils::displayByteSize }, // @todo: deprecated in 1.2
		{ CL_DEVICE_SINGLE_FP_CONFIG, clValueType::cl_device_fp_config, utils::displayFloatingPointConfig },
		{ CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, clValueType::cl_device_mem_cache_type, utils::displayMemCacheType },
		{ CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, clValueType::cl_uint, utils::displayByteSize },
		{ CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, clValueType::cl_ulong, utils::displayByteSize },
		{ CL_DEVICE_GLOBAL_MEM_SIZE, clValueType::cl_ulong, utils::displayByteSize },
		{ CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, clValueType::cl_ulong, utils::displayByteSize },
		{ CL_DEVICE_MAX_CONSTANT_ARGS, clValueType::cl_uint },
		{ CL_DEVICE_LOCAL_MEM_TYPE, clValueType::cl_device_local_mem_type, utils::displayLocalMemType },
		{ CL_DEVICE_LOCAL_MEM_SIZE, clValueType::cl_ulong, utils::displayByteSize },
		{ CL_DEVICE_ERROR_CORRECTION_SUPPORT, clValueType::cl_bool, utils::displayBool },
		{ CL_DEVICE_PROFILING_TIMER_RESOLUTION, clValueType::cl_size_t },
		{ CL_DEVICE_ENDIAN_LITTLE, clValueType::cl_bool, utils::displayBool },
		{ CL_DEVICE_COMPILER_AVAILABLE, clValueType::cl_bool, utils::displayBool },
		{ CL_DEVICE_EXECUTION_CAPABILITIES, clValueType::cl_device_exec_capabilities, utils::displayExecCapabilities },
		// { CL_DEVICE_QUEUE_PROPERTIES, clValueType:: }, @todo
	};
	for (auto info : infoList)
	{
		readDeviceInfoValue(info);
	}

	// OpenCL 1.1
	if (((clVersionMajor >= 1) && (clVersionMinor >= 1)) || (clVersionMajor > 2))
	{
		std::vector<DeviceInfoValueDescriptor> infoListCL11 = {
			{ CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, clValueType::cl_uint },
			{ CL_DEVICE_HOST_UNIFIED_MEMORY, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, clValueType::cl_uint },
			{ CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, clValueType::cl_uint },
			{ CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, clValueType::cl_uint },
			{ CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, clValueType::cl_uint },
			{ CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, clValueType::cl_uint },
			{ CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, clValueType::cl_uint },
			{ CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, clValueType::cl_uint },
			{ CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, clValueType::cl_uint },
			{ CL_DEVICE_OPENCL_C_VERSION, clValueType::cl_char, utils::displayText },
		};
		for (auto info : infoListCL11)
		{
			readDeviceInfoValue(info);
		}
	}

	// OpenCL 1.2
	if (((clVersionMajor >= 1) && (clVersionMinor >= 2)) || (clVersionMajor > 2))
	{
		std::vector<DeviceInfoValueDescriptor> infoListCL12 = {
			{ CL_DEVICE_LINKER_AVAILABLE, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_BUILT_IN_KERNELS, clValueType::cl_char, utils::displayText },
			{ CL_DEVICE_IMAGE_MAX_BUFFER_SIZE, clValueType::cl_size_t },
			{ CL_DEVICE_IMAGE_MAX_ARRAY_SIZE, clValueType::cl_size_t },
			// { CL_DEVICE_PARENT_DEVICE, clValueType::cl_device_id },
			{ CL_DEVICE_PARTITION_MAX_SUB_DEVICES, clValueType::cl_uint },
			{ CL_DEVICE_PARTITION_PROPERTIES, clValueType::cl_device_partition_property_array, utils::displayDevicePartitionProperties },
			{ CL_DEVICE_PARTITION_AFFINITY_DOMAIN, clValueType::cl_device_affinity_domain, utils::displayDeviceAffinityDomains },
			{ CL_DEVICE_PARTITION_TYPE, clValueType::cl_device_partition_property_array, utils::displayDevicePartitionProperties },
			{ CL_DEVICE_REFERENCE_COUNT, clValueType::cl_uint },
			{ CL_DEVICE_PREFERRED_INTEROP_USER_SYNC, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_PRINTF_BUFFER_SIZE, clValueType::cl_size_t, utils::displayByteSize },
		};
		for (auto info : infoListCL12)
		{
			readDeviceInfoValue(info);
		}
	}

	// OpenCL 2.0
	if (clVersionMajor == 2)
	{
		std::vector<DeviceInfoValueDescriptor> infoListCL20 = {
			{ CL_DEVICE_IMAGE_PITCH_ALIGNMENT, clValueType::cl_uint },
			{ CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT, clValueType::cl_uint },
			{ CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS, clValueType::cl_uint },
			{ CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE, clValueType::cl_size_t, utils::displayByteSize },
			{ CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES, clValueType::cl_command_queue_properties, utils::displayCommandQueueCapabilities },
			{ CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE, clValueType::cl_uint, utils::displayByteSize },
			{ CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE, clValueType::cl_uint, utils::displayByteSize },
			{ CL_DEVICE_MAX_ON_DEVICE_QUEUES, clValueType::cl_uint },
			{ CL_DEVICE_MAX_ON_DEVICE_EVENTS, clValueType::cl_uint },
			{ CL_DEVICE_SVM_CAPABILITIES, clValueType::cl_device_svm_capabilities, utils::displayDeviceSvmCapabilities },
			{ CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE, clValueType::cl_size_t, utils::displayByteSize },
			{ CL_DEVICE_MAX_PIPE_ARGS, clValueType::cl_uint },
			{ CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS, clValueType::cl_uint },
			{ CL_DEVICE_PIPE_MAX_PACKET_SIZE, clValueType::cl_uint, utils::displayByteSize },
			{ CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT, clValueType::cl_uint, utils::displayByteSize },
			{ CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT, clValueType::cl_uint, utils::displayByteSize },
			{ CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT, clValueType::cl_uint, utils::displayByteSize }
		};
		for (auto info : infoListCL20)
		{
			readDeviceInfoValue(info);
		}
	}

	// OpenCL 3.0
	if (clVersionMajor == 3) 
	{
		std::vector<DeviceInfoValueDescriptor> infoListCL30 = {
			{ CL_DEVICE_NUMERIC_VERSION, clValueType::cl_version, utils::displayVersion },
			{ CL_DEVICE_ILS_WITH_VERSION, clValueType::cl_name_version_array, utils::displayNameVersionArray },
			{ CL_DEVICE_BUILT_IN_KERNELS_WITH_VERSION, clValueType::cl_name_version_array, utils::displayNameVersionArray },
			{ CL_DEVICE_ATOMIC_MEMORY_CAPABILITIES, clValueType::cl_device_atomic_capabilities, utils::displayAtomicCapabilities },
			{ CL_DEVICE_ATOMIC_FENCE_CAPABILITIES, clValueType::cl_device_atomic_capabilities, utils::displayAtomicCapabilities },
			{ CL_DEVICE_NON_UNIFORM_WORK_GROUP_SUPPORT, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_OPENCL_C_ALL_VERSIONS, clValueType::cl_name_version_array, utils::displayNameVersionArray },
			{ CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, clValueType::cl_size_t },
			{ CL_DEVICE_WORK_GROUP_COLLECTIVE_FUNCTIONS_SUPPORT, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_GENERIC_ADDRESS_SPACE_SUPPORT, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_OPENCL_C_FEATURES, clValueType::cl_name_version_array, utils::displayNameVersionArray },
			{ CL_DEVICE_DEVICE_ENQUEUE_CAPABILITIES, clValueType::cl_device_device_enqueue_capabilities, utils::displayEnqueueCapabilities },
			{ CL_DEVICE_PIPE_SUPPORT, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_LATEST_CONFORMANCE_VERSION_PASSED, clValueType::cl_char },
		};
		for (auto info : infoListCL30)
		{
			readDeviceInfoValue(info);
		}
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
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_DOUBLE_FP_CONFIG, clValueType::cl_device_fp_config, utils::displayFloatingPointConfig },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_fp64");
		}
	}
	if (extensionSupported("cl_khr_fp16")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_HALF_FP_CONFIG, clValueType::cl_device_fp_config, utils::displayFloatingPointConfig },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_fp16");
		}
	}
	if (extensionSupported("cl_khr_il_program")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_IL_VERSION_KHR, clValueType::cl_char, utils::displayText },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_il_program");
		}
	}
	if (extensionSupported("cl_khr_image2D_from_buffer")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_IMAGE_PITCH_ALIGNMENT_KHR, clValueType::cl_uint },
			{ CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT_KHR, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_image2D_from_buffer");
		}
	}
	// @todo: values not defined in spec?
	if (extensionSupported("cl_khr_terminate_context")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			// @todo: bitfield
			//{ CL_DEVICE_TERMINATE_CAPABILITY_KHR, clValueType::cl_device_terminate_capability_khr },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_terminate_context");
		}
	}
	if (extensionSupported("cl_khr_spir")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_SPIR_VERSIONS, clValueType::cl_char, utils::displayText },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_spir");
		}
	}
	if (extensionSupported("cl_khr_subgroup_named_barrier")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_MAX_NAMED_BARRIER_COUNT_KHR, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_spir");
		}
	}
	if (extensionSupported("cl_khr_device_uuid")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_UUID_KHR, clValueType::special },
			{ CL_DRIVER_UUID_KHR, clValueType::special },
			{ CL_DEVICE_LUID_VALID_KHR, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_LUID_KHR, clValueType::special },
			{ CL_DEVICE_NODE_MASK_KHR, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_device_uuid");
		}
	}
	if (extensionSupported("cl_khr_extended_versioning")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_NUMERIC_VERSION_KHR, clValueType::cl_version_khr, utils::displayVersion },
			{ CL_DEVICE_OPENCL_C_NUMERIC_VERSION_KHR, clValueType::cl_version_khr, utils::displayVersion },
			{ CL_DEVICE_EXTENSIONS_WITH_VERSION_KHR, clValueType::cl_name_version_array, utils::displayNameVersionArray },
			{ CL_DEVICE_ILS_WITH_VERSION_KHR, clValueType::cl_name_version_array, utils::displayNameVersionArray },
			{ CL_DEVICE_BUILT_IN_KERNELS_WITH_VERSION_KHR, clValueType::cl_name_version_array, utils::displayNameVersionArray },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_extended_versioning");
		}
	}
	if (extensionSupported("cl_khr_pci_bus_info")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_PCI_BUS_INFO_KHR, clValueType::cl_device_pci_bus_info_khr },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_khr_pci_bus_info");
		}
	}
	
	// EXT
	// @todo: types
	if (extensionSupported("cl_ext_device_fission")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_PARENT_DEVICE_EXT, clValueType::cl_char },
			{ CL_DEVICE_PARTITION_TYPES_EXT, clValueType::cl_char },
			{ CL_DEVICE_AFFINITY_DOMAINS_EXT, clValueType::cl_char },
			{ CL_DEVICE_REFERENCE_COUNT_EXT, clValueType::cl_char },
			{ CL_DEVICE_PARTITION_STYLE_EXT, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_ext_device_fission");
		}
	}
	if (extensionSupported("cl_ext_cxx_for_opencl")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_CXX_FOR_OPENCL_NUMERIC_VERSION_EXT, clValueType::cl_char, utils::displayText },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_ext_cxx_for_opencl");
		}
	}

	// ARM
	/*
	* @todo: 
	* cl_arm_job_slot_selection
	*/

	if (extensionSupported("cl_arm_shared_virtual_memory")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_SVM_CAPABILITIES_ARM, clValueType::cl_device_svm_capabilities, utils::displayDeviceSvmCapabilities },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_arm_shared_virtual_memory");
		}
	}
	if (extensionSupported("cl_arm_get_core_id")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_COMPUTE_UNITS_BITFIELD_ARM, clValueType::cl_ulong },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_arm_get_core_id");
		}
	}
	if (extensionSupported("cl_arm_controlled_kernel_termination")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_CONTROLLED_TERMINATION_CAPABILITIES_ARM, clValueType::cl_device_controlled_termination_capabilities_arm, utils::displayControlledTerminationCapabilitiesARM },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_arm_controlled_kernel_termination");
		}
	}
	if (extensionSupported("cl_arm_scheduling_controls")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_SCHEDULING_CONTROLS_CAPABILITIES_ARM, clValueType::cl_device_scheduling_controls_capabilities_arm, utils::displaySchedulingControlsCapabilitiesARM },
			//@todo: int[]
			//{ CL_DEVICE_SUPPORTED_REGISTER_ALLOCATIONS_ARM, clValueType::cl_int[] },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_arm_scheduling_controls");
		}
	}

	// INTEL
	/*
	* @todo:
	* cl_intel_device_partition_by_names
	*/
	if (extensionSupported("cl_intel_advanced_motion_estimation")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_ME_VERSION_INTEL, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_intel_advanced_motion_estimation");
		}
	}
	if (extensionSupported("cl_intel_simultaneous_sharing")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			// @todo
			// { CL_DEVICE_SIMULTANEOUS_INTEROPS_INTEL, clValueType::cl_uint[] }, 
			{ CL_DEVICE_NUM_SIMULTANEOUS_INTEROPS_INTEL, clValueType::cl_uint },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_intel_simultaneous_sharing");
		}
	}
	if (extensionSupported("cl_intel_required_subgroup_size")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_SUB_GROUP_SIZES_INTEL, clValueType::cl_size_t_array, utils::displayNumberArray },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_intel_required_subgroup_size");
		}
	}
	if (extensionSupported("cl_intel_planar_yuv")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_PLANAR_YUV_MAX_WIDTH_INTEL, clValueType::cl_size_t },
			{ CL_DEVICE_PLANAR_YUV_MAX_HEIGHT_INTEL, clValueType::cl_size_t },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_intel_planar_yuv");
		}
	}
	if (extensionSupported("cl_intel_device_side_avc_motion_estimation")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_AVC_ME_VERSION_INTEL, clValueType::cl_uint },
			{ CL_DEVICE_AVC_ME_SUPPORTS_TEXTURE_SAMPLER_USE_INTEL, clValueType::cl_bool },
			{ CL_DEVICE_AVC_ME_SUPPORTS_PREEMPTION_INTEL, clValueType::cl_bool },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_intel_device_side_avc_motion_estimation");
		}
	}
	/* @todo: Extension is still in draft and not yet released
	if (extensionSupported("cl_intel_unified_shared_memory")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_HOST_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
			{ CL_DEVICE_DEVICE_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
			{ CL_DEVICE_SINGLE_DEVICE_SHARED_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
			{ CL_DEVICE_CROSS_DEVICE_SHARED_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
			{ CL_DEVICE_SHARED_SYSTEM_MEM_CAPABILITIES_INTEL, clValueType::cl_char },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_intel_unified_shared_memory");
		}
	}
	*/
	if (extensionSupported("cl_intel_command_queue_families")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			// @todo
			//{ CL_DEVICE_QUEUE_FAMILY_PROPERTIES_INTEL, clValueType::cl_queue_family_properties_intel },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_intel_command_queue_families");
		}
	}

	// QCOM
	if (extensionSupported("cl_qcom_ext_host_ptr")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_EXT_MEM_PADDING_IN_BYTES_QCOM, clValueType::cl_size_t, utils::displayByteSize },
			{ CL_DEVICE_PAGE_SIZE_QCOM, clValueType::cl_size_t, utils::displayByteSize},
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_qcom_ext_host_ptr");
		}
	}

	// AMD
	// @todo: types
	if (extensionSupported("cl_amd_device_attribute_query")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
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
			readDeviceInfoValue(info, "cl_amd_device_attribute_query");
		}
	}

	// NV
	if (extensionSupported("cl_nv_device_attribute_query")) {
		std::vector<DeviceInfoValueDescriptor> infoList = {
			{ CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, clValueType::cl_uint },
			{ CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, clValueType::cl_uint },
			{ CL_DEVICE_REGISTERS_PER_BLOCK_NV, clValueType::cl_uint },
			{ CL_DEVICE_WARP_SIZE_NV, clValueType::cl_uint },
			{ CL_DEVICE_GPU_OVERLAP_NV, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV, clValueType::cl_bool, utils::displayBool },
			{ CL_DEVICE_INTEGRATED_MEMORY_NV, clValueType::cl_bool, utils::displayBool },
		};
		for (auto info : infoList) {
			readDeviceInfoValue(info, "cl_nv_device_attribute_query");
		}
	}
}

void DeviceInfo::readSupportedImageFormats()
{
	// @todo: check version support
	std::vector<cl_mem_object_type> imageTypeList = {
		CL_MEM_OBJECT_IMAGE2D,
		CL_MEM_OBJECT_IMAGE3D,
		// CL_VERSION_1_2
		CL_MEM_OBJECT_IMAGE2D_ARRAY,
		CL_MEM_OBJECT_IMAGE1D,
		CL_MEM_OBJECT_IMAGE1D_ARRAY,
		CL_MEM_OBJECT_IMAGE1D_BUFFER,
	};
	// 
	std::vector<cl_mem_flags> memFlagList = {
		CL_MEM_READ_WRITE,
		CL_MEM_READ_ONLY,
		CL_MEM_WRITE_ONLY,
		CL_MEM_KERNEL_READ_AND_WRITE
	};

	// Device has to support images, so we need to check the device info first	
	cl_bool imageSupport = false;
	clGetDeviceInfo(deviceId, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, nullptr);
	if (imageSupport) {
		cl_int error;
		cl_context context = clCreateContext(nullptr, 1, &this->deviceId, nullptr, nullptr, &error);
		if (error == NULL) {
			for (auto& imgType : imageTypeList) {
				for (auto& memFlag : memFlagList) {
					cl_uint numSupportedFormats;
					clGetSupportedImageFormats(context, memFlag, imgType, NULL, nullptr, &numSupportedFormats);
					std::vector<cl_image_format> imageFormats(numSupportedFormats);
					clGetSupportedImageFormats(context, memFlag, imgType, numSupportedFormats, imageFormats.data(), nullptr);
					for (auto& imageFormat : imageFormats) {
						imageTypes[imgType].channelOrders[imageFormat.image_channel_order].channelTypes[imageFormat.image_channel_data_type].addFlag(memFlag);
					}
				}
			}
			clReleaseContext(context);
		}
	}

}

DeviceInfo::DeviceInfo()
{
}

void DeviceInfo::read()
{
	readOpenCLVersion();
	readDeviceIdentifier();
	readDeviceInfo();
	readExtensions();
	readExtensionInfo();
	readSupportedImageFormats();
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

	// Device identfication
	// Used by the database to uniquely identify the device
	QJsonObject jsonDeviceIdentifier;
	jsonDeviceIdentifier["devicename"] = identifier.name;
	jsonDeviceIdentifier["gpuname"] = identifier.gpuName;
	jsonDeviceIdentifier["deviceversion"] = identifier.deviceVersion;
	jsonDeviceIdentifier["driverversion"] = identifier.driverVersion;
	jsonRoot["identifier"] = jsonDeviceIdentifier;

	// Device info
	QJsonArray jsonDeviceInfos;
	for (auto& info : deviceInfo)
	{
		QJsonObject jsonNode;
		jsonNode["name"] = info.name;
		jsonNode["extension"] = info.extension;
		jsonNode["enumvalue"] = info.enumValue;
		jsonNode["value"] = info.value.toJsonValue();
		if (info.detailValues.size() > 0) {
			QJsonArray jsonDetails;
			for (auto& detail : info.detailValues) {
				QJsonObject jsonNodeDetail;
				jsonNodeDetail["name"] = detail.name;
				jsonNodeDetail["value"] = detail.value.toJsonValue();
				jsonDetails.append(jsonNodeDetail);
			}
			jsonNode["details"] = jsonDetails;
		}
		jsonDeviceInfos.append(jsonNode);
	}
	jsonRoot["info"] = jsonDeviceInfos;

	// Supported image formats
	// @todo
	QJsonArray jsonImages;
	int cnt = 0;
	for (auto& imageType : imageTypes) 
	{
		for (auto& channelOrder : imageType.second.channelOrders) 
		{
			for (auto& channelType : channelOrder.second.channelTypes) 
			{
				QJsonObject jsonNode;
				jsonNode["type"] = QJsonValue(int(imageType.first));
				jsonNode["channelorder"] = QJsonValue(int(channelOrder.first));
				jsonNode["channeltype"] = QJsonValue(int(channelType.first));
				jsonNode["flags"] = QJsonValue(int(channelType.second.memFlags));
				jsonImages.append(jsonNode);
				cnt++;
			}
		}
	}
	jsonRoot["imageformats"] = jsonImages;

	// Additional OpenCL info
	QJsonObject jsonDeviceInfosOpenCL;
	jsonDeviceInfosOpenCL["versionmajor"] = clVersionMajor;
	jsonDeviceInfosOpenCL["versionminor"] = clVersionMinor;
	jsonRoot["opencl"] = jsonDeviceInfosOpenCL;

	return jsonRoot;
}

void DeviceInfo::readExtensions()
{
	extensions.clear();
	if (clVersionMajor >= 3) {
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
	} else {
		size_t extStrSize;
		clGetDeviceInfo(this->deviceId, CL_DEVICE_EXTENSIONS, 0, nullptr, &extStrSize);
		std::string extensionString;
		extensionString.resize(extStrSize);
		clGetDeviceInfo(this->deviceId, CL_DEVICE_EXTENSIONS, extStrSize, &extensionString[0], nullptr);
		std::vector<std::string> extensions;
		extensions = utils::explode(extensionString, ' ');
		for (size_t i = 0; i < extensions.size(); i++) {
			DeviceExtension extension{};
			extension.name = QString::fromStdString(extensions[i]);
			// @todo
			extension.version = 0;
			this->extensions.push_back(extension);
		}
	}
}

DeviceInfoValue::DeviceInfoValue(cl_device_info info, QVariant value, QString extension, DisplayFn displayFunction)
{
	this->name = utils::deviceInfoString(info);
	this->value = value;
	this->extension = extension;
	this->enumValue = info;
	this->displayFunction = displayFunction;
}

void DeviceInfoValue::addDetailValue(QString name, QVariant value, DisplayFn displayFunction)
{
	detailValues.push_back(DeviceInfoValueDetailValue(name, value, displayFunction));
}

QString DeviceInfoValue::getDisplayValue()
{
	if (displayFunction) {
		return displayFunction(value);
	} else {
		return value.toString();
	}
}

DeviceInfoValueDescriptor::DeviceInfoValueDescriptor()
{
	this->name = 0;
	this->valueType = clValueType::cl_char;
	this->displayFunction = nullptr;
}

DeviceInfoValueDescriptor::DeviceInfoValueDescriptor(cl_device_info name, clValueType valueType, DisplayFn displayFunction)
{
	this->name = name;
	this->valueType = valueType;
	// @todo: nullptr or utils::displayDefault?
	//this->displayFunction = displayFunction ? displayFunction : utils::displayDefault;
	this->displayFunction = displayFunction;
}

DeviceInfoValueDetailValue::DeviceInfoValueDetailValue(QString name, QVariant value, DisplayFn displayFunction)
{
	this->name = name;
	this->value = value;
	this->displayFunction = displayFunction;
}

QString DeviceInfoValueDetailValue::getDisplayValue()
{
	if (displayFunction) {
		return displayFunction(value);
	} else {
		return value.toString();
	}
}
