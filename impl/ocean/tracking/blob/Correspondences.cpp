/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/blob/Correspondences.h"

#include "ocean/geometry/Quadric.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

using namespace CV;
using namespace CV::Detector;
using namespace CV::Detector::Blob;

Correspondences::CorrespondencePairs Correspondences::determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker)
{
	return determineFeatureCorrespondences<false>(forwardFeatures, backwardFeatures, numberForward, threshold, worker);
}

Correspondences::CorrespondencePairs Correspondences::determineFeatureCorrespondencesEarlyReject(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker)
{
	return determineFeatureCorrespondences<true>(forwardFeatures, backwardFeatures, numberForward, threshold, worker);
}

Correspondences::CorrespondencePairs Correspondences::determineFeatureCorrespondencesGuided(const AnyCamera& camera, const HomogenousMatrix4& model_T_camera, const CV::Detector::Blob::BlobFeature* cameraFeatures, const CV::Detector::Blob::BlobFeature* modelFeatures, const size_t numberCameraFeatures, const size_t numberModelFeatures, const Scalar threshold, const Scalar maxSqrProjectionError, const bool earlyReject, Worker* worker)
{
	ocean_assert(camera.isValid());

	if (earlyReject)
	{
		return determineFeatureCorrespondencesGuided<true>(camera, model_T_camera, cameraFeatures, modelFeatures, numberCameraFeatures, numberModelFeatures, threshold, maxSqrProjectionError, worker);
	}
	else
	{
		return determineFeatureCorrespondencesGuided<false>(camera, model_T_camera, cameraFeatures, modelFeatures, numberCameraFeatures, numberModelFeatures, threshold, maxSqrProjectionError, worker);
	}
}

void Correspondences::extractCorrespondingPoints(const BlobFeatures& forwardFeatures, const BlobFeatures& backwardFeatures, const CorrespondencePairs& correspondences, Geometry::ImagePoints& imagePoints, Geometry::ObjectPoints& objectPoints, Scalars* weights)
{
	ocean_assert(objectPoints.empty());
	ocean_assert(imagePoints.empty());

	objectPoints.reserve(correspondences.size());
	imagePoints.reserve(correspondences.size());

	if (weights)
	{
		ocean_assert(weights->empty());
		weights->reserve(correspondences.size());
	}

	for (const CorrespondencePair& correspondence : correspondences)
	{
		ocean_assert(correspondence.first < forwardFeatures.size());
		ocean_assert(correspondence.second < backwardFeatures.size());

		const BlobFeature& forwardFeature = forwardFeatures[correspondence.first];
		const BlobFeature& backwardFeature = backwardFeatures[correspondence.second];

		imagePoints.push_back(forwardFeature.observation());
		objectPoints.push_back(backwardFeature.position());

		if (weights)
		{
			ocean_assert(forwardFeature.scale() > 0);
			weights->push_back(1 / Numeric::sqr(max(Scalar(2), forwardFeature.scale())));
		}
	}
}

void Correspondences::extractCorrespondingPoints(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const CorrespondencePairs& correspondences, Geometry::ImagePoints& forwardImagePoints, Geometry::ImagePoints& backwardImagePoints, Scalars* weights)
{
	ocean_assert(forwardImagePoints.empty());
	ocean_assert(backwardImagePoints.empty());

	forwardImagePoints.reserve(correspondences.size());
	backwardImagePoints.reserve(correspondences.size());

	if (weights)
	{
		ocean_assert(weights->empty());
		weights->reserve(correspondences.size());
	}

	for (const CorrespondencePair& correspondence : correspondences)
	{
		ocean_assert(correspondence.first < forwardFeatures.size());
		ocean_assert(correspondence.second < backwardFeatures.size());

		const BlobFeature& forwardFeature = forwardFeatures[correspondence.first];
		const BlobFeature& backwardFeature = backwardFeatures[correspondence.second];

		forwardImagePoints.push_back(forwardFeature.observation());
		backwardImagePoints.push_back(backwardFeature.observation());

		if (weights)
		{
			ocean_assert(forwardFeature.scale() > 0);
			weights->push_back(1 / Numeric::sqr(max(Scalar(2), forwardFeature.scale())));
		}
	}
}

