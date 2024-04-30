// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/quest/QuestLiveVideo.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/Thread.h"

#include "ocean/platform/meta/quest/Device.h"

#include "metaonly/ocean/platform/meta/quest/MetaonlyDevice.h"

namespace Ocean
{

using namespace Platform::Meta::Quest::Sensors;

namespace Media
{

namespace Quest
{

QuestLiveVideo::FrameProviderManager::FrameProviderManager()
{
	// we will use the image content immediately
	initialized_ = initialize(FCM_USE_IMMEDIATELY);

	if (!initialized_)
	{
		Log::error() << "Failed to initialize QuestLiveVideo's FrameProvider";
	}
}

QuestLiveVideo::FrameProviderManager::~FrameProviderManager()
{
	ocean_assert(consumerPairs_.empty());
	ocean_assert(cameraFrameTypeCounterMap_.empty());
}

bool QuestLiveVideo::FrameProviderManager::registerLiveVideo(const OSSDK::Sensors::v3::FrameType& cameraFrameType,  QuestLiveVideo& liveVideo)
{
	const Timestamp startTimestamp(true);

	while (isBusyUnregisteringCameraFrameType_.load())
	{
		if (Timestamp(true) > startTimestamp + 5.0)
		{
			ocean_assert(false && "Quest live video registration timed out after 5 seconds, this should never happen!");
			return false;
		}

		Thread::sleep(1u);
	}

	const ScopedLock scopedLock(lock_);

	if (!initialized_)
	{
		return false;
	}

	for (ConsumerPair& consumerPair : consumerPairs_)
	{
		if (consumerPair.second == &liveVideo)
		{
			ocean_assert(false && "Already registered!");
			return false;
		}
	}

	if (cameraFrameTypeCounterMap_.find(cameraFrameType) == cameraFrameTypeCounterMap_.cend())
	{
		// this is the first live video object with this specific camera frame type, we need to start receiving the camera frames

		if (!startReceivingCameraFrames(cameraFrameType, CT_ALL_CAMERAS, true /*useRealtimeCalibration*/))
		{
			return false;
		}
	}

	cameraFrameTypeCounterMap_[cameraFrameType]++;

	consumerPairs_.emplace_back(cameraFrameType, &liveVideo);

	return true;
}

bool QuestLiveVideo::FrameProviderManager::unregisterLiveVideo(QuestLiveVideo& liveVideo)
{
	TemporaryScopedLock scopedLock(lock_);

	OSSDK::Sensors::v3::FrameType delayedUnregisterCameraFrameType = OSSDK::Sensors::v3::FrameType::NotReceived;

	ocean_assert(initialized_);

	for (size_t n = 0; n < consumerPairs_.size(); ++n)
	{
		if (consumerPairs_[n].second == &liveVideo)
		{
			const OSSDK::Sensors::v3::FrameType& cameraFrameType = consumerPairs_[n].first;

			const CameraFrameTypeCounterMap::iterator iCounter = cameraFrameTypeCounterMap_.find(cameraFrameType);

			ocean_assert(iCounter != cameraFrameTypeCounterMap_.cend());
			ocean_assert(iCounter->second >= 1u);

			--iCounter->second;

			if (iCounter->second == 0u)
			{
				// this is the last live video object with this specific camera frame type, we need to stop receiving the camera frames

				delayedUnregisterCameraFrameType = cameraFrameType;

				cameraFrameTypeCounterMap_.erase(iCounter);
			}

			consumerPairs_[n] = consumerPairs_.back();
			consumerPairs_.pop_back();

			isBusyUnregisteringCameraFrameType_ = delayedUnregisterCameraFrameType != OSSDK::Sensors::v3::FrameType::NotReceived;

			scopedLock.release();

			if (delayedUnregisterCameraFrameType != OSSDK::Sensors::v3::FrameType::NotReceived)
			{
				const bool result = stopReceivingCameraFrames(delayedUnregisterCameraFrameType);
				ocean_assert_and_suppress_unused(result, result);

				isBusyUnregisteringCameraFrameType_ = false;
			}

			return true;
		}
	}

	ocean_assert(false && "Not registered!");
	return false;
}

void QuestLiveVideo::FrameProviderManager::onFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType& cameraType, const Frames& frames, const SharedAnyCamerasD& cameras, const HomogenousMatrixD4& world_T_device, const HomogenousMatricesD4& device_T_cameras, const FrameMetadatas& frameMetadatas)
{
	const ScopedLock scopedLock(lock_);

	for (ConsumerPair& consumerPair : consumerPairs_)
	{
		if (consumerPair.first == cameraFrameType)
		{
			consumerPair.second->onFrames(cameraFrameType, cameraType, frames, cameras, world_T_device, device_T_cameras, frameMetadatas);
		}
	}
}

QuestLiveVideo::QuestLiveVideo(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	LiveVideo(url),
	startTimestamp_(false),
	pauseTimestamp_(false),
	stopTimestamp_(false)
{
	libraryName_ = nameQuestLibrary();

	isValid_ = false;

	OSSDK::Sensors::v3::FrameType cameraFrameType;
	unsigned int cameraIndex = (unsigned int)(-1);

	if (determineCameraFrameType(url, cameraFrameType, cameraIndex))
	{
		if (FrameProviderManager::get().registerLiveVideo(cameraFrameType, *this))
		{
			cameraFrameType_ = cameraFrameType;
			cameraIndex_ = cameraIndex;

			frameCollection_.setCapacity(10);

			isValid_ = true;
		}
	}
}

QuestLiveVideo::~QuestLiveVideo()
{
	if (isValid_)
	{
		FrameProviderManager::get().unregisterLiveVideo(*this);
	}
}

bool QuestLiveVideo::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

Timestamp QuestLiveVideo::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(startTimestamp_);

	return timestamp;
}

Timestamp QuestLiveVideo::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(pauseTimestamp_);

