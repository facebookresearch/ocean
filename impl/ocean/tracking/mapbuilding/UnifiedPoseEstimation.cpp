/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/UnifiedPoseEstimation.h"
#include "ocean/tracking/mapbuilding/PoseEstimationT.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

UnifiedBruteForcePoseEstimation::UnifiedBruteForcePoseEstimation(const ObjectPointMap& objectPointMap, const UnifiedDescriptorMap& unifiedDescriptorMap)
{
	objectPoints_.reserve(objectPointMap.size());
	objectPointIds_.reserve(objectPointMap.size());

	for (const ObjectPointMap::value_type& objectPointPair : objectPointMap)
	{
		objectPointIds_.emplace_back(objectPointPair.first);
		objectPoints_.emplace_back(objectPointPair.second);
	}

	objectPointDescriptors_ = extractObjectPointDescriptors(unifiedDescriptorMap, objectPointIds_);
}

bool UnifiedBruteForcePoseEstimation::determinePose(const AnyCamera& camera, const UnifiedDescriptors& imagePointDescriptors, const Vector2* imagePoints, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const UnifiedMatching::DistanceValue& maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate, Indices32* usedObjectPointIndices, Indices32* usedImagePointIndices, const HomogenousMatrix4& world_T_roughCamera, Worker* worker) const
{
	ocean_assert(camera.isValid());
	ocean_assert(imagePoints != nullptr);
	ocean_assert(imagePointDescriptors.numberDescriptors() >= 4);

	ocean_assert(maximalProjectionError >= Scalar(0));
	ocean_assert(inlierRate > Scalar(0) && inlierRate <= Scalar(1));

	ocean_assert(objectPoints_.size() >= 4);
	if (objectPoints_.size() < 4)
	{
		return false;
	}

	ocean_assert(objectPointDescriptors_ != nullptr);
	if (objectPointDescriptors_ == nullptr)
	{
		return false;
	}

	switch (objectPointDescriptors_->descriptorType())
	{
		case UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256:
		{
			const UnifiedDescriptorsFreakMultiLevelMultiViewDescriptor256* objectPointDescriptorsFreak = dynamic_cast<const UnifiedDescriptorsFreakMultiLevelMultiViewDescriptor256*>(objectPointDescriptors_.get());
			ocean_assert(objectPointDescriptorsFreak != nullptr);

			if (objectPointDescriptorsFreak != nullptr)
			{
				const UnifiedDescriptorsFreakMultiLevelSingleViewDescriptor256* imagePointDescriptorsFreak = dynamic_cast<const UnifiedDescriptorsFreakMultiLevelSingleViewDescriptor256*>(&imagePointDescriptors);

				ocean_assert(imagePointDescriptorsFreak != nullptr);
				if (imagePointDescriptorsFreak == nullptr)
				{
					return false;
				}

				using ImagePointDescriptor = DescriptorHandling::FreakMultiDescriptor256;
				using ObjectPointDescriptor = DescriptorHandling::FreakMultiDescriptors256;

				return PoseEstimationT::determinePoseBruteForce<ImagePointDescriptor, ObjectPointDescriptor, unsigned int, DescriptorHandling::determineFreakDistance>(camera, objectPoints_.data(), objectPointDescriptorsFreak->descriptors(), objectPoints_.size(), imagePoints, imagePointDescriptorsFreak->descriptors(), imagePointDescriptors.numberDescriptors(), randomGenerator, world_T_camera, minimalNumberCorrespondences, maximalDescriptorDistance.binaryDistance(), maximalProjectionError, inlierRate, usedObjectPointIndices, usedImagePointIndices, world_T_roughCamera, worker);
			}

			break;
		}

		case UnifiedDescriptor::DT_FLOAT_SINGLE_LEVEL_MULTI_VIEW_128:
		{
			const UnifiedDescriptorsFloatSingleLevelMultiView<128u>* objectPointDescriptorsFloat = dynamic_cast<const UnifiedDescriptorsFloatSingleLevelMultiView<128u>*>(objectPointDescriptors_.get());
			ocean_assert(objectPointDescriptorsFloat != nullptr);

			if (objectPointDescriptorsFloat != nullptr)
			{
				const UnifiedDescriptorsFloatSingleLevelSingleView<128u>* imagePointDescriptorsFloat = dynamic_cast<const UnifiedDescriptorsFloatSingleLevelSingleView<128u>*>(&imagePointDescriptors);

				ocean_assert(imagePointDescriptorsFloat != nullptr);
				if (imagePointDescriptorsFloat == nullptr)
				{
					return false;
				}

				using ImagePointDescriptor = UnifiedDescriptor::FloatDescriptor<128u>;
				using ObjectPointDescriptor = UnifiedDescriptor::FloatDescriptors<128u>;

				const float maximalSqrDescriptorDistance = NumericF::sqr(maximalDescriptorDistance.floatDistance());

				if (!PoseEstimationT::determinePoseBruteForce<ImagePointDescriptor, ObjectPointDescriptor, float, UnifiedDescriptorT<ImagePointDescriptor>::determineDistance>(camera, objectPoints_.data(), objectPointDescriptorsFloat->descriptors(), objectPoints_.size(), imagePoints, imagePointDescriptorsFloat->descriptors(), imagePointDescriptors.numberDescriptors(), randomGenerator, world_T_camera, minimalNumberCorrespondences, maximalSqrDescriptorDistance, maximalProjectionError, inlierRate, usedObjectPointIndices, usedImagePointIndices, world_T_roughCamera, worker))
				{
					return false;
				}

				return true;
			}

			break;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid feature descriptors!");
	return false;
}

bool UnifiedBruteForcePoseEstimation::determineBruteForceMatchings(const UnifiedDescriptors& descriptorsA, const UnifiedDescriptors& descriptorsB, const UnifiedMatching::DistanceValue& maximalDescriptorDistance, Indices32& indicesA, Indices32& indicesB, std::vector<double>* distances, Worker* worker)
{
	indicesA.clear();
	indicesB.clear();

	if (distances != nullptr)
	{
		distances->clear();
	}

	if (descriptorsA.descriptorType() != descriptorsB.descriptorType())
	{
		ocean_assert(false && "Invalid descriptor type!");
		return false;
	}

	switch (descriptorsA.descriptorType())
	{
		case UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_SINGLE_VIEW_256:
		{
			using SpecializedUnifiedDescriptors = UnifiedDescriptorsFreakMultiLevelSingleViewDescriptor256;
			using DistanceType = unsigned int;

			return determineBruteForceMatchings<SpecializedUnifiedDescriptors, SpecializedUnifiedDescriptors, DistanceType>(descriptorsA, descriptorsB, maximalDescriptorDistance.distance<DistanceType>(), indicesA, indicesB, distances, worker);
		}

		case UnifiedDescriptor::DT_FLOAT_SINGLE_LEVEL_SINGLE_VIEW_128:
		{
			using SpecializedUnifiedDescriptors = UnifiedDescriptorsFloatSingleLevelSingleView<128u>;
			using DistanceType = float;

			return determineBruteForceMatchings<SpecializedUnifiedDescriptors, SpecializedUnifiedDescriptors, DistanceType>(descriptorsA, descriptorsB, maximalDescriptorDistance.distance<DistanceType>(), indicesA, indicesB, distances, worker);
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid feature descriptors!");
	return false;
}

template <typename TUnifiedDescriptorsA, typename TUnifiedDescriptorsB, typename TDescriptorDistance>
bool UnifiedBruteForcePoseEstimation::determineBruteForceMatchings(const UnifiedDescriptors& descriptorsA, const UnifiedDescriptors& descriptorsB, const TDescriptorDistance maximalDescriptorDistance, Indices32& indicesA, Indices32& indicesB, std::vector<double>* distances, Worker* worker)
{
	const TUnifiedDescriptorsA* specializedDescriptorsA = dynamic_cast<const TUnifiedDescriptorsA*>(&descriptorsA);
	const TUnifiedDescriptorsB* specializedDescriptorsB = dynamic_cast<const TUnifiedDescriptorsB*>(&descriptorsB);

	ocean_assert(specializedDescriptorsA != nullptr && specializedDescriptorsB != nullptr);
	if (specializedDescriptorsA != nullptr && specializedDescriptorsB != nullptr)
	{
		using DescriptorA = typename TUnifiedDescriptorsA::Descriptor;
		using DescriptorB = typename TUnifiedDescriptorsB::Descriptor;

		determineBruteForceMatchings<DescriptorA, DescriptorB, TDescriptorDistance, UnifiedDescriptorT<DescriptorA>::determineDistance>(specializedDescriptorsA->descriptors(), specializedDescriptorsA->numberDescriptors(), specializedDescriptorsB->descriptors(), specializedDescriptorsB->numberDescriptors(), maximalDescriptorDistance, indicesA, indicesB, distances, worker);

		return true;
	}

	return false;
}

SharedUnifiedDescriptors UnifiedBruteForcePoseEstimation::extractObjectPointDescriptors(const UnifiedDescriptorMap& unifiedDescriptorMap, const Indices32& objectPointIds)
{
	switch (unifiedDescriptorMap.descriptorType())
	{
		case UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256:
		{
			const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256* unifiedDescriptorMapFreak = dynamic_cast<const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256*>(&unifiedDescriptorMap);
			ocean_assert(unifiedDescriptorMapFreak != nullptr);

			if (unifiedDescriptorMapFreak != nullptr)
			{
				return extractObjectPointDescriptors<UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(*unifiedDescriptorMapFreak, objectPointIds);
			}

			break;
		}

		case UnifiedDescriptor::DT_FLOAT_SINGLE_LEVEL_MULTI_VIEW_128:
		{
			const UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>* unifiedDescriptorMapFloat = dynamic_cast<const UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>*>(&unifiedDescriptorMap);
			ocean_assert(unifiedDescriptorMapFloat != nullptr);

			if (unifiedDescriptorMapFloat != nullptr)
			{
				return extractObjectPointDescriptors<UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>>(*unifiedDescriptorMapFloat, objectPointIds);
			}

			break;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid feature descriptors!");
	return nullptr;
}

template <typename T>
SharedUnifiedDescriptors UnifiedBruteForcePoseEstimation::extractObjectPointDescriptors(const T& unifiedDescriptorMap, const Indices32& objectPointIds)
{
	using DescriptorMap = typename T::DescriptorMap;
	using MultiViewDescriptor = typename T::Descriptor;
	using MultiViewDescriptors = std::vector<MultiViewDescriptor>;

	const DescriptorMap& descriptorMap = unifiedDescriptorMap.descriptorMap();

	MultiViewDescriptors multiViewDescriptors;
	multiViewDescriptors.reserve(objectPointIds.size());

	for (const Index32& objectPointId : objectPointIds)
	{
		const typename DescriptorMap::const_iterator iDescriptor = descriptorMap.find(objectPointId);

		if (iDescriptor == descriptorMap.cend())
		{
			ocean_assert(false && "Missing object point description!");
			return nullptr;
		}

		multiViewDescriptors.emplace_back(iDescriptor->second);
	}

	return std::make_shared<UnifiedDescriptorsT<MultiViewDescriptor>>(std::move(multiViewDescriptors));
}

}

}

}