void Correspondences::extractCorrespondingFeatures(const BlobFeatures& forwardFeatures, const BlobFeatures& backwardFeatures, const CorrespondencePairs& correspondencePairs, BlobFeatures& forwardCorrespondences, BlobFeatures& backwardCorrespondences)
{
	ocean_assert(forwardCorrespondences.empty());
	ocean_assert(backwardCorrespondences.empty());

	forwardCorrespondences.reserve(correspondencePairs.size());
	backwardCorrespondences.reserve(correspondencePairs.size());

	for (CorrespondencePairs::const_iterator i = correspondencePairs.begin(); i != correspondencePairs.end(); ++i)
	{
		ocean_assert(i->first < forwardFeatures.size());
		ocean_assert(i->second < backwardFeatures.size());

		forwardCorrespondences.push_back(forwardFeatures[i->first]);
		backwardCorrespondences.push_back(backwardFeatures[i->second]);
	}
}

void Correspondences::extractCorrespondingFeatures(const BlobFeatures& forwardFeatures, const BlobFeatures& backwardFeatures, const CorrespondencePairs& correspondencePairs, const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Scalar sqrDistance, BlobFeatures& forwardCorrespondences, BlobFeatures& backwardCorrespondences)
{
	ocean_assert(forwardCorrespondences.empty());
	ocean_assert(backwardCorrespondences.empty());

	forwardCorrespondences.reserve(correspondencePairs.size());
	backwardCorrespondences.reserve(correspondencePairs.size());

	const HomogenousMatrix4 flippedPose(PinholeCamera::flippedTransformationRightSide(pose));
	const HomogenousMatrix4 iFlippedPose(flippedPose.inverted());

	for (CorrespondencePairs::const_iterator i = correspondencePairs.begin(); i != correspondencePairs.end(); ++i)
	{
		ocean_assert(i->first < forwardFeatures.size());
		ocean_assert(i->second < backwardFeatures.size());

		const BlobFeature& forwardFeature(forwardFeatures[i->first]);
		const BlobFeature& backwardFeature(backwardFeatures[i->second]);

		const Vector2 projectedObject(pinholeCamera.projectToImageIF<true>(iFlippedPose, backwardFeature.position(), false));

		if ((forwardFeature.observation() - projectedObject).sqr() < sqrDistance)
		{
			forwardCorrespondences.push_back(forwardFeature);
			backwardCorrespondences.push_back(backwardFeature);
		}
	}
}

void Correspondences::extractCorrespondingFeatures(const BlobFeatures& forwardFeatures, const BlobFeatures& backwardFeatures, const CorrespondencePairs& candidatePairs, const FeatureIndices& correspondenceIndices, BlobFeatures& forwardCorrespondences, BlobFeatures& backwardCorrespondences)
{
	ocean_assert(forwardCorrespondences.empty());
	ocean_assert(backwardCorrespondences.empty());

	forwardCorrespondences.reserve(correspondenceIndices.size());
	backwardCorrespondences.reserve(correspondenceIndices.size());

	for (FeatureIndices::const_iterator i = correspondenceIndices.begin(); i != correspondenceIndices.end(); ++i)
	{
		ocean_assert(*i < candidatePairs.size());

		const CorrespondencePair& candidatePair(candidatePairs[*i]);

		ocean_assert(candidatePair.first < forwardFeatures.size());
		ocean_assert(candidatePair.second < backwardFeatures.size());

		forwardCorrespondences.push_back(forwardFeatures[candidatePair.first]);
		backwardCorrespondences.push_back(backwardFeatures[candidatePair.second]);
	}
}

void Correspondences::determineCandidates(const BlobFeatures& interestFeatures, const BlobFeatures& candidatePool, const Scalar window, MultiCandidates& multiCandidates, FeatureIndices& candidates)
{
	ocean_assert(window > 0);
	const Scalar windowHalf = window * Scalar(0.5);

	multiCandidates.clear();
	multiCandidates.resize(interestFeatures.size());

	CandidateSet candidateSet;

	for (unsigned int i = 0; i < interestFeatures.size(); ++i)
	{
		FeatureIndices& indices = multiCandidates[i];
		const BlobFeature& interest = interestFeatures[i];

		const Scalar minX = interest.observation().x() - windowHalf;
		const Scalar maxX = interest.observation().x() + windowHalf;
		const Scalar minY = interest.observation().y() - windowHalf;
		const Scalar maxY = interest.observation().y() + windowHalf;

		for (unsigned int c = 0; c < candidatePool.size(); ++c)
		{
			const BlobFeature& candidate = candidatePool[c];

			if (candidate.observation().x() >= minX && candidate.observation().x() <= maxX && candidate.observation().y() >= minY && candidate.observation().y() <= maxY)
			{
				indices.push_back(c);
				candidateSet.insert(c);
			}
		}
	}

	candidates.clear();
	candidates.reserve(candidateSet.size());

	for (CandidateSet::const_iterator i = candidateSet.begin(); i != candidateSet.end(); ++i)
		candidates.push_back(*i);
}

