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

#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

#include <QString>
#include <QSysInfo>
#ifdef _WIN32
#include <windows.h>
#endif

struct OperatingSystem
{
    QString name;
    QString version;
    QString architecture;
    QString kernelVersion;
    int type;
};

extern OperatingSystem operatingSystem;

void getOperatingSystem();

#endif