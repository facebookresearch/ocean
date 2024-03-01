// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_PHONE_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_PHONE_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/mapalignment/MapAlignment.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/media/FrameMedium.h"

#include "metaonly/ocean/network/verts/Driver.h"

#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Group.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/tracking/SmoothedTransformation.h"

#include "ocean/tracking/mapbuilding/Unified.h"
#include "ocean/tracking/mapbuilding/UnifiedFeatureMap.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the phone experience for MapAlignment.
 * @see MapAlignmentQuestExperience.
 * @ingroup xrplayground
 */
class MapAlignmentPhoneExperience final :
	public XRPlaygroundExperience,
	public MapAlignment,
	protected Thread
{
	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

		/**
		 * This class implements a storage for the latest feature map.
		 */
		class FeatureMap final : protected Thread
		{
			public:

				/**
				 * Returns the latest map for rendering, if any exists.
				 * @param objectPoints The resulting 3D object points of the feature map
				 * @return True, if a map existed
				 */
				bool latestMapForRendering(Vectors3& objectPoints);

				/**
				 * Returns the latest feautre map for relocalization, if any exists.
				 * @param unifiedFeatureMap The resulting feature map
				 * @return True, if a map existed
				 */
				bool latestFeatureMapForRelocalization(Tracking::MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap);

				/**
				 * Event function for received data from Quest.
				 * @param bitstream The bitstream containing the received data
				 */
				void onReceiveData(IO::InputBitstream& bitstream);

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// True, if this storage holds a valid map for rendering.
				bool hasLatestMapForRendering_ = false;

				/// True, if this storage holds a valid map for relocalization.
				bool hasLatestMapForRelocalization_ = false;

				/// The 3D object points for the feature map.
				Vectors3 mapObjectPoints_;

				/// The ids of the object points in the feature map, one for each 3D object point.
				Indices32 mapObjectPointIds_;

				/// The map mapping object point ids to feature descriptors
				std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> mapUnifiedDescriptorMap_;

				/// The feature map for relocalization.
				Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap_;

				/// The lock for the feature map.
				Lock lock_;
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MapAlignmentPhoneExperience() override;

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
		 * Mouse release event function.
		 * @see Experience::onMouseRelease().
		 */
		void onMouseRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Starts the scanning process.
		 * @return True, if succeeded
		 */
		bool start();

		/**
		 * Stops the scanning process.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Creates a new GravityAndHeadingNativeExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		MapAlignmentPhoneExperience() = default;

		/**
		 * Event function for new tracking samples from the tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from tracker
		 */
		void onTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

		/**
		 * The internal thread function.
		 */
		void threadRun() override;

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

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The world tracker SLAM tracker.
		Devices::Tracker6DOFRef slamTracker6DOF_;

		/// The object id of the scene tracker.
		Devices::Tracker6DOF::ObjectId objectId_ = Devices::Tracker6DOF::invalidObjectId();

		/// The subscription object for sample events from the scene tracker.
		Devices::Measurement::SampleEventSubscription trackerSampleEventSubscription_;

		/// The rendering transform node holding the rendering content.
		Rendering::TransformRef renderingTransformContent_;

		/// The Rendering Transform object for debug elements.
		Rendering::TransformRef renderingTransformDebug_;

		/// The frame texture for the debug element.
		Rendering::FrameTexture2DRef renderingFrameTextureDebug_;

		/// The transformation between the map's world and the headset's world.
		Tracking::SmoothedTransformation hmdWorld_T_phoneWorld_ = Tracking::SmoothedTransformation(0.5);

		/// The latest transformation between SLAM camera and hmd world.
		HomogenousMatrix4 hmdWorld_T_slamCamera_ = HomogenousMatrix4(false);

		/// The latest transformation between relocalization camera and hmd world.
		HomogenousMatrix4 hmdWorld_T_relocalizationCamera_ = HomogenousMatrix4(false);

		/// The frame medium of the video background.
		Media::FrameMediumRef frameMedium_;

		/// The rendering Text node providing user feedback.
		Rendering::TextRef renderingText_;

		/// The VERTS driver for the network communication.
		Network::Verts::SharedDriver vertsDriver_;

		/// The VERTS node containing the camera pose.
		Network::Verts::SharedNode vertsDevicePoseNode_;

		/// The subscription object for changed users events.
		Network::Verts::Driver::ChangedUsersScopedSubscription changedUsersScopedSubscription_;

		/// The subscription object for data receive events.
		Network::Verts::Driver::ReceiveContainerScopedSubscription receiveContainerScopedSubscription_;

		/// The ids of all active users.
		UnorderedIndexSet64 userIds_;

		/// The container holding the latest feature map.
		FeatureMap featureMap_;

		/// The experience's lock.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_PHONE_EXPERIENCE_H
