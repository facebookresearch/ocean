// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"

#include "ocean/geometry/SpatialDistribution.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

using namespace CV;
using namespace CV::Detector;
using namespace CV::Detector::Blob;

UnidirectionalCorrespondences::UnidirectionalCorrespondences(const unsigned int numberForward, const unsigned int numberBackward)
{
	forwardCounters.resize(numberForward);
	backwardCounters.resize(numberBackward);
}

void UnidirectionalCorrespondences::addCandidate(const unsigned int forwardIndex, const unsigned int backwardIndex)
{
	candidates.push_back(CorrespondencePair(forwardIndex, backwardIndex));

	ocean_assert(forwardIndex < forwardCounters.size());
	++forwardCounters[forwardIndex];

	ocean_assert(backwardIndex < backwardCounters.size());
	++backwardCounters[backwardIndex];
}

void UnidirectionalCorrespondences::lockedAddCandidate(const unsigned int forwardIndex, const unsigned int backwardIndex)
{
	const ScopedLock scopedLock(lock);
	addCandidate(forwardIndex, backwardIndex);
}

Correspondences::CorrespondencePairs UnidirectionalCorrespondences::correspondences() const
{
	CorrespondencePairs validCorrespondences;

	// find unidirectional correspondences
	for (CorrespondencePairs::const_iterator i = candidates.begin(); i != candidates.end(); ++i)
		if (forwardCounters[i->first] == 1u && backwardCounters[i->second] == 1u)
			validCorrespondences.push_back(CorrespondencePair(i->first, i->second));

	return validCorrespondences;
}

UnidirectionalCorrespondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker)
{
	return determineFeatureCorrespondences<false>(forwardFeatures, backwardFeatures, numberForward, threshold, worker);
}

UnidirectionalCorrespondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQuality(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, const Scalar qualityRatio, Worker* worker)
{
	return determineFeatureCorrespondencesWithQuality<false>(forwardFeatures, backwardFeatures, numberForward, threshold, qualityRatio, worker);
}

UnidirectionalCorrespondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondencesEarlyReject(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker)
{
	return determineFeatureCorrespondences<true>(forwardFeatures, backwardFeatures, numberForward, threshold, worker);
}

UnidirectionalCorrespondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, const Scalar qualityRatio, Worker* worker)
{
	return determineFeatureCorrespondencesWithQuality<true>(forwardFeatures, backwardFeatures, numberForward, threshold, qualityRatio, worker);
}

Correspondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondences(const BlobFeatures& interestFeatures, const BlobFeatures& featurePool, const MultiCandidates& multiCandidates, const Scalar threshold, const Scalar qualityRatio)
{
	ocean_assert(interestFeatures.size() == multiCandidates.size());
	UnidirectionalCorrespondences candidates((unsigned int)(interestFeatures.size()), (unsigned int)(featurePool.size()));

	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	for (unsigned int i = 0; i < multiCandidates.size(); ++i)
	{
		BlobDescriptor::DescriptorSSD bestSSD(BlobDescriptor::descriptorMaximalSSD());
		BlobDescriptor::DescriptorSSD secondBestSSD(BlobDescriptor::descriptorMaximalSSD());
		unsigned int minBackwardIndexFirst = (unsigned int)(-1);

		const BlobFeature& interestFeature = interestFeatures[i];

		for (FeatureIndices::const_iterator c = multiCandidates[i].begin(); c != multiCandidates[i].end(); ++c)
		{
			BlobDescriptor::DescriptorSSD ssd = 0;

			ocean_assert(*c < featurePool.size());

			if (interestFeature.isDescriptorEqualEarlyReject(featurePool[*c], normalizedThreshold, ssd))
			{
				if (ssd < bestSSD)
				{
					secondBestSSD = bestSSD;
					bestSSD = ssd;

					minBackwardIndexFirst = *c;
				}
				else if (ssd < secondBestSSD)
					secondBestSSD = ssd;
			}
		}

		// if we found a valid and reliable correspondence
		if (minBackwardIndexFirst != (unsigned int)(-1) && (secondBestSSD == BlobDescriptor::descriptorMaximalSSD() || bestSSD < qualityRatio * secondBestSSD))
			candidates.addCandidate(i, minBackwardIndexFirst);
	}

	return candidates.correspondences();
}

Correspondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondences(const BlobFeatures& interestFeatures, const BlobFeatures& featurePool, const MultiCandidates& multiCandidates, const Scalar threshold)
{
	ocean_assert(interestFeatures.size() == multiCandidates.size());
	UnidirectionalCorrespondences candidates((unsigned int)(interestFeatures.size()), (unsigned int)(featurePool.size()));

	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	for (unsigned int i = 0; i < multiCandidates.size(); ++i)
	{
		BlobDescriptor::DescriptorSSD bestSSD(BlobDescriptor::descriptorMaximalSSD());
		unsigned int minBackwardIndexFirst = (unsigned int)(-1);

		const BlobFeature& interestFeature = interestFeatures[i];

		for (FeatureIndices::const_iterator c = multiCandidates[i].begin(); c != multiCandidates[i].end(); ++c)
		{
			BlobDescriptor::DescriptorSSD ssd = 0;

			ocean_assert(*c < featurePool.size());

			if (interestFeature.isDescriptorEqualEarlyReject(featurePool[*c], normalizedThreshold, ssd) && ssd < bestSSD)
			{
				bestSSD = ssd;
				minBackwardIndexFirst = *c;
			}
		}

		// if we found a valid and reliable correspondence
		if (minBackwardIndexFirst != (unsigned int)(-1))
			candidates.addCandidate(i, minBackwardIndexFirst);
	}

	return candidates.correspondences();
}

void UnidirectionalCorrespondences::determineFeatureCorrespondences(const BlobFeatures* forward, const BlobFeatures* backward, const Scalar threshold, UnidirectionalCorrespondences* correspondencesCandidates, const unsigned int firstForward, const unsigned int numberForwards)
{
	ocean_assert(forward && backward && correspondencesCandidates);
	ocean_assert(firstForward + numberForwards <= forward->size());

	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	for (unsigned int f = firstForward; f < firstForward + numberForwards; ++f)
	{
		BlobDescriptor::DescriptorSSD bestSSD(BlobDescriptor::descriptorMaximalSSD());
		unsigned int minBackwardIndexFirst = (unsigned int)(-1);

		const BlobFeature& forwardFeature = (*forward)[f];

		for (unsigned int b = 0; b < backward->size(); ++b)
		{
			BlobDescriptor::DescriptorSSD ssd = 0;

			if (forwardFeature.isDescriptorEqualEarlyReject((*backward)[b], normalizedThreshold, ssd) && ssd < bestSSD)
			{
				bestSSD = ssd;
				minBackwardIndexFirst = b;
			}
		}

		// if we found a valid and reliable correspondence
		if (minBackwardIndexFirst != (unsigned int)(-1))
			correspondencesCandidates->lockedAddCandidate(f, minBackwardIndexFirst);
	}
}

template <bool tEarlyReject>
Correspondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondences(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, Worker* worker)
{
	Indices32 forwardIndicesForBackwardFeatures(backwardFeatures.size(), (unsigned int)(-1));

	ocean_assert(numberForward <= (unsigned int)forwardFeatures.size());
	const unsigned int forwardFeaturesSize = (unsigned int)min(numberForward, forwardFeatures.size());

	if (forwardFeaturesSize == 0u || backwardFeatures.empty())
		return CorrespondencePairs();

	if (worker)
	{
		Lock locks[8];
		worker->executeFunction(Worker::Function::createStatic(&UnidirectionalCorrespondences::determineFeatureCorrespondencesSubset<tEarlyReject, 8u>, forwardFeatures.data(), backwardFeatures.data(), (unsigned int)backwardFeatures.size(), threshold, forwardIndicesForBackwardFeatures.data(), locks, 0u, 0u), 0u, forwardFeaturesSize);
	}
	else
	{
		determineFeatureCorrespondencesSubset<tEarlyReject, 0u>(forwardFeatures.data(), backwardFeatures.data(), (unsigned int)backwardFeatures.size(), threshold, forwardIndicesForBackwardFeatures.data(), nullptr, 0u, forwardFeaturesSize);
	}

	CorrespondencePairs result;
	result.reserve(min(size_t(forwardFeaturesSize), backwardFeatures.size()));

	for (unsigned int b = 0u; b < forwardIndicesForBackwardFeatures.size(); ++b)
		if (forwardIndicesForBackwardFeatures[b] < forwardFeaturesSize)
			result.emplace_back(forwardIndicesForBackwardFeatures[b], b);

	return result;
}

