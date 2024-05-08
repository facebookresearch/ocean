/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_TRACKER_H
#define META_OCEAN_TRACKING_TRACKER_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements the base class for all tracker object.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT Tracker
{
	public:

		/**
		 * Creates a new tracker object.
		 */
		Tracker();

		/**
		 * Destructs a new tracker object.
		 */
		virtual ~Tracker();
};

}

}

#endif // META_OCEAN_TRACKING_TRACKER_H