	return timestamp;
}

Timestamp QuestLiveVideo::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(stopTimestamp_);

	return timestamp;
}

HomogenousMatrixD4 QuestLiveVideo::device_T_camera() const
{
	const ScopedLock scopedLock(lock_);

	return device_T_camera_;
}

bool QuestLiveVideo::determineCameraFrameType(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex)
{
	ocean_assert(!url.empty());

	switch (std::underlying_type<Platform::Meta::Quest::Device::DeviceType>::type(Platform::Meta::Quest::Device::deviceType()))
	{
		case Platform::Meta::Quest::Device::DT_QUEST:
		case Platform::Meta::Quest::Device::DT_QUEST_2:
			return determineCameraFrameTypeQuest1And2(url, cameraFrameType, cameraIndex);

		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			return determineCameraFrameTypeQuestPro(url, cameraFrameType, cameraIndex);

		case Platform::Meta::Quest::Device::DT_QUEST_3:
			return determineCameraFrameTypeQuest3(url, cameraFrameType, cameraIndex);

		case Platform::Meta::Quest::MetaonlyDevice::DT_VENTURA:
			return determineCameraFrameTypeVentura(url, cameraFrameType, cameraIndex);

		case Platform::Meta::Quest::Device::DT_UNKNOWN:
			// Nothing to do here; will fail below;
			break;

		// Intentionally no default case here!
	}

	Log::error() << "Failed to determine the camera frame type because the device type is invalid.";
	ocean_assert(false && "Never be here!");

	return false;
}

bool QuestLiveVideo::start()
{
	const ScopedLock scopedLock(lock_);

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	return true;
}

bool QuestLiveVideo::pause()
{
	const ScopedLock scopedLock(lock_);

	startTimestamp_.toInvalid();
	pauseTimestamp_.toNow();
	stopTimestamp_.toInvalid();

	return true;
}

bool QuestLiveVideo::stop()
{
	const ScopedLock scopedLock(lock_);

	pauseTimestamp_.toInvalid();
	startTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	return true;
}

void QuestLiveVideo::onFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const FrameProviderManager::CameraType& /*cameraType*/, const Frames& frames, const SharedAnyCamerasD& cameras, const HomogenousMatrixD4& /*world_T_device*/, const HomogenousMatricesD4& device_T_cameras, const FrameProviderManager::FrameMetadatas& /*frameMetadatas*/)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(cameraFrameType == cameraFrameType_);
	ocean_assert(cameraIndex_ < frames.size());

	// we simply ignore the frame if this LiveVideo object is not started

	if (startTimestamp_.isValid() && cameraIndex_ < frames.size())
	{
		ocean_assert(frames.size() == cameras.size());
		ocean_assert(frames.size() == device_T_cameras.size());

		device_T_camera_ = device_T_cameras[cameraIndex_];

		Frame frame(frames[cameraIndex_], Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
		SharedAnyCamera anyCamera = AnyCamera::convert(cameras[cameraIndex_]);

		deliverNewFrame(std::move(frame), std::move(anyCamera));
	}
}

