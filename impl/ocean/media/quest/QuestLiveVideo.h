// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_QUEST_LIVE_VIDEO_H
#define META_OCEAN_MEDIA_QUEST_LIVE_VIDEO_H

#include "ocean/media/quest/Quest.h"

#include "ocean/base/Singleton.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/platform/meta/quest/sensors/FrameProvider.h"

#include <atomic>

namespace Ocean
{

namespace Media
{

namespace Quest
{

/**
 * This class implements an live video class for Ouclus using SDAU as input source.
 * @ingroup mediaquest
 */
class OCEAN_MEDIA_QUEST_EXPORT QuestLiveVideo : virtual public LiveVideo
{
	friend class QuestLibrary;
	friend class QuestManager;

	protected:

		/**
		 * This class implements the manager forwarding the image content from SDAU's FrameProvider to the individual QuestLiveVideo objects.
		 */
		class FrameProviderManager :
			public Singleton<FrameProviderManager>,
			protected Platform::Meta::Quest::Sensors::FrameProvider
		{
			friend class Singleton<FrameProviderManager>;
			friend class QuestLiveVideo;

			protected:

				/**
				 * Definition of a pair combining a camera frame type with a Live Video object.
				 */
				typedef std::pair<OSSDK::Sensors::v3::FrameType, QuestLiveVideo*> ConsumerPair;

				/**
				 * Definition of a vector holding ConsumerPair objects.
				 */
				typedef std::vector<ConsumerPair> ConsumerPairs;

				/**
				 * Definition of an unordered map mapping camera frame types to counters.
				 */
				typedef std::unordered_map<OSSDK::Sensors::v3::FrameType, unsigned int> CameraFrameTypeCounterMap;

			public:

				/**
				 * Registers a new Live Video object as a frame consumer.
				 * @param cameraFrameType The camera frame type of the new consumer
				 * @param liveVideo The consumer to register
				 * @return True, if succeeded
				 */
				bool registerLiveVideo(const OSSDK::Sensors::v3::FrameType& cameraFrameType, QuestLiveVideo& liveVideo);

				/**
				 * Unregisters a Live Video object as a frame consumer.
				 * @param liveVideo The consumer to unregister, must have been registered before
				 * @return True, if succeeded
				 */
				bool unregisterLiveVideo(QuestLiveVideo& liveVideo);

			protected:

				/**
				 * Creates a new manager object.
				 */
				FrameProviderManager();

				/**
				 * Destructs a manager object.
				 */
				~FrameProviderManager() override;

				/**
				 * Event function for new frames.
				 * @see FrameProvider::onFrames().
				 */
				void onFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType& cameraType, const Frames& frames, const SharedAnyCamerasD& cameras, const HomogenousMatrixD4& world_T_device, const HomogenousMatricesD4& device_T_cameras, const FrameMetadatas& frameMetadatas) override;

			protected:

				/// True, if the manager can be used.
				bool initialized_ = false;

				/// The currently registered Live Video objects.
				ConsumerPairs consumerPairs_;

				/// The counter for the individual camera frame types.
				CameraFrameTypeCounterMap cameraFrameTypeCounterMap_;

				/// True, if the manager is currently registering/unregistering a video.
				std::atomic_bool isBusyUnregisteringCameraFrameType_ = false;

				/// The manager's lock.
				Lock lock_;
		};

	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @see FiniteMedium::startTimestamp().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see FiniteMedium::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see FiniteMedium::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

		/**
		 * Returns the transformation between the camera and device.
		 * @see FrameMedium::device_T_camera().
		 */
		HomogenousMatrixD4 device_T_camera() const override;

		/**
		 * Determines the camera frame type and the camera index for a given media url.
		 * The media urls are device-specific. For a definition of the accepted identifiers, check the corresponding device-specific function `determineCameraFrameType*()` in this class.
		 * @param url The media url for which the camera frame type and index will be determined, must not be empty
		 * @param cameraFrameType The resulting camera frame type
		 * @param cameraIndex The resulting camera index (of all camera frames with matching camera frame type)
		 * @return True, if succeeded
		 * @sa determineCameraFrameTypeQuest1And2()
		 * @sa determineCameraFrameTypeQuestPro()
		 */
		static bool determineCameraFrameType(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex);

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit QuestLiveVideo(const std::string& url);

