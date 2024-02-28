// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/DescriptorContainer.h"
#include "ocean/unifiedfeatures/DescriptorGenerator.h"
#include "ocean/unifiedfeatures/FeatureContainer.h"
#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/UnifiedFeatures.h"
#include "ocean/unifiedfeatures/Utilities.h"
#include "ocean/unifiedfeatures/oceanblob/OBFeatureContainer.h"
#include "ocean/unifiedfeatures/oceanblob/OBDescriptorContainer.h"


#include <ar/visual_features/BinaryDescriptor.h>
#include <ar/visual_features/FloatDescriptor.h>
#include <ar/visual_features/Keypoint.h>
#include <ar/visual_features/MultiLevelDescriptor.h>
#include <ar/visual_features/Uint8Descriptor.h>

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

#include <memory>

namespace Ocean
{

namespace UnifiedFeatures
{

/**
 * Definition of utility functions
 * @ingroup unifiedfeatures
 */
class SurrealUtilities
{
	public:

		/**
		 * Extracts Surreal keypoints and single-level descriptors from an image
		 * @param featureDetector The feature detector that will be used to extract keypoints, must be valid
		 * @param descriptorGenerator The generator that is used to create the descriptors for the keypoints, the must be single-level descriptors, must be valid
		 * @param frame The frame for which keypoints and descriptor will be computed, must be valid and have the pixel format that is expected by the detector and descriptor generator
		 * @param border The number of pixels around the image border that should be ignored by the detector and descriptor generator, range: [0, infinity)
		 * @param keypoints The extracted keypoints (using Surreal data structures)
		 * @param descriptors The generated single-level descriptors (using Surreal data structures)
		 * @param worker Optional worker instance for parallelization
		 * @return True if the extraction was successful, otherwise false
		 * @tparam tDescriptorCategory The descriptor category to indicate at compile-time which type of descriptor is used, must be valid
		 * @tparam TDescriptor The type of the underlying Surreal descriptor that should be used, must be a valid implementation
		 */
		template <DescriptorCategory tDescriptorCategory, typename TDescriptor>
		static bool extractFeaturesAndDescriptors(const std::shared_ptr<FeatureDetector>& featureDetector, const std::shared_ptr<DescriptorGenerator>& descriptorGenerator, const Frame& frame, const unsigned int border, std::vector<surreal::features::Keypoint>& keypoints, std::vector<TDescriptor>& descriptors, Worker* worker = nullptr);

		/**
		 * Extracts Surreal keypoints and multi-level descriptors from an image
		 * @param featureDetector The feature detector that will be used to extract keypoints, must be valid
		 * @param descriptorGenerator The generator that is used to create the descriptors for the keypoints, the must be multi-level descriptors, must be valid
		 * @param frame The frame for which keypoints and descriptor will be computed, must be valid and have the pixel format that is expected by the detector and descriptor generator
		 * @param border The number of pixels around the image border that should be ignored by the detector and descriptor generator, range: [0, infinity)
		 * @param keypoints The extracted keypoints (using Surreal data structures)
		 * @param multiLevelDescriptors The resulting Surreal multi-level binary descriptors
		 * @param worker Optional worker instance for parallelization
		 * @return True if the extraction was successful, otherwise false
		 * @tparam TDescriptor The type of the underlying Surreal descriptor that should be used, must be a valid implementation
		 * @tparam tDescriptorCategory The descriptor category to indicate at compile-time which type of descriptor is used, must be valid
		 * @tparam tSize The maximum number of layers that the selected descriptors can have, range: [1, infinity)
		 */
		template <DescriptorCategory tDescriptorCategory, typename TDescriptor, size_t tMaxLevels>
		static bool extractFeaturesAndMultiLevelDescriptors(const std::shared_ptr<FeatureDetector>& featureDetector, const std::shared_ptr<DescriptorGenerator>& descriptorGenerator, const Frame& frame, const unsigned int border, std::vector<surreal::features::Keypoint>& keypoints, std::vector<surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels>>& multiLevelDescriptors, Worker* worker = nullptr);

		/**
		 * Converts UFI-Thrift keypoints to Surreal keypoints
		 * @param thriftKeypoints The UFI-Thrift keypoints that will be converted into Surreal keypoints
		 * @param surrealKeypoints The resulting Surreal keypoints
		 * @return True if the conversion was successful, otherwise false
		 */
		inline static bool convertToSurrealKeypoints(const std::vector<Thrift::Keypoint>& thriftKeypoints, std::vector<surreal::features::Keypoint>& surrealKeypoints);

