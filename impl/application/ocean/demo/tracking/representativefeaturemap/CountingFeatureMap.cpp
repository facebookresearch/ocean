// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/representativefeaturemap/CountingFeatureMap.h"

#include "ocean/base/Subset.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"
#include "ocean/cv/detector/blob/BlobFeatureDetector.h"

#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"

const FeatureCount2IdMap& CountingFeatureMap::featureCount2Id()
{
	if (mapFeatureCount2Ids.size() != mapFeatureId2Count.size())
	{
		mapFeatureCount2Ids.clear();

		for (FeatureId2CountMap::const_iterator i = mapFeatureId2Count.cbegin(); i != mapFeatureId2Count.cend(); ++i)
		{
			ocean_assert(i->second >= 1u);
			mapFeatureCount2Ids.insert(std::make_pair(i->second, i->first));
		}
	}

	ocean_assert(mapFeatureCount2Ids.size() == mapFeatureId2Count.size());
	return mapFeatureCount2Ids;
}

bool CountingFeatureMap::modifyFrameAndDetermineCorrespondences(const Frame& frame, RandomGenerator& randomGenerator, const bool rotateFrame, const bool shrinkFrame, Worker* worker)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	unsigned int iterations = rotateFrame ? 4u : 1u;

	if (shrinkFrame)
	{
		iterations *= 2u;
	}

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		if (n != 0u)
		{
			Frame yRotated;
			CV::FrameInterpolatorNearestPixel::Comfort::rotate90(yFrame, yRotated, false, worker);
			yFrame = std::move(yRotated);
		}

		if (n == 4u)
		{
			if (!CV::FrameShrinker::downsampleByTwo11(yFrame, worker))
			{
				return false;
			}
		}

		Indices32 usdFeatureIds, ambiguousIds;

		if (determineCorrespondences(mapFeatures, yFrame, randomGenerator, usdFeatureIds, ambiguousIds, worker))
		{
			for (Indices32::const_iterator i = usdFeatureIds.begin(); i != usdFeatureIds.end(); ++i)
			{
				mapFeatureId2Count[*i]++;
			}

			for (Indices32::const_iterator i = ambiguousIds.begin(); i != ambiguousIds.end(); ++i)
			{
				mapFeatureId2Ambiguous[*i]++;
			}
		}

		mapFrames++;
	}

	return true;
}

unsigned int CountingFeatureMap::modifyFrameAndCheckCorrespondences(const FeaturePoints& mapFeatures, const Frame& frame, RandomGenerator& randomGenerator, unsigned int minimalCorrespondences, const bool rotateFrame, const bool shrinkFrame, unsigned int* numberFrames, Worker* worker)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	unsigned int iterations = rotateFrame ? 4u : 1u;

	if (shrinkFrame)
	{
		iterations *= 2u;
	}

	unsigned int validIterations = 0u;

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		if (n != 0u)
		{
			Frame yRotated;
			CV::FrameInterpolatorNearestPixel::Comfort::rotate90(yFrame, yRotated, false, worker);
			yFrame = std::move(yRotated);
		}

		if (n == 4u)
		{
			if (!CV::FrameShrinker::downsampleByTwo11(yFrame, worker))
			{
				return false;
			}
		}

		if (checkCorrespondences(mapFeatures, yFrame, randomGenerator, minimalCorrespondences, worker))
		{
			validIterations++;
		}
	}

	if (numberFrames)
	{
		*numberFrames = iterations;
	}

	return validIterations;
}

FeaturePoints CountingFeatureMap::determineBlobFeatures(const Frame& frame, const Scalar threshold, Worker* worker)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return FeaturePoints();
	}

	Frame integralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);

	FeaturePoints features;
	CV::Detector::Blob::BlobFeatureDetector::detectFeatures(integralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), CV::Detector::Blob::BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, features, worker);
	CV::Detector::Blob::BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, CV::Detector::Blob::BlobFeature::ORIENTATION_SLIDING_WINDOW, features, true, worker);

	return features;
}