template <bool tEarlyReject>
Correspondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQuality(const CV::Detector::Blob::BlobFeatures& forwardFeatures, const CV::Detector::Blob::BlobFeatures& backwardFeatures, const size_t numberForward, const Scalar threshold, const Scalar qualityRatio, Worker* worker)
{
	Indices32 forwardIndicesForBackwardFeatures(backwardFeatures.size(), (unsigned int)(-1));

	ocean_assert(numberForward <= (unsigned int)forwardFeatures.size());
	const unsigned int forwardFeaturesSize = (unsigned int)min(numberForward, forwardFeatures.size());

	if (forwardFeaturesSize == 0u || backwardFeatures.empty())
		return CorrespondencePairs();

	if (worker)
	{
		Lock locks[8];
		worker->executeFunction(Worker::Function::createStatic(&UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualitySubset<tEarlyReject, 8u>, forwardFeatures.data(), backwardFeatures.data(), (unsigned int)backwardFeatures.size(), threshold, qualityRatio, forwardIndicesForBackwardFeatures.data(), locks, 0u, 0u), 0u, forwardFeaturesSize);
	}
	else
	{
		determineFeatureCorrespondencesWithQualitySubset<tEarlyReject, 0u>(forwardFeatures.data(), backwardFeatures.data(), (unsigned int)backwardFeatures.size(), threshold, qualityRatio, forwardIndicesForBackwardFeatures.data(), nullptr, 0u, forwardFeaturesSize);
	}

	CorrespondencePairs result;
	result.reserve(min(size_t(forwardFeaturesSize), backwardFeatures.size()));

	for (unsigned int b = 0u; b < forwardIndicesForBackwardFeatures.size(); ++b)
		if (forwardIndicesForBackwardFeatures[b] < forwardFeaturesSize)
			result.emplace_back(forwardIndicesForBackwardFeatures[b], b);

	return result;
}

template <bool tEarlyReject, unsigned int tLocks>
void UnidirectionalCorrespondences::determineFeatureCorrespondencesSubset(const CV::Detector::Blob::BlobFeature* forwardFeatures, const CV::Detector::Blob::BlobFeature* backwardFeatures, const unsigned int backwardFeaturesSize, const Scalar threshold, Index32* forwardIndicesForBackwardFeatures, Lock* locks, const unsigned int firstForward, const unsigned int numberForwards)
{
	ocean_assert(forwardFeatures != nullptr && backwardFeatures != nullptr && forwardIndicesForBackwardFeatures != nullptr);
	ocean_assert(threshold > 0);

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

		// if we found a valid correspondence
		if (minBackwardIndexFirst != (unsigned int)(-1))
		{
			unsigned int& forwardIndexForBackwardFeature = forwardIndicesForBackwardFeatures[minBackwardIndexFirst];

			// lock-free read check
			if (forwardIndexForBackwardFeature == (unsigned int)(-2))
				continue;

			if constexpr (tLocks != 0u)
			{
				ocean_assert(locks != nullptr);
				const ScopedLock scopedLock(locks[minBackwardIndexFirst % (tLocks ? tLocks : 1u)]);

				switch (forwardIndexForBackwardFeature)
				{
						// we have more than two matching feature point
					case (unsigned int)(-2):
						break;

						// we have one feature point
					case (unsigned int)(-1):
						forwardIndexForBackwardFeature = f;
						break;

						// we have more than one matching feature point
					default:
						forwardIndexForBackwardFeature = (unsigned int)(-2);
						break;
				}
			}
			else
			{
				switch (forwardIndexForBackwardFeature)
				{
						// we have more than two matching feature point
					case (unsigned int)(-2):
						break;

						// we have one feature point
					case (unsigned int)(-1):
						forwardIndexForBackwardFeature = f;
						break;

						// we have more than one matching feature point
					default:
						forwardIndexForBackwardFeature = (unsigned int)(-2);
						break;
				}
			}
		}
	}
}

