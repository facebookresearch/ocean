// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_QT_LOG_DIALOG_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_QT_LOG_DIALOG_H

#include "application/ocean/xrplayground/common/XRPlaygroundCommon.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>

namespace Ocean
{

namespace XRPlayground
{

namespace QT
{

/**
 * This class implements the log dialog of the XRPlayground app.
 * The log dialog handles retrieving and displaying logs.
 * @ingroup xrplaygroundqt
 */
class LogDialog : public QDialog
{
	public:

		/**
		 * Creates a new log dialog.
		 */
 		explicit LogDialog(QWidget *parent = 0);

		/**
		 * Destructs this log dialog and releases all resources.
		 */
		~LogDialog() override;

	protected:

		// The timer for pulling messages and appending to the log.
		QTimer* logTimer_;
};

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_QT_LOG_DIALOG_H