		/**
		 * Converts a UFI-Thrift binary descriptor to Surreal multi-level binary descriptor
		 * @param thriftBinaryDescriptor The UFI-Thrift binary descriptor that will be converted
		 * @param multiLevelDescriptor The resulting Surreal multi-level binary descriptor
		 * @return True if the conversion was successful, otherwise false
		 */
		template <typename TDescriptor, size_t tMaxLevels>
		static bool convertToBinaryMultiLevelDescriptor(const Thrift::BinaryDescriptor& thriftBinaryDescriptor, surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels>& multiLevelDescriptor);

		/**
		 * Converts UFI-Thrift binary descriptors to Surreal multi-level binary descriptors
		 * @param thriftBinaryDescriptors The UFI-Thrift binary descriptors that will be converted
		 * @param multiLevelDescriptors The resulting Surreal multi-level binary descriptors
		 * @return True if the conversion was successful, otherwise false
		 */
		template <typename TDescriptor, size_t tMaxLevels>
		static bool convertToBinaryMultiLevelDescriptors(const std::vector<Thrift::BinaryDescriptor>& thriftBinaryDescriptors, std::vector<surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels>>& multiLevelDescriptors);

		/**
		 * Converts a UFI-Thrift float descriptor to Surreal single-level float descriptor
		 * @param thriftFloatDescriptor The UFI-Thrift float descriptor that will be converted
		 * @param descriptor The resulting Surreal single-level float descriptor
		 * @return True if the conversion was successful, otherwise false
		 */
		template <typename TDescriptor>
		static bool convertToFloatDescriptor(const Thrift::FloatDescriptor& thriftFloatDescriptor, TDescriptor& descriptor);

		/**
		 * Converts a UFI-Thrift uint8 descriptor to Surreal single-level uint8 descriptor
		 * @param thriftUint8Descriptor The UFI-Thrift uint8 descriptor that will be converted
		 * @param descriptor The resulting Surreal single-level uint8 descriptor
		 * @return True if the conversion was successful, otherwise false
		 */
		template <typename TDescriptor>
		static bool convertToUint8Descriptor(const Thrift::Uint8Descriptor& thriftUint8Descriptor, TDescriptor& descriptor);

		/**
		 * Converts UFI-Thrift float descriptors to Surreal single-level float descriptors
		 * @param thriftFloatDescriptors The UFI-Thrift float descriptors that will be converted
		 * @param descriptors The resulting Surreal single-level float descriptors
		 * @return True if the conversion was successful, otherwise false
		 */
		template <typename TDescriptor>
		static bool convertToFloatDescriptors(const std::vector<Thrift::FloatDescriptor>& thriftFloatDescriptors, std::vector<TDescriptor>& descriptors);

