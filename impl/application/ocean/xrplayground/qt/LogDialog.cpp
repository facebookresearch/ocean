// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/qt/LogDialog.h"

#include "ocean/base/DateTime.h"

#include <QtCore/QTimer>

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPlainTextEdit>

namespace Ocean
{

namespace XRPlayground
{

namespace QT
{

LogDialog::LogDialog(QWidget *parent) :
	QDialog(parent)
{
	setWindowTitle("XRPlayground: Log");

	QHBoxLayout *layout = new QHBoxLayout(this);

	QPlainTextEdit *logText = new QPlainTextEdit();
	logText->setReadOnly(true);
	layout->addWidget(logText);
	resize(800, 400);

	logTimer_ = new QTimer(this);
	connect(logTimer_, &QTimer::timeout, this, [logText](){
		Messenger::MessageType messageType = Messenger::TYPE_UNDEFINED;

		std::string location;
		std::string message;

		size_t remainingMessages = 50;

		while (remainingMessages-- != 0 && Messenger::get().popMessage(messageType, location, message))
		{
			if (messageType == Messenger::TYPE_ERROR)
			{
				message = "Error: " + message;
			}
			else if (messageType == Messenger::TYPE_WARNING)
			{
				message = "Warning: " + message;
			}

			logText->appendPlainText(QString::fromStdString(DateTime::localStringDate() + " " + DateTime::localStringTime(true) + " " + message));
		}
	});
	logTimer_->start(50);
}

LogDialog::~LogDialog()
{
	logTimer_->stop();
}

}

}

}