bool CountingFeatureMap::determineCorrespondences(const FeaturePoints& mapFeatures, const Frame& frame, RandomGenerator& randomGenerator, Indices32& foundMapFeatures, Indices32& ambiguousMapFeatures, Worker* worker)
{
	ocean_assert(foundMapFeatures.empty() && ambiguousMapFeatures.empty());

	foundMapFeatures.clear();
	ambiguousMapFeatures.clear();

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	const FeaturePoints frameFeatures = CountingFeatureMap::determineBlobFeatures(yFrame, Scalar(6.5), worker);

	const Tracking::Blob::Correspondences::CorrespondencePairs correspondences(Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(frameFeatures, mapFeatures, frameFeatures.size(), Scalar(0.1), Scalar(0.7), worker));

	Vectors2 mapPoints, videoPoints;
	mapPoints.reserve(correspondences.size());
	videoPoints.reserve(correspondences.size());

	Indices32 featureMapCandidateIndices;

	for (size_t n = 0; n < correspondences.size(); ++n)
	{
		videoPoints.push_back(frameFeatures[correspondences[n].first].observation());
		mapPoints.push_back(mapFeatures[correspondences[n].second].observation());

		featureMapCandidateIndices.push_back(correspondences[n].second);
	}

	SquareMatrix3 homography;
	Indices32 usedIndices;

	// we accept a maximal distance between feature points and transformed points of 5% of the frame size
	const Scalar maximalDistance = Scalar(max(frame.width(), frame.height())) * Scalar(0.05);

	if (Geometry::RANSAC::homographyMatrix(mapPoints.data(), videoPoints.data(), mapPoints.size(), randomGenerator, homography, 12u, true, 100u, Scalar(maximalDistance * maximalDistance), &usedIndices, worker))
	{
		foundMapFeatures = Subset::subset(featureMapCandidateIndices, usedIndices);

		ambiguousMapFeatures = Subset::invertedSubset(featureMapCandidateIndices, std::set<Index32>(usedIndices.begin(), usedIndices.end()));

		return true;
	}

	return false;
}

bool CountingFeatureMap::checkCorrespondences(const FeaturePoints& mapFeatures, const Frame& frame, RandomGenerator& randomGenerator, unsigned int minimalCorrespondences, Worker* worker)
{
	ocean_assert(minimalCorrespondences >= 4u);

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	const FeaturePoints frameFeatures = CountingFeatureMap::determineBlobFeatures(yFrame, Scalar(6.5), worker);

	const Tracking::Blob::Correspondences::CorrespondencePairs correspondences(Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(frameFeatures, mapFeatures, frameFeatures.size(), Scalar(0.1), Scalar(0.7), worker));

	Vectors2 mapPoints, framePoints;
	mapPoints.reserve(correspondences.size());
	framePoints.reserve(correspondences.size());

	for (size_t n = 0; n < correspondences.size(); ++n)
	{
		framePoints.push_back(frameFeatures[correspondences[n].first].observation());
		mapPoints.push_back(mapFeatures[correspondences[n].second].observation());
	}

	SquareMatrix3 homography;
	Indices32 usedIndices;

	// we accept a maximal distance between feature points and transformed points of 5% of the frame size
	const Scalar maximalDistance = Scalar(max(frame.width(), frame.height())) * Scalar(0.05);

	if (!Geometry::RANSAC::homographyMatrix(mapPoints.data(), framePoints.data(), mapPoints.size(), randomGenerator, homography, 12u, true, 100u, Scalar(maximalDistance * maximalDistance), &usedIndices, worker))
	{
		return false;
	}

	return usedIndices.size() >= minimalCorrespondences;
}

