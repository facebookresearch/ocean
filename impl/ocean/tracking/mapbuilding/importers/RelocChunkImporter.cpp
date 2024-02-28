// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/mapbuilding/importers/RelocChunkImporter.h"

#include <arvr/libraries/thrift_if/common/gen-cpp2/visual_features_types.h> // @manual=fbsource//arvr/libraries/thrift_if/common:visual_features-cpp2-types
#include <arvr/libraries/thrift_if/reloc/gen-cpp2/dfm2_reloc_data_types.h> // @manual=fbsource//arvr/libraries/thrift_if/reloc:reloc-cpp2-types

#include <thrift/lib/cpp2/protocol/Serializer.h>

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

namespace Importers
{

namespace
{

/**
 * Reads a reloc chunk from a given file.
 * @param filename The name of the file from which the reloc chunk will be read
 * @param relocChunk The resulting reloc chunk object
 * @return True, if succeeded
 */
static bool readRelocData(const std::string& filename, arvr::thrift_if::relocalization::dfm2::Dfm2RelocData& relocChunk)
{
	std::ifstream inputStream(filename, std::ios::binary);

	if (!inputStream.good())
	{
		return false;
	}

	std::ostringstream stringStream;
	stringStream << inputStream.rdbuf();

	if (!stringStream.good())
	{
		return false;
	}

	const std::string buffer = stringStream.str();

	if (buffer.size() == 0)
	{
		return false;
	}

	const size_t position = apache::thrift::CompactSerializer::deserialize(stringStream.str(), relocChunk);

	return position == buffer.size();
}

static bool importFromRelocData(arvr::thrift_if::relocalization::dfm2::Dfm2RelocData& relocChunk, Vectors3& objectPoints, Indices32& objectPointIds, UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap& descriptorMap)
{
	namespace dfm2 = arvr::thrift_if::relocalization::dfm2;
	namespace visual_features = arvr::thrift_if::common::visual_features;

	ocean_assert(relocChunk.descriptor_type() == visual_features::DescriptorType::Multi3FREAK32);

	const std::vector<dfm2::Point3D>& chunkPoints = *relocChunk.points();

	objectPoints.clear();
	objectPoints.reserve(chunkPoints.size());

	objectPointIds.clear();
	objectPointIds.reserve(chunkPoints.size());

	descriptorMap.clear();
	descriptorMap.reserve(chunkPoints.size());

	constexpr size_t maxNumberLevels = 3;

	// Copy the string data into a new descriptor.
	constexpr size_t kNumBytesPerDescriptor = 32;

	for (const dfm2::Point3D& chunkPoint : chunkPoints)
	{
		const Eigen::Vector3d& position = *chunkPoint.position_world();
		objectPoints.emplace_back(Scalar(position[0]), Scalar(position[1]), Scalar(position[2]));

		CV::Detector::FREAKDescriptors32 multiViewDescriptor;
		multiViewDescriptor.reserve((*chunkPoint.descriptors()).size());

		for (const visual_features::AnyFeatureDescriptor& anyDescriptor : *chunkPoint.descriptors())
		{
			ocean_assert(anyDescriptor.getType() == visual_features::AnyFeatureDescriptor::multi_binary_descriptor);

			const std::vector<std::string>& descriptor = *anyDescriptor.multi_binary_descriptor_ref();

			ocean_assert(descriptor.size() <= maxNumberLevels);
			const size_t numberLevels = std::min(descriptor.size(), maxNumberLevels);

			CV::Detector::FREAKDescriptor32::MultilevelDescriptorData outputData;
			for (unsigned int i = 0; i < numberLevels; ++i)
			{
				ocean_assert(descriptor[i].size() == kNumBytesPerDescriptor);
				std::memcpy(outputData[i].data(), descriptor[i].data(), std::min(descriptor[i].size(), kNumBytesPerDescriptor));
			}

			multiViewDescriptor.emplace_back(std::move(outputData), numberLevels, 0.0f /*orientation*/);
		}

		const Index32 objectPointId = Index32(objectPointIds.size());

		const bool emplaceResult = descriptorMap.emplace(objectPointId, std::move(multiViewDescriptor)).second;
		ocean_assert_and_suppress_unused(emplaceResult, emplaceResult);

		objectPointIds.emplace_back(objectPointId);
	}

	ocean_assert(objectPoints.size() == descriptorMap.size());
	ocean_assert(objectPoints.size() == objectPointIds.size());

	return true;
}

static bool importFromRelocData(arvr::thrift_if::relocalization::dfm2::Dfm2RelocData& relocChunk, Vectors3& objectPoints, Indices32& objectPointIds, UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>::DescriptorMap& descriptorMap)
{
	namespace dfm2 = arvr::thrift_if::relocalization::dfm2;
	namespace visual_features = arvr::thrift_if::common::visual_features;

	ocean_assert(relocChunk.descriptor_type() == visual_features::DescriptorType::UFIVlfeatSift);

	const std::vector<dfm2::Point3D>& chunkPoints = *relocChunk.points();

	objectPoints.clear();
	objectPoints.reserve(chunkPoints.size());

	objectPointIds.clear();
	objectPointIds.reserve(chunkPoints.size());

	descriptorMap.clear();
	descriptorMap.reserve(chunkPoints.size());

	for (const dfm2::Point3D& chunkPoint : chunkPoints)
	{
		const Eigen::Vector3d& position = *chunkPoint.position_world();
		objectPoints.emplace_back(Scalar(position[0]), Scalar(position[1]), Scalar(position[2]));

		RelocChunkImporter::FloatDescriptors128 multiViewDescriptor;
		multiViewDescriptor.reserve((*chunkPoint.descriptors()).size());

		for (const visual_features::AnyFeatureDescriptor& anyDescriptor : *chunkPoint.descriptors())
		{
			ocean_assert(anyDescriptor.getType() == visual_features::AnyFeatureDescriptor::float_descriptor);

			const folly::small_vector<float, 8>& descriptor = *anyDescriptor.float_descriptor_ref();

			if (descriptor.size() != 128)
			{
				ocean_assert(false && "Invalid descriptor!");
				return false;
			}

			multiViewDescriptor.emplace_back();

			RelocChunkImporter::FloatDescriptor128& singleViewDescriptor = multiViewDescriptor.back();
			memcpy(singleViewDescriptor.data(), descriptor.data(), sizeof(float) * descriptor.size());

#ifdef OCEAN_DEBUG
			{
				// ensuring that the descriptor is normalized

				float sqrLength = 0.0f;

				for (size_t n = 0; n < 128; ++n)
				{
					sqrLength += NumericF::sqr(singleViewDescriptor[n]);
				}

				ocean_assert(NumericF::isEqual(NumericF::sqrt(sqrLength), 1.0f));
			}
#endif
		}

		const Index32 objectPointId = Index32(objectPointIds.size());

		const bool emplaceResult = descriptorMap.emplace(objectPointId, std::move(multiViewDescriptor)).second;
		ocean_assert_and_suppress_unused(emplaceResult, emplaceResult);

		objectPointIds.emplace_back(objectPointId);
	}

	ocean_assert(objectPoints.size() == descriptorMap.size());
	ocean_assert(objectPoints.size() == objectPointIds.size());

	return true;
}

}

bool RelocChunkImporter::importFromFile(const std::string& filename, Vectors3& objectPoints)
{
	arvr::thrift_if::relocalization::dfm2::Dfm2RelocData relocChunk;

	if (!readRelocData(filename, relocChunk))
	{
		return false;
	}

	namespace dfm2 = arvr::thrift_if::relocalization::dfm2;

	const std::vector<dfm2::Point3D>& chunkPoints = *relocChunk.points();

	objectPoints.clear();
	objectPoints.reserve(chunkPoints.size());

	for (const dfm2::Point3D& chunkPoint : chunkPoints)
	{
		const Eigen::Vector3d& position = *chunkPoint.position_world(); // Eigen::Vector3d wrapper
		objectPoints.emplace_back(Scalar(position[0]), Scalar(position[1]), Scalar(position[2]));
	}

	return true;
}

bool RelocChunkImporter::importFromFile(const std::string& filename, Vectors3& objectPoints, Indices32& objectPointIds, SharedUnifiedDescriptorMap& unifiedDescriptorMap)
{
	namespace dfm2 = arvr::thrift_if::relocalization::dfm2;
	namespace visual_features = arvr::thrift_if::common::visual_features;

	dfm2::Dfm2RelocData relocChunk;

	if (!readRelocData(filename, relocChunk))
	{
		return false;
	}

	if (relocChunk.descriptor_type() == visual_features::DescriptorType::Multi3FREAK32)
	{
		UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap descriptorMap;

		if (!importFromRelocData(relocChunk, objectPoints, objectPointIds, descriptorMap))
		{
			return false;
		}

		unifiedDescriptorMap = std::make_shared<UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(std::move(descriptorMap));

		return true;
	}
	else if (relocChunk.descriptor_type() == visual_features::DescriptorType::UFIVlfeatSift)
	{
		UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>::DescriptorMap descriptorMap;

		if (!importFromRelocData(relocChunk, objectPoints, objectPointIds, descriptorMap))
		{
			return false;
		}

		unifiedDescriptorMap = std::make_shared<UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>>(std::move(descriptorMap));

		return true;
	}

	ocean_assert(false && "Descriptor type is not supported!");
	return false;
}

bool RelocChunkImporter::importFromFile(const std::string& filename, Vectors3& objectPoints, MultiViewFREAKDescriptors32& multiViewDescriptors)
{
	arvr::thrift_if::relocalization::dfm2::Dfm2RelocData relocChunk;

	if (!readRelocData(filename, relocChunk))
	{
		return false;
	}

	namespace dfm2 = arvr::thrift_if::relocalization::dfm2;
	namespace visual_features = arvr::thrift_if::common::visual_features;

	if (relocChunk.descriptor_type() != visual_features::DescriptorType::Multi3FREAK32)
	{
		Log::error() << "Reloc chunk does not have Multi3FREAK32 features";
		return false;
	}

	const std::vector<dfm2::Point3D>& chunkPoints = *relocChunk.points();

	objectPoints.clear();
	objectPoints.reserve(chunkPoints.size());

	multiViewDescriptors.clear();
	multiViewDescriptors.reserve(chunkPoints.size());

	constexpr size_t maxNumberLevels = 3;

	// Copy the string data into a new descriptor.
	constexpr size_t kNumBytesPerDescriptor = 32;

	for (const dfm2::Point3D& chunkPoint : chunkPoints)
	{
		const Eigen::Vector3d& position = *chunkPoint.position_world();
		objectPoints.emplace_back(Scalar(position[0]), Scalar(position[1]), Scalar(position[2]));

		CV::Detector::FREAKDescriptors32 multiViewDescriptor;
		multiViewDescriptor.reserve((*chunkPoint.descriptors()).size());

		for (const visual_features::AnyFeatureDescriptor& anyDescriptor : *chunkPoint.descriptors())
		{
			ocean_assert(anyDescriptor.getType() == visual_features::AnyFeatureDescriptor::multi_binary_descriptor);

			const std::vector<std::string>& descriptor = *anyDescriptor.multi_binary_descriptor_ref();

			ocean_assert(descriptor.size() <= maxNumberLevels);
			const size_t numberLevels = std::min(descriptor.size(), maxNumberLevels);

			CV::Detector::FREAKDescriptor32::MultilevelDescriptorData outputData;
			for (unsigned int i = 0; i < numberLevels; ++i)
			{
				ocean_assert(descriptor[i].size() == kNumBytesPerDescriptor);
				std::memcpy(outputData[i].data(), descriptor[i].data(), std::min(descriptor[i].size(), kNumBytesPerDescriptor));
			}

			multiViewDescriptor.emplace_back(std::move(outputData), numberLevels, 0.0f /*orientation*/);
		}

		multiViewDescriptors.emplace_back(std::move(multiViewDescriptor));
	}

	ocean_assert(objectPoints.size() == multiViewDescriptors.size());

	return true;
}

bool RelocChunkImporter::importFromFile(const std::string& filename, Vectors3& objectPoints, MultiViewFloatDescriptors128& multiViewDescriptors)
{
	arvr::thrift_if::relocalization::dfm2::Dfm2RelocData relocChunk;

	if (!readRelocData(filename, relocChunk))
	{
		return false;
	}

	namespace dfm2 = arvr::thrift_if::relocalization::dfm2;
	namespace visual_features = arvr::thrift_if::common::visual_features;

	if (relocChunk.descriptor_type() != visual_features::DescriptorType::UFIVlfeatSift)
	{
		Log::error() << "Reloc chunk does not have UFIVlfeatSift features";
		return false;
	}

	const std::vector<dfm2::Point3D>& chunkPoints = *relocChunk.points();

	objectPoints.clear();
	objectPoints.reserve(chunkPoints.size());

	multiViewDescriptors.clear();
	multiViewDescriptors.reserve(chunkPoints.size());

	for (const dfm2::Point3D& chunkPoint : chunkPoints)
	{
		const Eigen::Vector3d& position = *chunkPoint.position_world();
		objectPoints.emplace_back(Scalar(position[0]), Scalar(position[1]), Scalar(position[2]));

		FloatDescriptors128 multiViewDescriptor;
		multiViewDescriptor.reserve((*chunkPoint.descriptors()).size());

		for (const visual_features::AnyFeatureDescriptor& anyDescriptor : *chunkPoint.descriptors())
		{
			ocean_assert(anyDescriptor.getType() == visual_features::AnyFeatureDescriptor::float_descriptor);

			const folly::small_vector<float, 8>& descriptor = *anyDescriptor.float_descriptor_ref();

			if (descriptor.size() != 128)
			{
				ocean_assert(false && "Invalid descriptor!");
				return false;
			}

			multiViewDescriptor.emplace_back();
			memcpy(multiViewDescriptor.back().data(), descriptor.data(), sizeof(float) * descriptor.size());
		}

		multiViewDescriptors.emplace_back(std::move(multiViewDescriptor));
	}

#ifdef OCEAN_DEBUG

	ocean_assert(objectPoints.size() == multiViewDescriptors.size());

	for (const FloatDescriptors128& multiViewDescriptor : multiViewDescriptors)
	{
		for (const FloatDescriptor128& floatDescriptor : multiViewDescriptor)
		{
			float sqrLength = 0.0f;

			for (size_t n = 0; n < 128; ++n)
			{
				sqrLength += NumericF::sqr(floatDescriptor[n]);
			}

			ocean_assert(NumericF::isEqual(NumericF::sqrt(sqrLength), 1.0f));
		}
	}

#endif

	return true;
}

bool RelocChunkImporter::importFromFile(const std::string& filename, Vectors3& objectPoints, Indices32& objectPointIds, UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap& descriptorMap)
{
	MultiViewFREAKDescriptors32 multiViewDescriptors;

	if (!importFromFile(filename, objectPoints, multiViewDescriptors))
	{
		return false;
	}

	ocean_assert(objectPoints.size() == multiViewDescriptors.size());

	objectPointIds.clear();
	objectPointIds.reserve(objectPoints.size());

	descriptorMap.clear();
	descriptorMap.reserve(objectPoints.size());

	for (size_t nPoint = 0; nPoint < multiViewDescriptors.size(); ++nPoint)
	{
		const Index32 objectPointId = Index32(nPoint);
		objectPointIds.emplace_back(objectPointId);

		descriptorMap.emplace(objectPointId, std::move(multiViewDescriptors[nPoint]));
	}

	return true;
}

}

}

}

}
