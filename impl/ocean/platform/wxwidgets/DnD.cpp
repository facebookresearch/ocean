/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/DnD.h"
#include "ocean/platform/wxwidgets/Utilities.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

FileDropTarget::FileDropTarget(const Callback& callback) :
	targetCallback_(callback)
{
	// nothing to do here
}

bool FileDropTarget::OnDropFiles(wxCoord /*x*/, wxCoord /*y*/, const wxArrayString& filenames)
{
	ocean_assert(!filenames.empty());
	ocean_assert(targetCallback_);

	if (filenames.empty() || !targetCallback_)
	{
		return false;
	}

	std::vector<std::string> files;
	files.reserve(filenames.size());

	for (unsigned int n = 0; n < (unsigned int)filenames.size(); ++n)
	{
		files.emplace_back(Utilities::toAString(filenames[n]));
	}

	return targetCallback_(files);
}

}

}

}
