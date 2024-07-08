/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_LIVE_MEDIUM_H
#define META_OCEAN_MEDIA_DS_LIVE_MEDIUM_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSConfigMedium.h"
#include "ocean/media/directshow/DSMedium.h"

#include "ocean/media/LiveMedium.h"

#include "ocean/base/Scheduler.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class is the base class for all DirectShow live mediums.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSLiveMedium :
	public virtual DSConfigMedium,
	public virtual DSMedium,
	public virtual LiveMedium
{
	protected:

		/**
		 * Creates a new DSLiveMedium object by a given url.
		 * @param url Url of the medium
		 */
		explicit DSLiveMedium(const std::string& url);

		/**
		 * Destructs a DSLiveMedium object.
		 */
		~DSLiveMedium() override;

		/**
		 * Event function for the scheduler.
		 */
		virtual void onScheduler();
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_LIVE_MEDIUM_H