template <bool tEarlyReject, unsigned int tLocks>
void UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualitySubset(const CV::Detector::Blob::BlobFeature* forwardFeatures, const CV::Detector::Blob::BlobFeature* backwardFeatures, const unsigned int backwardFeaturesSize, const Scalar threshold, const Scalar qualityRatio, Index32* forwardIndicesForBackwardFeatures, Lock* locks, const unsigned int firstForward, const unsigned int numberForwards)
{
	ocean_assert(forwardFeatures != nullptr && backwardFeatures != nullptr && forwardIndicesForBackwardFeatures != nullptr);
	ocean_assert(threshold > 0 && qualityRatio > 0);

	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	BlobDescriptor::DescriptorSSD bestSSD;
	BlobDescriptor::DescriptorSSD secondBestSSD;

	unsigned int minBackwardIndexFirst;
	BlobDescriptor::DescriptorSSD ssd;

	for (unsigned int f = firstForward; f < firstForward + numberForwards; ++f)
	{
		bestSSD = BlobDescriptor::descriptorMaximalSSD();
		secondBestSSD = BlobDescriptor::descriptorMaximalSSD();

		minBackwardIndexFirst = (unsigned int)(-1);

		const BlobFeature& forwardFeature = forwardFeatures[f];

		for (unsigned int b = 0u; b < backwardFeaturesSize; ++b)
		{
			if constexpr (tEarlyReject)
			{
				if (forwardFeature.isDescriptorEqualEarlyReject(backwardFeatures[b], normalizedThreshold, ssd))
				{
					if (ssd < bestSSD)
					{
						secondBestSSD = bestSSD;
						bestSSD = ssd;

						minBackwardIndexFirst = b;
					}
					else if (ssd < secondBestSSD)
						secondBestSSD = ssd;
				}
			}
			else
			{
				if (forwardFeature.isDescriptorEqual(backwardFeatures[b], normalizedThreshold, ssd))
				{
					if (ssd < bestSSD)
					{
						secondBestSSD = bestSSD;
						bestSSD = ssd;

						minBackwardIndexFirst = b;
					}
					else if (ssd < secondBestSSD)
						secondBestSSD = ssd;
				}
			}
		}

		// if we found a valid !and! reliable correspondence
		if (minBackwardIndexFirst != (unsigned int)(-1) && (secondBestSSD == BlobDescriptor::descriptorMaximalSSD() || bestSSD < qualityRatio * secondBestSSD))
		{
			unsigned int& forwardIndexForBackwardFeature = forwardIndicesForBackwardFeatures[minBackwardIndexFirst];

			// lock-free read check
			if (forwardIndexForBackwardFeature == (unsigned int)(-2))
				continue;

			if constexpr (tLocks != 0u)
			{
				ocean_assert(locks != nullptr);
				const ScopedLock scopedLock(locks[minBackwardIndexFirst % (tLocks ? tLocks : 1u)]);

				switch (forwardIndexForBackwardFeature)
				{
						// we have more than two matching feature point
					case (unsigned int)(-2):
						break;

						// we have one feature point
					case (unsigned int)(-1):
						forwardIndexForBackwardFeature = f;
						break;

						// we have more than one matching feature point
					default:
						forwardIndexForBackwardFeature = (unsigned int)(-2);
						break;
				}
			}
			else
			{
				switch (forwardIndexForBackwardFeature)
				{
						// we have more than two matching feature point
					case (unsigned int)(-2):
						break;

						// we have one feature point
					case (unsigned int)(-1):
						forwardIndexForBackwardFeature = f;
						break;

						// we have more than one matching feature point
					default:
						forwardIndexForBackwardFeature = (unsigned int)(-2);
						break;
				}
			}
		}
	}
}

