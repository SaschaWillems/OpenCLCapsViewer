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

#include "displayutils.h"

namespace utils
{
    QString implode(QList<QString>& items, char separator = ',')
    {
        QString imploded;
        for (int i = 0; i < items.size(); i++) {
            imploded += items[i];
            if (i < items.size() - 1) {
                imploded += separator;
                imploded += ' ';
            }
        }
        return "[" + imploded + "]";
    }

    QString displayFlags(uint32_t value, std::unordered_map<uint32_t, QString>& flags) {
        QList<QString> list;
        for (auto flag : flags) {
            if (value & flag.first) {
                list.append(flag.second);
            }
        }
        return implode(list);
    }

    QString displayDefault(QVariant value)
    {
        return value.toString();
    }

    QString displayBool(QVariant value)
    {
        return value.toBool() ? "true" : "false";
    }

    QString displayNumberArray(QVariant value)
    {
        QList<QVariant> list = value.toList();
        QString imploded;
        for (size_t i = 0; i < list.size(); i++) {
            imploded += list[i].toString();
            if (i < list.size() - 1)
                imploded += ", ";
        }
        return "[" + imploded + "]";
    }

    QString displayByteSize(QVariant value)
    {
        return QString("%L1 bytes").arg(value.toUInt());
    }

    QString displayDeviceType(QVariant value)
    {
        switch (value.toInt())
        {
#define STR(r) case r: return #r
            STR(CL_DEVICE_TYPE_DEFAULT);
            STR(CL_DEVICE_TYPE_CPU);
            STR(CL_DEVICE_TYPE_GPU);
            STR(CL_DEVICE_TYPE_ACCELERATOR);
            STR(CL_DEVICE_TYPE_CUSTOM);
#undef STR
        default: return "unknown";
        }
    }

    QString displayVersion(QVariant value)
    {
        cl_version version = static_cast<cl_version>(value.toUInt());
        QString dv = QString("%1.%2.%3").arg(CL_VERSION_MAJOR(version)).arg(CL_VERSION_MINOR(version)).arg(CL_VERSION_PATCH(version));
        return dv;
    }

    QString displayAtomicCapabilities(QVariant value) {
        std::unordered_map<uint32_t, QString> flags = {
            { CL_DEVICE_ATOMIC_ORDER_RELAXED, "ATOMIC_ORDER_RELAXED" },
            { CL_DEVICE_ATOMIC_ORDER_ACQ_REL, "ATOMIC_ORDER_ACQ_REL" },
            { CL_DEVICE_ATOMIC_ORDER_SEQ_CST, "ATOMIC_ORDER_SEQ_CST" },
            { CL_DEVICE_ATOMIC_SCOPE_WORK_ITEM, "ATOMIC_SCOPE_WORK_ITEM" },
            { CL_DEVICE_ATOMIC_SCOPE_WORK_GROUP, "ATOMIC_SCOPE_WORK_GROUP" },
            { CL_DEVICE_ATOMIC_SCOPE_DEVICE, "ATOMIC_SCOPE_DEVICE" },
            { CL_DEVICE_ATOMIC_SCOPE_ALL_DEVICES, "ATOMIC_SCOPE_ALL_DEVICES" },
        };
        return displayFlags(value.toInt(), flags);
    }

    QString displayMemCacheType(QVariant value)
    {
        switch (value.toInt())
            {
#define STR(r) case r: return #r
                STR(CL_NONE);
                STR(CL_READ_ONLY_CACHE);
                STR(CL_READ_WRITE_CACHE);
#undef STR
        default: return "unknown";
        }
    }

    QString displayLocalMemType(QVariant value)
    {
        switch (value.toInt())
        {
#define STR(r) case r: return #r
            STR(CL_NONE);
            STR(CL_LOCAL);
            STR(CL_GLOBAL);
#undef STR
        default: return "unknown";
        }
    }

