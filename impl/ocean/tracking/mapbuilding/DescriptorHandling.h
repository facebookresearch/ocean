/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_DESCRIPTOR_HANDLING_H
#define META_OCEAN_TRACKING_MAPBUILDING_DESCRIPTOR_HANDLING_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptor.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements functions necessary when handling descriptors.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT DescriptorHandling
{
	public:

		/**
		 * Definition of a FREAK  Multi Descriptor with 32 bytes or 256 bits.
		 */
		typedef CV::Detector::FREAKDescriptor32 FreakMultiDescriptor256;

		/**
		 * Definition of a vector holding FREAK  Multi Descriptors with 32 bytes or 256 bits.
		 */
		typedef CV::Detector::FREAKDescriptors32 FreakMultiDescriptors256;

		/**
		 * Definition of an unordered map mapping FREAK  Multi descriptors.
		 */
		typedef std::unordered_map<Index32, FreakMultiDescriptors256> FreakMultiDescriptorMap256;

	public:

		/**
		 * Determines the minimal distance between one FREAK multi descriptor and another FREAK multi descriptors.
		 * @param descriptorA The first descriptor
		 * @param descriptorB The second descriptor
		 * @return The minimal distance between both descriptors
		 */
		static OCEAN_FORCE_INLINE unsigned int determineFreakDistance(const FreakMultiDescriptor256& descriptorA, const FreakMultiDescriptor256& descriptorB);

		/**
		 * Determines the minimal distance between one FREAK multi descriptor and several FREAK multi descriptors.
		 * @param descriptorA The first descriptor
		 * @param descriptorsB The set of second descriptors
		 * @return The minimal distance between both descriptors
		 */
		static OCEAN_FORCE_INLINE unsigned int determineFreakDistance(const FreakMultiDescriptor256& descriptorA, const FreakMultiDescriptors256& descriptorsB);

		/**
		 * Determines the minimal distance between two sets of FREAK  descriptors.
		 * @param descriptorsA The first set of descriptors
		 * @param descriptorsB The second set of descriptors
		 * @return The minimal distance between both descriptors
		 */
		static OCEAN_FORCE_INLINE unsigned int determineFreakDistance(const FreakMultiDescriptors256& descriptorsA, const FreakMultiDescriptors256& descriptorsB);

		/**
		 * Determines the minimal distance between a set of FREAK  descriptors and a second FREAK  descriptor.
		 * @param descriptorsA The first set of descriptors
		 * @param descriptorB The second descriptor
		 * @return The minimal distance between both descriptors
		 */
		static OCEAN_FORCE_INLINE unsigned int determineFreakDistance(const FreakMultiDescriptors256* const& descriptorsA, const UnifiedDescriptor::BinaryDescriptor<256u>& descriptorB);

		/**
		 * Determines the minimal distance between two set of FREAK descriptors.
		 * @param descriptorsA The first set of descriptors
		 * @param descriptorB The second set of descriptors
		 * @return The minimal distance between both descriptors
		 */
		static OCEAN_FORCE_INLINE unsigned int determineFreakDistance(const FreakMultiDescriptors256* const& descriptorsA, const FreakMultiDescriptors256& descriptorB);

		/**
		 * Determines the minimal distance between a set of FREAK descriptors and a binary descriptor.
		 * @param descriptorsA The first set of descriptors
		 * @param descriptorB The second descriptor
		 * @return The minimal distance between both descriptors
		 */
		static OCEAN_FORCE_INLINE unsigned int determineFreakDistance(const FreakMultiDescriptors256& descriptorsA, const UnifiedDescriptor::BinaryDescriptor<256u>& descriptorB);

		/**
		 * Determines the minimal distance between a FREAK descriptors and a binary descriptor.
		 * @param descriptorA The first descriptors
		 * @param descriptorB The second descriptor
		 * @return The minimal distance between both descriptors
		 */
		static OCEAN_FORCE_INLINE unsigned int calculateHammingDistance(const FreakMultiDescriptor256& descriptorA, const UnifiedDescriptor::BinaryDescriptor<256u>& descriptorB);


		/**
		 * Returns one binary descriptor from a FREAK Multi descriptor.
		 * @param multiDescriptor The multi descriptor from which one binary descriptor will be extracted
		 * @param index The index of the binary descriptor to extract, with range [0, multiDescriptor.descriptorLevels() - 1]
		 * @return The resulting binary descriptor
		 */
		static OCEAN_FORCE_INLINE const UnifiedDescriptor::BinaryDescriptor<256u>* multiDescriptorFunction(const FreakMultiDescriptor256& multiDescriptor, const size_t index);

		/**
		 * Returns one FREAK Multi descriptor from a FREAK Multi descriptor group.
		 * @param multiDescriptorGroup The multi descriptor group from which one multi descriptor will be extracted
		 * @param index The index of the multi descriptor to extract, with range [0, multiDescriptorGroup->size() - 1]
		 * @return The resulting binary descriptor
		 */
		static OCEAN_FORCE_INLINE const FreakMultiDescriptor256* multiDescriptorGroupFunction(const FreakMultiDescriptors256* const& multiDescriptorGroup, const size_t index);

		/**
		 * Computes the FREAK Multi descriptor for a given 2D location within an image.
		 * @param yFramePyramid The image pyramid with pixel format FORMAT_Y8, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param point The point defined inside the image pixel domain
		 * @param freakDescriptor The resulting descriptor
		 * @return True, if succeeded
		 */
		static bool computeFreakDescriptor(const CV::FramePyramid& yFramePyramid, const AnyCamera& anyCamera, const Vector2& point, FreakMultiDescriptor256& freakDescriptor);

		/**
		 * Replaces an image pyramid which is intended for FREAK descriptor extraction.
		 * @param yFramePyramid The source image pyramid without any additional filtering
		 * @param yFramePyramidForDescriptors The resulting image pyramid with FREAK-specific filtering
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool replaceDescriptorPyramid(const CV::FramePyramid& yFramePyramid, CV::FramePyramid& yFramePyramidForDescriptors, Worker* worker = nullptr);
};

OCEAN_FORCE_INLINE unsigned int DescriptorHandling::determineFreakDistance(const FreakMultiDescriptor256& descriptorA, const FreakMultiDescriptor256& descriptorB)
{
	return descriptorA.distance(descriptorB);
}

OCEAN_FORCE_INLINE unsigned int DescriptorHandling::determineFreakDistance(const FreakMultiDescriptor256& descriptorA, const FreakMultiDescriptors256& descriptorsB)
{
	unsigned int bestDistance = (unsigned int)(-1);

	for (const FreakMultiDescriptor256& descriptorB : descriptorsB)
	{
		const unsigned int distance = descriptorA.distance(descriptorB);

		if (distance < bestDistance)
		{
			bestDistance = distance;
		}
	}

	return bestDistance;
}

OCEAN_FORCE_INLINE unsigned int DescriptorHandling::determineFreakDistance(const FreakMultiDescriptors256& descriptorsA, const FreakMultiDescriptors256& descriptorsB)
{
	unsigned int bestDistance = (unsigned int)(-1);

	for (const FreakMultiDescriptor256& descriptorA : descriptorsA)
	{
		for (const FreakMultiDescriptor256& descriptorB : descriptorsB)
		{
			const unsigned int distance = descriptorA.distance(descriptorB);

			if (distance < bestDistance)
			{
				bestDistance = distance;
			}
		}
	}

	return bestDistance;
}

OCEAN_FORCE_INLINE unsigned int DescriptorHandling::determineFreakDistance(const FreakMultiDescriptors256* const& descriptorsA, const UnifiedDescriptor::BinaryDescriptor<256u>& descriptorB)
{
	ocean_assert(descriptorsA != nullptr);

	unsigned int bestDistance = (unsigned int)(-1);

	for (const FreakMultiDescriptor256& descriptorA : *descriptorsA)
	{
		const unsigned int distance = calculateHammingDistance(descriptorA, descriptorB);

		if (distance < bestDistance)
		{
			bestDistance = distance;
		}
	}

	return bestDistance;
}

OCEAN_FORCE_INLINE unsigned int DescriptorHandling::determineFreakDistance(const FreakMultiDescriptors256* const& descriptorsA, const FreakMultiDescriptors256& descriptorB)
{
	ocean_assert(descriptorsA != nullptr);

	unsigned int bestDistance = (unsigned int)(-1);

	for (const FreakMultiDescriptor256& descriptorA : *descriptorsA)
	{
		const unsigned int distance = determineFreakDistance(descriptorA, descriptorB);

		if (distance < bestDistance)
		{
			bestDistance = distance;
		}
	}

	return bestDistance;
}

OCEAN_FORCE_INLINE unsigned int DescriptorHandling::determineFreakDistance(const FreakMultiDescriptors256& descriptorsA, const UnifiedDescriptor::BinaryDescriptor<256u>& descriptorB)
{
	unsigned int bestDistance = (unsigned int)(-1);

	for (const FreakMultiDescriptor256& descriptorA : descriptorsA)
	{
		const unsigned int distance = calculateHammingDistance(descriptorA, descriptorB);

		if (distance < bestDistance)
		{
			bestDistance = distance;
		}
	}

	return bestDistance;
}

OCEAN_FORCE_INLINE unsigned int DescriptorHandling::calculateHammingDistance(const FreakMultiDescriptor256& descriptorA, const UnifiedDescriptor::BinaryDescriptor<256u>& descriptorB)
{
	unsigned int bestDistance = (unsigned int)(-1);

	for (unsigned int n = 0u; n < descriptorA.descriptorLevels(); ++n)
	{
		const unsigned int distance = CV::Detector::Descriptor::calculateHammingDistance<sizeof(UnifiedDescriptor::BinaryDescriptor<256u>) * 8>(&descriptorA.data()[n], &descriptorB);

		if (distance < bestDistance)
		{
			bestDistance = distance;
		}
	}

	return bestDistance;
}

OCEAN_FORCE_INLINE const UnifiedDescriptor::BinaryDescriptor<256u>* DescriptorHandling::multiDescriptorFunction(const FreakMultiDescriptor256& multiDescriptor, const size_t index)
{
	if (index >= multiDescriptor.descriptorLevels())
	{
		return nullptr;
	}

	return (const UnifiedDescriptor::BinaryDescriptor<256u>*)(&(multiDescriptor.data()[index]));
}

OCEAN_FORCE_INLINE const DescriptorHandling::FreakMultiDescriptor256* DescriptorHandling::multiDescriptorGroupFunction(const FreakMultiDescriptors256* const& multiDescriptorGroup, const size_t index)
{
	ocean_assert(multiDescriptorGroup != nullptr);

	if (index >= multiDescriptorGroup->size())
	{
		return nullptr;
	}

	return &((*multiDescriptorGroup)[index]);
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_DESCRIPTOR_HANDLING_H
