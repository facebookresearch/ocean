// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