		/**
		 * Destructs the live video object.
		 */
		~QuestLiveVideo() override;

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause():
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

		/**
		 * Event function for new frames.
		 * @param cameraFrameType The camera frame type of the frames
		 * @param cameraType The camera type of the frames
		 * @param frames The new frames
		 * @param cameras The camera profiles of the frames, one for each frame, based on online calibration if available; otherwise factory calibration
		 * @param world_T_device The transformation between device and world
		 * @param device_T_cameras The transformations between cameras and device, one for each camera, based on online calibration if available; otherwise factory calibration
		 * @param frameMetadatas The frame metadata, one object associated with each new frame
		 */
		void onFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const FrameProviderManager::CameraType& cameraType, const Frames& frames, const SharedAnyCamerasD& cameras, const HomogenousMatrixD4& world_T_device, const HomogenousMatricesD4& device_T_cameras, const FrameProviderManager::FrameMetadatas& frameMetadatas);

		/**
		 * Determines the camera frame type and the camera index for a given media url for Quest 1 and 2.
		 * The mapping is defined as follows:
		 *
		 * | Description                         | `LiveVideoId:` | `IOTCameraId:` | `HandCameraId:` | `ControllerCameraId:` | `StereoCamera0Id:` | `StereoCamera1Id:` | `StereoCamera2Id:` |
		 * |:------------------------------------|:--------------:|:--------------:|:---------------:|:---------------------:|:------------------:|:------------------:|:------------------:|
		 * | Bottom-left camera (IOT)            |              0 |             0  |                 |                       |                  0 |                    |                    |
		 * | Bottom-right camera (IOT)           |              1 |             1  |                 |                       |                  1 |                    |                    |
		 * | Top-left camera (IOT)               |              2 |             2  |                 |                       |                    |                    |                    |
		 * | Top-right camera (IOT)              |              3 |             3  |                 |                       |                    |                    |                    |
		 * | Bottom-left camera (hand-tracking)  |              4 |                |              0  |                       |                    |                  0 |                    |
		 * | Bottom-right camera (hand-tracking) |              5 |                |              1  |                       |                    |                  1 |                    |
		 * | Top-left camera (hand-tracking)     |              6 |                |              2  |                       |                    |                    |                    |
		 * | Top-right camera (hand-tracking)    |              7 |                |              3  |                       |                    |                    |                    |
		 * | Bottom-left camera (controller)     |              8 |                |                 |                     0 |                    |                    |                  0 |
		 * | Bottom-right camera (controller)    |              9 |                |                 |                     1 |                    |                    |                  1 |
		 * | Top-left camera (controller)        |             10 |                |                 |                     2 |                    |                    |                    |
		 * | Top-right camera (controller)       |             11 |                |                 |                     3 |                    |                    |                    |
		 *
		 * @param url The media url for which the camera frame type and index will be determined, must not be empty
		 * @param cameraFrameType The resulting camera frame type
		 * @param cameraIndex The resulting camera index (of all camera frames with matching camera frame type)
		 * @return True, if succeeded
		 */
		static bool determineCameraFrameTypeQuest1And2(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex);

