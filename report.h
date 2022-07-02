/*
*
* OpenCL hardware capability viewer
*
* Copyright (C) 2021-2022 by Sascha Willems (www.saschawillems.de)
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

#ifndef REPORT_H
#define REPORT_H

#include <QFile>
#include <iostream>
#include "deviceinfo.h"
#include "operatingsystem.h"
#include "appinfo.h"
#include "database.h"

class Report {
public:
	void toJson(DeviceInfo& device, QString submitter, QString comment, QJsonObject& jsonObject);
	void saveToFile(DeviceInfo& device, QString fileName, QString submitter, QString comment);
	int uploadNonVisual(DeviceInfo& device, QString submitter, QString comment);
};

#endif