bool Correspondences::determineCorresponding3DFeature(const BlobFeature& interestFeature, const FeatureMap& featureMap, const Vector3& poseTranslation, const Quaternion& poseOrientation, const PinholeCamera& pinholeCamera, const Scalar maximalHalfAngle, const Scalar maximalDescriptorThreshold, unsigned int& correspondenceIndex)
{
	ocean_assert(maximalHalfAngle > 0 && maximalHalfAngle < Numeric::pi_2());

	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(maximalDescriptorThreshold * BlobDescriptor::descriptorNormalizationSqr()));

	const Vector2 undistortedObservation = (interestFeature.distortionState() == BlobFeature::DS_DISTORTED && pinholeCamera.hasDistortionParameters()) ? pinholeCamera.undistortDamped(interestFeature.observation()) : interestFeature.observation();

	if (featureMap.isPlanar())
	{
		const Scalar sqrTanValue = Numeric::sqr(Numeric::tan(maximalHalfAngle));

		const Line3 featureRay(pinholeCamera.ray(undistortedObservation, poseTranslation, poseOrientation));

		Vector3 objectPoint;
		if (featureMap.plane().intersection(featureRay, objectPoint))
		{
			const Scalar sqrDistanceToPlane = poseTranslation.sqrDistance(objectPoint);
			const Scalar sqrDistanceOnPlane = sqrDistanceToPlane * sqrTanValue;

			BlobDescriptor::DescriptorSSD bestSSD = BlobDescriptor::descriptorMaximalSSD();
			unsigned int minBackwardIndexFirst = (unsigned int)(-1);

			for (unsigned int c = 0; c < featureMap.features().size(); ++c)
			{
				const BlobFeature& candidateFeature = featureMap.features()[c];

				if ((candidateFeature.position() - objectPoint).sqr() < sqrDistanceOnPlane)
				{
					BlobDescriptor::DescriptorSSD ssd = 0;

					if (interestFeature.isDescriptorEqualEarlyReject(candidateFeature, normalizedThreshold, ssd) && ssd < bestSSD)
					{
						bestSSD = ssd;
						minBackwardIndexFirst = c;
					}
				}
			}

			// if we found a valid and reliable correspondence
			if (minBackwardIndexFirst != (unsigned int)(-1))
			{
				correspondenceIndex = minBackwardIndexFirst;
				return true;
			}
		}
	}
	else
	{
		const Vector3 coneDirection(pinholeCamera.vector(undistortedObservation));
		ocean_assert(Numeric::isEqual(coneDirection.length(), 1));

		const Rotation rayRotation(Vector3(0, 0, -1), coneDirection);
		const HomogenousMatrix4 entireTransformation(HomogenousMatrix4(poseTranslation, poseOrientation) * HomogenousMatrix4(rayRotation));
		const Geometry::Cone cone(Numeric::deg2rad(3), entireTransformation);

		BlobDescriptor::DescriptorSSD bestSSD = BlobDescriptor::descriptorMaximalSSD();
		unsigned int minBackwardIndexFirst = (unsigned int)(-1);

		for (unsigned int c = 0; c < featureMap.features().size(); ++c)
		{
			const BlobFeature& candidateFeature = featureMap.features()[c];

			if (cone.isInside(candidateFeature.position()))
			{
				BlobDescriptor::DescriptorSSD ssd = 0;

				if (interestFeature.isDescriptorEqualEarlyReject(candidateFeature, normalizedThreshold, ssd) && ssd < bestSSD)
				{
					bestSSD = ssd;
					minBackwardIndexFirst = c;
				}
			}
		}

		// if we found a valid and reliable correspondence
		if (minBackwardIndexFirst != (unsigned int)(-1))
		{
			correspondenceIndex = minBackwardIndexFirst;
			return true;
		}
	}

	return false;
}

