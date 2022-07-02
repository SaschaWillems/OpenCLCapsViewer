## Adding new extensions to the application

### Device extension

#### Extension adds new device info types?

##### Add enum for new types
Add the new types as enum to the `clValueType` enum class in `openclutils.hpp`:

```cpp
enum class clValueType {
    ...
	cl_device_new_type,
};
```

##### Add new value type mapping
Add code to read the value types to the `DeviceInfo::readDeviceInfoValue` function's switch statement in `deviceinfo.cpp`:

```cpp
case clValueType::cl_device_new_type:
{
    cl_device_new_type value;
    _clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_new_type), &value, nullptr);
    deviceInfo.push_back(DeviceInfoValue(descriptor.name, QVariant::fromValue(value), extension, descriptor.displayFunction));
    break;
}
```

If the type is a struct with multiple members, add them as additional info:

```cpp
case clValueType::cl_device_new_type:
{
    cl_device_new_type value;
    _clGetDeviceInfo(this->deviceId, descriptor.name, sizeof(cl_device_new_type), &value, nullptr);
    DeviceInfoValue infoValue(descriptor.name, QVariant(), extension, descriptor.displayFunction);
    infoValue.addDetailValue("member_a", value.member_a);
    infoValue.addDetailValue("member_b", value.member_b);
    deviceInfo.push_back(infoValue);
    break;
}
```

##### Add new display functions
If there is no existing displa function for the new type(s), add new functions to `displayutils.cpp` to visualize the values in the application:

```cpp
QString displayNewValueType(QVariant value) {
    ...
}
```

##### Add string representation
To properly display the new device info type name, add a new line to `deviceInfoString`in `openclutils.h`:

```cpp
inline QString deviceInfoString(const cl_device_info info)
{
    switch (info)
    {
#define STR(r) case r: return #r
        ...
        // cl_new_extension
        STR(CL_DEVICE_NEW_IDENTIFIER);
#undef STR
    default: return "?";
    }
}
```

#### Add device info readout
Add code to read the value(s) to the `DeviceInfo::readExtensionInfo` function's switch statement in `deviceinfo.cpp`:

```cpp
if (extensionSupported("cl_new_extension")) {
    std::vector<DeviceInfoValueDescriptor> infoList = {
        { CL_DEVICE_INFO_NEW_IDENTIFIER_A, clValueType::cl_new_value_type_a, utils::displayNewValueTypeA },
        { CL_DEVICE_INFO_NEW_IDENTIFIER_B, clValueType::cl_new_value_type_b, utils::displayNewValueTypeB },
    };
    for (auto info : infoList) {
        readDeviceInfoValue(info, "cl_new_extension");
    }
}
```

### Platform extension

#### Extension adds new device info types?

##### Add enum for new types
Add the new types as enum to the `clValueType` enum class in `openclutils.hpp`:

```cpp
enum class clValueType {
    ...
	cl_device_new_type,
};
```

##### Add new value type mapping
Add code to read the value types to the `PlatformInfo::readPlatformInfoValue` function's switch statement in `platforminfo.cpp`:

```cpp
case clValueType::cl_device_new_type:
{
    cl_device_new_type value;
    _clGetPlatformInfo(this->platformId, info, sizeof(cl_device_new_type), &value, nullptr);
    platformInfo.push_back(PlatformInfoValue(info, QVariant::fromValue(value), extension));
    break;
}
```

##### Add new display functions
If there is no existing displa function for the new type(s), add new functions to `displayutils.cpp` to visualize the values in the application:

```cpp
QString displayNewValueType(QVariant value) {
    ...
}
```

##### Add string representation
To properly display the new device info type name, add a new line to `deviceInfoString`in `openclutils.h`:

```cpp
inline QString deviceInfoString(const cl_device_info info)
{
    switch (info)
    {
#define STR(r) case r: return #r
        ...
        // cl_new_extension
        STR(CL_DEVICE_NEW_IDENTIFIER);
#undef STR
    default: return "?";
    }
}
```

#### Add device info readout
Add code to read the value(s) to the `DeviceInfo::readExtensionInfo` function's switch statement in `deviceinfo.cpp`:

```cpp
if (extensionSupported("cl_new_extension")) {
    std::vector<DeviceInfoValueDescriptor> infoList = {
        { CL_DEVICE_INFO_NEW_IDENTIFIER_A, clValueType::cl_new_value_type_a, utils::displayNewValueTypeA },
        { CL_DEVICE_INFO_NEW_IDENTIFIER_B, clValueType::cl_new_value_type_b, utils::displayNewValueTypeB },
    };
    for (auto info : infoList) {
        readDeviceInfoValue(info, "cl_new_extension");
    }
}
```

**todo**