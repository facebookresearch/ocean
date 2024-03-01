// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_QUEST_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_QUEST_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/mapalignment/MapAlignment.h"

#include "ocean/base/Thread.h"

#include "metaonly/ocean/network/verts/Driver.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/Text.h"

#include "ocean/tracking/mapbuilding/UnifiedFeatureMap.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an experience allowing to create a feature map on Quest which then can be shared with a Phone (with MapAlignmentPhoneExperience).
 * @see MapAlignmentPhoneExperience.
 * @ingroup xrplayground
 */
class MapAlignmentQuestExperience final :
	public XRPlaygroundExperience,
	public MapAlignment,
	protected Thread
{
	protected:

		/**
		 * This class holds the relevant input data for all threads.
		 */
		class InputData
		{
			public:

				/**
				 * Updates the input data.
				 * @param yFrames The new frames, at least one
				 * @param cameras The new camera profiles, one for each frame
				 * @param The transformation between device and world, must be valid
				 * @param device_T_cameras The transformations between cameras and device, one for each frame
				 * @param timestamp The timestamp of the new input data, must be valid
				 */
				void updateInputData(std::shared_ptr<Frames> yFrames, SharedAnyCameras cameras, const HomogenousMatrix4& world_T_device, HomogenousMatrices4 device_T_cameras, const Timestamp& timestamp);

				/**
				 * Returns the latest input data.
				 * @param lastTimestamp The timestamp of the last input data the caller used, can be invalid, will be updated with the timestamp of the latest input data if any
				 * @param yFrames The resulting latest frames
				 * @param cameras The resulting camera profiles for the latest frames, one for each frame
				 * @param world_T_device The resulting transformation between device and world
				 * @param device_T_cameras The resulting transformations between cameras and device, one for each frame
				 * @return True, if new input data was available
				 */
				bool latestInputData(Timestamp& lastTimestamp, std::shared_ptr<Frames>& yFrames, SharedAnyCameras& cameras, HomogenousMatrix4& world_T_device, HomogenousMatrices4& device_T_cameras) const;

			protected:

				/// The timestamp of the current input data.
				Timestamp timestamp_ = Timestamp(false);

				/// The current frames.
				std::shared_ptr<Frames> yFrames_;

				/// The current camera profiles.
				SharedAnyCameras cameras_;

				/// The transformation between device and world.
				HomogenousMatrix4 world_T_device_ = HomogenousMatrix4(false);

				/// The transformations between cameras and device.
				HomogenousMatrices4 device_T_cameras_;

				/// The lock for the input data.
				mutable Lock lock_;
		};

		/**
		 * This class holds the relevant map data for all threads.
		 */
		class MapData
		{
			public:

				/**
				 * Updates the map data.
				 * @param objectPoints The 3D object points of the feature map
				 * @param objectPointStabilityFactors The stability factors of the individual feature points
				 * @param multiDescriptors The multi-descriptors of the individual feature poitns
				 */
				void updateMapData(Vectors3&& objectPoints, Scalars&& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>&& multiDescriptors);

				/**
				 * Returns the last map data.
				 * @param objectPoints The resulting 3D object points of the feature map
				 * @param objectPointStabilityFactors The resulting stability factors of the individual feature points
				 * @param multiDescriptors The resulting multi-descriptors of the individual feature poitns
				 * @return True, if this object contained new map data
				 */
				bool latestMapData(Vectors3& objectPoints, Scalars& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>& multiDescriptors) const;

				/**
				 * Updates the feature map.
				 * @param unifiedFeatureMap The new feature map
				 */
				void updateFeatureMap(Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap);

				/**
				 * Returns the latest feature map.
				 * @param unifiedFeatureMap The resulting feature map
				 * @return True, if this object contained a new feature map
				 */
				bool latestFeatureMap(Tracking::MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap) const;

			protected:

				/// The 3D object points of the feature map.
				Vectors3 objectPoints_;

				/// The stability factors of the individual feature points.
				Scalars objectPointStabilityFactors_;

				/// The multi-descriptors of the individual feature poitns.
				std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors_;

				/// The feature latest feature map.
				Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap_;

				/// The lock for the map data.
				mutable Lock lock_;
		};

		/**
		 * This class implements an own thread for the map creator.
		 */
		class MapCreatorThread final : protected Thread
		{
			public:

				/**
				 * Creates a new thread object.
				 * @param inputData The input data the map creator will used
				 * @param mapData The map data receiving the maps
				 */
				MapCreatorThread(const InputData& inputData, MapData& mapData);

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// The input data the map creator will used.
				const InputData& inputData_;

				/// The map data receiving the maps.
				MapData& mapData_;
		};

		/**
		 * This class implements an own thread for the relocalizer.
		 */
		class RelocalizerThread final : protected Thread
		{
			public:

				/**
				 * Creates a new thread object.
				 * @param owner The owner of this object
				 * @param inputData The input data the map creator will used
				 * @param mapData The map data the relocalizer will use
				 */
				explicit RelocalizerThread(MapAlignmentQuestExperience& owner, const InputData& inputData, const MapData& mapData);

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// The owner of this object.
				MapAlignmentQuestExperience& owner_;

				/// The input data the map creator will used.
				const InputData& inputData_;

				/// The map data.
				const MapData& mapData_;
		};

		/**
		 * This class implements an own thread for handling maps.
		 */
		class MapHandlingThread final : public Thread
		{
			public:

				/**
				 * Creates a new thread object.
				 * @param owner The owner of this object
				 * @param mapData The map data receiving the maps
				 */
				MapHandlingThread(MapAlignmentQuestExperience& owner, MapData& mapData);

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// The owner of this object.
				MapAlignmentQuestExperience& owner_;

				/// The map data.
				MapData& mapData_;
		};

		/**
		 * Definition buffer.
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		virtual ~MapAlignmentQuestExperience();

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Experience::onKeyPress().
		 */
		void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Creates a new WorldLayerCircleExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

    protected:

		/**
		 * Protected default constructor.
		 */
		MapAlignmentQuestExperience();

		/**
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Returns whether the connection to the phone is established.
		 * @return True, if so
		 */
		bool connectionEstablished() const;

		/**
		 * Sends a given map to the phone.
		 * @param buffer The map to send
		 */
		void sendMap(Buffer&& buffer);

		/**
		 * The event function for new entities.
		 * @param driver The driver sending the event
		 * @param entity The new entity
		 */
		void onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity);

		/**
		 * The event function for changed users.
		 * @param driver The driver sending the event
		 * @param addedUsers The ids of all added users
		 * @param removedUsers The ids of all removed users
		 */
		void onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers);

	protected:

		/// True, to show the real-time relocalization of the headset.
		bool showHeadsetRelocalization_ = true;

		/// True, to show the real-time relocalization of the phone.
		bool showPhoneRelocalization_ = true;

		/// The rendering Text object holding the instruction text.
		Rendering::TextRef renderingText_;

		/// The rendering Transform object holding coordinate system of the HMD's device pose.
		Rendering::TransformRef renderingDevicePoseCoordinateSystem_;

		/// The rendering Group holding the feature points.
		Rendering::GroupRef renderingGroup_;

		/// The rendering Transform object for the relocalization-based phone location.
		Rendering::TransformRef renderingTransformPhoneRelocalization_;

		/// The rendering Transform object for the SLAM-based phone location.
		Rendering::TransformRef renderingTransformPhoneSlam_;

		/// The Rendering Transform object for debug elements.
		Rendering::TransformRef renderingTransformDebug_;

		/// The frame texture for the debug element.
		Rendering::FrameTexture2DRef renderingFrameTextureDebug_;

		/// The timestamp when the latest headset relocalization-based transformation was received.
		std::atomic<Timestamp> latestTimestampHeadsetRelocalization_;

		/// The timestamp when the latest phone relocalization-based transformation was received.
		std::atomic<Timestamp> latestTimestampPhoneRelocalization_;

		/// The timestamp when the latest phone slam-based transformation was received.
		std::atomic<Timestamp> latestTimestampPhoneSlam_;

		/// The 3D object points of the latest feature map.
		Vectors3 latestFeaturePoints_;

		/// The stability factors for the individual 3D object points of the latest feature map, one for each 3D object point.
		Scalars latestFeaturePointStabilityFactors_;

		/// The map to be send to the phone.
		Buffer mapBuffer_;

		/// The VERTS driver for the network communication.
		Network::Verts::SharedDriver vertsDriver_;

		/// The VERTS node containing the camera pose.
		Network::Verts::SharedNode vertsDevicePoseNode_;

		/// The subscription object for new entity events.
		Network::Verts::Driver::NewEntityScopedSubscription newEntityScopedSubscription_;

		/// The subscription object for changed users events.
		Network::Verts::Driver::ChangedUsersScopedSubscription changedUsersScopedSubscription_;

		/// The ids of all active users.
		UnorderedIndexSet64 userIds_;

		/// The experience's lock.
		mutable Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_QUEST_EXPERIENCE_H