    QString displayFloatingPointConfig(QVariant value)
    {
        std::unordered_map<uint32_t, QString> flags = {
            { CL_FP_INF_NAN, "INF_NAN" },
            { CL_FP_ROUND_TO_NEAREST, "ROUND_TO_NEAREST" },
            { CL_FP_ROUND_TO_ZERO, "ROUND_TO_ZERO" },
            { CL_FP_ROUND_TO_INF, "ROUND_TO_INF" },
            { CL_FP_FMA, "FMA" },
            // 1.1
            { CL_FP_SOFT_FLOAT, "SOFT_FLOAT" },
            // 1.2
            { CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT, "CORRECTLY_ROUNDED_DIVIDE_SQRT" },
        };
        return displayFlags(value.toInt(), flags);
    }

    QString displayExecCapabilities(QVariant value)
    {
        std::unordered_map<uint32_t, QString> flags = {
            { CL_EXEC_KERNEL, "KERNEL" },
            { CL_EXEC_NATIVE_KERNEL, "NATIVE_KERNEL" },
        };
        return displayFlags(value.toInt(), flags);
    }

    QString displayEnqueueCapabilities(QVariant value)
    {
        std::unordered_map<uint32_t, QString> flags = {
            { CL_DEVICE_QUEUE_SUPPORTED, "QUEUE_SUPPORTED" },
            { CL_DEVICE_QUEUE_REPLACEABLE_DEFAULT, "QUEUE_REPLACEABLE_DEFAULT" },
        };
        return displayFlags(value.toInt(), flags);
    }

    QString displayCommandQueueCapabilities(QVariant value)
    {
        std::unordered_map<uint32_t, QString> flags = {
            { CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, "QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE" },
            { CL_QUEUE_PROFILING_ENABLE, "QUEUE_PROFILING_ENABLE" },
        };
        return displayFlags(value.toInt(), flags);
    }

    QString displayDeviceSvmCapabilities(QVariant value)
    {
        std::unordered_map<uint32_t, QString> flags = {
            { CL_DEVICE_SVM_COARSE_GRAIN_BUFFER, "SVM_COARSE_GRAIN_BUFFER"},
            { CL_DEVICE_SVM_FINE_GRAIN_BUFFER, "SVM_FINE_GRAIN_BUFFER" },
            { CL_DEVICE_SVM_FINE_GRAIN_SYSTEM, "SVM_FINE_GRAIN_SYSTEM" },
            { CL_DEVICE_SVM_ATOMICS, "SVM_ATOMICS" }
        };
        return displayFlags(value.toInt(), flags);
    }

    QString displayControlledTerminationCapabilitiesARM(QVariant value)
    {
        std::unordered_map<uint32_t, QString> flags = {
            { CL_DEVICE_CONTROLLED_TERMINATION_SUCCESS_ARM, "CONTROLLED_TERMINATION_SUCCESS_ARM"},
            { CL_DEVICE_CONTROLLED_TERMINATION_FAILURE_ARM, "CONTROLLED_TERMINATION_FAILURE_ARM" },
            { CL_DEVICE_CONTROLLED_TERMINATION_QUERY_ARM, "CONTROLLED_TERMINATION_QUERY_ARM" },
        };
        return displayFlags(value.toInt(), flags);
    }

    QString displaySchedulingControlsCapabilitiesARM(QVariant value)
    {
        std::unordered_map<uint32_t, QString> flags = {
            { CL_DEVICE_SCHEDULING_KERNEL_BATCHING_ARM, "CSCHEDULING_KERNEL_BATCHING_ARM"},
            { CL_DEVICE_SCHEDULING_WORKGROUP_BATCH_SIZE_ARM, "SCHEDULING_WORKGROUP_BATCH_SIZE_ARM" },
            { CL_DEVICE_SCHEDULING_WORKGROUP_BATCH_SIZE_MODIFIER_ARM, "SCHEDULING_WORKGROUP_BATCH_SIZE_MODIFIER_ARM" },
            { CL_DEVICE_SCHEDULING_DEFERRED_FLUSH_ARM, "SCHEDULING_DEFERRED_FLUSH_ARM" },
            { CL_DEVICE_SCHEDULING_REGISTER_ALLOCATION_ARM, "SCHEDULING_REGISTER_ALLOCATION_ARM" },
        };
        return displayFlags(value.toInt(), flags);
    }

}