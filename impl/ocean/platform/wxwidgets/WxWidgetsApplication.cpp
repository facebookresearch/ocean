/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/WxWidgetsApplication.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

bool WxWidgetsApplication::OnInit()
{
	return true;
}

bool WxWidgetsApplication::Initialize(int& inputArgc, wxChar** inputArgv)
{
	ocean_assert(inputArgc == 0 && inputArgv == nullptr);
	OCEAN_SUPPRESS_UNUSED_WARNING(inputArgc);
	OCEAN_SUPPRESS_UNUSED_WARNING(inputArgv);

	return true;
}

}

}

}
