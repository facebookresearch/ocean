/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_LIVE_VIDEO_H
#define META_OCEAN_MEDIA_MF_LIVE_VIDEO_H

#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/mediafoundation/MFFrameMedium.h"

#include "ocean/media/Library.h"
#include "ocean/media/LiveVideo.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This class implements a MediaFoundation live video object allowing to access e.g., webcams.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT MFLiveVideo :
	public virtual MFFrameMedium,
	public virtual LiveVideo
{
	friend class MFLibrary;

	public:

		/**
		 * Returns the supported stream types.
		 * @see LiveVideo::supportedStreamTypes().
		 */
		StreamTypes supportedStreamTypes() const override;

		/**
		 * Returns the supported stream configurations for a given stream type.
		 * @see supportedStreamConfigurations().
		 */
		StreamConfigurations supportedStreamConfigurations(const StreamType streamType) const override;

		/**
		 * Enumerates all currently available video devices.
		 * @param definitions The resulting video devices
		 * @return True, if succeeded
		 */
		static bool enumerateVideoDevices(Library::Definitions& definitions);

	protected:

		/**
		 * Creates a new live video by a given URL.
		 * @param url The URL of the live video
		 * @param symbolicLink Optional the symbolic link of the live video, if known
		 */
		explicit MFLiveVideo(const std::string& url, const std::string& symbolicLink);

		/**
		 * Destructs a MFLiveVideo object.
		 */
		~MFLiveVideo() override;

		/**
		 * Creates and builds the topology of this object.
		 * @see MFMedium::createTopology().
		 */
		bool createTopology(const bool respectPlaybackTime) override;

		/**
		 * Creates the media source object.
		 * @see MFMedium::createTopology().
		 */
		bool createMediaSource() override;

		/**
		 * Releases the topology.
		 */
		void releaseTopology() override;

	protected:

		/// The symbolic link of the device.
		std::string symbolicLink_;
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_LIVE_VIDEO_H
