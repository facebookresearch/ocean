/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/ImageList.h"
#include "ocean/platform/wxwidgets/Bitmap.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

ImageList::ImageList(const Frame& frame, const double scaleFactor)
{
	ocean_assert(frame && frame.width() % frame.height() == 0u);

	const Bitmap bitmap(frame, true, scaleFactor);

	if (bitmap.IsOk() && bitmap.GetHeight() != 0)
	{
		ocean_assert(bitmap.GetHeight() > 0 && bitmap.GetHeight() > 0);

		const unsigned int images = (unsigned int)(bitmap.GetWidth() / bitmap.GetHeight());
		const unsigned int imageSize = (unsigned int)bitmap.GetHeight();

		ocean_assert(imageSize * images == (unsigned int)bitmap.GetWidth());
		if(imageSize * images != (unsigned int)bitmap.GetWidth())
			return;

		Create(int(imageSize), int(imageSize), frame.hasAlphaChannel(), int(images));

		for (unsigned int n = 0u; n < images; ++n)
		{
			Add(bitmap.GetSubBitmap(wxRect(int(n * imageSize), 0, int(imageSize), int(imageSize))));
		}
	}
}

}

}

}
