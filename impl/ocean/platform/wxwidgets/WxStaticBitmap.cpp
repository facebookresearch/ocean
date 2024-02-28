// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/wxwidgets/WxStaticBitmap.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

WxStaticBitmap::WxStaticBitmap(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
	wxStaticBitmap(parent, id, bitmap, pos, size, style, name)
{
#if defined(wxABI_VERSION) && wxABI_VERSION > 20899

	// For WxWidgets verision >= 2.9, scaled bitmaps are possible and the min size of the static bitmap is set to the none scaled bitmap size
	// Therefore we must explicitly set the min size to the scaled bitmap size
	if (GetBitmap().IsOk())
	{
		SetMinSize(wxSize(GetBitmap().GetScaledWidth(), GetBitmap().GetScaledHeight()));
	}

#endif
}

void WxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
	wxStaticBitmap::SetBitmap(bitmap);

#if defined(wxABI_VERSION) && wxABI_VERSION > 20899

	// For WxWidgets verision >= 2.9, scaled bitmaps are possible and the min size of the static bitmap is set to the none scaled bitmap size
	// Therefore we must explicitly set the min size to the scaled bitmap size
	if (GetBitmap().IsOk())
	{
		SetMinSize(wxSize(GetBitmap().GetScaledWidth(), GetBitmap().GetScaledHeight()));
	}

#endif
}

bool WxStaticBitmap::HasTransparentBackground()
{
	return false;
}

}

}

}
