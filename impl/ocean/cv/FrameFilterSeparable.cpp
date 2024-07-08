/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterSeparable.h"

namespace Ocean
{

namespace CV
{

bool FrameFilterSeparable::filter(const Frame& source, Frame& target, const std::vector<unsigned int>& horizontalFilter, const std::vector<unsigned int>& verticalFilter, Worker* worker, ReusableMemory* reusableMemory, const ProcessorInstructions processorInstructions)
{
	ocean_assert(source.isValid());
	ocean_assert(sumFilterValues(horizontalFilter.data(), horizontalFilter.size()) != 0u);
	ocean_assert(sumFilterValues(verticalFilter.data(), verticalFilter.size()) != 0u);

	if (!source.isValid() || source.width() < (unsigned int)(horizontalFilter.size()) || source.height() < (unsigned int)(verticalFilter.size()))
	{
		return false;
	}

	if ((horizontalFilter.size() % 2) != 1 || (verticalFilter.size() % 2) != 1)
	{
		return false;
	}

	ocean_assert(source.numberPlanes() == 1u);
	if (source.numberPlanes() != 1u)
	{
		return false;
	}

	if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (!target.set(source.frameType(), false, true))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		return filter<uint8_t, uint32_t>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.channels(), source.paddingElements(), target.paddingElements(), horizontalFilter.data(), (unsigned int)(horizontalFilter.size()), verticalFilter.data(), (unsigned int)(verticalFilter.size()), worker, reusableMemory, processorInstructions);
	}

	if (source.dataType() == FrameType::DT_SIGNED_FLOAT_32)
	{
		if (!target.set(source.frameType(), false, true))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		const unsigned int sumHorizontalFilter = sumFilterValues(horizontalFilter.data(), horizontalFilter.size());
		if (sumHorizontalFilter == 0u)
		{
			ocean_assert(false && "Invalid filter!");
			return false;
		}

		const float invSumHorizontalFilter = 1.0f / float(sumHorizontalFilter);

		std::vector<float> localNormalizedHorizontalFilter;
		std::vector<float>& normalizedHorizontalFilter = reusableMemory != nullptr ? reusableMemory->normalizedHorizontalFilter_ : localNormalizedHorizontalFilter;

		normalizedHorizontalFilter.resize(horizontalFilter.size());
		for (unsigned int n = 0u; n < normalizedHorizontalFilter.size(); ++n)
		{
			normalizedHorizontalFilter[n] = float(horizontalFilter[n]) * invSumHorizontalFilter;
		}


		const unsigned int sumVerticalFilter = sumFilterValues(verticalFilter.data(), verticalFilter.size());
		if (sumVerticalFilter == 0u)
		{
			ocean_assert(false && "Invalid filter!");
			return false;
		}

		const float invSumVerticalFilter = 1.0f / float(sumVerticalFilter);

		std::vector<float> localNormalizedVerticalFilter;
		std::vector<float>& normalizedVerticalFilter = reusableMemory != nullptr ? reusableMemory->normalizedVerticalFilter_ : localNormalizedVerticalFilter;

		normalizedVerticalFilter.resize(verticalFilter.size());
		for (unsigned int n = 0u; n < normalizedVerticalFilter.size(); ++n)
		{
			normalizedVerticalFilter[n] = float(verticalFilter[n]) * invSumVerticalFilter;
		}

		return filter<float, float>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), target.paddingElements(), normalizedHorizontalFilter.data(), (unsigned int)(normalizedHorizontalFilter.size()), normalizedVerticalFilter.data(), (unsigned int)(normalizedVerticalFilter.size()), worker, reusableMemory, processorInstructions);
	}

	ocean_assert(false && "Unexpected pixel format!");
	return false;
}

}

}
