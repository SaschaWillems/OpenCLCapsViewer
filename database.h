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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QHttpMultiPart>
#include <QXmlStreamReader>
#include <QJsonObject>
#include <QJsonDocument>
#ifdef GUI_BUILD
#include <QMessageBox>
#endif

#pragma once

enum class ReportState { unknown, not_present, is_present, is_updatable };

class Database : public QObject
{
	Q_OBJECT
private:
	QNetworkProxy* proxy;
	QNetworkAccessManager* manager;
	void setCredentials(QUrl& url);
	QString get(QString url);
public:
	static QString username;
	static QString password;
	static QString databaseUrl;
	bool getReportId(QJsonObject json, int& id);
	bool getReportState(QJsonObject json, ReportState& state);
	bool uploadReport(QJsonObject json, QString& message);
	bool checkServerConnection(QString& message);
};

extern Database database;

