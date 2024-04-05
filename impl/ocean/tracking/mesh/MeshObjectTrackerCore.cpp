// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/mesh/MeshObjectTrackerCore.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"
#include "ocean/cv/detector/blob/BlobFeatureDetector.h"

#include "ocean/math/HomogenousMatrix4.h"

#include <algorithm>
#include <unordered_set>

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

MeshObjectTrackerCore::MeshObjectTrackerCore(const Options& options) :
	options_(options),
	quaternion_world_T_previousFrame_(false),
	nextRegisteredObjectId_(0u),
	timestampOfLastCheckForNewObjects_(false),
	timestampOfPreviousFrame_(false)
{
	// If any overriding values are provided, use them and set any missing values.
	if (options_.maxTimeAllowedForRecognition <= 0.0)
	{
#ifdef OCEAN_DEBUG
		options_.maxTimeAllowedForRecognition = 0.5;
#else
		options_.maxTimeAllowedForRecognition = 0.075;
#endif
	}

	if (options_.recognitionCadenceWithTrackedObjects <= 0.0)
	{
		options_.recognitionCadenceWithTrackedObjects = 0.5;
	}

	options_.recognitionCadenceWithoutTrackedObjects = std::max(options_.recognitionCadenceWithoutTrackedObjects, 0.0);

	ocean_assert(options_.maxTimeAllowedForRecognition > 0.0);
	ocean_assert(options_.recognitionCadenceWithTrackedObjects >= 0.0);
	ocean_assert(options_.recognitionCadenceWithoutTrackedObjects >= 0.0);
}

MeshObjectTrackerCore::~MeshObjectTrackerCore()
{
	// nothing to do here
}

MeshObjectTrackerCore::RegisteredObjectId MeshObjectTrackerCore::add(const Frame& yTextureFrame, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMappingRef, Worker* worker)
{
	ocean_assert(yTextureFrame && yTextureFrame.width() > 0u && yTextureFrame.height() > 0u);
	ocean_assert(meshUVTextureMappingRef && meshUVTextureMappingRef->isValid());

	if (!yTextureFrame || yTextureFrame.width() == 0u || yTextureFrame.height() == 0u || !meshUVTextureMappingRef || !meshUVTextureMappingRef->isValid())
	{
		return (unsigned int)(-1);
	}

	const ScopedLock scopedLock(trackerLock_);

	ocean_assert(registeredObjects_.find(nextRegisteredObjectId_) == registeredObjects_.end());

	const RegisteredObjectId objectId = nextRegisteredObjectId_++;
	registeredObjects_[objectId] = MeshObject(yTextureFrame, meshUVTextureMappingRef, worker);

	lastObjectIdTestedForRecognition_ = objectId;

	return objectId;
}

bool MeshObjectTrackerCore::remove(const RegisteredObjectId objectId)
{
	ocean_assert(objectId != kInvalidRegisteredObjectId);

	const ScopedLock scopedLock(trackerLock_);

	if (objectId >= nextRegisteredObjectId_)
	{
		ocean_assert(false && "This id is invalid and has never been used in this tracker!");
		return false;
	}

	ocean_assert(registeredObjects_.find(objectId) != registeredObjects_.end());
	registeredObjects_.erase(objectId);
	ocean_assert(registeredObjects_.find(objectId) == registeredObjects_.end());

	return true;
}

bool MeshObjectTrackerCore::clear()
{
	const ScopedLock scopedLock(trackerLock_);

	registeredObjects_.clear();

	return true;
}

void MeshObjectTrackerCore::untrackAll()
{
	const ScopedLock scopedLock(trackerLock_);

	currentFramePyramid_.clear();
	previousFramePyramid_.clear();

	for (RegisteredObjectMap::iterator i = registeredObjects_.begin(); i != registeredObjects_.end(); ++i)
	{
		i->second.reset(/* keepPoseGuess */ false);
	}

	quaternion_world_T_previousFrame_ = Quaternion(false);

	timestampOfLastCheckForNewObjects_.toInvalid();
	timestampOfPreviousFrame_.toInvalid();

	// unchanged: lastObjectIdTestedForRecognition_
}

bool MeshObjectTrackerCore::determinePoses(bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, VisualTracker::TransformationSamples& transformations, const Quaternion& quaternion_world_T_currentFrame, Worker* worker)
{
	ocean_assert(yFrame && pinholeCamera);
	if (!yFrame || !pinholeCamera.isValid())
		return false;

	const ScopedLock scopedLock(trackerLock_);

	if (registeredObjects_.empty())
		return false;

	ocean_assert(transformations.empty());
	transformations.clear();

	Quaternion quaternion_previousFrame_T_currentFrame(false);
	if (quaternion_world_T_previousFrame_.isValid() && quaternion_world_T_currentFrame.isValid())
	{
		quaternion_previousFrame_T_currentFrame = quaternion_world_T_previousFrame_.inverted() * quaternion_world_T_currentFrame;
	}

	ocean_assert(yFrame.timestamp().isValid());

	if (determinePoses(allowRecognition, yFrame, pinholeCamera, quaternion_previousFrame_T_currentFrame, worker))
	{
		for (RegisteredObjectMap::iterator i = registeredObjects_.begin(); i != registeredObjects_.end(); ++i)
		{
			if (i->second.pose_world_T_camera().isValid())
			{
				transformations.push_back(VisualTracker::TransformationSample(i->second.pose_world_T_camera(), i->first));
			}
		}
	}

	quaternion_world_T_previousFrame_ = quaternion_world_T_currentFrame;

	timestampOfPreviousFrame_ = yFrame.timestamp();

	return true;
}

