/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_FILE_DIALOG_H
#define META_OCEAN_PLATFORM_WIN_FILE_DIALOG_H

#include "ocean/platform/win/Win.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements capabilities to open file dialogs.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT FileDialog
{
	public:

		/**
		 * Shows a file dialog allowing to open a file.
		 * @param filePath The resulting file the user has selected
		 * @return True, if the user has selected a file
		 * @see saveFile().
		 */
		static bool openFile(std::wstring& filePath);

		/**
		 * Shows a file dialog allowing to save a file.
		 * @param filePath The resulting file the user has defined
		 * @return True, if the user has defined a file
		 * @see openFile().
		 */
		static bool saveFile(std::wstring& filePath);
};

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_FILE_DIALOG_H
