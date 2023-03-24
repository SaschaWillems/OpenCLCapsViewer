/*
*
* OpenCL hardware capability viewer
*
* Copyright (C) 2021-2023 by Sascha Willems (www.saschawillems.de)
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

#include "operatingsystem.h"

OperatingSystem operatingSystem{};

void getOperatingSystem()
{
    operatingSystem.name = QSysInfo::productType();
    operatingSystem.architecture = QSysInfo::buildCpuArchitecture();
    operatingSystem.version = QSysInfo::productVersion();
    operatingSystem.kernelVersion = QSysInfo::kernelVersion();
    operatingSystem.type = -1;
#if defined(_WIN32)
    operatingSystem.type = 0;
    // Windows 11 can only be detected by the build version
    HMODULE hModule = LoadLibrary(TEXT("ntdll.dll"));
    if (hModule) {
        typedef NTSTATUS(WINAPI* RtlGetVersionFN)(PRTL_OSVERSIONINFOW);
        RtlGetVersionFN RtlGetVersion = reinterpret_cast<RtlGetVersionFN>(GetProcAddress(hModule, "RtlGetVersion"));
        if (RtlGetVersion) {
            RTL_OSVERSIONINFOW osVersionInfo = { 0 };
            osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
            if (RtlGetVersion(&osVersionInfo) == S_OK) {
                if (osVersionInfo.dwBuildNumber >= 22000) {
                    operatingSystem.version = "11";
                }
            }
}
        FreeLibrary(hModule);
    }
#elif defined(__ANDROID__)
    operatingSystem.type = 2;
#elif defined(__linux__)
    operatingSystem.type = 1;
#elif __APPLE__
    // @todo: dinstinguish between macos and ios
#endif
}