bool MeshObjectTrackerCore::determinePoses(bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& quaternion_previousFrame_T_currentFrame, Worker* worker)
{
	ocean_assert(yFrame && FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(yFrame.width() == pinholeCamera.width() && yFrame.height() == pinholeCamera.height());

	// TODO (jtprice): ignoring Android-specific logic, for now -- will copy it later from the pattern tracker

	if (yFrame.pixels() > 1280u * 720u)
	{
		Frame yFrameDownsampled(FrameType(yFrame, yFrame.width() / 2u, yFrame.height() / 2u));
		CV::FrameShrinker::downsampleByTwo8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrameDownsampled.data<uint8_t>(), yFrame.width(), yFrame.height(), 1u, yFrame.paddingElements(), yFrameDownsampled.paddingElements(), nullptr);

		yFrameDownsampled.setTimestamp(yFrame.timestamp());

		const PinholeCamera cameraDownsampled(yFrameDownsampled.width(), yFrameDownsampled.height(), pinholeCamera);

		return determinePosesWithDownsampledResolution(allowRecognition, yFrameDownsampled, cameraDownsampled, quaternion_previousFrame_T_currentFrame, worker);
	}
	else
	{
		return determinePosesWithDownsampledResolution(allowRecognition, yFrame, pinholeCamera, quaternion_previousFrame_T_currentFrame, worker);
	}
}

bool MeshObjectTrackerCore::determinePosesWithDownsampledResolution(bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& quaternion_previousFrame_T_currentFrame, Worker* worker)
{
	ocean_assert(yFrame && FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(yFrame.width() == pinholeCamera.width() && yFrame.height() == pinholeCamera.height());

	// TODO (jtprice): hardcoded values
	const unsigned int pyramidLayers = previousFramePyramid_.isValid() ? previousFramePyramid_.layers() : CV::FramePyramid::idealLayers(yFrame.width(), yFrame.height(), 15u, 15u);

	ocean_assert(pyramidLayers >= 1u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	currentFramePyramid_.replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 1u, yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), true /*copyFirstLayer*/, worker, yFrame.timestamp());

	for (RegisteredObjectMap::iterator meshObjectIterator = registeredObjects_.begin(); meshObjectIterator != registeredObjects_.end(); ++meshObjectIterator)
	{
		MeshObject& meshObject = meshObjectIterator->second;
		meshObject.determinePose(options_.meshObjectTrackingOptions, pinholeCamera, previousFramePyramid_, currentFramePyramid_, quaternion_previousFrame_T_currentFrame, worker);
	}

	const unsigned int numTracked = numCurrentlyTrackedObjects();

	if (allowRecognition && (numTracked == 0u || numTracked < maxNumConcurrentlyTrackedObjects()) && yFrame.timestamp() > timestampOfLastCheckForNewObjects_ + maximumDurationBetweenRecognitionAttempts())
	{
		determinePosesForUntrackedObjects(pinholeCamera, yFrame, currentFramePyramid_, quaternion_previousFrame_T_currentFrame, worker);

		timestampOfLastCheckForNewObjects_ = yFrame.timestamp();
	}

	// we use the current frame pyramid as previous frame pyramid in the next tracking iteration
	std::swap(previousFramePyramid_, currentFramePyramid_);

	// At this moment, the previous pose is also the pose for the current frame.
	for (RegisteredObjectMap::iterator meshObjectIterator = registeredObjects_.begin(); meshObjectIterator != registeredObjects_.end(); ++meshObjectIterator)
	{
		MeshObject& meshObject = meshObjectIterator->second;
		if (meshObject.pose_world_T_camera().isValid())
		{
			meshObject.updatePoseGuess(yFrame.timestamp());
		}
	}

	return numCurrentlyTrackedObjects() != 0u;
}