		/**
		 * Converts UFI-Thrift uint8 descriptors to Surreal single-level float descriptors
		 * @param thriftUint8Descriptors The UFI-Thrift uint8 descriptors that will be converted
		 * @param descriptors The resulting Surreal single-level uint8 descriptors
		 * @return True if the conversion was successful, otherwise false
		 */
		template <typename TDescriptor>
		static bool convertToUint8Descriptors(const std::vector<Thrift::Uint8Descriptor>& thriftUint8Descriptors, std::vector<TDescriptor>& descriptors);
};

template <DescriptorCategory tDescriptorCategory, typename TDescriptor>
bool SurrealUtilities::extractFeaturesAndDescriptors(const std::shared_ptr<FeatureDetector>& featureDetector, const std::shared_ptr<DescriptorGenerator>& descriptorGenerator, const Frame& frame, const unsigned int border, std::vector<surreal::features::Keypoint>& keypoints, std::vector<TDescriptor>& descriptors, Worker* worker)
{
	static_assert(tDescriptorCategory != DescriptorCategory::UNKNOWN, "Invalid descriptor category selected.");

	keypoints.clear();
	descriptors.clear();

	if (featureDetector == nullptr || descriptorGenerator == nullptr || !frame.isValid())
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	std::shared_ptr<FeatureContainer> featureContainer;
	const FeatureDetector::Result detectionResult = featureDetector->detectFeatures(frame, featureContainer, border, worker);

	if (detectionResult != FeatureDetector::Result::SUCCESS)
	{
		ocean_assert(false && "Feature detection failed!");
		return false;
	}

	std::shared_ptr<DescriptorContainer> descriptorContainer;
	const DescriptorGenerator::Result descriptorResult = descriptorGenerator->generateDescriptors(frame, *featureContainer, descriptorContainer, worker);

	if(descriptorResult != DescriptorGenerator::Result::SUCCESS)
	{
		ocean_assert(false && "Computation of descriptors failed!");
		return false;
	}

	Thrift::ContainerPair thriftContainerPair;
	if (!Utilities::toThrift(*featureContainer, *descriptorContainer, thriftContainerPair))
	{
		assert(false && "Never be here!");
		return false;
	}

	// Keypoints
	const Thrift::FeatureContainer& thriftFeatureContainer = *(thriftContainerPair.featuresContainer_ref());
	if (thriftFeatureContainer.featureUnion_ref()->getType() != Thrift::FeatureUnion::Type::keypoints || !thriftFeatureContainer.featureUnion_ref()->keypoints_ref().has_value())
	{
		assert(false && "Never be here!");
		return false;
	}

	const std::vector<Thrift::Keypoint>& thriftKeypoints = *(thriftFeatureContainer.featureUnion_ref()->keypoints_ref());

	if (!SurrealUtilities::convertToSurrealKeypoints(thriftKeypoints, keypoints))
	{
		assert(false && "Never be here!");
		return false;
	}

	// Descriptors
	const Thrift::DescriptorContainer& thriftDescriptorContainer = *(thriftContainerPair.descriptorContainer_ref());

	if constexpr (tDescriptorCategory == DescriptorCategory::FLOAT_DESCRIPTOR)
	{
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::floatDescriptors);
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->floatDescriptors_ref().has_value());
		return SurrealUtilities::convertToFloatDescriptors(*(thriftDescriptorContainer.descriptorUnion_ref()->floatDescriptors_ref()), descriptors);
	}
	else if constexpr (tDescriptorCategory == DescriptorCategory::BINARY_DESCRIPTOR)
	{
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::binaryDescriptors);
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->binaryDescriptors_ref().has_value());
		ocean_assert(false && "Not implemented yet!");
		return false;
	}
	else if constexpr (tDescriptorCategory == DescriptorCategory::UINT8_DESCRIPTOR)
	{
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::uint8Descriptors);
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->uint8Descriptors_ref().has_value());
		return SurrealUtilities::convertToUint8Descriptors(*(thriftDescriptorContainer.descriptorUnion_ref()->uint8Descriptors_ref()), descriptors);
	}

	ocean_assert(false && "Never be here!");
	return false;
}

