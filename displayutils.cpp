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

#include "CL/cl.h"
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

}