void UnidirectionalCorrespondences::determineFeatureCorrespondences(const BlobFeatures* forward, const BlobFeatures* backward, const Scalar threshold, const Scalar qualityRatio, UnidirectionalCorrespondences* correspondencesCandidates, const unsigned int firstForward, const unsigned int numberForwards)
{
	ocean_assert(forward && backward && correspondencesCandidates);
	ocean_assert(firstForward + numberForwards <= forward->size());

	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	for (unsigned int f = firstForward; f < firstForward + numberForwards; ++f)
	{
		BlobDescriptor::DescriptorSSD bestSSD(BlobDescriptor::descriptorMaximalSSD());
		BlobDescriptor::DescriptorSSD secondBestSSD(BlobDescriptor::descriptorMaximalSSD());
		unsigned int minBackwardIndexFirst = (unsigned int)(-1);

		const BlobFeature& forwardFeature = (*forward)[f];

		for (unsigned int b = 0; b < backward->size(); ++b)
		{
			BlobDescriptor::DescriptorSSD ssd = 0;

			if (forwardFeature.isDescriptorEqualEarlyReject((*backward)[b], normalizedThreshold, ssd))
			{
				if (ssd < bestSSD)
				{
					secondBestSSD = bestSSD;
					bestSSD = ssd;

					minBackwardIndexFirst = b;
				}
				else if (ssd < secondBestSSD)
				{
					secondBestSSD = ssd;
				}
			}
		}

		// if we found a valid and reliable correspondence
		if (minBackwardIndexFirst != (unsigned int)(-1) && (secondBestSSD == BlobDescriptor::descriptorMaximalSSD() || bestSSD < qualityRatio * secondBestSSD))
			correspondencesCandidates->lockedAddCandidate(f, minBackwardIndexFirst);
	}
}

Correspondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondencesWithHomography(const SquareMatrix3& map_H_interest, const CV::Detector::Blob::BlobFeatures& interestFeatures, const CV::Detector::Blob::BlobFeatures& featureMapFeatures, const size_t numberInterestFeatures, const unsigned int featureMapWidth, const unsigned int featureMapHeight, const Scalar maximalProjectionError, const Scalar threshold, const Scalar qualityRatio)
{
	UnidirectionalCorrespondences candidates((unsigned int)(interestFeatures.size()), (unsigned int)(featureMapFeatures.size()));

	ocean_assert(map_H_interest.isHomography());
	ocean_assert(maximalProjectionError > Numeric::eps());
	ocean_assert(threshold >= 0 && qualityRatio >= 0);

	ocean_assert(!interestFeatures.empty() && !featureMapFeatures.empty() && numberInterestFeatures >= 1);

	if (interestFeatures.empty() || featureMapFeatures.empty() || numberInterestFeatures == 0 || maximalProjectionError <= Numeric::eps() || map_H_interest.isNull())
	{
		return candidates.correspondences();
	}

	const unsigned int maxInterestFeatures = (unsigned int)(std::min(numberInterestFeatures, interestFeatures.size()));
	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	// first, we project the 3D feature points of the feature map into the camera image and distribute all (visible) features into a 2D grid

	const unsigned int horizontalBins = std::max(1u, (unsigned int)(Scalar(featureMapWidth) / maximalProjectionError + Scalar(0.5)));
	const unsigned int verticalBins = std::max(1u, (unsigned int)(Scalar(featureMapHeight) / maximalProjectionError + Scalar(0.5)));

	Geometry::SpatialDistribution::DistributionArray featureMapDistributionArray(Scalar(0), Scalar(0), Scalar(featureMapWidth), Scalar(featureMapHeight), horizontalBins, verticalBins);

	for (size_t n = 0; n < featureMapFeatures.size(); ++n)
	{
		const int binX = featureMapDistributionArray.horizontalBin(featureMapFeatures[n].observation().x());
		const int binY = featureMapDistributionArray.horizontalBin(featureMapFeatures[n].observation().y());

		ocean_assert((unsigned int)binX < horizontalBins && (unsigned int)binY < verticalBins);

		if ((unsigned int)binX < horizontalBins && (unsigned int)binY < verticalBins)
		{
			featureMapDistributionArray(binX, binY).push_back((unsigned int)n);
		}
	}

	// now, we apply a guided matching based on the 9-neighborhood within the 2D grid for each live feature point

	const Scalar sqrMaximalProjectionError = Numeric::sqr(maximalProjectionError);

	for (unsigned int n = 0; n < maxInterestFeatures; ++n)
	{
		const BlobFeature& interestFeature = interestFeatures[n];

		Vector2 predictedFeatureMapLocation;
		if (!map_H_interest.multiply(interestFeature.observation(), predictedFeatureMapLocation))
		{
			continue;
		}

		const int binX = featureMapDistributionArray.horizontalBin(predictedFeatureMapLocation.x());
		const int binY = featureMapDistributionArray.horizontalBin(predictedFeatureMapLocation.y());

		if ((unsigned int)binX < horizontalBins && (unsigned int)binY < verticalBins)
		{
			BlobDescriptor::DescriptorSSD ssdBest(BlobDescriptor::descriptorMaximalSSD());
			BlobDescriptor::DescriptorSSD ssdSecondBest(BlobDescriptor::descriptorMaximalSSD());
			unsigned int bestFeatureMapIndexFirst = (unsigned int)(-1);

			// we iterate over the 9-neighborhood in the distribution array

			for (unsigned int bY = (unsigned int)std::max(0, binY - 1); bY < std::min((unsigned int)binY + 2u, verticalBins); ++bY)
			{
				for (unsigned int bX = (unsigned int)std::max(0, binX - 1); bX < std::min((unsigned int)binX + 2u, horizontalBins); ++bX)
				{
					const Indices32& featureMapIndices = featureMapDistributionArray(bX, bY);

					for (const Index32& featureMapIndex : featureMapIndices)
					{
						ocean_assert(featureMapIndex < (unsigned int)(featureMapFeatures.size()));

						if (featureMapFeatures[featureMapIndex].observation().sqrDistance(predictedFeatureMapLocation) <= sqrMaximalProjectionError)
						{
							BlobDescriptor::DescriptorSSD ssd;

							if (interestFeature.isDescriptorEqualEarlyReject(featureMapFeatures[featureMapIndex], normalizedThreshold, ssd))
							{
								if (ssd < ssdBest)
								{
									ssdSecondBest = ssdBest;
									ssdBest = ssd;

									bestFeatureMapIndexFirst = featureMapIndex;
								}
								else if (ssd < ssdSecondBest)
								{
									ssdSecondBest = ssd;
								}
							}
						}
					}
				}
			}

			if (bestFeatureMapIndexFirst != (unsigned int)(-1) && (ssdSecondBest == BlobDescriptor::descriptorMaximalSSD() || Scalar(ssdBest) < qualityRatio * Scalar(ssdSecondBest)))
			{
				candidates.addCandidate(n, bestFeatureMapIndexFirst);
			}
		}
	}

	return candidates.correspondences();
}

