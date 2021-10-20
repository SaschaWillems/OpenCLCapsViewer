#pragma once

#ifndef OPENCLFUNCTIONS_H
#define OPENCLFUNCTIONS_H

#include <CL/cl.h>
#include <QDebug>
#if defined(__ANDROID__)
#include <dlfcn.h>
#endif

typedef void (*F_PFN_notify)(const char *, const void *, size_t, void *);
typedef cl_int (*PFN_clGetPlatformIDs) (cl_uint, cl_platform_id *, cl_uint *);
typedef cl_int (*PFN_clGetPlatformInfo) (cl_platform_id, cl_platform_info, size_t, void *, size_t *);
typedef cl_int (*PFN_clGetDeviceIDs) (cl_platform_id, cl_device_type, cl_uint, cl_device_id *, cl_uint *);
typedef cl_int (*PFN_clGetDeviceInfo) (cl_device_id, cl_device_info, size_t, void *, size_t *);
typedef cl_context (*PFN_clCreateContext) (const cl_context_properties *, cl_uint, const cl_device_id *, F_PFN_notify, void *, cl_int *);
typedef cl_int (*PFN_clReleaseContext) (cl_context);
typedef cl_int (*PFN_clGetSupportedImageFormats) (cl_context, cl_mem_flags, cl_mem_object_type, cl_uint, cl_image_format *, cl_uint *);

extern PFN_clGetPlatformIDs _clGetPlatformIDs;
extern PFN_clGetPlatformInfo _clGetPlatformInfo;
extern PFN_clGetDeviceIDs _clGetDeviceIDs;
extern PFN_clGetDeviceInfo _clGetDeviceInfo;
extern PFN_clCreateContext _clCreateContext;
extern PFN_clReleaseContext _clReleaseContext;
extern PFN_clGetSupportedImageFormats _clGetSupportedImageFormats;

void loadFunctionPointers(void *library);

#endif
