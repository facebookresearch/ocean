/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_AR_SESSION_MANAGER_H
#define META_OCEAN_DEVICES_ARCORE_AR_SESSION_MANAGER_H

#include "ocean/devices/arcore/ARCore.h"
#include "ocean/devices/arcore/ACDevice.h"
#include "ocean/devices/arcore/ScopedARObject.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Singleton.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/FrameMedium.h"

#include <arcore_c_api.h>

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

/**
 * This class implements a wrapper around the actual ARCore tracker which may be used by several devices at the same time.
 * @ingroup devicearcore
 */
class OCEAN_DEVICES_ARCORE_EXPORT ARSessionManager : public Singleton<ARSessionManager>
{
	friend class Singleton<ARSessionManager>;
	friend class ACFactory;

	public:

		/**
		 * Redefinition of TrackerCapabilities from ACDevice.
		 */
		using TrackerCapabilities = ACDevice::TrackerCapabilities;

	protected:

		/**
		 * This class implements an AR session, one session is associated with a specific camera.
		 */
		class Session
		{
			protected:

				/**
				 * Definition of an unordered map mapping trackers to running states.
				 */
				typedef std::unordered_map<ACDevice*, unsigned int> TrackerMap;

				/**
				 * Definition of an unordered map mapping ArPlane objects to ids.
				 */
				typedef std::unordered_map<ArPlane*, Index32> PlaneIdMap;

			public:

				/**
				 * Creates a new session for a given frame medium.
				 * @param frameMedium The frame medium which will be used in the session.
				 */
				explicit Session(const Media::FrameMediumRef& frameMedium);

				/**
				 * Registers a new tracker with this session.
				 * @param tracker The tracker which will be registered with the session, must be valid
				 * @return True, if succeeded
				 */
				bool registerTracker(ACDevice* tracker);

				/**
				 * Unregisters a tracker with this session.
				 * @param tracker The tracker which will be unregistered from the session, must be valid
				 * @return True, if succeeded
				 */
				bool unregisterTracker(ACDevice* tracker);

				/**
				 * Starts the session for a given tracker.
				 * @param tracker The tracker for which the session will be started, must be valid
				 * @return True, if succeeded
				 */
				bool start(ACDevice* tracker);

				/**
				 * Pauses the session for a given tracker.
				 * @param tracker The tracker for which the session will be paused, must be valid
				 * @return True, if succeeded
				 */
				bool pause(ACDevice* tracker);

				/**
				 * Stops the session for a given tracker.
				 * @param tracker The tracker for which the session will be stopped, must be valid
				 * @return True, if succeeded
				 */
				bool stop(ACDevice* tracker);

				/**
				 * Updates the tracker with the most recent tracking results from ARCore.
				 * This function must be called out of the main GL rendering thread.
				 * @param textureId The id of the texture in which the camera image will be copied, must be valid
				 */
				void update(unsigned int textureId);

				/**
				 * Returns the number of registered trackers.
				 * @return The session's trackers, with range [0, infinity)
				 */
				size_t registeredTrackers();

			protected:

				/**
				 * Extracts all planes from an ArFrame.
				 * @param arSession The AR session to which the AR frame belongs, must be valid
				 * @param arFrame The AR frame from which the image will be extracted, must be valid
				 * @param planes The resulting planes
				 * @return True, if succeeded
				 */
				bool extractPlanes(ArSession* arSession, ArFrame* arFrame, SceneTracker6DOF::SceneElementPlanes::Planes& planes);

			protected:

				/// The AR Session object.
				ScopedARSession arSession_;

				/// The frame medium object used as input for the tracker
				Media::FrameMediumRef frameMedium_;

				/// The capabilities of the session.
				TrackerCapabilities sessionCapabilities_ = ACDevice::TC_INVALID;

				/// The map mapping tracker to running states.
				TrackerMap trackerMap_;

				/// The timestamp of the last ARFrame.
				int64_t lastTimestampNs_ = NumericT<int64_t>::minValue();

				/// The map mapping planes to ids.
				PlaneIdMap planeIdMap_;
		};

		typedef std::shared_ptr<Session> SharedSession;

