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

#include "openclfunctions.h"

PFN_clGetPlatformIDs _clGetPlatformIDs = nullptr;
PFN_clGetPlatformInfo _clGetPlatformInfo = nullptr;
PFN_clGetDeviceIDs _clGetDeviceIDs = nullptr;
PFN_clGetDeviceInfo _clGetDeviceInfo = nullptr;
PFN_clCreateContext _clCreateContext = nullptr;
PFN_clReleaseContext _clReleaseContext = nullptr;
PFN_clGetSupportedImageFormats _clGetSupportedImageFormats = nullptr;

void loadFunctionPointers(void *library)
{
    qInfo() << "Loading OpenCL function pointers";
#if defined(_WIN32)
    _clGetPlatformIDs = reinterpret_cast<PFN_clGetPlatformIDs>(GetProcAddress((HMODULE)library, "clGetPlatformIDs"));
    _clGetPlatformInfo = reinterpret_cast<PFN_clGetPlatformInfo>(GetProcAddress((HMODULE)library, "clGetPlatformInfo"));
    _clGetDeviceIDs = reinterpret_cast<PFN_clGetDeviceIDs>(GetProcAddress((HMODULE)library, "clGetDeviceIDs"));
    _clGetDeviceInfo = reinterpret_cast<PFN_clGetDeviceInfo>(GetProcAddress((HMODULE)library, "clGetDeviceInfo"));
    _clCreateContext = reinterpret_cast<PFN_clCreateContext>(GetProcAddress((HMODULE)library, "clCreateContext"));
    _clReleaseContext = reinterpret_cast<PFN_clReleaseContext>(GetProcAddress((HMODULE)library, "clReleaseContext"));
    _clGetSupportedImageFormats = reinterpret_cast<PFN_clGetSupportedImageFormats>(GetProcAddress((HMODULE)library, "clGetSupportedImageFormats"));
#else
    _clGetPlatformIDs = reinterpret_cast<PFN_clGetPlatformIDs>(dlsym(library, "clGetPlatformIDs"));
    _clGetPlatformInfo = reinterpret_cast<PFN_clGetPlatformInfo>(dlsym(library, "clGetPlatformInfo"));
    _clGetDeviceIDs = reinterpret_cast<PFN_clGetDeviceIDs>(dlsym(library, "clGetDeviceIDs"));
    _clGetDeviceInfo = reinterpret_cast<PFN_clGetDeviceInfo>(dlsym(library, "clGetDeviceInfo"));
    _clCreateContext = reinterpret_cast<PFN_clCreateContext>(dlsym(library, "clCreateContext"));
    _clReleaseContext = reinterpret_cast<PFN_clReleaseContext>(dlsym(library, "clReleaseContext"));
    _clGetSupportedImageFormats = reinterpret_cast<PFN_clGetSupportedImageFormats>(dlsym(library, "clGetSupportedImageFormats"));
#endif
}

bool checkOpenCLAvailability(QString& error)
{
    // Check if OpenCL is supported by trying to load the OpenCL library and getting a valid function pointer
    bool openCLAvailable = false;
    error = "";

#if defined(__ANDROID__)
    // Try to find the OepenCL library on one of the following paths
    static const char* libraryPaths[] = {
        // Generic
        "/system/vendor/lib64/libOpenCL.so",
        "/system/lib64/libOpenCL.so",
        "/system/vendor/lib/libOpenCL.so",
        "/system/lib/libOpenCL.so",
        // ARM Mali
        "/system/vendor/lib64/egl/libGLES_mali.so",
        "/system/lib64/egl/libGLES_mali.so",
        "/system/vendor/lib/egl/libGLES_mali.so",
        "/system/lib/egl/libGLES_mali.so",
        // PowerVR
        "/system/vendor/lib64/libPVROCL.so",
        "/system/lib64/libPVROCL.so",
        "/system/vendor/lib/libPVROCL.so",
        "/system/lib/libPVROCL.so"
    };
    void* libOpenCL = nullptr;
    for (auto libraryPath : libraryPaths) {
        qInfo() << "Trying to load library from" << libraryPath;
        libOpenCL = dlopen(libraryPath, RTLD_LAZY);
        if (libOpenCL) {
            qInfo() << "Found library in" << libraryPath;
            break;
        }
    }
    if (libOpenCL) {
        // OpenCl library loaded, now try to get a function pointer to check if it works
        PFN_clGetPlatformIDs test_fn = reinterpret_cast<PFN_clGetPlatformIDs>(dlsym(libOpenCL, "clGetPlatformIDs"));
        if (test_fn) {
            qInfo() << "Got valid function pointer for clGetPlatformIDs";
            openCLAvailable = true;
            loadFunctionPointers(libOpenCL);
        }
        else {
            error = "Could not get a valid function pointer";
        }
    }
    else {
        error = "Could not find a OpenCL library";
    }
#elif defined(__linux__)
    // Try to find the OepenCL library on one of the following paths
    static const char* libraryPaths[] = {
        "libOpenCL.so",
        "/usr/lib/libOpenCL.so",
        "/usr/local/lib/libOpenCL.so",
        "/usr/local/lib/libpocl.so",
        "/usr/lib64/libOpenCL.so",
        "/usr/lib32/libOpenCL.so",
        "libOpenCL.so.1",
        "/usr/lib/libOpenCL.so.1",
        "/usr/local/lib/libOpenCL.so.1",
        "/usr/local/lib/libpocl.so.1",
        "/usr/lib64/libOpenCL.so.1",
        "/usr/lib32/libOpenCL.so.1"
    };
    void* libOpenCL = nullptr;
    for (auto libraryPath : libraryPaths) {
        qInfo() << "Trying to load library from" << libraryPath;
        libOpenCL = dlopen(libraryPath, RTLD_LAZY);
        if (libOpenCL) {
            qInfo() << "Found library in" << libraryPath;
            break;
        }
    }
    if (libOpenCL) {
        // OpenCl library loaded, now try to get a function pointer to check if it works
        PFN_clGetPlatformIDs test_fn = reinterpret_cast<PFN_clGetPlatformIDs>(dlsym(libOpenCL, "clGetPlatformIDs"));
        if (test_fn) {
            qInfo() << "Got valid function pointer for clGetPlatformIDs";
            openCLAvailable = true;
            loadFunctionPointers(libOpenCL);
        }
        else {
            error = "Could not get a valid function pointer";
        }
    }
    else {
        error = "Could not find a OpenCL library";
    }
#elif defined(_WIN32)
    HMODULE libOpenCL = LoadLibraryA("OpenCL.dll");
    if (libOpenCL) {
        char libPath[MAX_PATH] = { 0 };
        GetModuleFileNameA(libOpenCL, libPath, sizeof(libPath));
        qInfo() << "Found library in" << libPath;
        PFN_clGetPlatformIDs test_fn = reinterpret_cast<PFN_clGetPlatformIDs>(GetProcAddress((HMODULE)libOpenCL, "clGetPlatformIDs"));
        if (test_fn) {
            qInfo() << "Got valid function pointer for clGetPlatformIDs";
            openCLAvailable = true;
            loadFunctionPointers(libOpenCL);
        }
        else {
            error = "Could not get a valid function pointer";
        }
    }
    else {
        error = "Could not find a OpenCL library";
    }
#endif
    if (error != "") {
        qCritical() << error;
    }
    return openCLAvailable;
}
