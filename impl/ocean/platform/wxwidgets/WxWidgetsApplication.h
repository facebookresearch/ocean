/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_WX_WIDGETS_APPLICATION_H
#define META_OCEAN_PLATFORM_WXWIDGETS_WX_WIDGETS_APPLICATION_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements a very simple WxWidgets application not registering for any kind of application event like e.g., file drag&drop events.
 * @ingroup platformwxwidgets
 */
class WxWidgetsApplication : public wxApp
{
	public:

		/**
		 * The initialization event function.
		 * This function actually does nothing.
		 * @return True, always
		 */
		bool OnInit() override;

		/**
		 * The initialization function.
		 * This function actually does nothing.
		 * @param argc The number of argument parameters, should be 0
		 * @param argv The argument parameters, should be nullptr
		 * @return True, always
		 */
		bool Initialize(int& argc, wxChar** argv) override;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_WX_WIDGETS_APPLICATION_H
