// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/blob/CorrespondenceManager.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

using namespace CV;
using namespace CV::Detector;
using namespace CV::Detector::Blob;

void CorrespondenceManager::points(Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints)
{
	objectPoints.clear();
	imagePoints.clear();

	objectPoints.reserve(managerCorrespondenceObjects.size());
	imagePoints.reserve(managerCorrespondenceObjects.size());

	for (CorrespondenceObjects::const_iterator i = managerCorrespondenceObjects.begin(); i != managerCorrespondenceObjects.end(); ++i)
	{
		objectPoints.push_back(i->objectFeature().position());
		imagePoints.push_back(i->imageFeature().observation());
	}
}

void CorrespondenceManager::points(const FeatureIndices& indices, Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints)
{
	objectPoints.clear();
	imagePoints.clear();

	objectPoints.reserve(indices.size());
	imagePoints.reserve(indices.size());

	for (FeatureIndices::const_iterator i = indices.begin(); i != indices.end(); ++i)
	{
		ocean_assert(*i < managerCorrespondenceObjects.size());

		objectPoints.push_back(managerCorrespondenceObjects[*i].objectFeature().position());
		imagePoints.push_back(managerCorrespondenceObjects[*i].imageFeature().observation());
	}
}

void CorrespondenceManager::setCorrespondences(const BlobFeatures& imageFeatures, const BlobFeatures& objectFeatures, const Timestamp timestamp)
{
	ocean_assert(objectFeatures.size() == imageFeatures.size());

	const size_t number = min(objectFeatures.size(), imageFeatures.size());

	managerCorrespondenceObjects.clear();
	managerCorrespondenceObjects.reserve(objectFeatures.size());

	for (size_t n = 0; n < number; ++n)
		managerCorrespondenceObjects.push_back(CorrespondenceObject(imageFeatures[n], objectFeatures[n], timestamp));
}

unsigned int CorrespondenceManager::addCorrespondence(const BlobFeature& imageFeature, const BlobFeature& objectFeature, const Timestamp timestamp)
{
	const unsigned int index = (unsigned int)(managerCorrespondenceObjects.size());
	managerCorrespondenceObjects.push_back(CorrespondenceObject(imageFeature, objectFeature, timestamp));

	return index;
}

void CorrespondenceManager::setCorrespondences(const BlobFeatures& forwardFeatures, const BlobFeatures& backwardFeatures, const CorrespondencePairs& candidatePairs, const FeatureIndices& correspondenceIndices, const Timestamp timestamp)
{
	managerCorrespondenceObjects.clear();
	managerCorrespondenceObjects.reserve(correspondenceIndices.size());

	for (FeatureIndices::const_iterator i = correspondenceIndices.begin(); i != correspondenceIndices.end(); ++i)
	{
		ocean_assert(*i < candidatePairs.size());

		const CorrespondencePair& candidatePair(candidatePairs[*i]);

		ocean_assert(candidatePair.first < forwardFeatures.size());
		ocean_assert(candidatePair.second < backwardFeatures.size());

		const BlobFeature& objectFeature(backwardFeatures[candidatePair.second]);
		const BlobFeature& imageFeature(forwardFeatures[candidatePair.first]);

		managerCorrespondenceObjects.push_back(CorrespondenceObject(imageFeature, objectFeature, timestamp));
	}
}

void CorrespondenceManager::updateCorrespondence(const BlobFeature& imageFeature, const unsigned int index, const Timestamp timestamp)
{
	ocean_assert(index < managerCorrespondenceObjects.size());
	managerCorrespondenceObjects[index].updateImageFeature(imageFeature, timestamp);
}

void CorrespondenceManager::invalidateCorrespondence(const unsigned int index)
{
	ocean_assert(index < managerCorrespondenceObjects.size());

	managerCorrespondenceObjects[index].invalidate();
}

void CorrespondenceManager::garbageCollector(const Timestamp timestamp, const double timeout, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose)
{
	ocean_assert(timeout > 0);

	const HomogenousMatrix4 flipped(PinholeCamera::flippedTransformationRightSide(pose));
	const HomogenousMatrix4 iFlipped(flipped.inverted());

	for (size_t n = 0; n < managerCorrespondenceObjects.size(); /* noop */)
	{
		CorrespondenceObject& correspondenceObject = managerCorrespondenceObjects[n];

		ocean_assert(timestamp >= correspondenceObject.timestamp());

		// if the correspondence hasn't been recovered for the specified timeout interval
		if (correspondenceObject.timestamp() + timeout < timestamp)
		{
			managerCorrespondenceObjects.erase(managerCorrespondenceObjects.begin() + n);
			continue;
		}

		// if the correspondence hasn't been recovered in the current tracking image
		if (correspondenceObject.timestamp() != timestamp)
		{
			// the unrecovered correspondence receives a new change for the next tracking image because of an updated image position
			correspondenceObject.updateImageFeaturePosition(pinholeCamera.projectToImageIF<true>(iFlipped, correspondenceObject.objectFeature().position(), false));
		}

		++n;
	}
}

BlobFeatures CorrespondenceManager::imageFeatures() const
{
	BlobFeatures result;
	result.reserve(managerCorrespondenceObjects.size());

	for (CorrespondenceObjects::const_iterator i = managerCorrespondenceObjects.begin(); i != managerCorrespondenceObjects.end(); ++i)
		result.push_back(i->imageFeature());

	return result;
}

BlobFeatures CorrespondenceManager::objectFeatures() const
{
	BlobFeatures result;
	result.reserve(managerCorrespondenceObjects.size());

	for (CorrespondenceObjects::const_iterator i = managerCorrespondenceObjects.begin(); i != managerCorrespondenceObjects.end(); ++i)
		result.push_back(i->objectFeature());

	return result;
}

Geometry::ObjectPoints CorrespondenceManager::objectPositions(const CorrespondencePairs& correspondences) const
{
	Geometry::ObjectPoints result;
	result.reserve(correspondences.size());

	for (CorrespondencePairs::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
	{
		ocean_assert(i->first < managerCorrespondenceObjects.size());
		result.push_back(managerCorrespondenceObjects[i->first].objectFeature().position());
	}

	return result;
}

}

}

}
