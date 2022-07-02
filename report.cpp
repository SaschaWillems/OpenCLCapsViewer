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

#include "report.h"

void Report::toJson(DeviceInfo& device, QString submitter, QString comment, QJsonObject& jsonObject)
{
    // Environment
    QJsonObject jsonEnv;
    jsonEnv["name"] = operatingSystem.name;
    jsonEnv["version"] = operatingSystem.version;
    jsonEnv["architecture"] = operatingSystem.architecture;
    jsonEnv["type"] = operatingSystem.type;
    jsonEnv["submitter"] = submitter;
    jsonEnv["comment"] = comment;
    jsonEnv["reportversion"] = reportVersion;
    jsonEnv["appversion"] = appVersion;
    jsonObject["environment"] = jsonEnv;
    // Platform
    jsonObject["platform"] = device.platform->toJson();
    // Device
    jsonObject["device"] = device.toJson();
}

void Report::saveToFile(DeviceInfo& device, QString fileName, QString submitter, QString comment)
{
    QJsonObject jsonReport;
    toJson(device, submitter, comment, jsonReport);
    QJsonDocument doc(jsonReport);
    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(doc.toJson(QJsonDocument::Indented));
    jsonFile.flush();
}

int Report::uploadNonVisual(DeviceInfo& device, QString submitter, QString comment)
{
    QString message;
    bool dbstatus = database.checkServerConnection(message);
    if (!dbstatus)
    {
#ifndef GUI_BUILD
        std::cout << "Database unreachable\n";
#endif
        qWarning() << "Database unreachable";
        return -1;
    }

    QJsonObject reportJson;
    toJson(device, submitter, comment, reportJson);
    int reportId;
    if (!database.getReportId(reportJson, reportId)) {
#ifndef GUI_BUILD
        std::cout << "Database unreachable\n";
#endif
        qWarning() << "Could not get report id from database";
        return -2;
    }

    if (reportId > -1)
    {
#ifndef GUI_BUILD
        std::cout << "Device already present in database\n";
#endif
        qWarning() << "Device already present in database";
        return -3;
    }

    if (database.uploadReport(reportJson, message))
    {
#ifndef GUI_BUILD
        std::cout << "Report successfully submitted. Thanks for your contribution!\n";
#endif
        qInfo() << "Report successfully submitted. Thanks for your contribution!";
        return 0;
    }
    else
    {
#ifndef GUI_BUILD
        std::cout << "The report could not be uploaded\n";
#endif
        qInfo() << "The report could not be uploaded : \n" << message;
        return -4;
    }
}