Correspondences::CorrespondencePairs UnidirectionalCorrespondences::determineFeatureCorrespondencesWithPose(const AnyCamera& anyCamera, const HomogenousMatrix4& world_P_camera, const CV::Detector::Blob::BlobFeatures& interestFeatures, const CV::Detector::Blob::BlobFeatures& featureMapFeatures, const size_t numberInterestFeatures, const Scalar maximalProjectionError, const Scalar threshold, const Scalar qualityRatio)
{
	UnidirectionalCorrespondences candidates((unsigned int)(interestFeatures.size()), (unsigned int)(featureMapFeatures.size()));

	ocean_assert(world_P_camera.isValid() && anyCamera.isValid());
	ocean_assert(maximalProjectionError > Numeric::eps());
	ocean_assert(threshold >= 0 && qualityRatio >= 0);

	ocean_assert(!interestFeatures.empty() && !featureMapFeatures.empty() && numberInterestFeatures >= 1);

	if (interestFeatures.empty() || featureMapFeatures.empty() || numberInterestFeatures == 0 || maximalProjectionError <= Numeric::eps() || !anyCamera.isValid() || !world_P_camera.isValid())
	{
		return candidates.correspondences();
	}

	const unsigned int maxInterestFeatures = (unsigned int)(std::min(numberInterestFeatures, interestFeatures.size()));
	const BlobDescriptor::DescriptorSSD normalizedThreshold(BlobDescriptor::DescriptorSSD(threshold * BlobDescriptor::descriptorNormalizationSqr()));

	// first, we project the 3D feature points of the feature map into the camera image and distribute all (visible) features into a 2D grid

	const unsigned int horizontalBins = std::max(1u, (unsigned int)(Scalar(anyCamera.width()) / maximalProjectionError + Scalar(0.5)));
	const unsigned int verticalBins = std::max(1u, (unsigned int)(Scalar(anyCamera.height()) / maximalProjectionError + Scalar(0.5)));

	Geometry::SpatialDistribution::DistributionArray projectedFeatureMapDistributionArray(Scalar(0), Scalar(0), Scalar(anyCamera.width()), Scalar(anyCamera.height()), horizontalBins, verticalBins);

	ocean_assert(!interestFeatures.empty());

	const HomogenousMatrix4 cameraFlipped_P_world = AnyCamera::standard2InvertedFlipped(world_P_camera);

	Vectors3 objectFeatureMapPoints(featureMapFeatures.size());
	Vectors2 projectedFeatureMapPoints(featureMapFeatures.size());

	for (size_t n = 0; n < featureMapFeatures.size(); ++n)
	{
		objectFeatureMapPoints[n] = featureMapFeatures[n].position();
	}

	anyCamera.projectToImageIF(cameraFlipped_P_world, objectFeatureMapPoints.data(), objectFeatureMapPoints.size(), projectedFeatureMapPoints.data());

	for (size_t n = 0; n < featureMapFeatures.size(); ++n)
	{
		const Vector2& projectedFeaturePoint = projectedFeatureMapPoints[n];

		const int binX = projectedFeatureMapDistributionArray.horizontalBin(projectedFeaturePoint.x());
		const int binY = projectedFeatureMapDistributionArray.horizontalBin(projectedFeaturePoint.y());

		if ((unsigned int)binX < horizontalBins && (unsigned int)binY < verticalBins)
		{
			ocean_assert(anyCamera.isInside(projectedFeaturePoint));

			projectedFeatureMapDistributionArray(binX, binY).push_back((unsigned int)n);
		}
	}

	// now, we apply a guided matching based on the 9-neighborhood within the 2D grid for each live feature point

	const Scalar sqrMaximalProjectionError = Numeric::sqr(maximalProjectionError);

	for (unsigned int n = 0; n < maxInterestFeatures; ++n)
	{
		const BlobFeature& interestFeature = interestFeatures[n];

		const int binX = projectedFeatureMapDistributionArray.horizontalBin(interestFeature.observation().x());
		const int binY = projectedFeatureMapDistributionArray.horizontalBin(interestFeature.observation().y());
		ocean_assert((unsigned int)binX < horizontalBins && (unsigned int)binY < verticalBins);

		BlobDescriptor::DescriptorSSD ssdBest(BlobDescriptor::descriptorMaximalSSD());
		BlobDescriptor::DescriptorSSD ssdSecondBest(BlobDescriptor::descriptorMaximalSSD());
		unsigned int bestFeatureMapIndexFirst = (unsigned int)(-1);

		// we iterate over the 9-neighborhood in the distribution array

		for (unsigned int bY = (unsigned int)std::max(0, binY - 1); bY < std::min((unsigned int)binY + 2u, verticalBins); ++bY)
		{
			for (unsigned int bX = (unsigned int)std::max(0, binX - 1); bX < std::min((unsigned int)binX + 2u, horizontalBins); ++bX)
			{
				const Indices32& featureMapIndices = projectedFeatureMapDistributionArray(bX, bY);

				for (const Index32& featureMapIndex : featureMapIndices)
				{
					ocean_assert(featureMapIndex < (unsigned int)(featureMapFeatures.size()));
					ocean_assert(featureMapIndex < (unsigned int)(projectedFeatureMapPoints.size()));

					if (projectedFeatureMapPoints[featureMapIndex].sqrDistance(interestFeature.observation()) <= sqrMaximalProjectionError)
					{
						BlobDescriptor::DescriptorSSD ssd;

						if (interestFeature.isDescriptorEqualEarlyReject(featureMapFeatures[featureMapIndex], normalizedThreshold, ssd))
						{
							if (ssd < ssdBest)
							{
								ssdSecondBest = ssdBest;
								ssdBest = ssd;

								bestFeatureMapIndexFirst = featureMapIndex;
							}
							else if (ssd < ssdSecondBest)
							{
								ssdSecondBest = ssd;
							}
						}
					}
				}
			}
		}

		if (bestFeatureMapIndexFirst != (unsigned int)(-1) && (ssdSecondBest == BlobDescriptor::descriptorMaximalSSD() || Scalar(ssdBest) < qualityRatio * Scalar(ssdSecondBest)))
		{
			candidates.addCandidate(n, bestFeatureMapIndexFirst);
		}
	}

	return candidates.correspondences();
}

}

}

}
