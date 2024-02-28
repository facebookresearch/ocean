// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/StereoImageProvider.h"
#include "ocean/media/vrs/Utilities.h"

#include "ocean/io/File.h"

#include "ocean/io/vrs/Reader.h"

#include "ocean/media/Manager.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

StereoImageProvider::StereoImageProvider(StereoImageProvider&& stereoImageProvider)
{
	*this = std::move(stereoImageProvider);
}

StereoImageProvider::StereoImageProvider(const std::string& vrsFilename, const std::string& posesFilename) :
	frameTimestampA_(false),
	frameTimestampB_(false),
	droppedFrameA_(0),
	droppedFrameB_(0),
	isValid_(false)
{
	std::string recordableA;
	std::string recordableB;

	if (!determineLowerStereoCameraRecordables(vrsFilename, recordableA, recordableB))
	{
		return;
	}

	imageSequenceA_ = Manager::get().newMedium(vrsFilename + "/" + recordableA, "VRS", Media::Medium::IMAGE_SEQUENCE);
	imageSequenceB_ = Manager::get().newMedium(vrsFilename + "/" + recordableB, "VRS", Media::Medium::IMAGE_SEQUENCE);

	if (imageSequenceA_.isNull() || imageSequenceB_.isNull())
	{
		Log::error() << "Failed to load VRS stream, media library registered?, correct VRS file?";

		release();
		return;
	}

	imageSequenceA_->setMode(Media::ImageSequence::SM_EXPLICIT);
	imageSequenceA_->setLoop(false);

	imageSequenceB_->setMode(Media::ImageSequence::SM_EXPLICIT);
	imageSequenceB_->setLoop(false);

	if (!imageSequenceA_->start() || !imageSequenceB_->start())
	{
		release();
		return;
	}

	if (!posesFilename.empty())
	{
		const IO::File posesFile(posesFilename);

		if (posesFile.exists())
		{
			map_world_T_device_ = Media::VRS::Utilities::loadPosesMap(posesFile());
		}

		if (map_world_T_device_.isEmpty())
		{
			Log::error() << "Could not extract device poses from poses file \"" << posesFile() << "\"";

			release();
			return;
		}
	}
	else
	{
		// the VRS file may contain the poses

		if (map_world_T_device_.isEmpty())
		{
			// we try to load the stream based on the type id and flavor

			IO::VRS::Reader reader;
			IO::VRS::PlayableHomogenousMatrix4 playableHomographyMatrix4;
			reader.addPlayable(&playableHomographyMatrix4, vrs::RecordableTypeId::PoseRecordableClass, "world_T_device");

			if (reader.read(vrsFilename) == 1)
			{
				map_world_T_device_ = playableHomographyMatrix4.homogenousMatrix4Map();
			}
		}

		if (map_world_T_device_.isEmpty())
		{
			// as a backup, we try to load the stream based on the name

			IO::VRS::Reader reader;
			IO::VRS::PlayableHomogenousMatrix4 playableHomographyMatrix4;
			reader.addPlayable(&playableHomographyMatrix4, "Pose Data Class #1");

			if (reader.read(vrsFilename) == 1)
			{
				map_world_T_device_ = playableHomographyMatrix4.homogenousMatrix4Map();
			}
		}
	}

	{
		// read the hand poses from the VRS file, if they exist
		IO::VRS::Reader reader;

		IO::VRS::PlayableHomogenousMatrices4 playableHandPosesLeft;
		IO::VRS::PlayableHomogenousMatrices4 playableHandPosesRight;

		reader.addPlayable(&playableHandPosesLeft, vrs::RecordableTypeId::PoseRecordableClass, "hand_pose_left");
		reader.addPlayable(&playableHandPosesRight, vrs::RecordableTypeId::PoseRecordableClass, "hand_pose_right");

		if (reader.read(vrsFilename) == 2)
		{
			map_leftHandPoses_ = std::move(playableHandPosesLeft.homogenousMatrices4Map());
			map_rightHandPoses_ = std::move(playableHandPosesRight.homogenousMatrices4Map());
		}
	}

	isValid_ = true;
}

