/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverterY8.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

const uint8_t* FrameConverterY8::LookupTableManager::lookupTable(const float gamma)
{
	ocean_assert(gamma > 0.0f && gamma < 2.0f);

	const ScopedLock scopedLock(lock_);

	const LookupTables::const_iterator i = lookupTables_.find(gamma);

	if (i != lookupTables_.cend())
	{
		return i->second.constdata<uint8_t>();
	}

	Memory newMemory = Memory::create<uint8_t>(256); // lookup with 8 bits

	uint8_t* const values = newMemory.data<uint8_t>();

	constexpr float inv255 = 1.0f / 255.0f;

	for (unsigned int n = 0u; n < 256u; ++n)
	{
		const float value = NumericF::pow(float(n) * inv255, gamma) * 255.0f + 0.5f;
		ocean_assert(value < 256.0f);

		values[n] = uint8_t(value);
	}

	lookupTables_.insert(std::make_pair(gamma, std::move(newMemory)));

	return values;
}

void FrameConverterY8::convertRowY8ToY8GammaLUT(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 1);
	ocean_assert(parameters != nullptr);

	const uint8_t* const lookupValues = reinterpret_cast<const uint8_t*>(parameters);

	size_t remaining = size;

	const size_t blocks8 = remaining / size_t(8);

	for (size_t n = 0; n < blocks8; ++n)
	{
		target[0] = lookupValues[source[0]];
		target[1] = lookupValues[source[1]];
		target[2] = lookupValues[source[2]];
		target[3] = lookupValues[source[3]];

		target[4] = lookupValues[source[4]];
		target[5] = lookupValues[source[5]];
		target[6] = lookupValues[source[6]];
		target[7] = lookupValues[source[7]];

		target += 8;
		source += 8;
	}

	remaining = remaining - blocks8 * size_t(8);
	ocean_assert(remaining <= size);

	for (size_t n = 0; n < remaining; ++n)
	{
		target[n] = lookupValues[source[n]];
	}
}

}

}
