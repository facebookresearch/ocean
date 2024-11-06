/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_AK_SCENE_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARKIT_AK_SCENE_TRACKER_6_DOF_H

#include "ocean/devices/arkit/ARKit.h"
#include "ocean/devices/arkit/AKDevice.h"

#include "ocean/base/StringApple.h"

#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * This class implements the 6DOF scene tracker.
 * @ingroup devicesarkit
 */
class OCEAN_DEVICES_ARKIT_EXPORT AKSceneTracker6DOF :
	virtual public AKDevice,
	virtual public SceneTracker6DOF,
	virtual public VisualTracker
{
	friend class AKFactory;

	protected:

		/**
		 * Definition of an unordered map mapping anchor identifier strings to unique ids.
		 */
		using IdentifierMap = std::unordered_map<std::string, Index32>;

		/**
		 * Helper class implementing a hash function for ARMeshAnchor.
		 */
		struct ARMeshAnchorHash
		{
			/**
			 * Hash function returning a hash value for an ARMeshAnchor object
			 * @param anchor The anchor for which the hash will be returned
			 * @return The resulting hash value
			 */
			inline size_t operator()(const ARMeshAnchor* anchor) const;
		};

		/**
		 * Definition of an unordered set holding ARMeshAnchor objects.
		 */
		using ARMeshAnchorSet = std::unordered_set<ARMeshAnchor*, ARMeshAnchorHash>;

	public:

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Pauses the device.
		 * @see Device::pause().
		 */
		bool pause() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Sets the multi-view visual input of this tracker.
		 * @see VisualTracker::setInput().
		 */
		void setInput(Media::FrameMediumRefs&& frameMediums) override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Event function for a new 6DOF pose.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param world_T_rotatedWorld The optional transformation between ARKit's rotated world and the standard world, should only be identity or the flip matrix around y-axis
		 * @param timestamp The timestamp of the new transformation
		 * @param arFrame The current ARFRame object containing additional data for the sample, must be valid
		 */
		API_AVAILABLE(ios(11.3))
		void onNewSample(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_rotatedWorld, const Timestamp& timestamp, ARFrame* arFrame);

		/**
		 * Event function for a new 6DOF pose.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param sceneElements The scene elements which are part of the sample, at least one
		 * @param timestamp The timestamp of the new transformation
		 * @param metadata The metadata of the sample
		 */
		void onNewSample(const HomogenousMatrix4& world_T_camera, SharedSceneElements&& sceneElements, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Event function for added anchors.
		 * @see AKDevice::onAddedAnchors().
		 */
		void onAddedAnchors(const ARAnchors& anchors) override;

		/**
		 * Event function for updated anchors.
		 * @see AKDevice::onUpdateAnchors().
		 */
		void onUpdateAnchors(const ARAnchors& anchors) override;

		/**
		 * Returns the name of this tracker.
		 * @return The trackers's name
		 */
		static inline std::string deviceNameAKSceneTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeAKSceneTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF scene tracker.
		 */
		explicit AKSceneTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~AKSceneTracker6DOF() override;

		/**
		 * Exracts the 3D vectors from a geometry source.
		 * @param geometrySource The geometry source from wich all vectors will be extracted
		 * @param vectors The resulting 3D vectors
		 * @return True, if succeeded
		 */
		API_AVAILABLE(ios(13.4))
		static bool extractVectors3(ARGeometrySource* geometrySource, Vectors3& vectors);

		/**
		 * Extracts the indices from a geometry element.
		 * @param geometryElement The geometry elemnt from which the indices will be extracted
		 * @param indices The resulting indices
		 * @return True, if succeeded
		 */
		API_AVAILABLE(ios(13.4))
		static bool extractIndices(ARGeometryElement* geometryElement, Indices32& indices);

	protected:

		/// The unique id for the world object.
		ObjectId worldObjectId_ = invalidObjectId();

		/// True, if the tracker has been started.
		bool isStarted_ = false;

		/// True, if the world object is currently tracked.
		bool worldIsTracked_ = false;

		/// The map mapping unique plane identifier strings to mesh ids.
		IdentifierMap identifierMap_;

		/// The counter for unique mesh ids.
		unsigned int meshIdCounter_ = 0u;

		/// The set holding all updated ARMeshAnchor objects.
		ARMeshAnchorSet updatedMeshAnchors_;
};

inline size_t AKSceneTracker6DOF::ARMeshAnchorHash::operator()(const ARMeshAnchor* anchor) const
{
	return std::hash<std::string>()(StringApple::toUTF8(anchor.identifier.UUIDString));
}

inline std::string AKSceneTracker6DOF::deviceNameAKSceneTracker6DOF()
{
	return std::string("ARKit 6DOF Scene Tracker");
}

inline AKSceneTracker6DOF::DeviceType AKSceneTracker6DOF::deviceTypeAKSceneTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL | SCENE_TRACKER_6DOF);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_AK_SCENE_TRACKER_6_DOF_H
