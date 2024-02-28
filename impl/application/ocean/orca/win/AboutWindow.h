// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_ABOUTWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_ABOUTWINDOW_H

#include "application/ocean/orca/win/OrcaWin.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

/**
 * This class implements an about window.
 * @ingroup orcawin
 */
class AboutWindow : public CDialogEx
{
	public:

		/**
		 * Creates a new about window.
		 */
		AboutWindow();

	private:

		/**
		 * Runtime event message function declaration.
		 */
		DECLARE_MESSAGE_MAP();
};

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_ABOUTWINDOW_H