bool MeshObjectTrackerCore::determinePosesForUntrackedObjects(const PinholeCamera& pinholeCamera, const Frame& yFrame, const CV::FramePyramid& currentFramePyramid, const Quaternion& /*quaternion_previousFrame_T_currentFrame*/, Worker* worker)
{
	if (numCurrentlyTrackedObjects() >= maxNumConcurrentlyTrackedObjects())
	{
		return true;
	}

	const Timestamp recognitionStartTimestamp(true);

	ocean_assert(pinholeCamera.isValid() && yFrame.isValid());
	ocean_assert(pinholeCamera.width() == yFrame.width() && pinholeCamera.height() == yFrame.height());

	const unsigned int* integralImage = updateIntegralImageBuffer(yFrame);
	ocean_assert(integralImage);

	// TODO (jtprice): this parameter may need to be adaptive to the device?
	// Although we may have downsampled the original input image, we still have the simple possibility
	// to select the sampling density for the Blob detector. Thus, as we do not downsample 1280x720
	// input images, we simply reduce the sampling density instead (for images having more pixels than
	// 640x480).
	constexpr unsigned int kMaxImageSizeForFullSampling = 640u * 480u;
	const CV::Detector::Blob::BlobFeatureDetector::SamplingDense samplingDensity =
		yFrame.pixels() > kMaxImageSizeForFullSampling ? CV::Detector::Blob::BlobFeatureDetector::SAMPLING_SPARSE : CV::Detector::Blob::BlobFeatureDetector::SAMPLING_NORMAL;

	// TODO (jtprice): hardcoded values
	CV::Detector::Blob::BlobFeatures features;
	CV::Detector::Blob::BlobFeatureDetector::detectFeatures(integralImage, yFrame.width(), yFrame.height(), samplingDensity, 10, true, features, worker);

	if (features.size() < 10u)
	{
		return false;
	}

	CV::Detector::Blob::BlobFeatures subsetFeatures;
	Vectors2 strongHarrisCorners;

	// Check if there are any currently tracked objects. If so, remove the features that lie within
	// their image projections.
	std::unordered_set<RegisteredObjectId> currentlyRegisteredObjectIds;
	currentlyRegisteredObjectIds.reserve(registeredObjects_.size());
	for (RegisteredObjectMap::const_iterator i = registeredObjects_.begin(); i != registeredObjects_.end(); ++i)
	{
		if (i->second.pose_world_T_camera().isValid())
		{
			currentlyRegisteredObjectIds.insert(i->first);
		}
	}

	if (!currentlyRegisteredObjectIds.empty())
	{
		subsetFeatures.reserve(features.size());

		// Only keep points that don't lie within the 2D projection of any currently tracked mesh.
		for (CV::Detector::Blob::BlobFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
		{
			bool isValid = true;

			for (const RegisteredObjectId& oid : currentlyRegisteredObjectIds)
			{
				const CV::SubRegion& subRegion = registeredObjects_[oid].projectedTrianglesSubRegion();
				if (subRegion.isInside(i->observation()))
				{
					isValid = false;
					break;
				}
			}

			if (isValid)
			{
				subsetFeatures.push_back(*i);
			}
		}

		features = std::move(subsetFeatures);
	}

	if (options_.maxNumberFeatures > 0u && features.size() > options_.maxNumberFeatures)
	{
		std::partial_sort(features.begin(), features.begin() + options_.maxNumberFeatures, features.end());
		features.resize(options_.maxNumberFeatures);
	}

	CV::Detector::Blob::BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImage, yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, CV::Detector::Blob::BlobFeature::ORIENTATION_SLIDING_WINDOW, features, static_cast<unsigned int>(features.size()), /*forceCalculation*/ false, worker);

	//
	// Try to detect new targets in a round-robin ordering. If 'options_.maxTimeAllowedForRecognition'
	// is exceeded before all patterns are tested, we bail on the detection and start with the next
	// scheduled object in the next frame.
	//

	RegisteredObjectMap::iterator trackedObjectIterator = registeredObjects_.upper_bound(lastObjectIdTestedForRecognition_);

	for (size_t index = 0; index < registeredObjects_.size(); ++index, ++trackedObjectIterator)
	{
		if (double(Timestamp(true) - recognitionStartTimestamp) > options_.maxTimeAllowedForRecognition)
		{
			return true;
		}

		if (trackedObjectIterator == registeredObjects_.end())
		{
			trackedObjectIterator = registeredObjects_.begin();
			ocean_assert(trackedObjectIterator != registeredObjects_.end());
		}

		lastObjectIdTestedForRecognition_ = trackedObjectIterator->first;
		if (currentlyRegisteredObjectIds.count(lastObjectIdTestedForRecognition_) != 0u)
		{
			continue;
		}

		MeshObject& meshObject = trackedObjectIterator->second;

		if (meshObject.determinePoseFromFrameFeatures(options_.meshObjectTrackingOptions, pinholeCamera, features, currentFramePyramid, worker))
		{
			// Remove all Blob features lying in the current subset.
			subsetFeatures.clear();
			subsetFeatures.reserve(features.size());

			const CV::SubRegion& subRegion = meshObject.projectedTrianglesSubRegion();

			for (const CV::Detector::Blob::BlobFeature& feature : features)
			{
				if (!subRegion.isInside(feature.observation()))
				{
					subsetFeatures.push_back(feature);
				}
			}

			features = std::move(subsetFeatures);

			if (numCurrentlyTrackedObjects() >= maxNumConcurrentlyTrackedObjects())
			{
				break; // can't track any more objects
			}
		}
	}

	return true;
}

const unsigned int* MeshObjectTrackerCore::updateIntegralImageBuffer(const Frame& yFrame)
{
	ocean_assert(yFrame && FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	integralImage_ = Frame(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	ocean_assert(integralImage_);

	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), integralImage_.data<unsigned int>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), integralImage_.paddingElements());

	return integralImage_.data<uint32_t>();
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean
