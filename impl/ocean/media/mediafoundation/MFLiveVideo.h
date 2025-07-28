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

	protected:

		/**
		 * Definition of an unordered map mapping stream properties to their frame rates.
		 */
		using StreamPropertyMap = std::unordered_map<StreamProperty, std::vector<double>, StreamProperty::Hash>;

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
		 * Returns the current exposure duration of this device.
		 * @see LiveVideo::exposureDuration().
		 */
		double exposureDuration(double* minDuration = nullptr, double* maxDuration = nullptr, ControlMode* exposureMode = nullptr) const override;

		/**
		 * Sets the exposure duration of this device.
		 * @see LiveVideo::setExposureDuration().
		 */
		bool setExposureDuration(const double duration, const bool allowShorterExposure = false) override;

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

		/**
		 * Session started event function.
		 * @see MFMedium::onSessionStarted().
		 */
		void onSessionStarted() override;

		/**
		 * Session stopped event function.
		 * @see MFMedium::onSessionStopped().
		 */
		void onSessionStopped() override;

		/**
		 * Determines the exposure range of the camera.
		 * @param iKsControl The IKsControl interface of the camera, must be valid
		 * @param minExposure The resulting minimal exposure value, in seconds
		 * @param maxExposure The resulting maximal exposure value, in seconds
		 * @return True, if succeeded
		 */
		static bool exposureRange(IKsControl* iKsControl, double& minExposure, double& maxExposure);

		/**
		 * Sets the exposure of the camera.
		 * @param iKsControl The IKsControl interface of the camera, must be valid
		 * @param exposure The exposure to be set, in seconds, with range (0, infinity), 0 for auto exposure
		 * @return True, if succeeded
		 */
		static bool setExposure(IKsControl* iKsControl, const double exposure);

		/**
		 * Returns the exposure of the camera.
		 * @param iKsControl The IKsControl interface of the camera, must be valid
		 * @param exposure The resulting exposure value, in seconds
		 * @param controlMode The resulting control mode
		 * @return True, if succeeded
		 */
		static bool exposure(IKsControl* iKsControl, double& exposure, ControlMode& controlMode);

		/**
		 * Translates the exposure value from log base 2 to a linear exposure in seconds.
		 * @param logBase2 The exposure value in log base 2, with range (-infinity, infinity)
		 * @return The resulting exposure value in seconds, with range [0, infinity)
		 */
		static double translateExposure(const LONG logBase2);

		/**
		 * Translates the exposure value from a linear exposure in seconds to log base 2.
		 * @param exposure The exposure value in seconds, with range [0, infinity)
		 * @return The resulting exposure value in log base 2, with range (-infinity, infinity)
		 */
		static LONG translateExposure(const double exposure);

	protected:

		/// The symbolic link of the device.
		std::string symbolicLink_;

		/// True, if the session has been started.
		bool sesssionStarted_ = false;

		/// The exposure duration of the camera which will be set once the session has been started.
		double delayedExposureDuration_ = NumericD::minValue();
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_LIVE_VIDEO_H
