/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/Utilities.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

bool Utilities::writeDescriptorMap(const UnifiedDescriptorMap& unifiedDescriptorMap, IO::OutputBitstream& outputStream)
{
	if (unifiedDescriptorMap.descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256)
	{
		return false;
	}

	const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256* unifiedDescriptorMapFreak256 = dynamic_cast<const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256*>(&unifiedDescriptorMap);
	ocean_assert(unifiedDescriptorMapFreak256 != nullptr);

	if (unifiedDescriptorMapFreak256 == nullptr)
	{
		return false;
	}

	if (!outputStream.write<std::string>("OCN_TRACKING_FREAK_FEATURES_256"))
	{
		return false;
	}

	constexpr unsigned int version = 1u;

	if (!outputStream.write<unsigned int>(version))
	{
		return false;
	}

	const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap& descriptorMap = unifiedDescriptorMapFreak256->descriptorMap();

	if (!outputStream.write<unsigned int>((unsigned int)(descriptorMap.size())))
	{
		return false;
	}

	for (FreakMultiDescriptorMap256::const_iterator iFreak = descriptorMap.cbegin(); iFreak != descriptorMap.cend(); ++iFreak)
	{
		const Index32& objectPointId = iFreak->first;
		const CV::Detector::FREAKDescriptors32& freakDescriptors = iFreak->second;

		if (!outputStream.write<unsigned int>(objectPointId))
		{
			return false;
		}

		if (!outputStream.write<unsigned int>((unsigned int)(freakDescriptors.size())))
		{
			return false;
		}

		for (const CV::Detector::FREAKDescriptor32& freakDescriptor : freakDescriptors)
		{
			if (!outputStream.write<float>(freakDescriptor.orientation()))
			{
				return false;
			}

			if (!outputStream.write<unsigned int>(freakDescriptor.descriptorLevels()))
			{
				return false;
			}

			for (unsigned int n = 0u; n < freakDescriptor.descriptorLevels(); ++n)
			{
				if (!outputStream.write(freakDescriptor.data()[n].data(), sizeof(std::array<CV::Detector::FREAKDescriptor32::PixelType, 32>)))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool Utilities::readDescriptorMap(IO::InputBitstream& inputStream, std::shared_ptr<UnifiedDescriptorMap>& unifiedDescriptorMap)
{
	std::string tag;
	if (!inputStream.read<std::string>(tag) || (tag != "OCN_TRACKING_FREAK_FEATURES" && tag != "OCN_TRACKING_FREAK_FEATURES_256")) // **TODO** add 32
	{
		return false;
	}

	unsigned int version = 0u;
	if (!inputStream.read<unsigned int>(version) || version != 1u)
	{
		return false;
	}

	UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap descriptorMap;

	unsigned int size = 0u;

	if (!inputStream.read<unsigned int>(size))
	{
		return false;
	}

	for (unsigned int n = 0u; n < size; ++n)
	{
		unsigned int objectPointId = (unsigned int)(-1);
		if (!inputStream.read<unsigned int>(objectPointId))
		{
			return false;
		}

		if (descriptorMap.find(objectPointId) != descriptorMap.cend())
		{
			return false;
		}

		unsigned int numberDescriptors;
		if (!inputStream.read<unsigned int>(numberDescriptors))
		{
			return false;
		}

		CV::Detector::FREAKDescriptors32& freakDescriptors = descriptorMap[objectPointId];

		for (unsigned int nDescriptor = 0u; nDescriptor < numberDescriptors; ++nDescriptor)
		{
			float orientation;
			if (!inputStream.read<float>(orientation))
			{
				return false;
			}

			unsigned int layers;
			if (!inputStream.read<unsigned int>(layers))
			{
				return false;
			}

			ocean_assert(layers <= 3u);
			if (layers > 3u)
			{
				return false;
			}

			CV::Detector::FREAKDescriptor32::MultilevelDescriptorData multiLevelData;

			for (unsigned int nLayer = 0u; nLayer < layers; ++nLayer)
			{
				if (!inputStream.read(multiLevelData[nLayer].data(), sizeof(std::array<CV::Detector::FREAKDescriptor32::PixelType, 32>)))
				{
					return false;
				}
			}

			freakDescriptors.emplace_back(std::move(multiLevelData), layers, orientation);
		}
	}

	unifiedDescriptorMap = std::make_shared<UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(std::move(descriptorMap));

	return true;
}

}

}

}
