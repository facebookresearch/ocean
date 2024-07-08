/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_LIVE_VIDEO_H
#define META_OCEAN_MEDIA_DS_LIVE_VIDEO_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSFrameMedium.h"
#include "ocean/media/directshow/DSLiveMedium.h"

#include "ocean/media/LiveVideo.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class implements a DirectShow live video object.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSLiveVideo :
	public virtual DSFrameMedium,
	public virtual DSLiveMedium,
	public virtual LiveVideo
{
	friend class DSLibrary;

	protected:

		/**
		 * Creates a new live video by a given url.
		 * @param url Url of the live video
		 */
		explicit DSLiveVideo(const std::string& url);

		/**
		 * Destructs a DSLiveVideo object.
		 */
		~DSLiveVideo() override;

		/**
		 * @see DSGraphObject::buildGraph().
		 */
		bool buildGraph() override;

		/**
		 * @see DSGraphObject::releaseGraph().
		 */
		void releaseGraph() override;

		/**
		 * Creates the DirectShow video source filter.
		 * @return True, if succeeded
		 */
		bool createVideoSourceFilter();

		/**
		 * Collects the preferred video formats.
		 * @param sortableFrameTypes Resulting frame types sorted by their popularity
		 * @return True, if succeeded
		 */
		bool collectPreferredVideoFormats(DSSortableFrameTypes& sortableFrameTypes);

		/**
		 * Releases the DirectShow video source filter.
		 */
		void releaseVideoSourceFilter();

		/**
		 * Starts a configuration possibility.
		 * @see ConfigMedium::configuration().
		 */
		bool configuration(const std::string& name, long long data) override;

		/**
		 * @see ConfigMedium::configs().
		 */
		ConfigNames configs() const override;

	protected:

		/// DirectShow video source filter.
		ScopedIBaseFilter videoSourceFilter_;

		/// DirectShow stream config interface.
		ScopedIAMStreamConfig videoSourceFilterStreamConfigInterface_;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_LIVE_VIDEO_H