template <bool tEarlyReject>
Correspondences::CorrespondencePairs Correspondences::determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker)
{
	ocean_assert(numberForward <= forwardFeatures.size());
	const size_t forwardFeaturesSize = min(numberForward, forwardFeatures.size());

	if (forwardFeaturesSize == 0 || backwardFeatures.empty())
	{
		return CorrespondencePairs();
	}

	Indices32 backwardIndicesForForwardFeatures(forwardFeaturesSize, (unsigned int)(-1));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&Correspondences::determineFeatureCorrespondencesSubset<tEarlyReject>, forwardFeatures.data(), backwardFeatures.data(), backwardFeatures.size(), threshold, backwardIndicesForForwardFeatures.data(), 0u, 0u), 0u, (unsigned int)forwardFeaturesSize);
	}
	else
	{
		determineFeatureCorrespondencesSubset<tEarlyReject>(forwardFeatures.data(), backwardFeatures.data(), backwardFeatures.size(), threshold, backwardIndicesForForwardFeatures.data(), 0u, (unsigned int)forwardFeaturesSize);
	}

	CorrespondencePairs result;
	result.reserve(min(forwardFeaturesSize, backwardFeatures.size()));

	for (unsigned int f = 0u; f < backwardIndicesForForwardFeatures.size(); ++f)
	{
		if (backwardIndicesForForwardFeatures[f] < forwardFeaturesSize)
		{
			result.emplace_back(f, backwardIndicesForForwardFeatures[f]);
		}
	}

	return result;
}

template <bool tEarlyReject>
Correspondences::CorrespondencePairs Correspondences::determineFeatureCorrespondencesGuided(const AnyCamera& camera, const HomogenousMatrix4& model_T_camera, const CV::Detector::Blob::BlobFeature* cameraFeatures, const CV::Detector::Blob::BlobFeature* modelFeatures, const size_t numberCameraFeatures, const size_t numberModelFeatures, const Scalar threshold, const Scalar maxSqrProjectionError, Worker* worker)
{
	ocean_assert(camera.isValid() && model_T_camera.isValid());

	ocean_assert(cameraFeatures != nullptr || numberCameraFeatures == 0);
	ocean_assert(modelFeatures != nullptr || numberModelFeatures == 0);

	if (numberCameraFeatures == 0 || numberModelFeatures == 0)
	{
		return CorrespondencePairs();
	}

	Indices32 modelIndicesForCameraFeatures(numberCameraFeatures, (unsigned int)(-1));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&Correspondences::determineFeatureCorrespondencesGuidedSubset<tEarlyReject>, &camera, &model_T_camera, cameraFeatures, modelFeatures, numberModelFeatures, threshold, maxSqrProjectionError, modelIndicesForCameraFeatures.data(), 0u, 0u), 0u, (unsigned int)(numberCameraFeatures));
	}
	else
	{
		determineFeatureCorrespondencesGuidedSubset<tEarlyReject>(&camera, &model_T_camera, cameraFeatures, modelFeatures, numberModelFeatures, threshold, maxSqrProjectionError, modelIndicesForCameraFeatures.data(), 0u, (unsigned int)(numberCameraFeatures));
	}

	CorrespondencePairs result;
	result.reserve(std::min(numberCameraFeatures, numberModelFeatures));

	for (size_t cameraFeatureIndex = 0; cameraFeatureIndex < numberCameraFeatures; ++cameraFeatureIndex)
	{
		if (modelIndicesForCameraFeatures[cameraFeatureIndex] < (unsigned int)(numberCameraFeatures))
		{
			result.emplace_back(Index32(cameraFeatureIndex), modelIndicesForCameraFeatures[cameraFeatureIndex]);
		}
	}

	return result;
}

