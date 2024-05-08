/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_WX_STATIC_BITMAP_H
#define META_OCEAN_PLATFORM_WXWIDGETS_WX_STATIC_BITMAP_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements a static bitmap.
 * @ingroup platformwxwidgets
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT WxStaticBitmap : public wxStaticBitmap
{
	public:

		/**
		 * Creates a new static bitmap.
		 * @param parent The parent window, may be nullptr
		 * @param id The id of the new static bitmap
		 * @param bitmap The bitmap of the new static bitmap
		 * @param pos Horizontal and vertical position of the new static bitmap, in pixel with range (-infinity, infinity)x(-infinity, infinity)
		 * @param size The width and height of the new static bitmap, in pixel with range [0, infinity)x[0, infinity)
		 * @param style The style of the new static bitmap
		 * @param name The optional name of the new static bitmap
		 */
		WxStaticBitmap(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxStaticBitmapNameStr);

		/**
		 * Sets the bitmap of this static bitmap.
		 * @param bitmap The bitmap to set
		 */
#if wxMAJOR_VERSION >= 3 && wxMINOR_VERSION >= 2 && wxRELEASE_NUMBER >= 4
		void SetBitmap(const wxBitmapBundle& bitmap) override;
#else
		void SetBitmap(const wxBitmap& bitmap) override;
#endif
		/**
		 * Returns false to not use a transparent background.
		 * @return Always false
		 */
		bool HasTransparentBackground() override;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_WX_STATIC_BITMAP_H
