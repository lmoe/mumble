// Copyright 2005-2016 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "mumble_pch.hpp"

#include "SocketRPC.h"

#include "Channel.h"
#include "ClientUser.h"
#include "Global.h"
#include "MainWindow.h"
#include "ServerHandler.h"
#include <QJsonDocument>

SocketRPCClient::SocketRPCClient(QLocalSocket *s, QObject *p) : QObject(p), qlsSocket(s), qbBuffer(NULL) {
	qlsSocket->setParent(this);

	connect(qlsSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(qlsSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(error(QLocalSocket::LocalSocketError)));
	connect(qlsSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));

	qbBuffer = new QBuffer(&qbaOutput, this);
	qbBuffer->open(QIODevice::WriteOnly);
}

void SocketRPCClient::disconnected() {
	deleteLater();
}

void SocketRPCClient::error(QLocalSocket::LocalSocketError) {
}

void SocketRPCClient::readyRead() {

    const QByteArray& result = qlsSocket->readLine();
    QJsonParseError* error = new QJsonParseError();

    const QJsonDocument& parsedRequest = QJsonDocument::fromJson(result, error);

    if (error->error == QJsonParseError::NoError) {
        qlsSocket->write("{\"ok\": true, \"error\": false}");
    } else {
        qlsSocket->write("{\"ok\": false, \"error\": true}");
    }
}

void SocketRPCClient::processRequest(const QJsonDocument& request) {

}

SocketRPC::SocketRPC(const QString &basename, QObject *p) : QObject(p) {
	qlsServer = new QLocalServer(this);

	QString pipepath;

#ifdef Q_OS_WIN
	pipepath = basename;
#else
	{
		QString xdgRuntimePath = QProcessEnvironment::systemEnvironment().value(QLatin1String("XDG_RUNTIME_DIR"));
		QDir xdgRuntimeDir = QDir(xdgRuntimePath);

		if (! xdgRuntimePath.isNull() && xdgRuntimeDir.exists()) {
			pipepath = xdgRuntimeDir.absoluteFilePath(basename + QLatin1String("Socket"));
		} else {
			pipepath = QDir::home().absoluteFilePath(QLatin1String(".") + basename + QLatin1String("Socket"));
		}
	}

	{
		QFile f(pipepath);
		if (f.exists()) {
			qWarning() << "SocketRPC: Removing old socket on" << pipepath;
			f.remove();
		}
	}
#endif

	if (! qlsServer->listen(pipepath)) {
		qWarning() << "SocketRPC: Listen failed";
		delete qlsServer;
		qlsServer = NULL;
	} else {
		connect(qlsServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	}
}

void SocketRPC::newConnection() {
	while (true) {
		QLocalSocket *qls = qlsServer->nextPendingConnection();
		if (! qls)
			break;
		new SocketRPCClient(qls, this);
	}
}