template <DescriptorCategory tDescriptorCategory, typename TDescriptor, size_t tMaxLevels>
bool SurrealUtilities::extractFeaturesAndMultiLevelDescriptors(const std::shared_ptr<FeatureDetector>& featureDetector, const std::shared_ptr<DescriptorGenerator>& descriptorGenerator, const Frame& frame, const unsigned int border, std::vector<surreal::features::Keypoint>& keypoints, std::vector<surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels>>& multiLevelDescriptors, Worker* worker)
{
	static_assert(tDescriptorCategory != DescriptorCategory::UNKNOWN, "Invalid descriptor category selected.");

	keypoints.clear();
	multiLevelDescriptors.clear();

	if (featureDetector == nullptr || descriptorGenerator == nullptr || !frame.isValid())
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	std::shared_ptr<FeatureContainer> featureContainer;
	const FeatureDetector::Result detectionResult = featureDetector->detectFeatures(frame, featureContainer, border, worker);

	if (detectionResult != FeatureDetector::Result::SUCCESS)
	{
		ocean_assert(false && "Feature detection failed!");
		return false;
	}

	std::shared_ptr<DescriptorContainer> descriptorContainer;
	const DescriptorGenerator::Result descriptorResult = descriptorGenerator->generateDescriptors(frame, *featureContainer, descriptorContainer, worker);

	if(descriptorResult != DescriptorGenerator::Result::SUCCESS)
	{
		ocean_assert(false && "Computation of descriptors failed!");
		return false;
	}

	Thrift::ContainerPair thriftContainerPair;
	if (!Utilities::toThrift(*featureContainer, *descriptorContainer, thriftContainerPair))
	{
		assert(false && "Never be here!");
		return false;
	}

	// Keypoints
	const Thrift::FeatureContainer& thriftFeatureContainer = *(thriftContainerPair.featuresContainer_ref());
	if (thriftFeatureContainer.featureUnion_ref()->getType() != Thrift::FeatureUnion::Type::keypoints || !thriftFeatureContainer.featureUnion_ref()->keypoints_ref().has_value())
	{
		assert(false && "Never be here!");
		return false;
	}

	const std::vector<Thrift::Keypoint>& thriftKeypoints = *(thriftFeatureContainer.featureUnion_ref()->keypoints_ref());

	if (!SurrealUtilities::convertToSurrealKeypoints(thriftKeypoints, keypoints))
	{
		assert(false && "Never be here!");
		return false;
	}

	// Descriptors
	const Thrift::DescriptorContainer& thriftDescriptorContainer = *(thriftContainerPair.descriptorContainer_ref());

	if constexpr (tDescriptorCategory == DescriptorCategory::FLOAT_DESCRIPTOR)
	{
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::floatDescriptors);
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->floatDescriptors_ref().has_value());
		ocean_assert(false && "Not implemented yet!");
		return false;
	}
	else if constexpr (tDescriptorCategory == DescriptorCategory::BINARY_DESCRIPTOR)
	{
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::binaryDescriptors);
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->binaryDescriptors_ref().has_value());
		return Ocean::UnifiedFeatures::SurrealUtilities::convertToBinaryMultiLevelDescriptors(*(thriftDescriptorContainer.descriptorUnion_ref()->binaryDescriptors_ref()), multiLevelDescriptors);
	}
	else if constexpr (tDescriptorCategory == DescriptorCategory::UINT8_DESCRIPTOR)
	{
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::uint8Descriptors);
		ocean_assert(thriftDescriptorContainer.descriptorUnion_ref()->uint8Descriptors_ref().has_value());
		ocean_assert(false && "Not implemented yet!");
		return false;
	}

	ocean_assert(false && "Never be here!");
	return false;
}

inline bool SurrealUtilities::convertToSurrealKeypoints(const std::vector<Thrift::Keypoint>& thriftKeypoints, std::vector<surreal::features::Keypoint>& surrealKeypoints)
{
	surrealKeypoints.clear();

	surrealKeypoints.reserve(thriftKeypoints.size());
	for (const Thrift::Keypoint& thriftKeypoint : thriftKeypoints)
	{
		const int octave = thriftKeypoint.octave_ref() ? *(thriftKeypoint.octave_ref()) : 0;
		const float strength = thriftKeypoint.strength_ref() ? *(thriftKeypoint.strength_ref()) : 0.0f;
		const float orientation = thriftKeypoint.orientation_ref() ? *(thriftKeypoint.orientation_ref()) : 0.0f;
		const float scale = thriftKeypoint.scale_ref() ? *(thriftKeypoint.scale_ref()) : 1.0f;
		surrealKeypoints.emplace_back(surreal::features::Keypoint{Eigen::Vector2f(thriftKeypoint.get_x(), thriftKeypoint.get_y()), octave, scale, strength, orientation});
	}

	return true;
}

template <typename TDescriptor, size_t tMaxLevels>
bool SurrealUtilities::convertToBinaryMultiLevelDescriptor(const Thrift::BinaryDescriptor& thriftBinaryDescriptor, surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels>& multiLevelDescriptor)
{
	ocean_assert(thriftBinaryDescriptor.descriptorData_ref().is_set());
	const std::vector<::std::string>& descriptorData = *(thriftBinaryDescriptor.descriptorData_ref());
	ocean_assert(descriptorData.size() != 0 && descriptorData.size() <= tMaxLevels);

	surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels> localMultiLevelDescriptor;

	ocean_assert(descriptorData.size() <= std::numeric_limits<uint8_t>::max() && "Potential overflow at MultiLevelDescriptor<T, U>::numValid");
	localMultiLevelDescriptor.numValid = uint8_t(descriptorData.size());

	for (size_t level = 0; level < descriptorData.size(); ++level)
	{
		// **TODO** Check descriptorData[level].size() == MultiLevelDescriptor<TDescriptor, tMaxLevels>::Descriptor::descriptorLengthInBytes, "Incompatible descriptor sizes"
		std::copy(descriptorData[level].cbegin(), descriptorData[level].cend(), localMultiLevelDescriptor.descriptors[level].begin());
	}

	multiLevelDescriptor = std::move(localMultiLevelDescriptor);

	return true;
}

