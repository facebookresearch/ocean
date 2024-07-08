/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSLiveMedium.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSLiveMedium::DSLiveMedium(const std::string& url) :
	Medium(url),
	DSMedium(url),
	ConfigMedium(url),
	DSConfigMedium(url),
	LiveMedium(url)
{
	Scheduler::get().registerFunction(Scheduler::Callback(*this, &DSLiveMedium::onScheduler));
}

DSLiveMedium::~DSLiveMedium()
{
	Scheduler::get().unregisterFunction(Scheduler::Callback(*this, &DSLiveMedium::onScheduler));
}

void DSLiveMedium::onScheduler()
{
	if (!graphIsRunning_)
	{
		return;
	}

	long code = 0;
	LONG_PTR param1 = 0;
	LONG_PTR param2 = 0;

	if (*mediaEventInterface_ && S_OK == mediaEventInterface_->GetEvent(&code, &param1, &param2, 0))
	{
		if (code == EC_DEVICE_LOST)
		{
			bool deviceLost = param2 == 0;
			mediaEventInterface_->FreeEventParams(code, param1, param2);

			if (deviceLost)
			{
				Log::info() << "Lost device: \"" << url() << "\".";
			}
			else
			{
				Log::info() << "Re-found device: \"" << url() << "\".";

				releaseGraph();

				Log::info() << "Try to restart \"" << url() << "\".";

				if (buildGraph())
				{
					if (start())
					{
						Log::info() << "Device \"" << url() << "\" has been restarted successfully.";
					}
					else
					{
						Log::info() << "Device \"" << url() << "\" could not be restarted.";
					}
				}
				else
				{
					Log::info() << "Device \"" << url() << "\" could not be restarted.";
				}
			}
		}
		else
		{
			mediaEventInterface_->FreeEventParams(code, param1, param2);
		}
	}
}

}

}

}
