/*  This file is part of Qwit.

    Copyright (C) 2009 Roopesh Chander <roop@forwardbias.in>

    Qwit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Qwit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Qwit.  If not, see <http://www.gnu.org/licenses/>. */


#include "TwitPicDialog.h"
#include <QVBoxLayout>
#include <QProgressBar>
#include <QDialogButtonBox>
#include <QLabel>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImageReader>
#include <QFile>
#include <QRegExp>
#include <QFileInfo>

#include "Configuration.h"

TwitPicDialog::TwitPicDialog(const QString &fileName, QWidget *parent)
		: QDialog(parent),
		m_fileName(fileName),
		m_pixmapLabel(0),
		m_postButton(0),
		m_progressBar(0)
{
	setWindowTitle("TwitPic");
	QVBoxLayout *layout = new QVBoxLayout;
	m_pixmapLabel = new QLabel(this);
	layout->addWidget(m_pixmapLabel);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
	m_postButton = buttonBox->addButton("Post image", QDialogButtonBox::AcceptRole);
	m_cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
	layout->addWidget(buttonBox, 0, Qt::AlignCenter);
	m_progressBar = new QProgressBar(this);
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(100);
	m_progressBar->setTextVisible(false);
	m_progressBar->hide();
	layout->addWidget(m_progressBar);
	m_errorMsgLabel = new QLabel(this);
	m_errorMsgLabel->hide();
	layout->addWidget(m_errorMsgLabel);
	setLayout(layout);
	m_postButton->setDefault(true);
	m_postButton->setEnabled(false);
	setTwitPixmap(QPixmap(m_fileName));
	connect(m_postButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(&http, SIGNAL(requestStarted(int)), this, SLOT(requestStarted(int)));
	connect(&http, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
	connect(&http, SIGNAL(dataSendProgress(int, int)), this, SLOT(updateProgressBar(int, int)));
}

void TwitPicDialog::setTwitPixmap(const QPixmap &pixmap) {
	QSize size(600, 200);
	if (pixmap.width() > size.width() || pixmap.height() > size.height()) {
		m_pixmapLabel->setPixmap(pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	} else {
		m_pixmapLabel->setPixmap(pixmap);
	}
}

void TwitPicDialog::setUser(const QString &username, const QString &password) {
	m_username = username;
	m_password = password;
	m_postButton->setEnabled(true);
}

void TwitPicDialog::accept() {
	Configuration *config = Configuration::getInstance();
	if (config->useProxy) {
		http.setProxy(config->proxyAddress, config->proxyPort, config->proxyUsername, config->proxyPassword);
	} else {
		http.setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
	}

	m_progressBar->reset();
	m_errorMsgLabel->hide();
	m_progressBar->show();

	QByteArray boundary = "------------------------------her-ad-hoc-person";

	QUrl url("http://twitpic.com/api/upload");

	QHttpRequestHeader header;
	header.setRequest("POST", url.path());
	header.setValue("Host", url.host());
	header.setContentType("multipart/form-data; boundary=" + boundary);

	if(url.toString().indexOf("https") == 0) {
	    http.setHost(url.host(), QHttp::ConnectionModeHttps, url.port(443));
    } else {
        http.setHost(url.host(), QHttp::ConnectionModeHttp, url.port(80));
    }

	QByteArray ba;
	ba.append("--" + boundary + "\r\n");
	ba.append("Content-Disposition: form-data; name=\"media\"; "
	          "filename=\"" + QFileInfo(m_fileName).baseName() + "\"\r\n");
	ba.append("Content-Type: image/" + QImageReader::imageFormat(m_fileName) + "\r\n\r\n");
	QFile file(m_fileName);
	file.open(QIODevice::ReadOnly);
	ba.append(file.readAll());
	ba.append("\r\n");
	ba.append("--" + boundary + "\r\n");
	ba.append("Content-Disposition: form-data; name=\"username\"\r\n\r\n");
	ba.append(m_username + "\r\n");
	ba.append("--" + boundary + "\r\n");
	ba.append("Content-Disposition: form-data; name=\"password\"\r\n\r\n");
	ba.append(m_password + "\r\n");
	ba.append("--" + boundary + "--" + "\r\n");
	header.setContentLength(ba.count());
	
	buffer.open(QIODevice::WriteOnly);
	postRequestId = http.request(header, ba, &buffer);
	m_postButton->setEnabled(false);
}

void TwitPicDialog::updateProgressBar(int sent, int total) {
	m_progressBar->setValue(sent * m_progressBar->maximum() / total);
}

void TwitPicDialog::requestStarted(int id) {
}

void TwitPicDialog::requestFinished(int id, bool error) {
	if (id != postRequestId) {
		return;
	}
	buffer.close();
	// The xml module is too heavy for this (and a pain to use). Managing with QRegExp for now.
	QString xmlReply = buffer.data();
	xmlReply.replace("\r\n", "");
	xmlReply.replace("\n", "");
	QString errorMsg = "";
	QRegExp rx("<rsp stat=\"(\\S+)\">");
	if (rx.indexIn(xmlReply) >= 0) {
		if (rx.capturedTexts().at(1) == "ok") {
			QRegExp rx2("<mediaurl>(\\S+)<\\/mediaurl>");
			if (rx2.indexIn(xmlReply) >= 0) {
				m_twitPickedUrlString = rx2.capturedTexts().at(1);
			}
		} else {
			QRegExp rx("<err .* msg=\"([^\"]*)\"");
			if (rx.indexIn(xmlReply) >= 0) {
				errorMsg = rx.capturedTexts().at(1);
			}
		}
	}
	if (!m_twitPickedUrlString.isEmpty()) {
		QDialog::accept();
	} else if (!errorMsg.isEmpty()) {
		m_errorMsgLabel->setText("TwitPic said: " + errorMsg);
		m_progressBar->hide();
		m_errorMsgLabel->show();
	}
	m_postButton->setEnabled(true);
}

QString TwitPicDialog::twitPickedUrlString() const {
	return m_twitPickedUrlString;
}
