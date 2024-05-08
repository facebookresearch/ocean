/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/math/lookup/osx/Wrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/Exception.h"
#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"

#include "ocean/math/Lookup2.h"
#include "ocean/math/Vector3.h"

using namespace Ocean;

const char* oceanBaseBuildStringStatic()
{
	static const std::string value(Build::buildString());

	return value.c_str();
}

void wrapperRandomInitialize()
{
	RandomI::initialize();
}

unsigned char* wrapperAllocFrameData()
{
	static_assert(sizeof(uint8_t) == sizeof(unsigned char), "Invalid data type!");

	unsigned char* result = (unsigned char*)(malloc(800 * 800 * 3));

	if (result == nullptr)
	{
		throw OceanException("Not enough memory!");
	}

	return result;
}

void wrapperReleaseFrameData(const unsigned char* data)
{
	ocean_assert(data != nullptr);

	free((void*)data);
}

void wrapperDetermineLookupFrame(unsigned char* data, const unsigned int viewId)
{
	static_assert(sizeof(uint8_t) == sizeof(unsigned char), "Invalid data type!");

	ocean_assert(data != nullptr);
	ocean_assert(viewId <= 5u);

	static LookupCenter2<Ocean::Vector3> lookupCenter(800, 800, 4, 4);
	static LookupCorner2<Ocean::Vector3> lookupCorner(800, 800, 4, 4);

	constexpr unsigned int dataPaddingElements = 0u;

	Frame frame(FrameType((unsigned int)(lookupCenter.sizeX()), (unsigned int)(lookupCenter.sizeY()), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), (uint8_t*)(data), Frame::CM_USE_KEEP_LAYOUT, dataPaddingElements);

	switch (viewId % 6u)
	{
		case 0u:
		{
			for (unsigned int y = 0u; y < lookupCenter.binsY(); ++y)
			{
				for (unsigned int x = 0u; x < lookupCenter.binsX(); ++x)
				{
					lookupCenter.setBinCenterValue(x, y, Vector3(RandomI::random(0, 1000) % 2 == 0 ? 0 : 1, RandomI::random(0, 1000) % 2 == 0 ? 0 : 1, RandomI::random(0, 1000) % 2 == 0 ? 0 : 1));
				}
			}


			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(lookupCenter.nearestValue(Scalar(x), Scalar(y)));

					row[0] = uint8_t(color.x() * Scalar(255) + Scalar(0.5));
					row[1] = uint8_t(color.y() * Scalar(255) + Scalar(0.5));
					row[2] = uint8_t(color.z() * Scalar(255) + Scalar(0.5));

					row += 3;
				}
			}

			break;
		}

		case 1u:
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(lookupCenter.bilinearValue(Scalar(x), Scalar(y)));

					row[0] = uint8_t(color.x() * Scalar(255) + Scalar(0.5));
					row[1] = uint8_t(color.y() * Scalar(255) + Scalar(0.5));
					row[2] = uint8_t(color.z() * Scalar(255) + Scalar(0.5));

					row += 3;
				}
			}

			break;
		}

		case 2u:
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(lookupCenter.bicubicValue(Scalar(x), Scalar(y)));

					row[0] = uint8_t(minmax<Scalar>(0, color.x() * Scalar(255) + Scalar(0.5), 255));
					row[1] = uint8_t(minmax<Scalar>(0, color.y() * Scalar(255) + Scalar(0.5), 255));
					row[2] = uint8_t(minmax<Scalar>(0, color.z() * Scalar(255) + Scalar(0.5), 255));

					row += 3;
				}
			}

			break;
		}

		case 3u:
		{
			for (unsigned int y = 0u; y <= lookupCenter.binsY(); ++y)
			{
				for (unsigned int x = 0u; x <= lookupCenter.binsX(); ++x)
				{
					lookupCorner.setBinTopLeftCornerValue(x, y, Vector3(RandomI::random(0, 1000) % 2 == 0 ? 0 : 1, RandomI::random(0, 1000) % 2 == 0 ? 0 : 1, RandomI::random(0, 1000) % 2 == 0 ? 0 : 1));
				}
			}


			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(lookupCorner.nearestValue(Scalar(x), Scalar(y)));

					row[0] = uint8_t(color.x() * Scalar(255) + Scalar(0.5));
					row[1] = uint8_t(color.y() * Scalar(255) + Scalar(0.5));
					row[2] = uint8_t(color.z() * Scalar(255) + Scalar(0.5));

					row += 3;
				}
			}

			break;
		}

		case 4u:
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(lookupCorner.bilinearValue(Scalar(x), Scalar(y)));

					row[0] = uint8_t(color.x() * Scalar(255) + Scalar(0.5));
					row[1] = uint8_t(color.y() * Scalar(255) + Scalar(0.5));
					row[2] = uint8_t(color.z() * Scalar(255) + Scalar(0.5));

					row += 3;
				}
			}

			break;
		}

		case 5u:
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(lookupCorner.bicubicValue(Scalar(x), Scalar(y)));

					row[0] = uint8_t(minmax<Scalar>(0, color.x() * Scalar(255) + Scalar(0.5), 255));
					row[1] = uint8_t(minmax<Scalar>(0, color.y() * Scalar(255) + Scalar(0.5), 255));
					row[2] = uint8_t(minmax<Scalar>(0, color.z() * Scalar(255) + Scalar(0.5), 255));

					row += 3;
				}
			}

			break;
		}
	}
}
