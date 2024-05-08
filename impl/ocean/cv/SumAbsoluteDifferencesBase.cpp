/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/SumAbsoluteDifferencesBase.h"

namespace Ocean
{

namespace CV
{

uint32_t SumAbsoluteDifferencesBase::patch8BitPerChannel(const uint8_t* patch0, const uint8_t* patch1, const unsigned int channels, const unsigned int patchSize, const unsigned int patch0StrideElements, const unsigned int patch1StrideElements)
{
	ocean_assert(patch0 != nullptr && patch1 != nullptr);

	ocean_assert(channels > 0u);
	ocean_assert(patchSize % 2u == 1u);

	ocean_assert(patch0StrideElements >= patchSize * channels);
	ocean_assert(patch1StrideElements >= patchSize * channels);

	uint32_t result = 0u;
	int32_t value = 0;

	for (unsigned int y = 0u; y < patchSize; ++y)
	{
		for (unsigned int n = 0u; n < patchSize * channels; ++n)
		{
			value = int16_t(patch0[n]) - int16_t(patch1[n]);
			result += std::abs(value);
		}

		patch0 += patch0StrideElements;
		patch1 += patch1StrideElements;
	}

	return result;
}

uint32_t SumAbsoluteDifferencesBase::buffer8BitPerChannel(const uint8_t* const buffer0, const uint8_t* const buffer1, const unsigned int bufferSize)
{
	ocean_assert(buffer0 != nullptr && buffer1 != nullptr);
	ocean_assert(bufferSize != 0u);

	int32_t difference = 0;
	uint32_t sad = 0u;

	for (unsigned int n = 0u; n < bufferSize; ++n)
	{
		difference = int16_t(buffer0[n]) - int16_t(buffer1[n]);
		sad += std::abs(difference);
	}

	return sad;
}

}

}
