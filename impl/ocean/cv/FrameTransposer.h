/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_TRANSPOSER_H
#define META_OCEAN_CV_FRAME_TRANSPOSER_H

#include "ocean/cv/CV.h"
#include "ocean/cv/NEON.h"
#include "ocean/cv/FrameChannels.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a frame transposer.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameTransposer
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Rotates a given frame either clockwise or counter-clockwise by 90 degrees.
				 * @param input The input frame which will be rotated, must be valid
				 * @param output The resulting rotated output frame, the frame type will be set automatically
				 * @param clockwise True, to rotate the frame clockwise; False, to rotate the frame counter-clockwise
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool rotate90(const Frame& input, Frame& output, const bool clockwise, Worker* worker = nullptr);

				/**
				 * Rotates a given frame either clockwise or counter-clockwise by 90 degrees.
				 * @param frame The frame to rotate, must be valid
				 * @param clockwise True, to rotate the frame clockwise; False, to rotate the frame counter-clockwise
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static inline bool rotate90(Frame& frame, const bool clockwise, Worker* worker = nullptr);

				/**
				 * Rotates a given frame by 180 degrees.
				 * @param input The input frame which will be rotated, must be valid
				 * @param output The resulting rotated output frame, the frame type will be set automatically
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool rotate180(const Frame& input, Frame& output, Worker* worker = nullptr);

				/**
				 * Rotates a given frame by 180 degrees.
				 * @param frame The frame to rotate, must be valid
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static inline bool rotate180(Frame& frame, Worker* worker = nullptr);

				/**
				 * Rotates a given frame with 90 degree steps.
				 * @param input The input frame which will be rotated, must be valid
				 * @param output The resulting rotated output frame, the frame type will be set automatically
				 * @param angle The clockwise rotation angle to be used, must be a multiple of +/- 90, with range (-infinity, infinity)
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool rotate(const Frame& input, Frame& output, const int angle, Worker* worker = nullptr);

				/**
				 * Rotates a given frame with 90 degree steps.
				 * @param frame The frame to rotate, must be valid
				 * @param angle The clockwise rotation angle to be used, must be a multiple of +/- 90, with range (-infinity, infinity)
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static inline bool rotate(Frame& frame, const int angle, Worker* worker = nullptr);
		};

	protected:

		/**
		 * Definition of individual flip directions which can be applied to a transposed frame.
		 * Flipping the transposed result allows to rotate the image by 90 degree (clockwise and counter clockwise).
		 */
		enum FlipDirection
		{
			/// Applying no flip.
			FD_NONE,
			/// Applying a left-right flip like a mirror, combined with a transpose operation an image can be rotated clockwise.
			FD_LEFT_RIGHT,
			/// Applying a top-bottom flip, combined with a transpose operation an image can be rotated counter clockwise.
			FD_TOP_BOTTOM
		};

		/**
		 * Helper class for functions transposing blocks.
		 * The class is necessary to allow a partially specialization of template parameters.
		 * @tparam T The data type of each elements, e.g., 'uint8_t', 'int8_t', 'float'
		 * @tparam tChannels The number of channels the given data has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		class BlockTransposer
		{
			public:

				/**
				 * Transposes a block of 8x8 pixels.
				 * @param sourceBlock The pointer to the start location of the source block, must be valid
				 * @param targetBlock The pointer to the start location of the target block, must be valid
				 * @param sourceStrideElements The number of elements between two successive rows, in elements, with range [8 * tChannels, infinity)
				 * @param targetStrideElements The number of elements between two successive rows, in elements, with range [8 * tChannels, infinity)
				 * @tparam tFlipDirection The flip direction to be applied after transposing the block
				 * @see transposeBlock().
				 */
				template <FlipDirection tFlipDirection>
				static OCEAN_FORCE_INLINE void transposeBlock8x8(const T* sourceBlock, T* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

				/**
				 * Transposes a block of n x m pixels.
				 * This function should be used for blocks smaller than 8x8.
				 * @param sourceBlock The pointer to the start location of the source block, must be valid
				 * @param targetBlock The pointer to the start location of the target block, must be valid
				 * @param blockWidth The width of the block to transpose, with range [1, 7]
				 * @param blockHeight The height of the block to transpose, with range [1, 7]
				 * @param sourceStrideElements The number of elements between two successive rows, in elements, with range [8 * tChannels, infinity)
				 * @param targetStrideElements The number of elements between two successive rows, in elements, with range [8 * tChannels, infinity)
				 * @tparam tFlipDirection The flip direction to be applied after transposing the block
				 * @see transposeBlock8x8().
				 */
				template <FlipDirection tFlipDirection>
				static OCEAN_FORCE_INLINE void transposeBlock(const T* sourceBlock, T* targetBlock, const unsigned int blockWidth, const unsigned int blockHeight, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

				/**
				 * Transposes a block of 4x4 pixels.
				 * @param sourceBlock The pointer to the start location of the source block, must be valid
				 * @param targetBlock The pointer to the start location of the target block, must be valid
				 * @param sourceStrideElements The number of elements between two successive rows, in elements, with range [4 * tChannels, infinity)
				 * @param targetStrideElements The number of elements between two successive rows, in elements, with range [4 * tChannels, infinity)
				 * @tparam tFlipDirection The flip direction to be applied after transposing the block
				 * @see transposeBlock().
				 */
				template <FlipDirection tFlipDirection>
				static OCEAN_FORCE_INLINE void transposeBlock4x4NEON(const T* sourceBlock, T* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
		};

	public:

		/**
		 * Transposes a given frame.
		 * Beware: This function has a significantly bigger binary size impact than the corresponding template-based function.
		 * @param source The source frame to transpose, must be valid
		 * @param target The target frame receiving the transposed image, if the frame type of the target frame does not match the transposed source frame, the target frame will be adjusted accordingly, must not be 'source'
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool transpose(const Frame& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Transposes a given frame.
		 * Beware: This function has a significantly bigger binary size impact than the corresponding template-based function.
		 * @param frame The frame to transpose, must be valid
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static inline bool transpose(Frame& frame, Worker* worker = nullptr);

		/**
		 * Transposes a given image buffer.
		 * @param source The source buffer to transpose, must be valid
		 * @param target The target buffer receiving the transposed image, must not be 'source', must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each row of the target frame, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation
		 * @tparam T The data type of each channel
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void transpose(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Rotates a given image buffer 90 degrees clockwise or counter clockwise.
		 * @param source The source buffer to transpose, must be valid
		 * @param target The target buffer receiving the rotated image, must not be 'source', must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param clockwise True, to rotate the source image clockwise; False, to rotate the image counter clockwise
		 * @param worker Optional worker to distribute the computation
		 * @param sourcePaddingElements The optional number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each row of the target frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each channel
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void rotate90(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const bool clockwise, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Rotates a given image buffer 180 degrees.
		 * @param source The source buffer to transpose, must be valid
		 * @param target The target buffer receiving the rotated image, must not be 'source', must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param worker Optional worker to distribute the computation
		 * @param sourcePaddingElements The optional number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each row of the target frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each channel
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void rotate180(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Rotates a given image with 90 degree steps.
		 * @param source The source buffer to rotated, must be valid
		 * @param target The target buffer receiving the rotated image, must not be 'source', must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param angle The clockwise rotation angle to be used, must be a multiple of +/- 90, with range (-infinity, infinity)
		 * @param worker Optional worker to distribute the computation
		 * @param sourcePaddingElements The optional number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each row of the target frame, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each channel
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool rotate(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const int angle, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Transposes the subset of a given image buffer.
		 * @param source The source buffer to transpose
		 * @param target The target buffer receiving the transposed image
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The optional number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each row of the target frame, in elements, with range [0, infinity)
		 * @param firstSourceRow First source row to be handled
		 * @param numberSourceRows The number of source rows to be handled
		 * @tparam T The data type of each channel
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tFlipDirection The flip direction to be applied after transposing
		 * @see transposeBlock8x8(), transposeBlock().
		 */
		template <typename T, unsigned int tChannels, FlipDirection tFlipDirection>
		static void transposeSubset(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstSourceRow, const unsigned int numberSourceRows);

		/**
		 * Rotates a subset of a given frame either clockwise or counter-clockwise by 90 degree.
		 * @param source The source frame which will be rotated, must be valid
		 * @param target The resulting rotated target frame, must be valid and must have the same buffer size as the source frame
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param clockwise True, to rotate the frame clockwise; False, to rotate the frame counter-clockwise
		 * @param sourcePaddingElements Number of padding elements in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Number of padding elements in the target frame, range: [0, infinity)
		 * @param firstTargetRow The first target row to be handled, with range [0, sourceWidth)
		 * @param numberTargetRows The number of target rows to be handled, with range [1, sourceWidth - firstTargetRow]
		 * @tparam TElementType Data type of the elements of the image pixels
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 */
		template <typename TElementType, unsigned int tChannels>
		static void rotate90Subset(const TElementType* source, TElementType* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const bool clockwise, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);
};

inline bool FrameTransposer::Comfort::rotate90(Frame& frame, const bool clockwise, Worker* worker)
{
	ocean_assert(frame.isValid());

	Frame tmpFrame;
	if (!rotate90(frame, tmpFrame, clockwise, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

inline bool FrameTransposer::Comfort::rotate180(Frame& frame, Worker* worker)
{
	ocean_assert(frame.isValid());

	Frame tmpFrame;
	if (!rotate180(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

inline bool FrameTransposer::Comfort::rotate(Frame& frame, const int angle, Worker* worker)
{
	ocean_assert(frame.isValid());

	if (angle == 0)
	{
		return frame.isValid();
	}

	Frame tmpFrame;
	if (!rotate(frame, tmpFrame, angle, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

inline bool FrameTransposer::transpose(Frame& frame, Worker* worker)
{
	ocean_assert(frame);

	Frame tmpFrame;

	if (!transpose(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

template <typename T, unsigned int tChannels>
void FrameTransposer::transpose(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source && target);
	ocean_assert(source != target);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);

	const unsigned int xBlocks8 = (sourceWidth + 7u) / 8u;
	const unsigned int yBlocks8 = (sourceHeight + 7u) / 8u;

	const unsigned int blocks8 = xBlocks8 * yBlocks8;

	typedef typename TypeMapper<T>::Type MappedType;

	if (worker && blocks8 >= 800u)
	{
		worker->executeFunction(Worker::Function::createStatic(&transposeSubset<MappedType, tChannels, FD_NONE>, (const MappedType*)(source), (MappedType*)(target), sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, blocks8);
	}
	else
	{
		transposeSubset<MappedType, tChannels, FD_NONE>((const MappedType*)(source), (MappedType*)(target), sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, 0u, blocks8);
	}
}

template <typename T, unsigned int tChannels>
void FrameTransposer::rotate90(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const bool clockwise, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source && target);
	ocean_assert(source != target);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);

	typedef typename TypeMapper<T>::Type MappedType;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION > 0

	// on x86 CPUs, the SIMD implementation is slower than the non-SIMD implementation
	// therefore, using a function without explicit SIMD instructions

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(rotate90Subset<MappedType, tChannels>, (const MappedType*)(source), (MappedType*)(target), sourceWidth, sourceHeight, clockwise, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, sourceWidth, 7u, 8u, 20u);
	}
	else
	{
		rotate90Subset<MappedType, tChannels>((const MappedType*)(source), (MappedType*)(target), sourceWidth, sourceHeight, clockwise, sourcePaddingElements, targetPaddingElements, 0u, sourceWidth);
	}

#else

	// on non-x86 CPUs (e.g., ARM), the SIMD implementation is significantly faster

	const unsigned int xBlocks8 = (sourceWidth + 7u) / 8u;
	const unsigned int yBlocks8 = (sourceHeight + 7u) / 8u;

	const unsigned int blocks8 = xBlocks8 * yBlocks8;

	if (worker && blocks8 >= 800u)
	{
		if (clockwise)
		{
			worker->executeFunction(Worker::Function::createStatic(&transposeSubset<MappedType, tChannels, FD_LEFT_RIGHT>, (const MappedType*)(source), (MappedType*)(target), sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, blocks8);
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&transposeSubset<MappedType, tChannels, FD_TOP_BOTTOM>, (const MappedType*)(source), (MappedType*)(target), sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, blocks8);
		}
	}
	else
	{
		if (clockwise)
		{
			transposeSubset<MappedType, tChannels, FD_LEFT_RIGHT>((const MappedType*)(source), (MappedType*)(target), sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, 0u, blocks8);
		}
		else
		{
			transposeSubset<MappedType, tChannels, FD_TOP_BOTTOM>((const MappedType*)(source), (MappedType*)(target), sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, 0u, blocks8);
		}
	}

#endif
}

template <typename T, unsigned int tChannels>
void FrameTransposer::rotate180(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr);
	ocean_assert(target != nullptr);

	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);

	FrameChannels::transformGeneric<T, tChannels>(source, target, sourceWidth, sourceHeight, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, sourcePaddingElements, targetPaddingElements, worker);
}

template <typename T, unsigned int tChannels>
bool FrameTransposer::rotate(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const int angle, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr);
	ocean_assert(target != nullptr);

	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);

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
			CV::FrameChannels::subFrame<T>(source, target, sourceWidth, sourceHeight, sourceWidth, sourceHeight, tChannels, 0u, 0u, 0u, 0u, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements);
			return true;

		case 90:
			rotate90<T, tChannels>(source, target, sourceWidth, sourceHeight, true /*clockwise*/, sourcePaddingElements, targetPaddingElements, worker);
			return true;

		case 180:
			rotate180<T, tChannels>(source, target, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, worker);
			return true;

		case 270:
			rotate90<T, tChannels>(source, target, sourceWidth, sourceHeight, false /*clockwise*/, sourcePaddingElements, targetPaddingElements, worker);
			return true;

		default:
			break;
	}

	ocean_assert(false && "This should never happen!");
	return false;
}

template <typename T, unsigned int tChannels, FrameTransposer::FlipDirection tFlipDirection>
void FrameTransposer::transposeSubset(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstBlock8, const unsigned int numberBlocks8)
{
	static_assert(sizeof(T) != 0, "Invalid data type!");
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source && target);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = sourceHeight * tChannels + targetPaddingElements;

	const unsigned int xBlocks8 = (sourceWidth + 7u) / 8u;
	const unsigned int yBlocks8 = (sourceHeight + 7u) / 8u;
	ocean_assert(firstBlock8 + numberBlocks8 <= xBlocks8 * yBlocks8);

	const unsigned int xSmallBlockIndex = xBlocks8 * 8u == sourceWidth ? (unsigned int)(-1) : (xBlocks8 - 1u);
	const unsigned int ySmallBlockIndex = yBlocks8 * 8u == sourceHeight ? (unsigned int)(-1) : (yBlocks8 - 1u);

	for (unsigned int block8 = firstBlock8; block8 < firstBlock8 + numberBlocks8; ++block8)
	{
		const unsigned int yBlock8 = block8 / xBlocks8;
		const unsigned int xBlock8 = block8 % xBlocks8;

		const T* sourceBlockTopLeft = nullptr;
		T* targetBlockTopLeft = nullptr;

		switch (tFlipDirection)
		{
			case FD_NONE:
			{
				// simply transposing the block

				sourceBlockTopLeft = source + sourceStrideElements * yBlock8 * 8u + xBlock8 * 8u * tChannels;
				targetBlockTopLeft = target + targetStrideElements * xBlock8 * 8u + yBlock8 * 8u * tChannels;

				break;
			}

			case FD_LEFT_RIGHT:
			{
				// transposing the block and applying a left-right flip like a mirror, actually a 90 degree clockwise rotation

				const unsigned int xTarget = (unsigned int)(std::max(0, int(sourceHeight) - int((yBlock8 + 1u) * 8u)));

				sourceBlockTopLeft = source + sourceStrideElements * yBlock8 * 8u + xBlock8 * 8u * tChannels;
				targetBlockTopLeft = target + targetStrideElements * xBlock8 * 8u + xTarget * tChannels;

				break;
			}

			case FD_TOP_BOTTOM:
			{
				// transposing the block and applying a top-bottom flip, actually a 90 degree counter clockwise rotation

				const unsigned int yTarget = (unsigned int)(std::max(0, int(sourceWidth) - int((xBlock8 + 1u) * 8u)));

				sourceBlockTopLeft = source + sourceStrideElements * yBlock8 * 8u + xBlock8 * 8u * tChannels;
				targetBlockTopLeft = target + targetStrideElements * yTarget + yBlock8 * 8u * tChannels;

				break;
			}

			default:
				ocean_assert(false && "Invalid flip direction!");
		}

		ocean_assert(sourceBlockTopLeft != nullptr);
		ocean_assert(targetBlockTopLeft != nullptr);

		if (xBlock8 != xSmallBlockIndex && yBlock8 != ySmallBlockIndex)
		{
			BlockTransposer<T, tChannels>::template transposeBlock8x8<tFlipDirection>(sourceBlockTopLeft, targetBlockTopLeft, sourceStrideElements, targetStrideElements);
		}
		else
		{
			const unsigned int blockWidth = min(sourceWidth - xBlock8 * 8u, 8u);
			const unsigned int blockHeight = min(sourceHeight - yBlock8 * 8u, 8u);

			BlockTransposer<T, tChannels>::template transposeBlock<tFlipDirection>(sourceBlockTopLeft, targetBlockTopLeft, blockWidth, blockHeight, sourceStrideElements, targetStrideElements);
		}
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<uint8_t, 1u>::transposeBlock8x8(const uint8_t* sourceBlock, uint8_t* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);
	ocean_assert(sourceStrideElements >= 8u && targetStrideElements >= 8u);

	// A B C D E F G H
	// a b c d e f g h
	// 0 1 2 3 4 5 6 7
	// ! @ # $ % ^ & *
	// ...

	__m128 line02_f_32x4 = _mm_setzero_ps(); // A B C D E F G H   0 1 2 3 4 5 6 7
	__m128 line13_f_32x4 = _mm_setzero_ps(); // a b c d e f g h   ! @ # $ % ^ & *

	line02_f_32x4 = _mm_loadl_pi(line02_f_32x4, (const __m64*)(sourceBlock + sourceStrideElements * 0u));
	line13_f_32x4 = _mm_loadl_pi(line13_f_32x4, (const __m64*)(sourceBlock + sourceStrideElements * 1u));
	line02_f_32x4 = _mm_loadh_pi(line02_f_32x4, (const __m64*)(sourceBlock + sourceStrideElements * 2u));
	line13_f_32x4 = _mm_loadh_pi(line13_f_32x4, (const __m64*)(sourceBlock + sourceStrideElements * 3u));

	const __m128i line01_u_8x16 = _mm_unpacklo_epi8(_mm_castps_si128(line02_f_32x4), _mm_castps_si128(line13_f_32x4)); // A a B b C c D d   E e F f G g H h
	const __m128i line23_u_8x16 = _mm_unpackhi_epi8(_mm_castps_si128(line02_f_32x4), _mm_castps_si128(line13_f_32x4)); // 0 ! 1 @ 2 # 3 $   4 % 5 ^ 6 & 7 *

	const __m128i intermediateA_03_u_8x16 = _mm_unpacklo_epi16(line01_u_8x16, line23_u_8x16); // A a 0 ! B b 1 @   C c 2 # D d 3 $
	const __m128i intermediateB_03_u_8x16 = _mm_unpackhi_epi16(line01_u_8x16, line23_u_8x16); // E e 4 % F f 5 ^   G g 6 & H h 7 *

	__m128 line46_f_32x4 = _mm_setzero_ps();
	__m128 line57_f_32x4 = _mm_setzero_ps();
	line46_f_32x4 = _mm_loadl_pi(line46_f_32x4, (const __m64*)(sourceBlock + sourceStrideElements * 4u));
	line57_f_32x4 = _mm_loadl_pi(line57_f_32x4, (const __m64*)(sourceBlock + sourceStrideElements * 5u));
	line46_f_32x4 = _mm_loadh_pi(line46_f_32x4, (const __m64*)(sourceBlock + sourceStrideElements * 6u));
	line57_f_32x4 = _mm_loadh_pi(line57_f_32x4, (const __m64*)(sourceBlock + sourceStrideElements * 7u));

	const __m128i line45_u_8x16 = _mm_unpacklo_epi8(_mm_castps_si128(line46_f_32x4), _mm_castps_si128(line57_f_32x4));
	const __m128i line67_u_8x16 = _mm_unpackhi_epi8(_mm_castps_si128(line46_f_32x4), _mm_castps_si128(line57_f_32x4));

	const __m128i intermediateA_47_u_8x16 = _mm_unpacklo_epi16(line45_u_8x16, line67_u_8x16);
	const __m128i intermediateB_47_u_8x16 = _mm_unpackhi_epi16(line45_u_8x16, line67_u_8x16);

	__m128i transposed01 = _mm_unpacklo_epi32(intermediateA_03_u_8x16, intermediateA_47_u_8x16);
	__m128i transposed23 = _mm_unpackhi_epi32(intermediateA_03_u_8x16, intermediateA_47_u_8x16);
	__m128i transposed45 = _mm_unpacklo_epi32(intermediateB_03_u_8x16, intermediateB_47_u_8x16);
	__m128i transposed67 = _mm_unpackhi_epi32(intermediateB_03_u_8x16, intermediateB_47_u_8x16);

	switch (tFlipDirection)
	{
		case FD_LEFT_RIGHT:
		{
			const __m128i reverseSuffleMask_u_16x8 = _mm_set_epi64x(0x08090A0B0C0D0E0Fll, 0x0001020304050607ll);

			transposed01 = _mm_shuffle_epi8(transposed01, reverseSuffleMask_u_16x8);
			transposed23 = _mm_shuffle_epi8(transposed23, reverseSuffleMask_u_16x8);
			transposed45 = _mm_shuffle_epi8(transposed45, reverseSuffleMask_u_16x8);
			transposed67 = _mm_shuffle_epi8(transposed67, reverseSuffleMask_u_16x8);

			// no break, as we use the store function from FD_NONE
			[[fallthrough]];
		}

		case FD_NONE:
		{
			_mm_storel_pi((__m64*)(targetBlock + targetStrideElements * 0u), _mm_castsi128_ps(transposed01));
			_mm_storeh_pi((__m64*)(targetBlock + targetStrideElements * 1u), _mm_castsi128_ps(transposed01));
			_mm_storel_pi((__m64*)(targetBlock + targetStrideElements * 2u), _mm_castsi128_ps(transposed23));
			_mm_storeh_pi((__m64*)(targetBlock + targetStrideElements * 3u), _mm_castsi128_ps(transposed23));
			_mm_storel_pi((__m64*)(targetBlock + targetStrideElements * 4u), _mm_castsi128_ps(transposed45));
			_mm_storeh_pi((__m64*)(targetBlock + targetStrideElements * 5u), _mm_castsi128_ps(transposed45));
			_mm_storel_pi((__m64*)(targetBlock + targetStrideElements * 6u), _mm_castsi128_ps(transposed67));
			_mm_storeh_pi((__m64*)(targetBlock + targetStrideElements * 7u), _mm_castsi128_ps(transposed67));

			break;
		}

		case FD_TOP_BOTTOM:
		{
			_mm_storeh_pi((__m64*)(targetBlock + targetStrideElements * 0u), _mm_castsi128_ps(transposed67));
			_mm_storel_pi((__m64*)(targetBlock + targetStrideElements * 1u), _mm_castsi128_ps(transposed67));
			_mm_storeh_pi((__m64*)(targetBlock + targetStrideElements * 2u), _mm_castsi128_ps(transposed45));
			_mm_storel_pi((__m64*)(targetBlock + targetStrideElements * 3u), _mm_castsi128_ps(transposed45));
			_mm_storeh_pi((__m64*)(targetBlock + targetStrideElements * 4u), _mm_castsi128_ps(transposed23));
			_mm_storel_pi((__m64*)(targetBlock + targetStrideElements * 5u), _mm_castsi128_ps(transposed23));
			_mm_storeh_pi((__m64*)(targetBlock + targetStrideElements * 6u), _mm_castsi128_ps(transposed01));
			_mm_storel_pi((__m64*)(targetBlock + targetStrideElements * 7u), _mm_castsi128_ps(transposed01));

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

template <>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<uint8_t, 2u>::transposeBlock8x8(const uint8_t* sourceBlock, uint8_t* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);
	ocean_assert(sourceStrideElements >= 8u && targetStrideElements >= 8u);

	// AA BB CC DD EE FF GG HH
	// aa bb cc dd ee ff gg hh
	// 00 11 22 33 44 55 66 77
	// !! @@ ## $$ %% ^^ && **
	// ...

	const __m128i line0_u_8x16 = _mm_loadu_si128((const __m128i*)(sourceBlock + sourceStrideElements * 0u)); // AA BB CC DD EE FF GG HH
	const __m128i line1_u_8x16 = _mm_loadu_si128((const __m128i*)(sourceBlock + sourceStrideElements * 1u)); // aa bb cc dd ee ff gg hh
	const __m128i line2_u_8x16 = _mm_loadu_si128((const __m128i*)(sourceBlock + sourceStrideElements * 2u));
	const __m128i line3_u_8x16 = _mm_loadu_si128((const __m128i*)(sourceBlock + sourceStrideElements * 3u));
	const __m128i line4_u_8x16 = _mm_loadu_si128((const __m128i*)(sourceBlock + sourceStrideElements * 4u));
	const __m128i line5_u_8x16 = _mm_loadu_si128((const __m128i*)(sourceBlock + sourceStrideElements * 5u));
	const __m128i line6_u_8x16 = _mm_loadu_si128((const __m128i*)(sourceBlock + sourceStrideElements * 6u));
	const __m128i line7_u_8x16 = _mm_loadu_si128((const __m128i*)(sourceBlock + sourceStrideElements * 7u));

	const __m128i line01_A_u_8x16 = _mm_unpacklo_epi16(line0_u_8x16, line1_u_8x16); // AA aa BB bb CC cc DD dd
	const __m128i line01_B_u_8x16 = _mm_unpackhi_epi16(line0_u_8x16, line1_u_8x16); // EE ee FF ff GG gg HH hh
	const __m128i line23_A_u_8x16 = _mm_unpacklo_epi16(line2_u_8x16, line3_u_8x16); // 00 !! 11 @@ ...
	const __m128i line23_B_u_8x16 = _mm_unpackhi_epi16(line2_u_8x16, line3_u_8x16); // 44 %% 55 ^^ ...
	const __m128i line45_A_u_8x16 = _mm_unpacklo_epi16(line4_u_8x16, line5_u_8x16);
	const __m128i line45_B_u_8x16 = _mm_unpackhi_epi16(line4_u_8x16, line5_u_8x16);
	const __m128i line67_A_u_8x16 = _mm_unpacklo_epi16(line6_u_8x16, line7_u_8x16);
	const __m128i line67_B_u_8x16 = _mm_unpackhi_epi16(line6_u_8x16, line7_u_8x16);

	const __m128i intermediateAA_03_u_8x16 = _mm_unpacklo_epi32(line01_A_u_8x16, line23_A_u_8x16); // AA aa 00 !! BB bb 11 @@
	const __m128i intermediateAB_03_u_8x16 = _mm_unpackhi_epi32(line01_A_u_8x16, line23_A_u_8x16); // CC cc 22 ## DD dd 33 $$
	const __m128i intermediateBA_03_u_8x16 = _mm_unpacklo_epi32(line01_B_u_8x16, line23_B_u_8x16);
	const __m128i intermediateBB_03_u_8x16 = _mm_unpackhi_epi32(line01_B_u_8x16, line23_B_u_8x16);
	const __m128i intermediateAA_47_u_8x16 = _mm_unpacklo_epi32(line45_A_u_8x16, line67_A_u_8x16);
	const __m128i intermediateAB_47_u_8x16 = _mm_unpackhi_epi32(line45_A_u_8x16, line67_A_u_8x16);
	const __m128i intermediateBA_47_u_8x16 = _mm_unpacklo_epi32(line45_B_u_8x16, line67_B_u_8x16);
	const __m128i intermediateBB_47_u_8x16 = _mm_unpackhi_epi32(line45_B_u_8x16, line67_B_u_8x16);

	__m128i transposed0 = _mm_unpacklo_epi64(intermediateAA_03_u_8x16, intermediateAA_47_u_8x16);
	__m128i transposed1 = _mm_unpackhi_epi64(intermediateAA_03_u_8x16, intermediateAA_47_u_8x16);
	__m128i transposed2 = _mm_unpacklo_epi64(intermediateAB_03_u_8x16, intermediateAB_47_u_8x16);
	__m128i transposed3 = _mm_unpackhi_epi64(intermediateAB_03_u_8x16, intermediateAB_47_u_8x16);
	__m128i transposed4 = _mm_unpacklo_epi64(intermediateBA_03_u_8x16, intermediateBA_47_u_8x16);
	__m128i transposed5 = _mm_unpackhi_epi64(intermediateBA_03_u_8x16, intermediateBA_47_u_8x16);
	__m128i transposed6 = _mm_unpacklo_epi64(intermediateBB_03_u_8x16, intermediateBB_47_u_8x16);
	__m128i transposed7 = _mm_unpackhi_epi64(intermediateBB_03_u_8x16, intermediateBB_47_u_8x16);

	switch (tFlipDirection)
	{
		case FD_LEFT_RIGHT:
		{
			const __m128i reverseSuffleMask_u_16x8 = _mm_set_epi64x(0x0100030205040706ll, 0x09080B0A0D0C0F0Ell);

			transposed0 = _mm_shuffle_epi8(transposed0, reverseSuffleMask_u_16x8);
			transposed1 = _mm_shuffle_epi8(transposed1, reverseSuffleMask_u_16x8);
			transposed2 = _mm_shuffle_epi8(transposed2, reverseSuffleMask_u_16x8);
			transposed3 = _mm_shuffle_epi8(transposed3, reverseSuffleMask_u_16x8);
			transposed4 = _mm_shuffle_epi8(transposed4, reverseSuffleMask_u_16x8);
			transposed5 = _mm_shuffle_epi8(transposed5, reverseSuffleMask_u_16x8);
			transposed6 = _mm_shuffle_epi8(transposed6, reverseSuffleMask_u_16x8);
			transposed7 = _mm_shuffle_epi8(transposed7, reverseSuffleMask_u_16x8);

			// no break, as we use the store function from FD_NONE
			[[fallthrough]];
		}

		case FD_NONE:
		{
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 0u), transposed0);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 1u), transposed1);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 2u), transposed2);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 3u), transposed3);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 4u), transposed4);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 5u), transposed5);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 6u), transposed6);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 7u), transposed7);

			break;
		}

		case FD_TOP_BOTTOM:
		{
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 0u), transposed7);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 1u), transposed6);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 2u), transposed5);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 3u), transposed4);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 4u), transposed3);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 5u), transposed2);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 6u), transposed1);
			_mm_storeu_si128((__m128i*)(targetBlock + targetStrideElements * 7u), transposed0);

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

