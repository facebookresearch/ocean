// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_MF_FRAME_MEDIUM_H
#define META_OCEAN_MEDIA_MF_FRAME_MEDIUM_H

#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/mediafoundation/SampleGrabber.h"
#include "ocean/media/mediafoundation/MFMedium.h"

#include "ocean/media/FrameMedium.h"

#include <vector>

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This is the base class for all Media Foundation frame mediums.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT MFFrameMedium :
	virtual public MFMedium,
	virtual public FrameMedium
{
	public:

		/**
		 * Sets the preferred frame dimension.
		 * @see FrameMedium::setPreferredFrameDimension().
		 */
		bool setPreferredFrameDimension(const unsigned int width, const unsigned int height) override;

		/**
		 * Sets the preferred frame pixel format.
		 * @see FrameMedium::setPreferredFramePixelFormat().
		 */
		bool setPreferredFramePixelFormat(const FrameType::PixelFormat format) override;

		/**
		 * Sets the preferred frame frequency in Hz.
		 * @see FrameMedium::setPreferredFrameFrequency().
		 */
		bool setPreferredFrameFrequency(const FrameFrequency frequency) override;

		/**
		 * Returns whether the frame medium respects the media playback time or whether the frames are provided as fast as possible.
		 * @see FrameMedium::respectPlaybackTime().
		 */
		bool respectPlaybackTime() const override;

		/**
		 * Specifies whether the media playback time will be respected or whether the samples are provided as fast as possible.
		 * @see FrameMedium::setRespectPlaybackTime().
		 */
		bool setRespectPlaybackTime(const bool state) override;

		/**
		 * Extracts the video format of a given Media Foundation media type.
		 * @param mediaType Media Foundation media type, must be valid
		 * @param frameType Resulting frame type
		 * @return True, if succeeded
		 */
		static bool extractFrameFormat(IMFMediaType* mediaType, MediaFrameType& frameType);

		/**
		 * Creates the video format of a given Media Foundation media type.
		 * @param frameType Frame type used for modification, must be valid
		 * @return The resulting media type, invalid otherwise
		 */
		static ScopedIMFMediaType createMediaType(const MediaFrameType& frameType);

		/**
		 * Determines the video media type of a given topology.
		 * @param topology The topology for that the media type is determined, must be valid
		 * @param mediaFrameType Resulting media frame type
		 * @return True, if succeeded
		 */
		static bool determineMediaType(IMFTopology* topology, FrameMedium::MediaFrameType& mediaFrameType);

		/**
		 * Determines the video media type of a given topology node.
		 * @param node The topology node for that the media type is determined, must be valid
		 * @param mediaFrameType Resulting media frame type
		 * @return True, if succeeded
		 */
		static bool determineMediaType(IMFTopologyNode* node, FrameMedium::MediaFrameType& mediaFrameType);

	protected:

		/**
		 * Creates a new frame medium by a given URL.
		 * @param url The URL of the frame medium
		 */
		explicit MFFrameMedium(const std::string& url);

		/**
		 * Destructs a MFFrameMedium object.
		 */
		~MFFrameMedium() override;

		/**
		 * Returns the timestamp of the most recent media sample.
		 * @see MFMedium::recentMediaSampleTimestamp().
		 */
		bool recentMediaSampleTimestamp(LONGLONG& timestamp, LONGLONG* nextTimestamp) const override;

		/**
		 * Called by if a new sample arrived.
		 * @param guidMajorMediaType Major media type of the sample
		 * @param dwSampleFlags Reserved
		 * @param llSampleTime The presentation time for this sample, in 100-nanosecond units
		 * @param llSampleDuration The duration of the sample, in 100-nanosecond units
		 * @param pSampleBuffer A pointer to a buffer that contains the sample data
		 * @param dwSampleSize Size of the pSampleBuffer buffer, in bytes
		 */
		virtual void onNewSample(const GUID& guidMajorMediaType, const unsigned int dwSampleFlags, const long long llSampleTime, const long long llSampleDuration, const void* pSampleBuffer, const unsigned int dwSampleSize);

		/**
		 * Topology set event function.
		 * @see MFMedium::onTopologySet().
		 */
		void onTopologySet(IMFTopology* topology) override;

		/**
		 * Format type changed event function.
		 * @see MFMedium::onFormatTypeChanged().
		 */
		void onFormatTypeChanged(const TOPOID nodeId) override;

		/**
		 * Builds the frame topology of this object.
		 * @return True, if succeeded
		 */
		virtual bool buildFrameTopology();

		/**
		 * Releases the frame topology of this object.
		 */
		virtual void releaseFrameTopology();

	protected:

		/// Frame sample grabber.
		ScopedSampleGrabber frameSampleGrabber_;

		/// Intermediate respect playback time state.
		bool sinkRespectPlaybackTime_ = true;

		/// Frame type of the most recent (upcoming frame) frame.
		FrameType recentFrameType_;

		/// Internal frame type of the most recent (upcoming frame) frame.
		FrameType internalRecentFrameType_;

		/// Optional cropping position left.
		unsigned int croppingLeft_ = 0u;

		/// Optional cropping position top.
		unsigned int croppingTop_ = 0u;

		/// Optional cropping width, 0 if no cropping is defined.
		unsigned int croppingWidth_ = 0u;

		/// Optional cropping height, 0 if no cropping is defined.
		unsigned int croppingHeight_ = 0u;

		/// True, if the medium waits for the first frame after the topology has been changed.
		bool waitingForFirstFrame_ = true;
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_FRAME_MEDIUM_H