		/**
		 * Definition of an unordered map mapping medium urls to sessions.
		 */
		typedef std::unordered_map<std::string, SharedSession> SessionMap;

		/**
		 * Definition of an unordered map mapping trackers to medium urls.
		 */
		typedef std::unordered_map<ACDevice*, std::string> TrackerMap;

	public:

		/**
		 * Returns whether ARCore is available on the device.
		 * @return True, if so
		 */
		inline bool isARCoreAvailable() const;

		/**
		 * Registers a new tracker.
		 * Each register() call must be balanced with a call of unregister().
		 * @param tracker The tracker to register, must be valid
		 * @param frameMedium The frame medium the tracker will use, must be valid
		 * @return True, if succeeded
		 * @see unregisterTracker().
		 */
		bool registerTracker(ACDevice* tracker, const Media::FrameMediumRef& frameMedium);

		/**
		 * Unregisteres a given tracker.
		 * Needs to be called before the tracker is disposed.
		 * @param tracker The tracker to unregister, must be valid
		 * @return True, if succeeded
		 */
		bool unregisterTracker(ACDevice* tracker);

		/**
		 * Starts the session for a given tracker.
		 * @param tracker The tracker for which the session will be started, must be valid
		 * @return True, if succeeded
		 */
		bool start(ACDevice* tracker);

		/**
		 * Pauses the session for a given tracker.
		 * @param tracker The tracker for which the session will be paused, must be valid
		 * @return True, if succeeded
		 */
		bool pause(ACDevice* tracker);

		/**
		 * Stops the session for a given tracker.
		 * @param tracker The tracker for which the session will be stopped, must be valid
		 * @return True, if succeeded
		 */
		bool stop(ACDevice* tracker);

	protected:

		/**
		 * Creates a new manager.
		 */
		ARSessionManager();

		/**
		 * Updates the tracker with the most recent tracking results from ARCore.
		 * This function must be called out of the main GL rendering thread.
		 * @param textureId The id of the texture in which the camera image will be copied, must be valid
		 */
		void update(unsigned int textureId);

		/**
		 * Extracts the image from an ArFrame.
		 * @param arSession The AR session to which the AR frame belongs, must be valid
		 * @param arFrame The AR frame from which the image will be extracted, must be valid
		 * @return The extracted image, invalid in case of a failure
		 */
		static Frame extractImage(ArSession* arSession, ArFrame* arFrame);

		/**
		 * Extracts the depth from an ArFrame.
		 * @param arSession The AR session to which the AR frame belongs, must be valid
		 * @param arFrame The AR frame from which the depth will be extracted, must be valid
		 * @return The extracted depth, invalid in case of a failure
		 */
		static Frame extractDepth(const ArSession* arSession, const ArFrame* arFrame);

		/**
		 * Extracts the camera pose and camera profile from an ArFrame.
		 * @param arSession The AR session to which the AR frame belongs, must be valid
		 * @param arFrame The AR frame from which the image will be extracted, must be valid
		 * @param world_T_device The resulting transformation between device and world
		 * @param anyCamera The resulting camera profile
		 * @return True, if succeeded
		 */
		static bool extractPose(ArSession* arSession, ArFrame* arFrame, HomogenousMatrixF4& world_T_device, SharedAnyCamera& anyCamera);

		/**
		 * Extracts the point cloud for an ArFrame.
		 * @param arSession The AR session to which the AR frame belongs, must be valid
		 * @param arFrame The AR frame from which the image will be extracted, must be valid
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of the 3D object points, one for each 3D object point
		 * @return True, if succeeded
		 */
		static bool extractPointCloud(ArSession* arSession, ArFrame* arFrame, Vectors3& objectPoints, Indices64& objectPointIds);

	protected:

		/// True, if ARCore is available on the device.
		bool isARCoreAvailable_ = false;

		/// The map mapping medium urls to sessions.
		SessionMap sessionMap_;

		/// The map mapping trackers to medium urls.
		TrackerMap trackerMap_;

		/// The manager's lock.
		mutable Lock lock_;
};

inline bool ARSessionManager::isARCoreAvailable() const
{
	const ScopedLock scopedLock(lock_);

	return isARCoreAvailable_;
}

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_AR_SESSION_MANAGER_H