bool StereoImageProvider::nextStereoImages(FisheyeCamera& fisheyeCameraA, FisheyeCamera& fisheyeCameraB, Frame& imageA, Frame& imageB, HomogenousMatrixD4* world_T_device, HomogenousMatrixD4* device_T_cameraA, HomogenousMatrixD4* device_T_cameraB, HomogenousMatricesD4* world_T_leftHandJoints, HomogenousMatricesD4* world_T_rightHandJoints, bool* abort)
{
	const ScopedLock scopedLock(lock_);

	if (!isValid())
	{
		return false;
	}

	while ((abort == nullptr || *abort == false) && imageSequenceA_->stopTimestamp().isInvalid() && imageSequenceB_->stopTimestamp().isInvalid())
	{
		SharedAnyCamera anyCameraA;
		const FrameRef frameA = imageSequenceA_->frame(&anyCameraA);

		SharedAnyCamera anyCameraB;
		const FrameRef frameB = imageSequenceB_->frame(&anyCameraB);

		if (frameA.isNull() || frameB.isNull() || !anyCameraA || !anyCameraB || frameA->timestamp() == frameTimestampA_ || frameB->timestamp() == frameTimestampB_)
		{
			// the new frame is not available yet

			Thread::sleep(1u);
			continue;
		}

		if (frameA->timestamp() > frameB->timestamp() + 0.0001)
		{
			// source A had a dropped frame, so that we need to get the next frame from source B

			droppedFrameA_++;

			imageSequenceB_->forceNextFrame();
			Thread::sleep(100u);
			continue;
		}

		if (frameB->timestamp() > frameA->timestamp() + 0.0001)
		{
			// source B had a dropped frame, so that we need to get the next frame from source A

			droppedFrameB_++;

			imageSequenceA_->forceNextFrame();
			Thread::sleep(100u);
			continue;
		}

		ocean_assert(frameA->isValid() && FrameType::formatIsGeneric(frameA->pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
		ocean_assert(frameB->isValid() && FrameType::formatIsGeneric(frameB->pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
		ocean_assert(NumericD::isEqual(double(frameA->timestamp()), double(frameB->timestamp()), 0.0001));

		imageA = Frame(*frameA, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
		imageB = Frame(*frameB, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		frameTimestampA_ = frameA->timestamp();
		frameTimestampB_ = frameB->timestamp();

		if (anyCameraA->name() == AnyCameraFisheye::WrappedCamera::name())
		{
			fisheyeCameraA = ((const AnyCameraFisheye&)(*anyCameraA)).actualCamera();
		}
		else
		{
			ocean_assert(false && "Invalid camera type!");
			return false;
		}

		if (anyCameraB->name() == AnyCameraFisheye::WrappedCamera::name())
		{
			fisheyeCameraB = ((const AnyCameraFisheye&)(*anyCameraB)).actualCamera();
		}
		else
		{
			ocean_assert(false && "Invalid camera type!");
			return false;
		}

		if (world_T_device)
		{
			if (map_world_T_device_.isEmpty() || !map_world_T_device_.sample(double(frameTimestampA_ + frameTimestampB_) * 0.5, TransformationMap::IS_TIMESTAMP_INTERPOLATE, *world_T_device))
			{
				world_T_device->toNull();
			}
		}

		if (device_T_cameraA)
		{
			*device_T_cameraA = imageSequenceA_.force<Media::VRS::VRSImageSequence>().device_T_camera();
		}

		if (device_T_cameraB)
		{
			*device_T_cameraB = imageSequenceB_.force<Media::VRS::VRSImageSequence>().device_T_camera();
		}


		HomogenousMatricesD4* world_T_leftRightHandJoints[2] =
		{
			world_T_leftHandJoints,
			world_T_rightHandJoints
		};

		for (size_t nHand = 0; nHand < 2; ++nHand)
		{
			if (world_T_leftRightHandJoints[nHand] == nullptr)
			{
				continue;
			}

			world_T_leftRightHandJoints[nHand]->clear();

			const HandPoseMap& map_handPoses = nHand == 0 ? map_leftHandPoses_ : map_rightHandPoses_;

			const HandPoseMap::const_iterator iHandPoses = map_handPoses.find(double(frameTimestampA_)); // ignoring frameTimestampB_

			if (iHandPoses != map_handPoses.cend())
			{
				ocean_assert(world_T_leftRightHandJoints[nHand]->empty());
				world_T_leftRightHandJoints[nHand]->insert(world_T_leftRightHandJoints[nHand]->end(), iHandPoses->second.begin(), iHandPoses->second.end());
			}
		}

		imageSequenceA_->forceNextFrame();
		imageSequenceB_->forceNextFrame();

		break;
	}

	return (abort == nullptr || *abort == false) && imageSequenceA_->stopTimestamp().isInvalid() && imageSequenceB_->stopTimestamp().isInvalid();
}

bool StereoImageProvider::restart()
{
	if (!isValid())
	{
		return false;
	}

	ocean_assert(imageSequenceA_ && imageSequenceB_);

	if (!imageSequenceA_->setPosition(0.0) || !imageSequenceB_->setPosition(0.0))
	{
		return false;
	}

	if (!imageSequenceA_->start() || !imageSequenceB_->start())
	{
		return false;
	}

	droppedFrameA_ = 0;
	droppedFrameB_ = 0;

	frameTimestampA_.toInvalid();
	frameTimestampB_.toInvalid();

	return true;
}

bool StereoImageProvider::hasDeviceTransformations() const
{
	return !map_world_T_device_.isEmpty();
}

bool StereoImageProvider::hasCameraTransformations() const
{
	return true;
}

StereoImageProvider& StereoImageProvider::operator=(StereoImageProvider&& stereoImageProvider)
{
	if (this != &stereoImageProvider)
	{
		imageSequenceA_ = std::move(stereoImageProvider.imageSequenceA_);
		imageSequenceB_ = std::move(stereoImageProvider.imageSequenceB_);

		frameTimestampA_ = stereoImageProvider.frameTimestampA_;
		frameTimestampB_ = stereoImageProvider.frameTimestampB_;
		stereoImageProvider.frameTimestampA_.toInvalid();
		stereoImageProvider.frameTimestampB_.toInvalid();

		droppedFrameA_ = stereoImageProvider.droppedFrameA_;
		droppedFrameB_ = stereoImageProvider.droppedFrameB_;
		stereoImageProvider.droppedFrameA_ = size_t(0);
		stereoImageProvider.droppedFrameB_ = size_t(0);

		isValid_ = stereoImageProvider.isValid_;
		stereoImageProvider.isValid_ = false;
	}

	return *this;
}

bool StereoImageProvider::determineLowerStereoCameraRecordables(const std::string& vrsFile, std::string& recordableA, std::string& recordableB)
{
	std::vector<std::string> cameraRecordables;

	HomogenousMatricesD4 device_T_cameras;
	if (!Utilities::readCameraCalibrationData(vrsFile, cameraRecordables, nullptr, &device_T_cameras))
	{
		return false;
	}

	ocean_assert(cameraRecordables.size() == device_T_cameras.size());

	if (cameraRecordables.size() < 2)
	{
		return false;
	}

	double lowestY = NumericD::maxValue();
	double secondLowestY = NumericD::maxValue();

	size_t lowestIndex = size_t(-1);
	size_t secondLowestIndex = size_t(-1);

	for (size_t n = 0; n < device_T_cameras.size(); ++n)
	{
		const double cameraOriginY = device_T_cameras[n].translation().y();

		if (cameraOriginY < lowestY)
		{
			secondLowestY = lowestY;
			secondLowestIndex = lowestIndex;

			lowestY = cameraOriginY;
			lowestIndex = n;
		}
		else if (cameraOriginY < secondLowestY)
		{
			secondLowestY = cameraOriginY;
			secondLowestIndex = n;
		}
	}

	ocean_assert(lowestIndex != size_t(-1) && secondLowestIndex != size_t(-1));

	recordableA = cameraRecordables[lowestIndex];
	recordableB = cameraRecordables[secondLowestIndex];

	return !recordableA.empty() && !recordableB.empty();
}

bool StereoImageProvider::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return isValid_;
}

void StereoImageProvider::release()
{
	const ScopedLock scopedLock(lock_);

	imageSequenceA_.release();
	imageSequenceB_.release();

	frameTimestampA_.toInvalid();
	frameTimestampB_.toInvalid();

	droppedFrameA_ = 0;
	droppedFrameB_ = 0;

	isValid_ = false;
}

}

}

}
