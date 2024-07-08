/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Utilities.h"
#include "ocean/base/String.h"

namespace Ocean
{

std::vector<std::string> Utilities::separateValues(const std::string& values, const char delimiter, const bool removeQuotes, const bool trim)
{
	std::vector<std::string> result;

	std::string::size_type pos = 0;

	while (pos != std::string::npos)
	{
		std::string subString;

		const std::string::size_type endPos = values.find(delimiter, pos);

		if (endPos == std::string::npos)
		{
			subString = values.substr(pos);
			pos = std::string::npos;
		}
		else
		{
			subString = values.substr(pos, endPos - pos);
			pos = endPos + 1;
		}

		bool modified = true;
		while (modified)
		{
			modified = false;

			if (trim)
			{
				std::string trimmed = String::trim(subString);

				if (trimmed != subString)
				{
					modified = true;
				}

				subString = std::move(trimmed);
			}

			if (removeQuotes)
			{
				if (!subString.empty() && subString[0] == '\"')
				{
					subString = subString.substr(1);
					modified = true;
				}

				if (!subString.empty() && subString[subString.length() - 1] == '\"')
				{
					subString = subString.substr(0, subString.length() - 1);
					modified = true;
				}
			}
		}

		if (!subString.empty())
		{
			result.emplace_back(std::move(subString));
		}
	}

	return result;
}

void Utilities::multiply64(const uint64_t left, const uint64_t right, uint64_t& resultHigh, uint64_t& resultLow)
{
	const uint64_t a1 = left >> 32u;
	const uint64_t a0 = left & uint64_t(0xFFFFFFFFull);
	const uint64_t b1 = right >> 32u;
	const uint64_t b0 = right & uint64_t(0xFFFFFFFFull);

	const uint64_t a0b0 = a0 * b0;
	const uint64_t a0b1 = a0 * b1;
	const uint64_t a1b0 = a1 * b0;
	const uint64_t a1b1 = a1 * b1;

	/*
	Bit layout of components:
	128     96      64      32      0
	|.......|.......|.......|.......|
	|<-    a1b1    ->               |
	|       <-     a0b1    ->       |
	|       <-     a1b0    ->       |
	|               <-     a0b0   ->|
	*/

	const uint64_t t = (a0b0 >> 32u) + (a1b0 & uint64_t(0xFFFFFFFFull)) + (a0b1 & uint64_t(0xFFFFFFFFull));

	resultLow = ((t & uint64_t(0xFFFFFFFFull)) << 32u) | (a0b0 & uint64_t(0xFFFFFFFFull));
	resultHigh = a1b1 + (a1b0 >> 32u) + (a0b1 >> 32u) + (t >> 32u);
}

}
