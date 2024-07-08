/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameTransposer.h"
#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace CV
{

bool FrameTransposer::Comfort::rotate90(const Frame& input, Frame& output, const bool clockwise, Worker* worker)
{
	ocean_assert(input.isValid());
	ocean_assert(!output.isValid() || !input.haveIntersectingMemory(output));

	if (!input.isValid())
	{
		return false;
	}

	if (input.height() % FrameType::widthMultiple(input.pixelFormat()) != 0u
			|| input.width() % FrameType::heightMultiple(input.pixelFormat()) != 0u)
	{
		ocean_assert(false && "The image cannot be rotated due to the image dimension!");
		return false;
	}

	if (input.pixelFormat() == FrameType::FORMAT_BGGR10_PACKED || input.pixelFormat() == FrameType::FORMAT_RGGB10_PACKED || input.pixelFormat() == FrameType::FORMAT_Y10_PACKED)
	{
		ocean_assert(false && "Packed pixel formats are not supported!");
		return false;
	}

	if (!output.set(FrameType(input, input.height(), input.width()), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const bool adjustedClockwise = input.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT ? clockwise : !clockwise;

	switch (input.dataType())
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
		case FrameType::DT_SIGNED_INTEGER_8:
		{
			for (unsigned int planeIndex = 0u; planeIndex < input.numberPlanes(); ++planeIndex)
			{
				const unsigned int planeChannels = input.planeChannels(planeIndex);

				switch (planeChannels)
				{
					case 1u:
						FrameTransposer::rotate90<uint8_t, 1u>(input.constdata<uint8_t>(planeIndex), output.data<uint8_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 2u:
						FrameTransposer::rotate90<uint8_t, 2u>(input.constdata<uint8_t>(planeIndex), output.data<uint8_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 3u:
						FrameTransposer::rotate90<uint8_t, 3u>(input.constdata<uint8_t>(planeIndex), output.data<uint8_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 4u:
						FrameTransposer::rotate90<uint8_t, 4u>(input.constdata<uint8_t>(planeIndex), output.data<uint8_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					default:
						ocean_assert(false && "Invalid input frame!");
						return false;
				}
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_FLOAT_16:
		{
			for (unsigned int planeIndex = 0u; planeIndex < input.numberPlanes(); ++planeIndex)
			{
				const unsigned int planeChannels = input.planeChannels(planeIndex);

				switch (planeChannels)
				{
					case 1u:
						FrameTransposer::rotate90<uint16_t, 1u>(input.constdata<uint16_t>(planeIndex), output.data<uint16_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 2u:
						FrameTransposer::rotate90<uint16_t, 2u>(input.constdata<uint16_t>(planeIndex), output.data<uint16_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 3u:
						FrameTransposer::rotate90<uint16_t, 3u>(input.constdata<uint16_t>(planeIndex), output.data<uint16_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 4u:
						FrameTransposer::rotate90<uint16_t, 4u>(input.constdata<uint16_t>(planeIndex), output.data<uint16_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					default:
						ocean_assert(false && "Invalid input frame!");
						return false;
				}
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_FLOAT_32:
		{
			for (unsigned int planeIndex = 0u; planeIndex < input.numberPlanes(); ++planeIndex)
			{
				const unsigned int planeChannels = input.planeChannels(planeIndex);

				switch (planeChannels)
				{
					case 1u:
						FrameTransposer::rotate90<uint32_t, 1u>(input.constdata<uint32_t>(planeIndex), output.data<uint32_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 2u:
						FrameTransposer::rotate90<uint32_t, 2u>(input.constdata<uint32_t>(planeIndex), output.data<uint32_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 3u:
						FrameTransposer::rotate90<uint32_t, 3u>(input.constdata<uint32_t>(planeIndex), output.data<uint32_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 4u:
						FrameTransposer::rotate90<uint32_t, 4u>(input.constdata<uint32_t>(planeIndex), output.data<uint32_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					default:
						ocean_assert(false && "Invalid input frame!");
						return false;
				}
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_FLOAT_64:
		{
			for (unsigned int planeIndex = 0u; planeIndex < input.numberPlanes(); ++planeIndex)
			{
				const unsigned int planeChannels = input.planeChannels(planeIndex);

				switch (planeChannels)
				{
					case 1u:
						FrameTransposer::rotate90<uint64_t, 1u>(input.constdata<uint64_t>(planeIndex), output.data<uint64_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 2u:
						FrameTransposer::rotate90<uint64_t, 2u>(input.constdata<uint64_t>(planeIndex), output.data<uint64_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 3u:
						FrameTransposer::rotate90<uint64_t, 3u>(input.constdata<uint64_t>(planeIndex), output.data<uint64_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 4u:
						FrameTransposer::rotate90<uint64_t, 4u>(input.constdata<uint64_t>(planeIndex), output.data<uint64_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), adjustedClockwise, input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					default:
						ocean_assert(false && "Invalid input frame!");
						return false;
				}
			}

			break;
		}

		default:
			ocean_assert(false && "This should never happen!");
			return false;
	}

	return true;
}

bool FrameTransposer::Comfort::rotate180(const Frame& input, Frame& output, Worker* worker)
{
	ocean_assert(input.isValid());
	ocean_assert(!output.isValid() || !input.haveIntersectingMemory(output));

	if (!input.isValid())
	{
		return false;
	}

	if (input.pixelFormat() == FrameType::FORMAT_BGGR10_PACKED || input.pixelFormat() == FrameType::FORMAT_RGGB10_PACKED || input.pixelFormat() == FrameType::FORMAT_Y10_PACKED)
	{
		ocean_assert(false && "Packed pixel formats are not supported!");
		return false;
	}

	if (!output.set(input.frameType(), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	switch (input.dataType())
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
		case FrameType::DT_SIGNED_INTEGER_8:
		{
			for (unsigned int planeIndex = 0u; planeIndex < input.numberPlanes(); ++planeIndex)
			{
				const unsigned int planeChannels = input.planeChannels(planeIndex);

				switch (planeChannels)
				{
					case 1u:
						FrameTransposer::rotate180<uint8_t, 1u>(input.constdata<uint8_t>(planeIndex), output.data<uint8_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 2u:
						FrameTransposer::rotate180<uint8_t, 2u>(input.constdata<uint8_t>(planeIndex), output.data<uint8_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 3u:
						FrameTransposer::rotate180<uint8_t, 3u>(input.constdata<uint8_t>(planeIndex), output.data<uint8_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 4u:
						FrameTransposer::rotate180<uint8_t, 4u>(input.constdata<uint8_t>(planeIndex), output.data<uint8_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					default:
						ocean_assert(false && "Invalid input frame!");
						return false;
				}
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_FLOAT_16:
		{
			for (unsigned int planeIndex = 0u; planeIndex < input.numberPlanes(); ++planeIndex)
			{
				const unsigned int planeChannels = input.planeChannels(planeIndex);

				switch (planeChannels)
				{
					case 1u:
						FrameTransposer::rotate180<uint16_t, 1u>(input.constdata<uint16_t>(planeIndex), output.data<uint16_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 2u:
						FrameTransposer::rotate180<uint16_t, 2u>(input.constdata<uint16_t>(planeIndex), output.data<uint16_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 3u:
						FrameTransposer::rotate180<uint16_t, 3u>(input.constdata<uint16_t>(planeIndex), output.data<uint16_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 4u:
						FrameTransposer::rotate180<uint16_t, 4u>(input.constdata<uint16_t>(planeIndex), output.data<uint16_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					default:
						ocean_assert(false && "Invalid input frame!");
						return false;
				}
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_FLOAT_32:
		{
			for (unsigned int planeIndex = 0u; planeIndex < input.numberPlanes(); ++planeIndex)
			{
				const unsigned int planeChannels = input.planeChannels(planeIndex);

				switch (planeChannels)
				{
					case 1u:
						FrameTransposer::rotate180<uint32_t, 1u>(input.constdata<uint32_t>(planeIndex), output.data<uint32_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 2u:
						FrameTransposer::rotate180<uint32_t, 2u>(input.constdata<uint32_t>(planeIndex), output.data<uint32_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 3u:
						FrameTransposer::rotate180<uint32_t, 3u>(input.constdata<uint32_t>(planeIndex), output.data<uint32_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 4u:
						FrameTransposer::rotate180<uint32_t, 4u>(input.constdata<uint32_t>(planeIndex), output.data<uint32_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					default:
						ocean_assert(false && "Invalid input frame!");
						return false;
				}
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_FLOAT_64:
		{
			for (unsigned int planeIndex = 0u; planeIndex < input.numberPlanes(); ++planeIndex)
			{
				const unsigned int planeChannels = input.planeChannels(planeIndex);

				switch (planeChannels)
				{
					case 1u:
						FrameTransposer::rotate180<uint64_t, 1u>(input.constdata<uint64_t>(planeIndex), output.data<uint64_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 2u:
						FrameTransposer::rotate180<uint64_t, 2u>(input.constdata<uint64_t>(planeIndex), output.data<uint64_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 3u:
						FrameTransposer::rotate180<uint64_t, 3u>(input.constdata<uint64_t>(planeIndex), output.data<uint64_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					case 4u:
						FrameTransposer::rotate180<uint64_t, 4u>(input.constdata<uint64_t>(planeIndex), output.data<uint64_t>(planeIndex), input.planeWidth(planeIndex), input.planeHeight(planeIndex), input.paddingElements(planeIndex), output.paddingElements(planeIndex), worker);
						break;

					default:
						ocean_assert(false && "Invalid input frame!");
						return false;
				}
			}

			break;
		}

		default:
			ocean_assert(false && "This should never happen!");
			return false;
	}

	return true;
}

bool FrameTransposer::Comfort::rotate(const Frame& input, Frame& output, const int angle, Worker* worker)
{
	ocean_assert(input.isValid());
	ocean_assert(!output.isValid() || !input.haveIntersectingMemory(output));

	if (angle % 90 != 0)
	{
		ocean_assert(false && "Angle must be multiple of +/- 90");
		return false;
	}

	int adjustedAngle = angle % 360;

	if (adjustedAngle < 0)
	{
		adjustedAngle = 360 + adjustedAngle;
	}

	ocean_assert(adjustedAngle == 0 || adjustedAngle == 90 || adjustedAngle == 180 || adjustedAngle == 270);

	switch (adjustedAngle)
	{
		case 0:
		{
			if (!output.set(input.frameType(), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			return output.copy(0, 0, input);
		}

		case 90:
			return rotate90(input, output, true /*clockwise*/, worker);

		case 180:
			return rotate180(input, output, worker);

		case 270:
			return rotate90(input, output, false /*clockwise*/, worker);

		default:
			break;
	}

	ocean_assert(false && "This should never happen!");
	return false;
}

bool FrameTransposer::transpose(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid());
	ocean_assert(source.numberPlanes() == 1u);

	ocean_assert(&source != &target);
	ocean_assert(!target.isValid() || source.constdata<void>() != target.constdata<void>());

	const FrameType::DataType pixelFormatDataType = source.dataType();

	switch (pixelFormatDataType)
	{
		case FrameType::DT_SIGNED_INTEGER_8:
		case FrameType::DT_UNSIGNED_INTEGER_8:
		{
			if (!target.set(FrameType(source, source.height(), source.width()), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (source.channels())
			{
				case 1u:
					transpose<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 2u:
					transpose<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 3u:
					transpose<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 4u:
					transpose<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;
			}
		}

		case FrameType::DT_SIGNED_INTEGER_16:
		case FrameType::DT_UNSIGNED_INTEGER_16:
		{
			if (!target.set(FrameType(source, source.height(), source.width()), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (source.channels())
			{
				case 1u:
					transpose<uint16_t, 1u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 2u:
					transpose<uint16_t, 2u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 3u:
					transpose<uint16_t, 3u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 4u:
					transpose<uint16_t, 4u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;
			}
		}

		case FrameType::DT_SIGNED_FLOAT_32:
		case FrameType::DT_SIGNED_INTEGER_32:
		case FrameType::DT_UNSIGNED_INTEGER_32:
		{
			if (!target.set(FrameType(source, source.height(), source.width()), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (source.channels())
			{
				case 1u:
					transpose<uint32_t, 1u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 2u:
					transpose<uint32_t, 2u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 3u:
					transpose<uint32_t, 3u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 4u:
					transpose<uint32_t, 4u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;
			}
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

}

}
