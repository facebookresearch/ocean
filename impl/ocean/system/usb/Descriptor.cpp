/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/Descriptor.h"

#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace System
{

namespace USB
{

std::string Descriptor::guid2string(const uint8_t* guid)
{
	ocean_assert(guid != nullptr);

	if (guid == nullptr)
	{
		return "Null";
	}

	// pattern 44444444-2222-2222-2222-666666666666

	std::string result = String::toAStringHex(guid + 0, 4u)
		+ "-" + String::toAStringHex(guid + 4, 2u)
		+ "-" + String::toAStringHex(guid + 6, 2u)
		+ "-" + String::toAStringHex(guid + 8, 2u)
		+ "-" + String::toAStringHex(guid + 10, 6u);

	return result;
}

double Descriptor::interval2frequency(const uint32_t timeInterval)
{
	ocean_assert(timeInterval >= 1u);

	const double intervalSeconds = Timestamp::nanoseconds2seconds(uint64_t(timeInterval) * 100ull);

	return NumericD::ratio(1.0, intervalSeconds, -1.0);
}

uint32_t Descriptor::frequency2interval(const double frequency)
{
	ocean_assert(frequency >= 0.01);

	if (frequency < 0.01)
	{
		return 0u;
	}

	const double intervalSeconds = 1.0 / frequency;

	const int64_t interval = Timestamp::seconds2nanoseconds(intervalSeconds / 100.0); // in 100ns
	ocean_assert(interval >= 0 && interval <= int64_t(uint32_t(-1)));

	return uint32_t(interval);
}

}

}

}
