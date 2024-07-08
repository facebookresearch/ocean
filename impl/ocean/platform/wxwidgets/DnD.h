/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_D_N_D_H
#define META_OCEAN_PLATFORM_WXWIDGETS_D_N_D_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/base/Callback.h"

#include <wx/dnd.h>

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements a drop target.
 * @ingroup platformwxw
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT FileDropTarget : public wxFileDropTarget
{
	public:

		/**
		 * Definition of a callback function for file drag and drop events.
		 */
		typedef Callback<bool, const std::vector<std::string>&> Callback;

	public:

		/**
		 * Creates a new drop target object.
		 * @param callback File event callback function
		 */
		explicit FileDropTarget(const Callback& callback);

	private:

		/**
		 * Drag and Drop event function for files.
		 * @param x Horizontal mouse position
		 * @param y Vertical mouse position
		 * @param filenames Names of the file to be dropped
		 * @return True, if the files are accepted
		 */
		virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

	private:

		/// Callback event function.
		Callback targetCallback_;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_D_N_D_H
