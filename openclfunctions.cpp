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