#endif // defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SEE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<uint8_t, 4u>::transposeBlock4x4NEON(const uint8_t* sourceBlock, uint8_t* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);
	ocean_assert(sourceStrideElements >= 4u * 3u && targetStrideElements >= 4u * 3u);

	// the NEON code is straight forward simply using the VTRN (transpose) instruction

	const uint32x4_t line0_u_32x4 = vreinterpretq_u32_u8(vld1q_u8(sourceBlock + sourceStrideElements * 0u));
	const uint32x4_t line1_u_32x4 = vreinterpretq_u32_u8(vld1q_u8(sourceBlock + sourceStrideElements * 1u));

	// A B C D      A a C c
	// a b c d  ->  B b D d
	const uint32x4x2_t line01_u_32x4x2 = vtrnq_u32(line0_u_32x4, line1_u_32x4);

	const uint32x4_t line2_u_32x4 = vreinterpretq_u32_u8(vld1q_u8(sourceBlock + sourceStrideElements * 2u));
	const uint32x4_t line3_u_32x4 = vreinterpretq_u32_u8(vld1q_u8(sourceBlock + sourceStrideElements * 3u));

	// 0 1 2 3      0 ! 2 #
	// ! @ # $  ->  1 @ 3 $
	const uint32x4x2_t line23_u_32x4x2 = vtrnq_u32(line2_u_32x4, line3_u_32x4);

	// Aa Cc      Aa 0!
	// Bb Dd  ->  Bb 1@
	// 0! 2#      Cc 2#
	// 1@ 3$      Dd 3$
	const uint32x4_t result0_u_32x4 = vcombine_u32(vget_low_u32(line01_u_32x4x2.val[0]), vget_low_u32(line23_u_32x4x2.val[0]));
	const uint32x4_t result1_u_32x4 = vcombine_u32(vget_low_u32(line01_u_32x4x2.val[1]), vget_low_u32(line23_u_32x4x2.val[1]));
	const uint32x4_t result2_u_32x4 = vcombine_u32(vget_high_u32(line01_u_32x4x2.val[0]), vget_high_u32(line23_u_32x4x2.val[0]));
	const uint32x4_t result3_u_32x4 = vcombine_u32(vget_high_u32(line01_u_32x4x2.val[1]), vget_high_u32(line23_u_32x4x2.val[1]));

	switch (tFlipDirection)
	{
		case FD_NONE:
		{
			vst1q_u8(targetBlock + targetStrideElements * 0u, vreinterpretq_u8_u32(result0_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 1u, vreinterpretq_u8_u32(result1_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 2u, vreinterpretq_u8_u32(result2_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 3u, vreinterpretq_u8_u32(result3_u_32x4));

			break;
		}

		case FD_LEFT_RIGHT:
		{
			const uint32x4_t halfReverseResult0_u_32x4 = vrev64q_u32(result0_u_32x4);
			const uint8x16_t reverseResult0_u_32x4 = vreinterpretq_u8_u32(vcombine_u32(vget_high_u32(halfReverseResult0_u_32x4), vget_low_u32(halfReverseResult0_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 0u, reverseResult0_u_32x4);

			const uint32x4_t halfReverseResult1_u_32x4 = vrev64q_u32(result1_u_32x4);
			const uint8x16_t reverseResult1_u_32x4 = vreinterpretq_u8_u32(vcombine_u32(vget_high_u32(halfReverseResult1_u_32x4), vget_low_u32(halfReverseResult1_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 1u, reverseResult1_u_32x4);

			const uint32x4_t halfReverseResult2_u_32x4 = vrev64q_u32(result2_u_32x4);
			const uint8x16_t reverseResult2_u_32x4 = vreinterpretq_u8_u32(vcombine_u32(vget_high_u32(halfReverseResult2_u_32x4), vget_low_u32(halfReverseResult2_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 2u, reverseResult2_u_32x4);

			const uint32x4_t halfReverseResult3_u_32x4 = vrev64q_u32(result3_u_32x4);
			const uint8x16_t reverseResult3_u_32x4 = vreinterpretq_u8_u32(vcombine_u32(vget_high_u32(halfReverseResult3_u_32x4), vget_low_u32(halfReverseResult3_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 3u, reverseResult3_u_32x4);

			break;
		}

		case FD_TOP_BOTTOM:
		{
			vst1q_u8(targetBlock + targetStrideElements * 0u, vreinterpretq_u8_u32(result3_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 1u, vreinterpretq_u8_u32(result2_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 2u, vreinterpretq_u8_u32(result1_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 3u, vreinterpretq_u8_u32(result0_u_32x4));

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

template <>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<uint8_t, 1u>::transposeBlock8x8(const uint8_t* sourceBlock, uint8_t* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);
	ocean_assert(sourceStrideElements >= 8u && targetStrideElements >= 8u);

	// the NEON code is straight forward simply using the VTRN (transpose) instruction

	const uint8x8_t line0_u_8x8 = vld1_u8(sourceBlock + sourceStrideElements * 0u);
	const uint8x8_t line1_u_8x8 = vld1_u8(sourceBlock + sourceStrideElements * 1u);

	// A B C D E F G H      A a C c E e G g
	// a b c d e f g h  ->  B b D d F f H h
	const uint8x8x2_t line01_u_8x8x2 = vtrn_u8(line0_u_8x8, line1_u_8x8);

	const uint8x8_t line2_u_8x8 = vld1_u8(sourceBlock + sourceStrideElements * 2u);
	const uint8x8_t line3_u_8x8 = vld1_u8(sourceBlock + sourceStrideElements * 3u);

	// 0 1 2 3 4 5 6 7      0 ! 2 # 4 % 6 &
	// ! @ # $ % ^ & *  ->  1 @ 3 $ 5 ^ 7 *
	const uint8x8x2_t line23_u_8x8x2 = vtrn_u8(line2_u_8x8, line3_u_8x8);

	// Aa Cc Ee Gg      Aa 0! Ee 4%
	// 0! 2# 4% 6&  ->  Cc 2# Gg 6&
	const uint16x4x2_t line02_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line01_u_8x8x2.val[0]), vreinterpret_u16_u8(line23_u_8x8x2.val[0]));

	// Bb Dd Ff Hh      Bb 1@ Ef 5^
	// 1@ 3$ 5^ 7*  ->  Dd 3$ Hh 7*
	const uint16x4x2_t line13_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line01_u_8x8x2.val[1]), vreinterpret_u16_u8(line23_u_8x8x2.val[1]));

	const uint8x8_t line4_u_8x8 = vld1_u8(sourceBlock + sourceStrideElements * 4u);
	const uint8x8_t line5_u_8x8 = vld1_u8(sourceBlock + sourceStrideElements * 5u);

	const uint8x8x2_t line45_u_8x8x2 = vtrn_u8(line4_u_8x8, line5_u_8x8);

	const uint8x8_t line6_u_8x8 = vld1_u8(sourceBlock + sourceStrideElements * 6u);
	const uint8x8_t line7_u_8x8 = vld1_u8(sourceBlock + sourceStrideElements * 7u);

	const uint8x8x2_t line67_u_8x8x2 = vtrn_u8(line6_u_8x8, line7_u_8x8);

	const uint16x4x2_t line46_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line45_u_8x8x2.val[0]), vreinterpret_u16_u8(line67_u_8x8x2.val[0]));
	const uint16x4x2_t line57_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line45_u_8x8x2.val[1]), vreinterpret_u16_u8(line67_u_8x8x2.val[1]));

	const uint32x2x2_t line04_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line02_u_16x4x2.val[0]), vreinterpret_u32_u16(line46_u_16x4x2.val[0]));
	const uint32x2x2_t line26_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line02_u_16x4x2.val[1]), vreinterpret_u32_u16(line46_u_16x4x2.val[1]));

	const uint32x2x2_t line15_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line13_u_16x4x2.val[0]), vreinterpret_u32_u16(line57_u_16x4x2.val[0]));
	const uint32x2x2_t line37_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line13_u_16x4x2.val[1]), vreinterpret_u32_u16(line57_u_16x4x2.val[1]));

	switch (tFlipDirection)
	{
		case FD_NONE:
		{
			vst1_u8(targetBlock + targetStrideElements * 0u, vreinterpret_u8_u32(line04_u_32x2x2.val[0]));
			vst1_u8(targetBlock + targetStrideElements * 1u, vreinterpret_u8_u32(line15_u_32x2x2.val[0]));
			vst1_u8(targetBlock + targetStrideElements * 2u, vreinterpret_u8_u32(line26_u_32x2x2.val[0]));
			vst1_u8(targetBlock + targetStrideElements * 3u, vreinterpret_u8_u32(line37_u_32x2x2.val[0]));
			vst1_u8(targetBlock + targetStrideElements * 4u, vreinterpret_u8_u32(line04_u_32x2x2.val[1]));
			vst1_u8(targetBlock + targetStrideElements * 5u, vreinterpret_u8_u32(line15_u_32x2x2.val[1]));
			vst1_u8(targetBlock + targetStrideElements * 6u, vreinterpret_u8_u32(line26_u_32x2x2.val[1]));
			vst1_u8(targetBlock + targetStrideElements * 7u, vreinterpret_u8_u32(line37_u_32x2x2.val[1]));

			break;
		}

		case FD_LEFT_RIGHT:
		{
			vst1_u8(targetBlock + targetStrideElements * 0u, vrev64_u8(vreinterpret_u8_u32(line04_u_32x2x2.val[0])));
			vst1_u8(targetBlock + targetStrideElements * 1u, vrev64_u8(vreinterpret_u8_u32(line15_u_32x2x2.val[0])));
			vst1_u8(targetBlock + targetStrideElements * 2u, vrev64_u8(vreinterpret_u8_u32(line26_u_32x2x2.val[0])));
			vst1_u8(targetBlock + targetStrideElements * 3u, vrev64_u8(vreinterpret_u8_u32(line37_u_32x2x2.val[0])));
			vst1_u8(targetBlock + targetStrideElements * 4u, vrev64_u8(vreinterpret_u8_u32(line04_u_32x2x2.val[1])));
			vst1_u8(targetBlock + targetStrideElements * 5u, vrev64_u8(vreinterpret_u8_u32(line15_u_32x2x2.val[1])));
			vst1_u8(targetBlock + targetStrideElements * 6u, vrev64_u8(vreinterpret_u8_u32(line26_u_32x2x2.val[1])));
			vst1_u8(targetBlock + targetStrideElements * 7u, vrev64_u8(vreinterpret_u8_u32(line37_u_32x2x2.val[1])));

			break;
		}

		case FD_TOP_BOTTOM:
		{
			vst1_u8(targetBlock + targetStrideElements * 0u, vreinterpret_u8_u32(line37_u_32x2x2.val[1]));
			vst1_u8(targetBlock + targetStrideElements * 1u, vreinterpret_u8_u32(line26_u_32x2x2.val[1]));
			vst1_u8(targetBlock + targetStrideElements * 2u, vreinterpret_u8_u32(line15_u_32x2x2.val[1]));
			vst1_u8(targetBlock + targetStrideElements * 3u, vreinterpret_u8_u32(line04_u_32x2x2.val[1]));
			vst1_u8(targetBlock + targetStrideElements * 4u, vreinterpret_u8_u32(line37_u_32x2x2.val[0]));
			vst1_u8(targetBlock + targetStrideElements * 5u, vreinterpret_u8_u32(line26_u_32x2x2.val[0]));
			vst1_u8(targetBlock + targetStrideElements * 6u, vreinterpret_u8_u32(line15_u_32x2x2.val[0]));
			vst1_u8(targetBlock + targetStrideElements * 7u, vreinterpret_u8_u32(line04_u_32x2x2.val[0]));

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

template <>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<uint8_t, 2u>::transposeBlock8x8(const uint8_t* sourceBlock, uint8_t* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);
	ocean_assert(sourceStrideElements >= 8u * 2u && targetStrideElements >= 8u * 2u);

	// the NEON code is straight forward simply using the VTRN (transpose) instruction
	// the 2-channel code is similar to the 1-channel code but simply transposes 16 bit values instead of 8 bit values

	const uint16x8_t line0_u_16x8 = vreinterpretq_u16_u8(vld1q_u8(sourceBlock + sourceStrideElements * 0u));
	const uint16x8_t line1_u_16x8 = vreinterpretq_u16_u8(vld1q_u8(sourceBlock + sourceStrideElements * 1u));

	// A B C D E F G H      A a C c E e G g
	// a b c d e f g h  ->  B b D d F f H h
	const uint16x8x2_t line01_u_16x8x2 = vtrnq_u16(line0_u_16x8, line1_u_16x8);

	const uint16x8_t line2_u_16x8 = vreinterpretq_u16_u8(vld1q_u8(sourceBlock + sourceStrideElements * 2u));
	const uint16x8_t line3_u_16x8 = vreinterpretq_u16_u8(vld1q_u8(sourceBlock + sourceStrideElements * 3u));

	// 0 1 2 3 4 5 6 7      0 ! 2 # 4 % 6 &
	// ! @ # $ % ^ & *  ->  1 @ 3 $ 5 ^ 7 *
	const uint16x8x2_t line23_u_16x8x2 = vtrnq_u16(line2_u_16x8, line3_u_16x8);

	// Aa Cc Ee Gg      Aa 0! Ee 4%
	// 0! 2# 4% 6&  ->  Cc 2# Gg 6&
	const uint32x4x2_t line02_u_32x4x2 = vtrnq_u32(vreinterpretq_u32_u16(line01_u_16x8x2.val[0]), vreinterpretq_u32_u16(line23_u_16x8x2.val[0]));

	// Bb Dd Ff Hh      Bb 1@ Ef 5^
	// 1@ 3$ 5^ 7*  ->  Dd 3$ Hh 7*
	const uint32x4x2_t line13_u_32x4x2 = vtrnq_u32(vreinterpretq_u32_u16(line01_u_16x8x2.val[1]), vreinterpretq_u32_u16(line23_u_16x8x2.val[1]));

	const uint16x8_t line4_u_16x8 = vreinterpretq_u16_u8(vld1q_u8(sourceBlock + sourceStrideElements * 4u));
	const uint16x8_t line5_u_16x8 = vreinterpretq_u16_u8(vld1q_u8(sourceBlock + sourceStrideElements * 5u));

	const uint16x8x2_t line45_u_16x8x2 = vtrnq_u16(line4_u_16x8, line5_u_16x8);

	const uint16x8_t line6_u_16x8 = vreinterpretq_u16_u8(vld1q_u8(sourceBlock + sourceStrideElements * 6u));
	const uint16x8_t line7_u_16x8 = vreinterpretq_u16_u8(vld1q_u8(sourceBlock + sourceStrideElements * 7u));

	const uint16x8x2_t line67_u_16x8x2 = vtrnq_u16(line6_u_16x8, line7_u_16x8);

	const uint32x4x2_t line46_u_32x4x2 = vtrnq_u32(vreinterpretq_u32_u16(line45_u_16x8x2.val[0]), vreinterpretq_u32_u16(line67_u_16x8x2.val[0]));
	const uint32x4x2_t line57_u_32x4x2 = vtrnq_u32(vreinterpretq_u32_u16(line45_u_16x8x2.val[1]), vreinterpretq_u32_u16(line67_u_16x8x2.val[1]));

	const uint32x4_t result0_u_32x4 = vcombine_u32(vget_low_u32(line02_u_32x4x2.val[0]), vget_low_u32(line46_u_32x4x2.val[0]));
	const uint32x4_t result1_u_32x4 = vcombine_u32(vget_low_u32(line13_u_32x4x2.val[0]), vget_low_u32(line57_u_32x4x2.val[0]));

	const uint32x4_t result2_u_32x4 = vcombine_u32(vget_low_u32(line02_u_32x4x2.val[1]), vget_low_u32(line46_u_32x4x2.val[1]));
	const uint32x4_t result3_u_32x4 = vcombine_u32(vget_low_u32(line13_u_32x4x2.val[1]), vget_low_u32(line57_u_32x4x2.val[1]));

	const uint32x4_t result4_u_32x4 = vcombine_u32(vget_high_u32(line02_u_32x4x2.val[0]), vget_high_u32(line46_u_32x4x2.val[0]));
	const uint32x4_t result5_u_32x4 = vcombine_u32(vget_high_u32(line13_u_32x4x2.val[0]), vget_high_u32(line57_u_32x4x2.val[0]));

	const uint32x4_t result6_u_32x4 = vcombine_u32(vget_high_u32(line02_u_32x4x2.val[1]), vget_high_u32(line46_u_32x4x2.val[1]));
	const uint32x4_t result7_u_32x4 = vcombine_u32(vget_high_u32(line13_u_32x4x2.val[1]), vget_high_u32(line57_u_32x4x2.val[1]));

	switch (tFlipDirection)
	{
		case FD_NONE:
		{
			vst1q_u8(targetBlock + targetStrideElements * 0u, vreinterpretq_u8_u32(result0_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 1u, vreinterpretq_u8_u32(result1_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 2u, vreinterpretq_u8_u32(result2_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 3u, vreinterpretq_u8_u32(result3_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 4u, vreinterpretq_u8_u32(result4_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 5u, vreinterpretq_u8_u32(result5_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 6u, vreinterpretq_u8_u32(result6_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 7u, vreinterpretq_u8_u32(result7_u_32x4));

			break;
		}

		case FD_LEFT_RIGHT:
		{
			const uint8x16_t targetHalfReverse0_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u32(result0_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 0u, vcombine_u8(vget_high_u8(targetHalfReverse0_u_8x16), vget_low_u8(targetHalfReverse0_u_8x16)));

			const uint8x16_t targetHalfReverse1_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u32(result1_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 1u, vcombine_u8(vget_high_u8(targetHalfReverse1_u_8x16), vget_low_u8(targetHalfReverse1_u_8x16)));

			const uint8x16_t targetHalfReverse2_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u32(result2_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 2u, vcombine_u8(vget_high_u8(targetHalfReverse2_u_8x16), vget_low_u8(targetHalfReverse2_u_8x16)));

			const uint8x16_t targetHalfReverse3_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u32(result3_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 3u, vcombine_u8(vget_high_u8(targetHalfReverse3_u_8x16), vget_low_u8(targetHalfReverse3_u_8x16)));

			const uint8x16_t targetHalfReverse4_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u32(result4_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 4u, vcombine_u8(vget_high_u8(targetHalfReverse4_u_8x16), vget_low_u8(targetHalfReverse4_u_8x16)));

			const uint8x16_t targetHalfReverse5_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u32(result5_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 5u, vcombine_u8(vget_high_u8(targetHalfReverse5_u_8x16), vget_low_u8(targetHalfReverse5_u_8x16)));

			const uint8x16_t targetHalfReverse6_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u32(result6_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 6u, vcombine_u8(vget_high_u8(targetHalfReverse6_u_8x16), vget_low_u8(targetHalfReverse6_u_8x16)));

			const uint8x16_t targetHalfReverse7_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u32(result7_u_32x4)));
			vst1q_u8(targetBlock + targetStrideElements * 7u, vcombine_u8(vget_high_u8(targetHalfReverse7_u_8x16), vget_low_u8(targetHalfReverse7_u_8x16)));

			break;
		}

		case FD_TOP_BOTTOM:
		{
			vst1q_u8(targetBlock + targetStrideElements * 0u, vreinterpretq_u8_u32(result7_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 1u, vreinterpretq_u8_u32(result6_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 2u, vreinterpretq_u8_u32(result5_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 3u, vreinterpretq_u8_u32(result4_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 4u, vreinterpretq_u8_u32(result3_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 5u, vreinterpretq_u8_u32(result2_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 6u, vreinterpretq_u8_u32(result1_u_32x4));
			vst1q_u8(targetBlock + targetStrideElements * 7u, vreinterpretq_u8_u32(result0_u_32x4));

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

template <>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<uint8_t, 3u>::transposeBlock8x8(const uint8_t* sourceBlock, uint8_t* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);
	ocean_assert(sourceStrideElements >= 8u * 3u && targetStrideElements >= 8u * 3u);

	// the NEON code is straight forward simply using the VTRN (transpose) instruction
	// the 3-channel code is similar to the 1-channel code but uses vld3_u8/vst3_u8 instead of vld1_u8/vst1_u8

	const uint8x8x3_t line0_u_8x8x3 = vld3_u8(sourceBlock + sourceStrideElements * 0u);
	const uint8x8x3_t line1_u_8x8x3 = vld3_u8(sourceBlock + sourceStrideElements * 1u);

	// A B C D E F G H      A a C c E e G g
	// a b c d e f g h  ->  B b D d F f H h
	const uint8x8x2_t line01_channel0_u_8x8x2 = vtrn_u8(line0_u_8x8x3.val[0], line1_u_8x8x3.val[0]);
	const uint8x8x2_t line01_channel1_u_8x8x2 = vtrn_u8(line0_u_8x8x3.val[1], line1_u_8x8x3.val[1]);
	const uint8x8x2_t line01_channel2_u_8x8x2 = vtrn_u8(line0_u_8x8x3.val[2], line1_u_8x8x3.val[2]);

	const uint8x8x3_t line2_u_8x8x3 = vld3_u8(sourceBlock + sourceStrideElements * 2u);
	const uint8x8x3_t line3_u_8x8x3 = vld3_u8(sourceBlock + sourceStrideElements * 3u);

	// 0 1 2 3 4 5 6 7      0 ! 2 # 4 % 6 &
	// ! @ # $ % ^ & *  ->  1 @ 3 $ 5 ^ 7 *
	const uint8x8x2_t line23_channel0_u_8x8x2 = vtrn_u8(line2_u_8x8x3.val[0], line3_u_8x8x3.val[0]);
	const uint8x8x2_t line23_channel1_u_8x8x2 = vtrn_u8(line2_u_8x8x3.val[1], line3_u_8x8x3.val[1]);
	const uint8x8x2_t line23_channel2_u_8x8x2 = vtrn_u8(line2_u_8x8x3.val[2], line3_u_8x8x3.val[2]);

	// Aa Cc Ee Gg      Aa 0! Ee 4%
	// 0! 2# 4% 6&  ->  Cc 2# Gg 6&
	const uint16x4x2_t line02_channel0_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line01_channel0_u_8x8x2.val[0]), vreinterpret_u16_u8(line23_channel0_u_8x8x2.val[0]));
	const uint16x4x2_t line02_channel1_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line01_channel1_u_8x8x2.val[0]), vreinterpret_u16_u8(line23_channel1_u_8x8x2.val[0]));
	const uint16x4x2_t line02_channel2_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line01_channel2_u_8x8x2.val[0]), vreinterpret_u16_u8(line23_channel2_u_8x8x2.val[0]));

	// Bb Dd Ff Hh      Bb 1@ Ef 5^
	// 1@ 3$ 5^ 7*  ->  Dd 3$ Hh 7*
	const uint16x4x2_t line13_channel0_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line01_channel0_u_8x8x2.val[1]), vreinterpret_u16_u8(line23_channel0_u_8x8x2.val[1]));
	const uint16x4x2_t line13_channel1_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line01_channel1_u_8x8x2.val[1]), vreinterpret_u16_u8(line23_channel1_u_8x8x2.val[1]));
	const uint16x4x2_t line13_channel2_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line01_channel2_u_8x8x2.val[1]), vreinterpret_u16_u8(line23_channel2_u_8x8x2.val[1]));

	const uint8x8x3_t line4_u_8x8x3 = vld3_u8(sourceBlock + sourceStrideElements * 4u);
	const uint8x8x3_t line5_u_8x8x3 = vld3_u8(sourceBlock + sourceStrideElements * 5u);

	const uint8x8x2_t line45_channel0_u_8x8x2 = vtrn_u8(line4_u_8x8x3.val[0], line5_u_8x8x3.val[0]);
	const uint8x8x2_t line45_channel1_u_8x8x2 = vtrn_u8(line4_u_8x8x3.val[1], line5_u_8x8x3.val[1]);
	const uint8x8x2_t line45_channel2_u_8x8x2 = vtrn_u8(line4_u_8x8x3.val[2], line5_u_8x8x3.val[2]);

	const uint8x8x3_t line6_u_8x8x3 = vld3_u8(sourceBlock + sourceStrideElements * 6u);
	const uint8x8x3_t line7_u_8x8x3 = vld3_u8(sourceBlock + sourceStrideElements * 7u);

	const uint8x8x2_t line67_channel0_u_8x8x2 = vtrn_u8(line6_u_8x8x3.val[0], line7_u_8x8x3.val[0]);
	const uint8x8x2_t line67_channel1_u_8x8x2 = vtrn_u8(line6_u_8x8x3.val[1], line7_u_8x8x3.val[1]);
	const uint8x8x2_t line67_channel2_u_8x8x2 = vtrn_u8(line6_u_8x8x3.val[2], line7_u_8x8x3.val[2]);

	const uint16x4x2_t line46_channel0_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line45_channel0_u_8x8x2.val[0]), vreinterpret_u16_u8(line67_channel0_u_8x8x2.val[0]));
	const uint16x4x2_t line46_channel1_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line45_channel1_u_8x8x2.val[0]), vreinterpret_u16_u8(line67_channel1_u_8x8x2.val[0]));
	const uint16x4x2_t line46_channel2_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line45_channel2_u_8x8x2.val[0]), vreinterpret_u16_u8(line67_channel2_u_8x8x2.val[0]));

	const uint16x4x2_t line57_channel0_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line45_channel0_u_8x8x2.val[1]), vreinterpret_u16_u8(line67_channel0_u_8x8x2.val[1]));
	const uint16x4x2_t line57_channel1_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line45_channel1_u_8x8x2.val[1]), vreinterpret_u16_u8(line67_channel1_u_8x8x2.val[1]));
	const uint16x4x2_t line57_channel2_u_16x4x2 = vtrn_u16(vreinterpret_u16_u8(line45_channel2_u_8x8x2.val[1]), vreinterpret_u16_u8(line67_channel2_u_8x8x2.val[1]));

	const uint32x2x2_t line04_channel0_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line02_channel0_u_16x4x2.val[0]), vreinterpret_u32_u16(line46_channel0_u_16x4x2.val[0]));
	const uint32x2x2_t line04_channel1_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line02_channel1_u_16x4x2.val[0]), vreinterpret_u32_u16(line46_channel1_u_16x4x2.val[0]));
	const uint32x2x2_t line04_channel2_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line02_channel2_u_16x4x2.val[0]), vreinterpret_u32_u16(line46_channel2_u_16x4x2.val[0]));

	const uint32x2x2_t line26_channel0_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line02_channel0_u_16x4x2.val[1]), vreinterpret_u32_u16(line46_channel0_u_16x4x2.val[1]));
	const uint32x2x2_t line26_channel1_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line02_channel1_u_16x4x2.val[1]), vreinterpret_u32_u16(line46_channel1_u_16x4x2.val[1]));
	const uint32x2x2_t line26_channel2_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line02_channel2_u_16x4x2.val[1]), vreinterpret_u32_u16(line46_channel2_u_16x4x2.val[1]));

	const uint32x2x2_t line15_channel0_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line13_channel0_u_16x4x2.val[0]), vreinterpret_u32_u16(line57_channel0_u_16x4x2.val[0]));
	const uint32x2x2_t line15_channel1_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line13_channel1_u_16x4x2.val[0]), vreinterpret_u32_u16(line57_channel1_u_16x4x2.val[0]));
	const uint32x2x2_t line15_channel2_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line13_channel2_u_16x4x2.val[0]), vreinterpret_u32_u16(line57_channel2_u_16x4x2.val[0]));

	const uint32x2x2_t line37_channel0_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line13_channel0_u_16x4x2.val[1]), vreinterpret_u32_u16(line57_channel0_u_16x4x2.val[1]));
	const uint32x2x2_t line37_channel1_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line13_channel1_u_16x4x2.val[1]), vreinterpret_u32_u16(line57_channel1_u_16x4x2.val[1]));
	const uint32x2x2_t line37_channel2_u_32x2x2 = vtrn_u32(vreinterpret_u32_u16(line13_channel2_u_16x4x2.val[1]), vreinterpret_u32_u16(line57_channel2_u_16x4x2.val[1]));

	switch (tFlipDirection)
	{
		case FD_NONE:
		{
			uint8x8x3_t result0_u_8x8x3;
			result0_u_8x8x3.val[0] = vreinterpret_u8_u32(line04_channel0_u_32x2x2.val[0]);
			result0_u_8x8x3.val[1] = vreinterpret_u8_u32(line04_channel1_u_32x2x2.val[0]);
			result0_u_8x8x3.val[2] = vreinterpret_u8_u32(line04_channel2_u_32x2x2.val[0]);
			vst3_u8(targetBlock + targetStrideElements * 0u, result0_u_8x8x3);

			uint8x8x3_t result1_u_8x8x3;
			result1_u_8x8x3.val[0] = vreinterpret_u8_u32(line15_channel0_u_32x2x2.val[0]);
			result1_u_8x8x3.val[1] = vreinterpret_u8_u32(line15_channel1_u_32x2x2.val[0]);
			result1_u_8x8x3.val[2] = vreinterpret_u8_u32(line15_channel2_u_32x2x2.val[0]);
			vst3_u8(targetBlock + targetStrideElements * 1u, result1_u_8x8x3);

			uint8x8x3_t result2_u_8x8x3;
			result2_u_8x8x3.val[0] = vreinterpret_u8_u32(line26_channel0_u_32x2x2.val[0]);
			result2_u_8x8x3.val[1] = vreinterpret_u8_u32(line26_channel1_u_32x2x2.val[0]);
			result2_u_8x8x3.val[2] = vreinterpret_u8_u32(line26_channel2_u_32x2x2.val[0]);
			vst3_u8(targetBlock + targetStrideElements * 2u, result2_u_8x8x3);

			uint8x8x3_t result3_u_8x8x3;
			result3_u_8x8x3.val[0] = vreinterpret_u8_u32(line37_channel0_u_32x2x2.val[0]);
			result3_u_8x8x3.val[1] = vreinterpret_u8_u32(line37_channel1_u_32x2x2.val[0]);
			result3_u_8x8x3.val[2] = vreinterpret_u8_u32(line37_channel2_u_32x2x2.val[0]);
			vst3_u8(targetBlock + targetStrideElements * 3u, result3_u_8x8x3);

			uint8x8x3_t result4_u_8x8x3;
			result4_u_8x8x3.val[0] = vreinterpret_u8_u32(line04_channel0_u_32x2x2.val[1]);
			result4_u_8x8x3.val[1] = vreinterpret_u8_u32(line04_channel1_u_32x2x2.val[1]);
			result4_u_8x8x3.val[2] = vreinterpret_u8_u32(line04_channel2_u_32x2x2.val[1]);
			vst3_u8(targetBlock + targetStrideElements * 4u, result4_u_8x8x3);

			uint8x8x3_t result5_u_8x8x3;
			result5_u_8x8x3.val[0] = vreinterpret_u8_u32(line15_channel0_u_32x2x2.val[1]);
			result5_u_8x8x3.val[1] = vreinterpret_u8_u32(line15_channel1_u_32x2x2.val[1]);
			result5_u_8x8x3.val[2] = vreinterpret_u8_u32(line15_channel2_u_32x2x2.val[1]);
			vst3_u8(targetBlock + targetStrideElements * 5u, result5_u_8x8x3);

			uint8x8x3_t result6_u_8x8x3;
			result6_u_8x8x3.val[0] = vreinterpret_u8_u32(line26_channel0_u_32x2x2.val[1]);
			result6_u_8x8x3.val[1] = vreinterpret_u8_u32(line26_channel1_u_32x2x2.val[1]);
			result6_u_8x8x3.val[2] = vreinterpret_u8_u32(line26_channel2_u_32x2x2.val[1]);
			vst3_u8(targetBlock + targetStrideElements * 6u, result6_u_8x8x3);

			uint8x8x3_t result7_u_8x8x3;
			result7_u_8x8x3.val[0] = vreinterpret_u8_u32(line37_channel0_u_32x2x2.val[1]);
			result7_u_8x8x3.val[1] = vreinterpret_u8_u32(line37_channel1_u_32x2x2.val[1]);
			result7_u_8x8x3.val[2] = vreinterpret_u8_u32(line37_channel2_u_32x2x2.val[1]);
			vst3_u8(targetBlock + targetStrideElements * 7u, result7_u_8x8x3);

			break;
		}

		case FD_LEFT_RIGHT:
		{
			uint8x8x3_t result0_u_8x8x3;
			result0_u_8x8x3.val[0] = vrev64_u8(vreinterpret_u8_u32(line04_channel0_u_32x2x2.val[0]));
			result0_u_8x8x3.val[1] = vrev64_u8(vreinterpret_u8_u32(line04_channel1_u_32x2x2.val[0]));
			result0_u_8x8x3.val[2] = vrev64_u8(vreinterpret_u8_u32(line04_channel2_u_32x2x2.val[0]));
			vst3_u8(targetBlock + targetStrideElements * 0u, result0_u_8x8x3);

			uint8x8x3_t result1_u_8x8x3;
			result1_u_8x8x3.val[0] = vrev64_u8(vreinterpret_u8_u32(line15_channel0_u_32x2x2.val[0]));
			result1_u_8x8x3.val[1] = vrev64_u8(vreinterpret_u8_u32(line15_channel1_u_32x2x2.val[0]));
			result1_u_8x8x3.val[2] = vrev64_u8(vreinterpret_u8_u32(line15_channel2_u_32x2x2.val[0]));
			vst3_u8(targetBlock + targetStrideElements * 1u, result1_u_8x8x3);

			uint8x8x3_t result2_u_8x8x3;
			result2_u_8x8x3.val[0] = vrev64_u8(vreinterpret_u8_u32(line26_channel0_u_32x2x2.val[0]));
			result2_u_8x8x3.val[1] = vrev64_u8(vreinterpret_u8_u32(line26_channel1_u_32x2x2.val[0]));
			result2_u_8x8x3.val[2] = vrev64_u8(vreinterpret_u8_u32(line26_channel2_u_32x2x2.val[0]));
			vst3_u8(targetBlock + targetStrideElements * 2u, result2_u_8x8x3);

			uint8x8x3_t result3_u_8x8x3;
			result3_u_8x8x3.val[0] = vrev64_u8(vreinterpret_u8_u32(line37_channel0_u_32x2x2.val[0]));
			result3_u_8x8x3.val[1] = vrev64_u8(vreinterpret_u8_u32(line37_channel1_u_32x2x2.val[0]));
			result3_u_8x8x3.val[2] = vrev64_u8(vreinterpret_u8_u32(line37_channel2_u_32x2x2.val[0]));
			vst3_u8(targetBlock + targetStrideElements * 3u, result3_u_8x8x3);

			uint8x8x3_t result4_u_8x8x3;
			result4_u_8x8x3.val[0] = vrev64_u8(vreinterpret_u8_u32(line04_channel0_u_32x2x2.val[1]));
			result4_u_8x8x3.val[1] = vrev64_u8(vreinterpret_u8_u32(line04_channel1_u_32x2x2.val[1]));
			result4_u_8x8x3.val[2] = vrev64_u8(vreinterpret_u8_u32(line04_channel2_u_32x2x2.val[1]));
			vst3_u8(targetBlock + targetStrideElements * 4u, result4_u_8x8x3);

			uint8x8x3_t result5_u_8x8x3;
			result5_u_8x8x3.val[0] = vrev64_u8(vreinterpret_u8_u32(line15_channel0_u_32x2x2.val[1]));
			result5_u_8x8x3.val[1] = vrev64_u8(vreinterpret_u8_u32(line15_channel1_u_32x2x2.val[1]));
			result5_u_8x8x3.val[2] = vrev64_u8(vreinterpret_u8_u32(line15_channel2_u_32x2x2.val[1]));
			vst3_u8(targetBlock + targetStrideElements * 5u, result5_u_8x8x3);

			uint8x8x3_t result6_u_8x8x3;
			result6_u_8x8x3.val[0] = vrev64_u8(vreinterpret_u8_u32(line26_channel0_u_32x2x2.val[1]));
			result6_u_8x8x3.val[1] = vrev64_u8(vreinterpret_u8_u32(line26_channel1_u_32x2x2.val[1]));
			result6_u_8x8x3.val[2] = vrev64_u8(vreinterpret_u8_u32(line26_channel2_u_32x2x2.val[1]));
			vst3_u8(targetBlock + targetStrideElements * 6u, result6_u_8x8x3);

			uint8x8x3_t result7_u_8x8x3;
			result7_u_8x8x3.val[0] = vrev64_u8(vreinterpret_u8_u32(line37_channel0_u_32x2x2.val[1]));
			result7_u_8x8x3.val[1] = vrev64_u8(vreinterpret_u8_u32(line37_channel1_u_32x2x2.val[1]));
			result7_u_8x8x3.val[2] = vrev64_u8(vreinterpret_u8_u32(line37_channel2_u_32x2x2.val[1]));
			vst3_u8(targetBlock + targetStrideElements * 7u, result7_u_8x8x3);

			break;
		}

		case FD_TOP_BOTTOM:
		{
			uint8x8x3_t result7_u_8x8x3;
			result7_u_8x8x3.val[0] = vreinterpret_u8_u32(line37_channel0_u_32x2x2.val[1]);
			result7_u_8x8x3.val[1] = vreinterpret_u8_u32(line37_channel1_u_32x2x2.val[1]);
			result7_u_8x8x3.val[2] = vreinterpret_u8_u32(line37_channel2_u_32x2x2.val[1]);
			vst3_u8(targetBlock + targetStrideElements * 0u, result7_u_8x8x3);

			uint8x8x3_t result6_u_8x8x3;
			result6_u_8x8x3.val[0] = vreinterpret_u8_u32(line26_channel0_u_32x2x2.val[1]);
			result6_u_8x8x3.val[1] = vreinterpret_u8_u32(line26_channel1_u_32x2x2.val[1]);
			result6_u_8x8x3.val[2] = vreinterpret_u8_u32(line26_channel2_u_32x2x2.val[1]);
			vst3_u8(targetBlock + targetStrideElements * 1u, result6_u_8x8x3);

			uint8x8x3_t result5_u_8x8x3;
			result5_u_8x8x3.val[0] = vreinterpret_u8_u32(line15_channel0_u_32x2x2.val[1]);
			result5_u_8x8x3.val[1] = vreinterpret_u8_u32(line15_channel1_u_32x2x2.val[1]);
			result5_u_8x8x3.val[2] = vreinterpret_u8_u32(line15_channel2_u_32x2x2.val[1]);
			vst3_u8(targetBlock + targetStrideElements * 2u, result5_u_8x8x3);

			uint8x8x3_t result4_u_8x8x3;
			result4_u_8x8x3.val[0] = vreinterpret_u8_u32(line04_channel0_u_32x2x2.val[1]);
			result4_u_8x8x3.val[1] = vreinterpret_u8_u32(line04_channel1_u_32x2x2.val[1]);
			result4_u_8x8x3.val[2] = vreinterpret_u8_u32(line04_channel2_u_32x2x2.val[1]);
			vst3_u8(targetBlock + targetStrideElements * 3u, result4_u_8x8x3);

			uint8x8x3_t result3_u_8x8x3;
			result3_u_8x8x3.val[0] = vreinterpret_u8_u32(line37_channel0_u_32x2x2.val[0]);
			result3_u_8x8x3.val[1] = vreinterpret_u8_u32(line37_channel1_u_32x2x2.val[0]);
			result3_u_8x8x3.val[2] = vreinterpret_u8_u32(line37_channel2_u_32x2x2.val[0]);
			vst3_u8(targetBlock + targetStrideElements * 4u, result3_u_8x8x3);

			uint8x8x3_t result2_u_8x8x3;
			result2_u_8x8x3.val[0] = vreinterpret_u8_u32(line26_channel0_u_32x2x2.val[0]);
			result2_u_8x8x3.val[1] = vreinterpret_u8_u32(line26_channel1_u_32x2x2.val[0]);
			result2_u_8x8x3.val[2] = vreinterpret_u8_u32(line26_channel2_u_32x2x2.val[0]);
			vst3_u8(targetBlock + targetStrideElements * 5u, result2_u_8x8x3);

			uint8x8x3_t result1_u_8x8x3;
			result1_u_8x8x3.val[0] = vreinterpret_u8_u32(line15_channel0_u_32x2x2.val[0]);
			result1_u_8x8x3.val[1] = vreinterpret_u8_u32(line15_channel1_u_32x2x2.val[0]);
			result1_u_8x8x3.val[2] = vreinterpret_u8_u32(line15_channel2_u_32x2x2.val[0]);
			vst3_u8(targetBlock + targetStrideElements * 6u, result1_u_8x8x3);

			uint8x8x3_t result0_u_8x8x3;
			result0_u_8x8x3.val[0] = vreinterpret_u8_u32(line04_channel0_u_32x2x2.val[0]);
			result0_u_8x8x3.val[1] = vreinterpret_u8_u32(line04_channel1_u_32x2x2.val[0]);
			result0_u_8x8x3.val[2] = vreinterpret_u8_u32(line04_channel2_u_32x2x2.val[0]);
			vst3_u8(targetBlock + targetStrideElements * 7u, result0_u_8x8x3);

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

template <>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<uint8_t, 4u>::transposeBlock8x8(const uint8_t* sourceBlock, uint8_t* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);
	ocean_assert(sourceStrideElements >= 8u * 4u && targetStrideElements >= 8u * 4u);

	// we simply tranpose four blocks of 4x4 pixels

	switch (tFlipDirection)
	{
		case FD_NONE:
		{
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock, targetBlock, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 16, targetBlock + 4 * targetStrideElements, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 4 * sourceStrideElements, targetBlock + 16, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 4 * sourceStrideElements + 16, targetBlock + 4 * targetStrideElements + 16, sourceStrideElements, targetStrideElements);

			break;
		}

		case FD_LEFT_RIGHT:
		{
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock, targetBlock + 16, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 16, targetBlock + 4 * targetStrideElements + 16, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 4 * sourceStrideElements, targetBlock, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 4 * sourceStrideElements + 16, targetBlock + 4 * targetStrideElements, sourceStrideElements, targetStrideElements);

			break;
		}

		case FD_TOP_BOTTOM:
		{
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock, targetBlock + 4 * targetStrideElements, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 16, targetBlock, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 4 * sourceStrideElements, targetBlock + 4 * targetStrideElements + 16, sourceStrideElements, targetStrideElements);
			transposeBlock4x4NEON<tFlipDirection>(sourceBlock + 4 * sourceStrideElements + 16, targetBlock + 16, sourceStrideElements, targetStrideElements);

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename T, unsigned int tChannels>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<T, tChannels>::transposeBlock8x8(const T* sourceBlock, T* targetBlock, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);
	ocean_assert(sourceStrideElements >= 8u && targetStrideElements >= 8u);

	typedef typename DataType<T, tChannels>::Type PixelType;

	switch (tFlipDirection)
	{
		case FD_NONE:
		{
			// simply transposing the block

			for (unsigned int y = 0u; y < 8u; ++y)
			{
				const PixelType* const sourcePixel = (const PixelType*)sourceBlock;

				for (unsigned int x = 0u; x < 8u; ++x)
				{
					*((PixelType*)(targetBlock + targetStrideElements * x)) = sourcePixel[x];
				}

				sourceBlock += sourceStrideElements;
				targetBlock += tChannels;
			}

			break;
		}

		case FD_LEFT_RIGHT:
		{
			// transposing the block and applying a left-right flip like a mirror, actually a 90 degree clockwise rotation

			for (unsigned int y = 0u; y < 8u; ++y)
			{
				const PixelType* const sourcePixel = (const PixelType*)sourceBlock;

				for (unsigned int x = 0u; x < 8u; ++x)
				{
					*((PixelType*)(targetBlock + targetStrideElements * x) + (8u - y - 1u)) = sourcePixel[x];
				}

				sourceBlock += sourceStrideElements;
			}

			break;
		}

		case FD_TOP_BOTTOM:
		{
			// transposing the block and applying a top-bottom flip, actually a 90 degree counter clockwise rotation

			for (unsigned int y = 0u; y < 8u; ++y)
			{
				const PixelType* const sourcePixel = (const PixelType*)sourceBlock;

				for (unsigned int x = 0u; x < 8u; ++x)
				{
					*((PixelType*)(targetBlock + targetStrideElements * (8u - x - 1u)) + y) = sourcePixel[x];
				}

				sourceBlock += sourceStrideElements;
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

template <typename T, unsigned int tChannels>
template <FrameTransposer::FlipDirection tFlipDirection>
OCEAN_FORCE_INLINE void FrameTransposer::BlockTransposer<T, tChannels>::transposeBlock(const T* sourceBlock, T* targetBlock, const unsigned int blockWidth, const unsigned int blockHeight, const unsigned int sourceStrideElements, const unsigned int targetStrideElements)
{
	ocean_assert(sourceBlock && targetBlock);

	ocean_assert(blockWidth >= 1u && blockHeight >= 1u);
	ocean_assert(blockWidth < 8u || blockHeight < 8u);

	ocean_assert(sourceStrideElements >= blockWidth);
	ocean_assert(targetStrideElements >= blockHeight);

	typedef typename DataType<T, tChannels>::Type PixelType;

	switch (tFlipDirection)
	{
		case FD_NONE:
		{
			// simply transposing the block

			for (unsigned int y = 0u; y < blockHeight; ++y)
			{
				const PixelType* const sourcePixel = (const PixelType*)sourceBlock;

				for (unsigned int x = 0u; x < blockWidth; ++x)
				{
					*((PixelType*)(targetBlock + targetStrideElements * x)) = sourcePixel[x];
				}

				sourceBlock += sourceStrideElements;
				targetBlock += tChannels;
			}

			break;
		}

		case FD_LEFT_RIGHT:
		{
			// transposing the block and applying a left-right flip like a mirror, actually a 90 degree clockwise rotation

			for (unsigned int y = 0u; y < blockHeight; ++y)
			{
				const PixelType* const sourcePixel = (const PixelType*)sourceBlock;

				for (unsigned int x = 0u; x < blockWidth; ++x)
				{
					*((PixelType*)(targetBlock + targetStrideElements * x) + (blockHeight - y - 1u)) = sourcePixel[x];
				}

				sourceBlock += sourceStrideElements;
			}

			break;
		}

		case FD_TOP_BOTTOM:
		{
			// transposing the block and applying a top-bottom flip, actually a 90 degree counter clockwise rotation

			for (unsigned int y = 0u; y < blockHeight; ++y)
			{
				const PixelType* const sourcePixel = (const PixelType*)sourceBlock;

				for (unsigned int x = 0u; x < blockWidth; ++x)
				{
					*((PixelType*)(targetBlock + targetStrideElements * (blockWidth - x - 1u)) + y) = sourcePixel[x];
				}

				sourceBlock += sourceStrideElements;
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid flip direction!");
	}
}

template <typename TElementType, unsigned int tChannels>
inline void FrameTransposer::rotate90Subset(const TElementType* source, TElementType* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const bool clockwise, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && target);
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u);

	ocean_assert(firstTargetRow + numberTargetRows <= sourceWidth);

	const unsigned int& targetWidth = sourceHeight;

	// Clockwise:        Counter-Clockwise:
	// Source:           Source:
	// ^ ^ ^ ^           ... D C B A
	// | | | |               | | | |
	// | | | |               | | | |
	// | | | |               | | | |
	// A B C D ...           v v v v
	// Target:          Target:
	// A ------>        A ------>
	// B ------>        B ------>
	// C ------>        C ------>
	// D ...            D ...
	// E ...            E ...

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	TElementType* targetRowStartElement = target + firstTargetRow * targetStrideElements;
	const TElementType* const targetEndElement = targetRowStartElement + numberTargetRows * targetStrideElements - targetPaddingElements;
	ocean_assert_and_suppress_unused(targetRowStartElement < targetEndElement || numberTargetRows == 0u, targetEndElement);

	if (clockwise)
	{
		const TElementType* sourceColumnStartElement = source + (sourceHeight - 1u) * sourceStrideElements + tChannels * firstTargetRow;

		for (unsigned row = 0u; row < numberTargetRows; ++row)
		{
			const TElementType* sourceElement = sourceColumnStartElement;

			TElementType* targetElement = targetRowStartElement;
			const TElementType* const targetRowEndElement = targetRowStartElement + tChannels * targetWidth;
			ocean_assert(targetRowEndElement <= targetEndElement);

			while (targetElement != targetRowEndElement)
			{
				ocean_assert(sourceElement < source + sourceHeight * sourceStrideElements - sourcePaddingElements);
				ocean_assert(targetElement < targetEndElement);
				ocean_assert(targetElement < targetRowEndElement);

				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					targetElement[c] = sourceElement[c];
				}

				sourceElement -= sourceStrideElements;
				targetElement += tChannels;
			}

			sourceColumnStartElement += tChannels;
			targetRowStartElement += targetStrideElements;
		}
	}
	else
	{
		const TElementType* sourceColumnStartElement = source + tChannels * (sourceWidth - firstTargetRow - 1u);

		for (unsigned row = 0u; row < numberTargetRows; ++row)
		{
			const TElementType* sourceElement = sourceColumnStartElement;
			ocean_assert(sourceElement >= source);

			TElementType* targetElement = targetRowStartElement;
			const TElementType* const targetRowEndElement = targetRowStartElement + tChannels * targetWidth;
			ocean_assert(targetRowEndElement <= targetEndElement);

			while (targetElement != targetRowEndElement)
			{
				ocean_assert(sourceElement < source + sourceHeight * sourceStrideElements - sourcePaddingElements);
				ocean_assert(targetElement < targetEndElement);
				ocean_assert(targetElement < targetRowEndElement);

				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					targetElement[c] = sourceElement[c];
				}

				sourceElement += sourceStrideElements;
				targetElement += tChannels;
			}

			sourceColumnStartElement -= tChannels;
			targetRowStartElement += targetStrideElements;
		}
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_TRANSPOSER_H
