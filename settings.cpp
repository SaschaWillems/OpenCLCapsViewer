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

#include "settings.h"
#include "QNetworkProxy"

Settings::Settings()
{
}


Settings::~Settings()
{
}

void Settings::restore()
{
	QSettings settings("saschawillems", "openclcapsviewer");
	submitterName = settings.value("global/submitterName", "").toString();
	proxyDns = settings.value("proxy/dns", "").toString();
	proxyPort = settings.value("proxy/port", "").toString();
	proxyUserName = settings.value("proxy/user", "").toString();
	proxyUserPassword = settings.value("proxy/password", "").toString();
	proxyEnabled = settings.value("proxy/enabled", "").toBool();

	// Apply proxy settings
	if (proxyEnabled) {
		QNetworkProxy proxy;
		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(proxyDns);
		if (!proxyPort.isEmpty()) {
			proxy.setPort(proxyPort.toInt());
		}
		else {
			proxy.setPort(80);
		}
		proxy.setUser(proxyUserName);
		proxy.setPassword(proxyUserPassword);
		QNetworkProxy::setApplicationProxy(proxy);
	} else {
		QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
	}

}

