// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
		void SetBitmap(const wxBitmap& bitmap) override;

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