void CountingFeatureMap::determineRepresentativeFeatures(CountingFeatureMaps& countingFeatureMaps, std::vector<Indices32>& representativeFeatureIndexGroups, const unsigned int idealFeatureNumber, const bool uniformDistributed, Worker* worker)
{
	// we seek the feature points most often used/detected, and without ambiguous within a pattern, and without ambiguous between all other patterns

	ocean_assert(representativeFeatureIndexGroups.empty());
	representativeFeatureIndexGroups.clear();

	std::vector<IndexSet32> interframeAmbiguousSets(countingFeatureMaps.size());

	for (size_t outer = 0; outer < countingFeatureMaps.size(); ++outer)
	{
		const FeaturePoints& outerFeatures = countingFeatureMaps[outer].features();
		IndexSet32& outerInterframeAmbiguous = interframeAmbiguousSets[outer];

		for (size_t inner = outer + 1; inner < countingFeatureMaps.size(); ++inner)
		{
			const FeaturePoints& innerFeatures = countingFeatureMaps[inner].features();
			IndexSet32& innerInterframeAmbiguous = interframeAmbiguousSets[inner];

			const Tracking::Blob::Correspondences::CorrespondencePairs correspondences(Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(outerFeatures, innerFeatures, outerFeatures.size(), Scalar(0.1), Scalar(0.7), worker));

			for (size_t n = 0; n < correspondences.size(); ++n)
			{
				outerInterframeAmbiguous.insert(correspondences[n].first);
				innerInterframeAmbiguous.insert(correspondences[n].second);
			}
		}
	}

	for (size_t n = 0; n < countingFeatureMaps.size(); ++n)
	{
		CountingFeatureMap& countingFeatureMap = countingFeatureMaps[n];
		const IndexSet32& interframeAmbiguous = interframeAmbiguousSets[n];

		const unsigned int maximalAmbiguous = (countingFeatureMap.frames() * 5u + 50u) / 100u; // 5% of all test frames

		const unsigned int numberBins = max(1u, (unsigned int)(Numeric::sqrt(Scalar(idealFeatureNumber * 2u)) + Scalar(0.5)));
		Geometry::SpatialDistribution::OccupancyArray occupancyArray(Scalar(0), Scalar(0), Scalar(countingFeatureMap.patternFrame().width()), Scalar(countingFeatureMap.patternFrame().height()), numberBins, numberBins);

		Indices32 goodFeaturePointIds;
		goodFeaturePointIds.reserve(idealFeatureNumber);

		for (FeatureCount2IdMap::const_reverse_iterator iC = countingFeatureMap.featureCount2Id().rbegin(); goodFeaturePointIds.size() < idealFeatureNumber && iC != countingFeatureMap.featureCount2Id().rend(); ++iC)
		{
			ocean_assert(iC->first < countingFeatureMap.frames());

			const unsigned int featureId = iC->second;

			const FeatureCount2IdMap::const_iterator iA = countingFeatureMap.featureId2Ambiguous().find(featureId);
			ocean_assert(iA != countingFeatureMap.featureId2Ambiguous().end());

			if (iA->second >= maximalAmbiguous)
				continue;

			if (interframeAmbiguous.find(featureId) != interframeAmbiguous.end())
				continue;

			const Vector2& observation = countingFeatureMap.features()[featureId].observation();

			if (!uniformDistributed || occupancyArray.addPoint(observation))
				goodFeaturePointIds.push_back(featureId);
		}

		ocean_assert(std::set<Index32>(goodFeaturePointIds.begin(), goodFeaturePointIds.end()).size() == goodFeaturePointIds.size());

		representativeFeatureIndexGroups.push_back(std::move(goodFeaturePointIds));
	}
}

void CountingFeatureMap::determineReliableFeatures(CountingFeatureMap& countingFeatureMap, Indices32& reliableFeatureIndices, const unsigned int idealFeatureNumber, const bool uniformDistributed, Worker* worker)
{
	// we seek the feature points most often used/detected, and without ambiguous within a pattern

	ocean_assert(reliableFeatureIndices.empty());
	reliableFeatureIndices.clear();

	const unsigned int maximalAmbiguous = (countingFeatureMap.frames() * 100u + 2u) / 10u; // 10% of all test frames

	const unsigned int numberBins = max(1u, (unsigned int)(Numeric::sqrt(Scalar(idealFeatureNumber * 2u)) + Scalar(0.5)));
	Geometry::SpatialDistribution::OccupancyArray occupancyArray(Scalar(0), Scalar(0), Scalar(countingFeatureMap.patternFrame().width()), Scalar(countingFeatureMap.patternFrame().height()), numberBins, numberBins);

	reliableFeatureIndices.reserve(idealFeatureNumber);

	for (FeatureCount2IdMap::const_reverse_iterator iC = countingFeatureMap.featureCount2Id().rbegin(); reliableFeatureIndices.size() < idealFeatureNumber && iC != countingFeatureMap.featureCount2Id().rend(); ++iC)
	{
		ocean_assert(iC->first < countingFeatureMap.frames());

		const unsigned int featureId = iC->second;

		const FeatureCount2IdMap::const_iterator iA = countingFeatureMap.featureId2Ambiguous().find(featureId);
		ocean_assert(iA != countingFeatureMap.featureId2Ambiguous().end());

		if (iA->second >= maximalAmbiguous)
			continue;

		const Vector2& observation = countingFeatureMap.features()[featureId].observation();

		if (!uniformDistributed || occupancyArray.addPoint(observation))
			reliableFeatureIndices.push_back(featureId);
	}

	ocean_assert(std::set<Index32>(reliableFeatureIndices.begin(), reliableFeatureIndices.end()).size() == reliableFeatureIndices.size());
}
