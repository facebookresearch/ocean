/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_LIVE_MEDIUM_H
#define META_OCEAN_MEDIA_LIVE_MEDIUM_H

#include "ocean/media/Media.h"
#include "ocean/media/Medium.h"
#include "ocean/media/ConfigMedium.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class LiveMedium;

/**
 * Definition of a smart medium reference holding a live medium object.
 * @see SmartMediumRef, LiveMedium.
 * @ingroup media
 */
typedef SmartMediumRef<LiveMedium> LiveMediumRef;

/**
 * This class it the base class for all live mediums.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT LiveMedium :
	public virtual Medium,
	public virtual ConfigMedium
{
	protected:

		/**
		 * Creates a new live medium by a given url.
		 */
		explicit LiveMedium(const std::string& url);
};

}

}

#endif // META_OCEAN_MEDIA_LIVE_MEDIUM_H
