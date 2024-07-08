/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_AK_DEVICE_H
#define META_OCEAN_DEVICES_ARKIT_AK_DEVICE_H

#include "ocean/devices/arkit/ARKit.h"

#include "ocean/base/Singleton.h"

#include "ocean/devices/Device.h"
#include "ocean/devices/Measurement.h"

#include "ocean/media/FrameMedium.h"

#include <ARKit/ARKit.h>

@interface AKTracker6DOFDelegate : NSObject<ARSessionDelegate>
@end

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/// Forward declaration.
class AKGeoAnchorsTracker6DOF;

/// Forward declaration.
class AKSceneTracker6DOF;

/// Forward declaration.
class AKWorldTracker6DOF;

/**
 * This class implements a device for the ARKit library.
 * @ingroup devicesarkit
 */
class OCEAN_DEVICES_ARKIT_EXPORT AKDevice : virtual public Device
{
	public:

		/**
		 * Definition of individual capabilities.
		 */
		enum TrackerCapabilities : uint32_t
		{
			/// An invalid capability.
			TC_INVALID = 0u,
			/// The tracker provides basic SLAM.
			TC_SLAM = 1u << 0u,
			/// The tracker provides Geo Anchors.
			TC_GEO_ANCHORS = 1u << 1u,
			/// The tracker provide plane detection.
			TC_PLANE_DETECTION = 1u << 2u,
			/// The tracker provide mesh reconstruction.
			TC_MESH_RECONSTRUCTION = 1u << 3u,
			/// The tracker provides depth information.
			TC_DEPTH = 1u << 4u,
			/// The tracker is able to track faces.
			TC_FACE = 1u << 5u
		};

	protected:

		/**
		 * This class implements a wrapper around the actual ARSessionDelegate which may be used by several devices at the same time.
		 */
		class OCEAN_DEVICES_ARKIT_EXPORT ARSessionManager : public Singleton<ARSessionManager>
		{
			friend class Singleton<ARSessionManager>;

			public:

				/**
				 * Starts the session for a given tracker.
				 * @param tracker The tracker for which the session will be started, must be valid
				 * @param frameMedium The frame medium the session will use, must be valid
				 * @return True, if succeeded
				 */
				bool start(AKDevice* tracker, const Media::FrameMediumRef& frameMedium);

				/**
				 * Pauses the session for a given tracker.
				 * @param tracker The tracker for which the session will be paused, must be valid
				 * @return True, if succeeded
				 */
				bool pause(AKDevice* tracker);

				/**
				 * Stops the session for a given tracker.
				 * @param tracker The tracker for which the session will be stopped, must be valid
				 * @return True, if succeeded
				 */
				bool stop(AKDevice* tracker);
				/**
				 * Registers a new geo anchor.
				 * @param objectId The id of the object associated with the geo anchor, must be valid
				 * @param latitude The latitude of the geo anchor, with range [-90, 90]
				 * @param longitude The longitude of the geo anchor, with range [-180, 180]
				 * @param altitude The altitude of the geo anchor, with range [-10,000, 30,000], or NumericD::minValue() if unknown
				 * @return True, if succeeded
				 */
				bool registerGeoAnchor(const Measurement::ObjectId& objectId, const double latitude, const double longitude, const double altitude);

				/**
				 * Unregisters a geo anchor.
				 * @param objectId The id of the object which is associated with the geo anchor to unregister, must be valid
				 * @return True, if succeeded
				 */
				bool unregisterGeoAnchor(const Measurement::ObjectId& objectId);

			protected:

				/**
				 * Creates a new manager.
				 */
				ARSessionManager();

			protected:

				/// The delegate of the ARKit Tracker.
				AKTracker6DOFDelegate* akTracker6DOFDelegate_ = nullptr;
		};

	public:

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

		/**
		 * Returns the capabilities of the tracker necessary for this device.
		 * @return The tracker capabilities
		 */
		inline TrackerCapabilities trackerCapabilities() const;

		/**
		 * Translates the value of an ARGeoTrackingState to a readable string.
		 * @param state The state to translate
		 * @return The readable string
		 */
		API_AVAILABLE(ios(14.0))
		static std::string translateGeoTrackingState(const ARGeoTrackingState& state);

		/**
		 * Translates the value of an ARGeoTrackingStateReason to a readable string.
		 * @param stateReason The state reason to translate
		 * @return The readable string
		 */
		API_AVAILABLE(ios(14.0))
		static std::string translateGeoTrackingStateReason(const ARGeoTrackingStateReason& stateReason);

		/**
		 * Translates the value of an ARGeoTrackingAccuracy to a readable string.
		 * @param accuracy The accuracy to translate
		 * @return The readable string
		 */
		API_AVAILABLE(ios(14.0))
		static std::string translateGeoTrackingAccuracy(const ARGeoTrackingAccuracy& accuracy);

	protected:

		/**
		 * Creates a new device by is name.
		 * @param trackerCapabilities The capabilities of the tracker necessary for this device
		 * @param name The name of the device
		 * @param type Major and minor device type of the device
		 */
		AKDevice(const TrackerCapabilities trackerCapabilities, const std::string& name, const DeviceType type);

	protected:

		/// The capabilities of the tracker for this device.
		TrackerCapabilities trackerCapabilities_ = TC_INVALID;
};

inline AKDevice::TrackerCapabilities AKDevice::trackerCapabilities() const
{
	return trackerCapabilities_;
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_AK_DEVICE_H