template <bool tEarlyReject>
void Correspondences::determineFeatureCorrespondencesSubset(const CV::Detector::Blob::BlobFeature* forwardFeatures, const CV::Detector::Blob::BlobFeature* backwardFeatures, const size_t backwardFeaturesSize, const Scalar threshold, Index32* backwardIndicesForForwardFeatures, const unsigned int firstForward, const unsigned int numberForwards)
{
	ocean_assert(forwardFeatures != nullptr && backwardFeatures != nullptr && backwardIndicesForForwardFeatures != nullptr);
	ocean_assert(threshold >= 0);

	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	BlobDescriptor::DescriptorSSD bestSSD;
	unsigned int minBackwardIndexFirst;
	BlobDescriptor::DescriptorSSD ssd;

	for (unsigned int f = firstForward; f < firstForward + numberForwards; ++f)
	{
		bestSSD = BlobDescriptor::descriptorMaximalSSD();
		minBackwardIndexFirst = (unsigned int)(-1);

		const BlobFeature& forwardFeature = forwardFeatures[f];

		for (unsigned int b = 0u; b < backwardFeaturesSize; ++b)
		{
			if constexpr (tEarlyReject)
			{
				if (forwardFeature.isDescriptorEqualEarlyReject(backwardFeatures[b], normalizedThreshold, ssd) && ssd < bestSSD)
				{
					bestSSD = ssd;
					minBackwardIndexFirst = b;
				}
			}
			else
			{
				if (forwardFeature.isDescriptorEqual(backwardFeatures[b], normalizedThreshold, ssd) && ssd < bestSSD)
				{
					bestSSD = ssd;
					minBackwardIndexFirst = b;
				}
			}
		}

		backwardIndicesForForwardFeatures[f] = minBackwardIndexFirst;
	}
}

template <bool tEarlyReject>
void Correspondences::determineFeatureCorrespondencesGuidedSubset(const AnyCamera* camera, const HomogenousMatrix4* model_T_camera, const CV::Detector::Blob::BlobFeature* cameraFeatures, const CV::Detector::Blob::BlobFeature* modelFeatures, const size_t numberModelFeatures, const Scalar threshold, const Scalar maxSqrProjectionError, Index32* modelIndicesForCameraFeatures, const unsigned int firstCameraFeature, const unsigned int numberCameraFeatures)
{
	ocean_assert(camera != nullptr && model_T_camera != nullptr);

	ocean_assert(cameraFeatures != nullptr && modelFeatures != nullptr && modelIndicesForCameraFeatures != nullptr);
	ocean_assert(threshold >= 0 && maxSqrProjectionError >= 0);

	const HomogenousMatrix4 flippedCamera_T_model(PinholeCamera::standard2InvertedFlipped(*model_T_camera));

	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	BlobDescriptor::DescriptorSSD bestSSD;
	BlobDescriptor::DescriptorSSD localSSD;

	unsigned int bestModelFeatureIndex;

	for (unsigned int cameraFeatureIndex = firstCameraFeature; cameraFeatureIndex < firstCameraFeature + numberCameraFeatures; ++cameraFeatureIndex)
	{
		bestSSD = BlobDescriptor::descriptorMaximalSSD();
		bestModelFeatureIndex = (unsigned int)(-1);

		const BlobFeature& cameraFeature = cameraFeatures[cameraFeatureIndex];
		ocean_assert(camera->isInside(cameraFeature.observation()));

		for (size_t modelFeatureIndex = 0; modelFeatureIndex < numberModelFeatures; ++modelFeatureIndex)
		{
			const CV::Detector::Blob::BlobFeature& modelFeature = modelFeatures[modelFeatureIndex];

			const Vector2 projectedModelFeature = camera->projectToImageIF(flippedCamera_T_model, modelFeature.position());

			if (projectedModelFeature.sqrDistance(cameraFeature.observation()) <= maxSqrProjectionError)
			{
				if constexpr (tEarlyReject)
				{
					if (cameraFeature.isDescriptorEqualEarlyReject(modelFeature, normalizedThreshold, localSSD) && localSSD < bestSSD)
					{
						bestSSD = localSSD;
						bestModelFeatureIndex = (unsigned int)(modelFeatureIndex);
					}
				}
				else
				{
					if (cameraFeature.isDescriptorEqual(modelFeature, normalizedThreshold, localSSD) && localSSD < bestSSD)
					{
						bestSSD = localSSD;
						bestModelFeatureIndex = (unsigned int)(modelFeatureIndex);
					}
				}
			}
		}

		modelIndicesForCameraFeatures[cameraFeatureIndex] = bestModelFeatureIndex;
	}
}

}

}

}
