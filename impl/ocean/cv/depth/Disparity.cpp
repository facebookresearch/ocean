/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/depth/Disparity.h"

namespace Ocean
{

namespace CV
{

namespace Depth
{

bool Disparity::fillHolesDisparityMap(Frame& disparityMap)
{
	if (disparityMap.numberPlanes() != 1u)
	{
		return false;
	}

	switch (disparityMap.dataType())
	{
		case FrameType::DT_SIGNED_FLOAT_32:
			return fillHolesDisparityMap<float>(disparityMap);

		case FrameType::DT_SIGNED_FLOAT_64:
			return fillHolesDisparityMap<double>(disparityMap);

		default:
			ocean_assert(false && "Invalid data type!");
			break;
	}

	return false;
}

bool Disparity::mergeDisparityMaps(const Frames& disparityMaps, const double inlierThreshold, const size_t minInliers, Frame& mergedDisparityMap)
{
	if (disparityMaps.size() < 2)
	{
		return false;
	}

	if (disparityMaps[0].numberPlanes() != 1u)
	{
		return false;
	}

	for (size_t n = 1; n < disparityMaps.size(); ++n)
	{
		if (!disparityMaps[0].isFrameTypeCompatible(disparityMaps[n], false))
		{
			return false;
		}
	}

	ocean_assert(inlierThreshold >= 0.0);
	ocean_assert(minInliers >= 1);

	if (inlierThreshold < 0.0 || minInliers < 1)
	{
		return false;
	}

	if (!mergedDisparityMap.set(disparityMaps[0], false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	switch (mergedDisparityMap.dataType())
	{
		case FrameType::DT_SIGNED_FLOAT_32:
			return mergeDisparityMaps<float>(disparityMaps, inlierThreshold,  minInliers, mergedDisparityMap);

		case FrameType::DT_SIGNED_FLOAT_64:
			return mergeDisparityMaps<double>(disparityMaps, inlierThreshold,  minInliers, mergedDisparityMap);

		default:
			ocean_assert(false && "Invalid data type!");
			break;
	}

	return false;
}

template <typename T>
bool Disparity::fillHolesDisparityMap(Frame& disparityMap)
{
	constexpr T kMaxDispGradient = T(0.01);  // TODO: verify disparity gradient threshold

	// run twice to fill slightly more holes
	for (unsigned int iteration = 0u; iteration < 2u; ++iteration)
	{
		// fill single-pixel holes in each row
		for (unsigned int y = 0; y < disparityMap.height(); ++y)
		{
			for (unsigned int x = 1; x < disparityMap.width() - 1; x++)
			{
				const T prev = disparityMap.constpixel<T>(x - 1, y)[0];
				const T curr = disparityMap.constpixel<T>(x, y)[0];
				const T next = disparityMap.constpixel<T>(x + 1, y)[0];

				if (isValid(prev) && !isValid(curr) && isValid(next))
				{
					T newval = prev;
					if (NumericT<T>::abs(prev - next) < kMaxDispGradient)
					{
						newval = (prev + next) / T(2.0);
					}
					disparityMap.pixel<T>(x, y)[0] = newval;
				}
			}
		}

		// fill single-pixel holes in each column
		for (unsigned int x = 0; x < disparityMap.width(); x++)
		{
			for (unsigned int y = 1; y < disparityMap.height() - 1; ++y)
			{
				const T prev = disparityMap.constpixel<T>(x, y - 1)[0];
				const T curr = disparityMap.constpixel<T>(x, y)[0];
				const T next = disparityMap.constpixel<T>(x, y + 1)[0];

				if (isValid(prev) && !isValid(curr) && isValid(next))
				{
					T newval = prev;
					if (NumericT<T>::abs(prev - next) < kMaxDispGradient)
					{
						newval = (prev + next) / T(2.0);
					}
					disparityMap.pixel<T>(x, y)[0] = newval;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool Disparity::mergeDisparityMaps(const Frames& disparityMaps, const double inlierThreshold, const size_t minInliers, Frame& mergedDisparityMap)
{
	ocean_assert(disparityMaps.size() >= 2);

	ocean_assert(inlierThreshold >= 0.0);
	ocean_assert(minInliers >= 1);

	const size_t minInliers_2 = minInliers / 2;

	std::vector<T> pixelValues;
	pixelValues.reserve(disparityMaps.size());

	bool atLeastOneMergedValue = false;

	for (unsigned int y = 0u; y < mergedDisparityMap.height(); ++y)
	{
		for (unsigned int x = 0u; x < mergedDisparityMap.width(); ++x)
		{
			pixelValues.clear();

			for (const Frame& disparityMap : disparityMaps)
			{
				const T value = disparityMap.constpixel<T>(x, y)[0];

				if (isValid(value))
				{
					pixelValues.emplace_back(value);
				}
			}

			T mergedValue = NumericT<T>::nan();

			if (minInliers <= pixelValues.size())
			{
				std::sort(pixelValues.begin(), pixelValues.end());

				const size_t medianIndex = pixelValues.size() / 2;

				const size_t lowIndex = medianIndex - minInliers_2;
				const size_t highIndex = lowIndex + minInliers - 1;
				ocean_assert(lowIndex >= 0 && highIndex < pixelValues.size());

				const T distance = pixelValues[highIndex] - pixelValues[lowIndex];

				if (distance <= 2 * inlierThreshold)
				{
					T sum = 0;
					for (size_t i = lowIndex; i <= highIndex; ++i)
					{
						sum += pixelValues[i];
					}

					const size_t count = highIndex - lowIndex + 1;
					mergedValue = sum / T(count);

					atLeastOneMergedValue = true;
				}
			}

			mergedDisparityMap.pixel<T>(x, y)[0u] = mergedValue;
		}
	}

	return atLeastOneMergedValue;
}

}

}

}