		/**
		 * Determines the camera frame type and the camera index for a given media url for Quest Pro.
		 * The mapping is defined as follows:
		 *
		 * | Description                           | `LiveVideoId:` | `GenericSlot0Id:` | `GenericSlot1Id:` | `GenericSlot2Id:` | `ColorCameraId:` | `StereoCamera0Id:` | `StereoCamera1Id:` | `StereoCamera2Id:` |
		 * |:--------------------------------------|:--------------:|:-----------------:|:-----------------:|:-----------------:|:----------------:|:------------------:|:------------------:|:------------------:|
		 * | Glacier, left (GENERIC_SLOT_0)        |              0 |                 0 |                   |                   |                  |                  0 |                    |                    |
		 * | Glacier, right (GENERIC_SLOT_0)       |              1 |                 1 |                   |                   |                  |                  1 |                    |                    |
		 * | Canyon, left (GENERIC_SLOT_0)         |              2 |                 2 |                   |                   |                  |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_0)        |              3 |                 3 |                   |                   |                  |                    |                    |                    |
		 * | Glacier, left (GENERIC_SLOT_1)        |              4 |                   |                 0 |                   |                  |                    |                  0 |                    |
		 * | Glacier, right (GENERIC_SLOT_1)       |              5 |                   |                 1 |                   |                  |                    |                  1 |                    |
		 * | Canyon, left (GENERIC_SLOT_1)         |              6 |                   |                 2 |                   |                  |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_1)        |              7 |                   |                 3 |                   |                  |                    |                    |                    |
		 * | Glacier, left (GENERIC_SLOT_2)        |              8 |                   |                   |                 0 |                  |                    |                    |                  0 |
		 * | Glacier, right (GENERIC_SLOT_2)       |              9 |                   |                   |                 1 |                  |                    |                    |                  1 |
		 * | Canyon, left (GENERIC_SLOT_2)         |             10 |                   |                   |                 2 |                  |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_2)        |             11 |                   |                   |                 3 |                  |                    |                    |                    |
		 * | Teton camera (COLOR)                  |             12 |                   |                   |                   |                0 |                    |                    |                    |
		 *
		 * @param url The media url for which the camera frame type and index will be determined, must not be empty
		 * @param cameraFrameType The resulting camera frame type
		 * @param cameraIndex The resulting camera index (of all camera frames with matching camera frame type)
		 * @return True, if succeeded
		 */
		static bool determineCameraFrameTypeQuestPro(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex);

		/**
		 * Determines the camera frame type and the camera index for a given media url for Quest 3.
		 * The mapping is defined as follows:
		 *
		 * | Description                           | `LiveVideoId:` | `GenericSlot0Id:` | `GenericSlot1Id:` | `GenericSlot2Id:` | `ColorCameraId:` | `StereoCamera0Id:` | `StereoCamera1Id:` | `StereoCamera2Id:` | `StereoCamera3Id:` |
		 * |:--------------------------------------|:--------------:|:-----------------:|:-----------------:|:-----------------:|:----------------:|:------------------:|:------------------:|:------------------:|:------------------:|
		 * | Glacier, left (GENERIC_SLOT_0)        |              0 |                 0 |                   |                   |                  |                  0 |                    |                    |                    |
		 * | Glacier, right (GENERIC_SLOT_0)       |              1 |                 1 |                   |                   |                  |                  1 |                    |                    |                    |
		 * | Canyon, left (GENERIC_SLOT_0)         |              2 |                 2 |                   |                   |                  |                    |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_0)        |              3 |                 3 |                   |                   |                  |                    |                    |                    |                    |
		 * | Glacier, left (GENERIC_SLOT_1)        |              4 |                   |                 0 |                   |                  |                    |                  0 |                    |                    |
		 * | Glacier, right (GENERIC_SLOT_1)       |              5 |                   |                 1 |                   |                  |                    |                  1 |                    |                    |
		 * | Canyon, left (GENERIC_SLOT_1)         |              6 |                   |                 2 |                   |                  |                    |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_1)        |              7 |                   |                 3 |                   |                  |                    |                    |                    |                    |
		 * | Glacier, left (GENERIC_SLOT_2)        |              8 |                   |                   |                 0 |                  |                    |                    |                  0 |                    |
		 * | Glacier, right (GENERIC_SLOT_2)       |              9 |                   |                   |                 1 |                  |                    |                    |                  1 |                    |
		 * | Canyon, left (GENERIC_SLOT_2)         |             10 |                   |                   |                 2 |                  |                    |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_2)        |             11 |                   |                   |                 3 |                  |                    |                    |                    |                    |
		 * | Jackson, left (COLOR)                 |             12 |                   |                   |                   |                0 |                    |                    |                    |                  0 |
		 * | Jackson, right (COLOR)                |             13 |                   |                   |                   |                1 |                    |                    |                    |                  1 |
		 *
		 * @param url The media url for which the camera frame type and index will be determined, must not be empty
		 * @param cameraFrameType The resulting camera frame type
		 * @param cameraIndex The resulting camera index (of all camera frames with matching camera frame type)
		 * @return True, if succeeded
		 */
		static bool determineCameraFrameTypeQuest3(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex);

