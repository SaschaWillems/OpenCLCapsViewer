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

#include "database.h"

QString Database::username = "";
QString Database::password = "";
QString Database::databaseUrl = "http://opencl.gpuinfo.org/";

void Database::setCredentials(QUrl& url)
{
	if ((username != "") && (password != ""))
	{
		url.setUserName(username);
		url.setPassword(password);
	}
}

QString Database::get(QString url)
{
	manager = new QNetworkAccessManager(NULL);
	QUrl qurl(url);
	setCredentials(qurl);
	QNetworkReply* reply = manager->get(QNetworkRequest(qurl));
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray bytes = reply->readAll();
		QString replyStr(bytes);
		delete(manager);
		return replyStr;
	}
	else
	{
		QString err;
		err = reply->errorString();
		delete(manager);
		return "";
	}
}

QString Database::encodeUrl(QString url)
{
	// @todo: probably not required
	QString urlStr(url);
	urlStr.replace("+", "%2B");
	return urlStr;
}

bool Database::getReportId(QJsonObject json, int& id)
{
	manager = new QNetworkAccessManager(nullptr);
	QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QHttpPart httpPart;
	httpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"data\"; filename=\"update_check_report.json\""));
	QJsonDocument doc(json);
	httpPart.setBody(doc.toJson());
	multiPart->append(httpPart);
	QUrl qurl(databaseUrl + "api/v1/getreportid.php");
	QNetworkRequest request(qurl);
	QNetworkReply* reply = manager->post(request, multiPart);
	multiPart->setParent(reply);
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	bool result = false;
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray bytes = reply->readAll();
		QString replyStr(bytes);
		if (!replyStr.isEmpty()) {
			id = replyStr.toInt();
			result = true;
		}
	}
	delete(manager);
	return result;
}

bool Database::getReportState(QJsonObject json, ReportState& state)
{
	manager = new QNetworkAccessManager(nullptr);
	QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QHttpPart httpPart;
	httpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"data\"; filename=\"update_check_report.json\""));
	QJsonDocument doc(json);
	httpPart.setBody(doc.toJson());
	multiPart->append(httpPart);
	QUrl qurl(databaseUrl + "api/v1/getreportstate.php");
	QNetworkRequest request(qurl);
	QNetworkReply* reply = manager->post(request, multiPart);
	multiPart->setParent(reply);
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	bool result = false;
	state = ReportState::unknown;
	if (reply->error() == QNetworkReply::NoError)
	{
		QString message = reply->readAll();
		if (message == "report_present") {
			state = ReportState::is_present;
		}
		if (message == "report_not_present") {
			state = ReportState::not_present;
		}
		if (message == "report_updatable") {
			state = ReportState::is_updatable;
		}
		result = true;
    } else {
        QString message = reply->errorString();
        QMessageBox::warning(nullptr, tr("Error"), "Could not get report state: " + message);
    }
	delete(manager);
	return result;
}

bool Database::uploadReport(QJsonObject json, QString &message)
{
	manager = new QNetworkAccessManager(nullptr);
	QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QHttpPart httpPart;
	httpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"data\"; filename=\"openclreport.json\""));
	QJsonDocument doc(json);
	httpPart.setBody(doc.toJson());
	multiPart->append(httpPart);
	QUrl qurl(databaseUrl + "api/v1/uploadreport.php");
	QNetworkRequest request(qurl);
	QNetworkReply* reply = manager->post(request, multiPart);
	multiPart->setParent(reply);
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	bool result = false;
	if (reply->error() == QNetworkReply::NoError)
	{
		result = true;
	} else {
		message = reply->errorString();
		result = false;
	}
	delete(manager);
	return result;
}

bool Database::checkServerConnection(QString& message)
{
	manager = new QNetworkAccessManager(nullptr);
	QUrl qurl(databaseUrl + "api/v1/serverstate.php");
	setCredentials(qurl);
	QNetworkReply* reply = manager->get(QNetworkRequest(qurl));
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	message = reply->errorString();
	return (reply->error() == QNetworkReply::NoError);
}
