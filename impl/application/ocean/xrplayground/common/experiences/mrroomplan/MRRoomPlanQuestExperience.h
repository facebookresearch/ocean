// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRROOMPLAN_MR_ROOM_PLAN_QUEST_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRROOMPLAN_MR_ROOM_PLAN_QUEST_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/mrroomplan/MRRoomPlan.h"

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
 * This class implements an experience allowing to create a feature map on Quest which then can be shared with a Phone (with MRRoomPlanPhoneExperience).
 * @see MRRoomPlanPhoneExperience.
 * @ingroup xrplayground
 */
class MRRoomPlanQuestExperience final :
	public XRPlaygroundExperience,
	public MRRoomPlan,
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
				MapHandlingThread(MRRoomPlanQuestExperience& owner, MapData& mapData);

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// The owner of this object.
				MRRoomPlanQuestExperience& owner_;

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
		virtual ~MRRoomPlanQuestExperience();

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
		MRRoomPlanQuestExperience();

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

		/**
		 * Event function for received data on a channel.
		 * @param driver The VERTS driver sending the event
		 * @param sessionId The session id of the sender, will be valid
		 * @param userId The user id of the sender, will be valid
		 * @param identifier The identifier of the container, will be valid
		 * @param version The version of the container, with range [0, infinity)
		 * @param buffer The buffer of the container
		 */
		void onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer);

	protected:

		/// The map holding the latest room objects.
		Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap roomObjectMap_;

		/// The rendering Group object holding the planar room objects.
		Rendering::GroupRef renderingGroupPlanarRoomObjects_;

		/// The rendering Group object holding the volumetric room objects.
		Rendering::GroupRef renderingGroupVolumetricRoomObjects_;

		/// The rendering Text object holding the instruction text.
		Rendering::TextRef renderingText_;

		/// The rendering Transform object for the SLAM-based phone location.
		Rendering::TransformRef renderingTransformPhoneSlam_;

		/// The text node showing instructions on the phone.
		Rendering::TextRef renderingTextPhoneInstruction_;

		/// The timestamp when the latest phone slam-based transformation was received.
		std::atomic<Timestamp> latestTimestampPhoneSlam_;

		/// The alpha value to be used when rendering volumetric objects.
		float volumetricObjectsAlpha_ = 0.75f;

		/// The map to be send to the phone.
		Buffer mapBuffer_;

		/// The subscription object for new entity events.
		Network::Verts::Driver::NewEntityScopedSubscription newEntityScopedSubscription_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRROOMPLAN_MR_ROOM_PLAN_QUEST_EXPERIENCE_H