		/**
		 * Determines the camera frame type and the camera index for a given media url for Quest 3.
		 * The mapping is defined as follows:
		 *
		 * | Description                           | `LiveVideoId:` | `GenericSlot0Id:` | `GenericSlot1Id:` | `GenericSlot2Id:` | `ColorCameraId:` | `StereoCamera0Id:` | `StereoCamera1Id:` | `StereoCamera2Id:` | `StereoCamera3Id:` |
		 * |:--------------------------------------|:--------------:|:-----------------:|:-----------------:|:-----------------:|:----------------:|:------------------:|:------------------:|:------------------:|:------------------:|
		 * | Canyon, front-left (GENERIC_SLOT_0)   |              0 |                 0 |                   |                   |                  |                  0 |                    |                    |                    |
		 * | Canyon, front-right (GENERIC_SLOT_0)  |              1 |                 1 |                   |                   |                  |                  1 |                    |                    |                    |
		 * | Canyon, left (GENERIC_SLOT_0)         |              2 |                 2 |                   |                   |                  |                    |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_0)        |              3 |                 3 |                   |                   |                  |                    |                    |                    |                    |
		 * | Canyon, front-left (GENERIC_SLOT_1)   |              4 |                   |                 0 |                   |                  |                    |                  0 |                    |                    |
		 * | Canyon, front-right (GENERIC_SLOT_1)  |              5 |                   |                 1 |                   |                  |                    |                  1 |                    |                    |
		 * | Canyon, left (GENERIC_SLOT_1)         |              6 |                   |                 2 |                   |                  |                    |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_1)        |              7 |                   |                 3 |                   |                  |                    |                    |                    |                    |
		 * | Canyon, front-left (GENERIC_SLOT_2)   |              8 |                   |                   |                 0 |                  |                    |                    |                  0 |                    |
		 * | Canyon, front-right (GENERIC_SLOT_2)  |              9 |                   |                   |                 1 |                  |                    |                    |                  1 |                    |
		 * | Canyon, left (GENERIC_SLOT_2)         |             10 |                   |                   |                 2 |                  |                    |                    |                    |                    |
		 * | Canyon, right (GENERIC_SLOT_2)        |             11 |                   |                   |                 3 |                  |                    |                    |                    |                    |
		 * | Jackson, left (COLOR)                 |             12 |                   |                   |                   |                0 |                    |                    |                    |                  0 |
		 * | Jackson, right (COLOR)                |             13 |                   |                   |                   |                1 |                    |                    |                    |                  1 |
		 *
		 * @param url The media url for which the camera frame type and index will be determined, must not be empty
		 * @param cameraFrameType The resulting camera frame type
		 * @param cameraIndex The resulting camera index (of all camera frames with matching camera frame type)
		 * @return True, if succeeded
		 */
		static bool determineCameraFrameTypeVentura(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex);

		/**
		 * Extract the type and camera index of a given media url.
		 * @param url The media url for which the live video type and id will be determined, must not be empty
		 * @param liveVideoType The resulting live video type that will be extracted from the URL
		 * @param liveVideoId The resulting live video id that will be extracted from the URL
		 * @return True, if succeeded
		 */
		static bool determineLiveVideoTypeAndId(const std::string& url, std::string& liveVideoType, unsigned int& liveVideoId);

	protected:

		/// The camera frame type this live video objects has.
		OSSDK::Sensors::v3::FrameType cameraFrameType_ = OSSDK::Sensors::v3::FrameType::Invalid;

		/// The camera index (of all camera frames with matching camera frame type) of this object.
		unsigned int cameraIndex_ = (unsigned int)(-1);

		/// The transformation between camera and device, invalid until the first frame arrived.
		HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(false);

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Pause timestamp.
		Timestamp pauseTimestamp_;

		/// Stop timestamp.
		Timestamp stopTimestamp_;
};

}

}

}

#endif // META_OCEAN_MEDIA_QUEST_LIVE_VIDEO_H