bool QuestLiveVideo::determineCameraFrameTypeQuest1And2(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex)
{
	ocean_assert(!url.empty());

	std::string liveVideoType;
	unsigned int liveVideoId = (unsigned int)(-1);

	if (!determineLiveVideoTypeAndId(url, liveVideoType, liveVideoId))
	{
		return false;
	}

	if (liveVideoType == "LiveVideoId")
	{
		if (liveVideoId < 4u)
		{
			liveVideoType = "HeadsetCameraId";
			// liveVideoId stays unchanged
		}
		else if (liveVideoId < 8u)
		{
			liveVideoType = "HandCameraId";
			liveVideoId = liveVideoId - 4u;
		}
		else if (liveVideoId < 12u)
		{
			liveVideoType = "ControllerCameraId";
			liveVideoId = liveVideoId - 8u;
		}
	}

	if (liveVideoId >= 4u)
	{
		ocean_assert(false && "Invalid live video index");
		return false;
	}

	// Mapping the LiveVideo IDs to the camera indices used by OSSDK
	//
	// | Camera       | LiveVideo Id | OSSDK camera index |
	// +--------------+--------------+--------------------+
	// | bottom-left  |            0 |                  1 |
	// | bottom-right |            1 |                  2 |
	// | top-left     |            2 |                  0 |
	// | top-right    |            3 |                  3 |

	constexpr unsigned int cameraIndexFromLiveVideoId[4] = { 1u, 2u, 0u, 3u };

	cameraIndex = cameraIndexFromLiveVideoId[liveVideoId];

	if (liveVideoType == "HeadsetCameraId" || (liveVideoType == "StereoCamera0Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::Headset;

		return true;
	}
	else if (liveVideoType == "HandCameraId" || (liveVideoType == "StereoCamera1Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::Hand;

		return true;
	}
	else if (liveVideoType == "ControllerCameraId" || (liveVideoType == "StereoCamera2Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::Controller;

		return true;
	}

	cameraIndex = (unsigned int)(-1);

	return false;
}

bool QuestLiveVideo::determineCameraFrameTypeQuestPro(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex)
{
	ocean_assert(!url.empty());

	std::string liveVideoType;
	unsigned int liveVideoId = (unsigned int)(-1);

	if (!determineLiveVideoTypeAndId(url, liveVideoType, liveVideoId))
	{
		return false;
	}

	if (liveVideoType == "LiveVideoId")
	{
		if (liveVideoId < 4u)
		{
			liveVideoType = "GenericSlot0CameraId";
			// liveVideoId stays unchanged
		}
		else if (liveVideoId < 8u)
		{
			liveVideoType = "GenericSlot1CameraId";
			liveVideoId = liveVideoId - 4u;
		}
		else if (liveVideoId < 12u)
		{
			liveVideoType = "GenericSlot2CameraId";
			liveVideoId = liveVideoId - 8u;
		}
		else if (liveVideoId == 12u)
		{
			liveVideoType = "ColorCameraId";
			liveVideoId = 0u;
		}
		else
		{
			liveVideoType = "";
			liveVideoId = (unsigned int)(-1);
		}
	}

	if ((liveVideoType == "GenericSlot0CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera0Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot0;
		cameraIndex = liveVideoId;

		return true;
	}
	else if ((liveVideoType == "GenericSlot1CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera1Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot1;
		cameraIndex = liveVideoId;

		return true;
	}
	else if ((liveVideoType == "GenericSlot2CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera2Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot2;
		cameraIndex = liveVideoId;

		return true;
	}
	else if (liveVideoType == "ColorCameraId" && liveVideoId == 0)
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::Color;
		cameraIndex = 0u;

		return true;
	}

	Log::error() << "Failed to determine camera frame type for <" << url << ">.";

	return false;
}

bool QuestLiveVideo::determineCameraFrameTypeQuest3(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex)
{
	ocean_assert(!url.empty());

	std::string liveVideoType;
	unsigned int liveVideoId = (unsigned int)(-1);

	if (!determineLiveVideoTypeAndId(url, liveVideoType, liveVideoId))
	{
		return false;
	}

	if (liveVideoType == "LiveVideoId")
	{
		if (liveVideoId < 4u)
		{
			liveVideoType = "GenericSlot0CameraId";
			// liveVideoId stays unchanged
		}
		else if (liveVideoId < 8u)
		{
			liveVideoType = "GenericSlot1CameraId";
			liveVideoId = liveVideoId - 4u;
		}
		else if (liveVideoId < 12u)
		{
			liveVideoType = "GenericSlot2CameraId";
			liveVideoId = liveVideoId - 8u;
		}
		else if (liveVideoId < 14u)
		{
			liveVideoType = "ColorCameraId";
			liveVideoId = liveVideoId - 12u;
		}
		else
		{
			liveVideoType = "";
			liveVideoId = (unsigned int)(-1);
		}
	}

	if ((liveVideoType == "GenericSlot0CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera0Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot0;
		cameraIndex = liveVideoId;

		return true;
	}
	else if ((liveVideoType == "GenericSlot1CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera1Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot1;
		cameraIndex = liveVideoId;

		return true;
	}
	else if ((liveVideoType == "GenericSlot2CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera2Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot2;
		cameraIndex = liveVideoId;

		return true;
	}
	else if ((liveVideoType == "ColorCameraId" && liveVideoId < 2u) || (liveVideoType == "StereoCamera3Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::Color;
		cameraIndex = liveVideoId;

		return true;
	}

	Log::error() << "Failed to determine camera frame type for <" << url << ">.";

	return false;
}

bool QuestLiveVideo::determineCameraFrameTypeVentura(const std::string& url, OSSDK::Sensors::v3::FrameType& cameraFrameType, unsigned int& cameraIndex)
{
	ocean_assert(!url.empty());

	std::string liveVideoType;
	unsigned int liveVideoId = (unsigned int)(-1);

	if (!determineLiveVideoTypeAndId(url, liveVideoType, liveVideoId))
	{
		return false;
	}

	if (liveVideoType == "LiveVideoId")
	{
		if (liveVideoId < 4u)
		{
			liveVideoType = "GenericSlot0CameraId";
			// liveVideoId stays unchanged
		}
		else if (liveVideoId < 8u)
		{
			liveVideoType = "GenericSlot1CameraId";
			liveVideoId = liveVideoId - 4u;
		}
		else if (liveVideoId < 12u)
		{
			liveVideoType = "GenericSlot2CameraId";
			liveVideoId = liveVideoId - 8u;
		}
		else if (liveVideoId < 14u)
		{
			liveVideoType = "ColorCameraId";
			liveVideoId = liveVideoId - 12u;
		}
		else
		{
			liveVideoType = "";
			liveVideoId = (unsigned int)(-1);
		}
	}

	if ((liveVideoType == "GenericSlot0CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera0Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot0;
		cameraIndex = liveVideoId;

		return true;
	}
	else if ((liveVideoType == "GenericSlot1CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera1Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot1;
		cameraIndex = liveVideoId;

		return true;
	}
	else if ((liveVideoType == "GenericSlot2CameraId" && liveVideoId < 4u) || (liveVideoType == "StereoCamera2Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::GenericSlot2;
		cameraIndex = liveVideoId;

		return true;
	}
	else if ((liveVideoType == "ColorCameraId" && liveVideoId < 2u) || (liveVideoType == "StereoCamera3Id" && liveVideoId < 2u))
	{
		cameraFrameType = OSSDK::Sensors::v3::FrameType::Color;
		cameraIndex = liveVideoId;

		return true;
	}

	Log::error() << "Failed to determine camera frame type for <" << url << ">.";

	return false;
}

bool QuestLiveVideo::determineLiveVideoTypeAndId(const std::string& url, std::string& liveVideoType, unsigned int& liveVideoId)
{
	ocean_assert(!url.empty());

	const std::string::size_type delimiterPosition = url.find_first_of(":");

	if (delimiterPosition == std::string::npos || (delimiterPosition + 1) == url.size())
	{
		return false;
	}

	liveVideoType = url.substr(0, delimiterPosition);

	if (liveVideoType.empty())
	{
		return false;
	}

	int liveVideoIdInt = -1;
	String::isInteger32(url.substr(delimiterPosition + 1), &liveVideoIdInt);

	if (liveVideoIdInt >= 0)
	{
		liveVideoId = (unsigned int)(liveVideoIdInt);

		return true;
	}

	return false;
}

}

}

}
