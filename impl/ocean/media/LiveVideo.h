// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_LIVE_VIDEO_H
#define META_OCEAN_MEDIA_LIVE_VIDEO_H

#include "ocean/media/Media.h"
#include "ocean/media/FrameMedium.h"
#include "ocean/media/LiveMedium.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class LiveVideo;

/**
 * Definition of a smart medium reference holding a live video object.
 * @see SmartMediumRef, LiveVideo.
 * @ingroup media
 */
typedef SmartMediumRef<LiveVideo> LiveVideoRef;

/**
 * This class is the base class for all live videos.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT LiveVideo :
	public virtual FrameMedium,
	public virtual LiveMedium
{
	public:

		/**
		 * Returns the current exposure duration of this device.
		 * @param minDuration Optional resulting minimal duration to set, in seconds, with range (0, infinity), -1 if unknown
		 * @param maxDuration Optional resulting maximal duration to set, in seconds, with range [minDuration, infinity), -1 if unknown
		 * @return The duration in seconds, with range [minDuration, maxDuration], 0 for auto exposure duration, -1 if unknown
		 * @see setExposureDuration().
		 */
		virtual double exposureDuration(double* minDuration = nullptr, double* maxDuration = nullptr) const;

		/**
		 * Returns the current ISO of this device.
		 * @param minISO Optional resulting minimal ISO to set, with range (0, infinity), -1 if unknown
		 * @param maxISO Optional resulting maximal ISO to set, with range (0, infinity), -1 if unknown
		 * @return The current ISO, with range [minISO, maxISO], 0 for auto IOS, -1 if unknnown
		 */
		virtual float iso(float* minISO = nullptr, float* maxISO = nullptr) const;

		/**
		 * Returns the current focus of this device.
		 * @return The device's focus, with range [0, 1] with 0 shortest distance and 1 furthest distance, -1 for auto focus
		 */
		virtual float focus() const;

		/**
		 * Sets the exposure duriation of this device.
		 * @param duration The exposure duration to be set, in seconds, with range (0, infinity), 0 for auto exposure, -1 for a one-time auto exposure
		 * @return True, if succeeded
		 * @see exposureDuration().
		 */
		virtual bool setExposureDuration(const double duration);

		/**
		 * Sets the ISO of this device.
		 * @param iso The iso to be set, with range (0, infinity), -1 for auto ISO
		 * @return True, if succeeded
		 * @see iso().
		 */
		virtual bool setISO(const float iso);

		/**
		 * Sets the focus of this device.
		 * @param position The focus position to be set, with range [0, 1] with 0 shortest distance and 1 furthest distance, -1 for auto focus
		 * @return True, if succeeded
		 * @see focus().
		 */
		virtual bool setFocus(const float position);

	protected:

		/**
		 * Creates a new live video source by a given url.
		 * @param url Url of the live video source
		 */
		explicit LiveVideo(const std::string& url);
};

}

}

#endif // META_OCEAN_MEDIA_LIVE_VIDEO_H