template <typename TDescriptor, size_t tMaxLevels>
bool SurrealUtilities::convertToBinaryMultiLevelDescriptors(const std::vector<Thrift::BinaryDescriptor>& thriftBinaryDescriptors, std::vector<surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels>>& multiLevelDescriptors)
{
	std::vector<surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels>> localMultiLevelDescriptors;
	localMultiLevelDescriptors.reserve(thriftBinaryDescriptors.size());

	for (size_t i = 0; i < thriftBinaryDescriptors.size(); ++i)
	{
		surreal::features::MultiLevelDescriptor<TDescriptor, tMaxLevels> multiLevelDescriptor;
		if (!convertToBinaryMultiLevelDescriptor(thriftBinaryDescriptors[i], multiLevelDescriptor))
		{
			return false;
		}

		localMultiLevelDescriptors.emplace_back(std::move(multiLevelDescriptor));
	}

	multiLevelDescriptors = std::move(localMultiLevelDescriptors);

	return true;
}

template <typename TDescriptor>
bool SurrealUtilities::convertToFloatDescriptor(const Thrift::FloatDescriptor& thriftFloatDescriptor, TDescriptor& descriptor)
{
	ocean_assert(thriftFloatDescriptor.descriptorData_ref().is_set());
	const std::vector<std::vector<float>>& descriptorData = *(thriftFloatDescriptor.descriptorData_ref());

	ocean_assert(descriptorData.size() == 1);
	ocean_assert(descriptorData[0].size() == TDescriptor::dimensions);

	surreal::features::FloatDescriptorData<TDescriptor::dimensions> floatDescriptorData;
	std::copy(descriptorData[0].cbegin(), descriptorData[0].cend(), /* floatDescriptorData.data() == */ &(floatDescriptorData(0)));

	descriptor.setData(floatDescriptorData);

	return true;
}

template <typename TDescriptor>
bool SurrealUtilities::convertToUint8Descriptor(const Thrift::Uint8Descriptor& thriftUint8Descriptor, TDescriptor& descriptor)
{
	ocean_assert(thriftUint8Descriptor.descriptorData_ref().is_set());
	const std::vector<std::vector<int8_t>>& descriptorData = *(thriftUint8Descriptor.descriptorData_ref());

	ocean_assert(descriptorData.size() == 1);
	ocean_assert(descriptorData[0].size() == TDescriptor::dimensions);

	surreal::features::Uint8Descriptor<TDescriptor::dimensions> uint8DescriptorData;
	std::copy(descriptorData[0].cbegin(), descriptorData[0].cend(), uint8DescriptorData.data());

	descriptor.setData(uint8DescriptorData);

	return true;
}

template <typename TDescriptor>
bool SurrealUtilities::convertToFloatDescriptors(const std::vector<Thrift::FloatDescriptor>& thriftFloatDescriptors, std::vector<TDescriptor>& descriptors)
{
	std::vector<TDescriptor> localDescriptors;
	localDescriptors.reserve(thriftFloatDescriptors.size());

	for (size_t i = 0; i < thriftFloatDescriptors.size(); ++i)
	{
		TDescriptor descriptor;
		if (!convertToFloatDescriptor(thriftFloatDescriptors[i], descriptor))
		{
			return false;
		}

		localDescriptors.emplace_back(std::move(descriptor));
	}

	descriptors = std::move(localDescriptors);

	return true;
}

template <typename TDescriptor>
bool SurrealUtilities::convertToUint8Descriptors(const std::vector<Thrift::Uint8Descriptor>& thriftUint8Descriptors, std::vector<TDescriptor>& descriptors)
{
	std::vector<TDescriptor> localDescriptors;
	localDescriptors.reserve(thriftUint8Descriptors.size());

	for (size_t i = 0; i < thriftUint8Descriptors.size(); ++i)
	{
		TDescriptor descriptor;
		if (!convertToUint8Descriptor(thriftUint8Descriptors[i], descriptor))
		{
			return false;
		}

		localDescriptors.emplace_back(std::move(descriptor));
	}

	descriptors = std::move(localDescriptors);

	return true;
}

} // namespace Ocean

} // namespace UnifiedFeatures
