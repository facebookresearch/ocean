// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_SYNTHESIS_IMAGE_COMPLETION_LAYER_PATCH_MASK_H
#define META_OCEAN_CV_SYNTHESIS_IMAGE_COMPLETION_LAYER_PATCH_MASK_H

#include "ocean/cv/synthesis/Synthesis.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/SumSquareDifferences.h"

#include "ocean/math/Math.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements an image completion layer based on randomly approximated patch matches.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT ImageCompletionLayer_patchmask
{
	public:

		/**
		 * This class implementes a patch match between two images.
		 * The 2D position of a patch is defined by the center pixel.
		 */
		class OCEAN_CV_SYNTHESIS_EXPORT Patch
		{
			public:

				/**
				 * Creates a new empty patch match.
				 */
				inline Patch();

				/**
				 * Creates a new patch match by given matching parameters.
				 * @param x X position of the second patch
				 * @param y Y position of the second patch
				 * @param ssd Summed square distance between the pixel values of two patches
				 * @param sqrDistance Summed square (spatial) distance of two patches
				 */
				inline Patch(const unsigned int x, const unsigned int y, const unsigned int ssd, const unsigned int sqrDistance);

				/**
				 * Returns the x position of the second (matching) patch.
				 * @return X position of the second patch
				 */
				inline unsigned int x() const;

				/**
				 * Returns the y position of the second (matching) patch.
				 * @return Y position of the second patch
				 */
				inline unsigned int y() const;

				/**
				 * Returns the summed square distance of the color values bettween two patches.
				 * @return Summed square distance of the pixel values
				 */
				inline unsigned int ssd() const;

				/**
				 * Returns the spatial summed square distance between two patches.
				 * @return Summed square distance (spatial)
				 */
				inline unsigned int sqrDistance() const;

			private:

				/// X position of the second (matching) patch.
				unsigned int patchX;

				/// Y position of the second (matching) patch.
				unsigned int patchY;

				/// Summed square distance of patch pixel values
				unsigned int patchSSD;

				/// Spatial summed square distance of two patches
				unsigned int patchSqrDistance;
		};

	public:

		ImageCompletionLayer_patchmask();

		ImageCompletionLayer_patchmask(const ImageCompletionLayer_patchmask& layer);

		ImageCompletionLayer_patchmask(const unsigned int width, const unsigned int height);

		ImageCompletionLayer_patchmask(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer);

		ImageCompletionLayer_patchmask(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer, Worker& worker);

		ImageCompletionLayer_patchmask(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		~ImageCompletionLayer_patchmask();

		inline unsigned int width() const
		{
			return width_;
		}

		inline unsigned int height() const
		{
			return height_;
		}

		inline unsigned int pixels() const
		{
			return width_ * height_;
		}

		bool initializeNull(const uint8_t* mask, const unsigned int patchSize);

		bool initializeRandom8Bit(const uint8_t* frame, const uint8_t* mask, const unsigned int patchSize);

		bool initializeRandom24Bit(const uint8_t* frame, const uint8_t* mask, const unsigned int patchSize);

		inline bool improveRandom8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations)
		{
			if (frame == nullptr || mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
				return false;

			const unsigned int patchHalf = patchSize >> 1;
			improveRandom8Bit(frame, mask, sourcePatchMask, targetPatchMask, patchSize, iterations, patchHalf, width_ - patchSize + 1, patchHalf, height_ - patchSize + 1);

			return true;
		}

		inline bool improveRandom24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations)
		{
			if (frame == nullptr || mask == nullptr || patchSize == 0 || (patchSize % 2) != 1)
				return false;

			const unsigned int patchHalf = patchSize >> 1;
			improveRandom24Bit(frame, mask, sourcePatchMask, targetPatchMask, patchSize, iterations, patchHalf, width_ - patchSize + 1, patchHalf, height_ - patchSize + 1);

			return true;
		}

		bool improveRandom8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, Worker& worker);

		bool improveRandom24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, Worker& worker);

		bool improveRandom8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		bool improveRandom24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		bool coherenceImage8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, uint8_t* result, const unsigned int patchSize);

		bool coherenceImage8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, uint8_t* result, const unsigned int patchSize, const unsigned int offset);

		bool coherenceImage24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, uint8_t* result, const unsigned int patchSize);

		bool coherenceImage24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, uint8_t* result, const unsigned int patchSize, const unsigned int offset);

		void clearAndAdopt(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		void clearAndAdopt(const unsigned int width, const unsigned int height, const uint8_t* mask, const unsigned int patchSize, const ImageCompletionLayer_patchmask& layer, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		ImageCompletionLayer_patchmask& operator=(const ImageCompletionLayer_patchmask& layer);

		explicit inline operator bool() const
		{
			return layerPatches != nullptr;
		}

	private:

		void improveRandom8Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		void improveRandom24Bit(const uint8_t* frame, const uint8_t* mask, const uint8_t* sourcePatchMask, const uint8_t* targetPatchMask, const unsigned int patchSize, const unsigned int iterations, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		inline void adopt(const ImageCompletionLayer_patchmask& layer, const uint8_t* mask, const unsigned int patchSize)
		{
			adopt(&layer, mask, patchSize, 0, width_, 0, height_);

			for (unsigned int n = 0; n < pixels(); ++n)
				ocean_assert(layerPatches[n].x() == 0xFFFFFFFF || layerPatches[n].x() < width());
		}

		void adopt(const ImageCompletionLayer_patchmask& layer, const uint8_t* mask, const unsigned int patchSize, Worker& worker);

		void adopt(const ImageCompletionLayer_patchmask& layer, const uint8_t* mask, const unsigned int patchSize, Worker& worker, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		void adopt(const ImageCompletionLayer_patchmask* layer, const uint8_t* mask, const unsigned int patchSize, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		inline unsigned int calculateSSD8Bit(const uint8_t* frame, const unsigned int width, const unsigned int firstX, const unsigned int firstY, const unsigned int secondX, const unsigned int secondY, const unsigned int patchHalf);
		inline unsigned int calculateSSD24Bit(const uint8_t* frame, const unsigned int width, const unsigned int firstX, const unsigned int firstY, const unsigned int secondX, const unsigned int secondY, const unsigned int patchHalf);

		inline static unsigned int sqrLength(const int first, const int second)
		{
			// **TEST** **JH** original:
			return first * first + second * second;
			//return 0;

			//return (first * first + second * second) * (first * first + second * second);
		}

	public:

		unsigned int width_;

		unsigned int height_;

		Patch* layerPatches;

		static const Scalar searchAreaFactors[12];
};

inline ImageCompletionLayer_patchmask::Patch::Patch() :
	patchX(0xFFFFFFFF),
	patchY(0xFFFFFFFF),
	patchSSD(0xFFFFFFFF),
	patchSqrDistance(0xFFFFFFFF)
{
	// nothing to do here
}

inline ImageCompletionLayer_patchmask::Patch::Patch(const unsigned int x, const unsigned int y, const unsigned int ssd, const unsigned int sqrDistance) :
	patchX(x),
	patchY(y),
	patchSSD(ssd),
	patchSqrDistance(sqrDistance)
{
	// nothing to do here
}

inline unsigned int ImageCompletionLayer_patchmask::Patch::x() const
{
	return patchX;
}

inline unsigned int ImageCompletionLayer_patchmask::Patch::y() const
{
	return patchY;
}

inline unsigned int ImageCompletionLayer_patchmask::Patch::ssd() const
{
	return patchSSD;
}

inline unsigned int ImageCompletionLayer_patchmask::Patch::sqrDistance() const
{
	return patchSqrDistance;
}

inline unsigned int ImageCompletionLayer_patchmask::calculateSSD8Bit(const uint8_t* frame, const unsigned int width, const unsigned int firstX, const unsigned int firstY, const unsigned int secondX, const unsigned int secondY, const unsigned int patchHalf)
{
	constexpr unsigned int framePaddingElements = 0u;

	unsigned int ssd = 0;

	switch (patchHalf)
	{
		/// patch size 7
		case 3:
		{
			/*unsigned int pSecondY = secondY - patchHalf;

			for (unsigned int pFirstY = firstY - patchHalf; pFirstY <= firstY + patchHalf; ++pFirstY)
			{
				unsigned int pSecondX = secondX - patchHalf;

				for (unsigned int pFirstX = firstX - patchHalf; pFirstX <= firstX + patchHalf; ++pFirstX)
				{
					const int value = frame[pFirstY * width + pFirstX] - frame[pSecondY * width + pSecondX];
					ssd += value * value;

					++pSecondX;
				}

				++pSecondY;
			}

			const unsigned int ssdCopy = ssd;
			ssd = 0;*/

			const uint8_t* first = frame + (firstY - 3) * width + firstX - 3;
			const uint8_t* second = frame + (secondY - 3) * width + secondX - 3;
			const unsigned int offset = width - 6;

			int value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			//ocean_assert(ssd == ssdCopy);

			break;
		}

		/// patch size 13
		case 6:
		{
			/*unsigned int pSecondY = secondY - patchHalf;

			for (unsigned int pFirstY = firstY - patchHalf; pFirstY <= firstY + patchHalf; pFirstY += 2)
			{
				unsigned int pSecondX = secondX - patchHalf;

				for (unsigned int pFirstX = firstX - patchHalf; pFirstX <= firstX + patchHalf; pFirstX += 2)
				{
					const int value = frame[pFirstY * width + pFirstX] - frame[pSecondY * width + pSecondX];
					ssd += value * value;

					pSecondX += 2;
				}

				pSecondY += 2;
			}

			const unsigned int ssdCopy = ssd;
			ssd = 0;*/

			const uint8_t* first = frame + (firstY - 6) * width + firstX - 6;
			const uint8_t* second = frame + (secondY - 6) * width + secondX - 6;
			const unsigned int offset = (width << 1) - 12;

			int value = *first - *second;
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;
			value = *(first += 2) - *(second += 2);
			ssd += value * value;

			//ocean_assert(ssd == ssdCopy);

			break;
		}

		/// patch size 25
		case 12:
		{
			/*unsigned int pSecondY = secondY - patchHalf;

			for (unsigned int pFirstY = firstY - patchHalf; pFirstY <= firstY + patchHalf; pFirstY += 4)
			{
				unsigned int pSecondX = secondX - patchHalf;

				for (unsigned int pFirstX = firstX - patchHalf; pFirstX <= firstX + patchHalf; pFirstX += 4)
				{
					const int value = frame[pFirstY * width + pFirstX] - frame[pSecondY * width + pSecondX];
					ssd += value * value;

					pSecondX += 4;
				}

				pSecondY += 4;
			}

			const unsigned int ssdCopy = ssd;
			ssd = 0;*/

			const uint8_t* first = frame + (firstY - 12) * width + firstX - 12;
			const uint8_t* second = frame + (secondY - 12) * width + secondX - 12;
			const unsigned int offset = (width << 2) - 24;

			int value = *first - *second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;

			//ocean_assert(ssd == ssdCopy);

			break;
		}


		/// patch size 49
		case 24:
		{
			unsigned int pSecondY = secondY - patchHalf;

			for (unsigned int pFirstY = firstY - patchHalf; pFirstY <= firstY + patchHalf; pFirstY += 8)
			{
				unsigned int pSecondX = secondX - patchHalf;

				for (unsigned int pFirstX = firstX - patchHalf; pFirstX <= firstX + patchHalf; pFirstX += 8)
				{
					const int value = frame[pFirstY * width + pFirstX] - frame[pSecondY * width + pSecondX];
					ssd += value * value;

					pSecondX += 8;
				}

				pSecondY += 8;
			}

#ifdef OCEAN_DEBUG
			unsigned int ssdCopyDebug = ssd;
#endif
			ssd = 0;

			const uint8_t* first = frame + (firstY - 24) * width + firstX - 24;
			const uint8_t* second = frame + (secondY - 24) * width + secondX - 24;
			const unsigned int offset = (width << 3) - 48;

			int value = *first - *second;
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;
			value = *(first += 8) - *(second += 8);
			ssd += value * value;

			ocean_assert(ssdCopyDebug == ssd);

			break;
		}

		default:
		{
			ssd = CV::SumSquareDifferencesBase::patch8BitPerChannel(frame, frame, 1u, patchHalf * 2u + 1u, width, width, firstX, firstY, secondX, secondY, framePaddingElements, framePaddingElements);

			break;
		}
	}

	return ssd;
}

inline unsigned int ImageCompletionLayer_patchmask::calculateSSD24Bit(const uint8_t* frame, const unsigned int width, const unsigned int firstX, const unsigned int firstY, const unsigned int secondX, const unsigned int secondY, const unsigned int patchHalf)
{
	constexpr unsigned int framePaddingElements = 0u;

	const unsigned int width3 = width * 3;
	unsigned int ssd = 0;

	switch (patchHalf)
	{
		/// patch size 7
		case 3:
		{
			/*unsigned int pSecondY = secondY - patchHalf;

			for (unsigned int pFirstY = firstY - patchHalf; pFirstY <= firstY + patchHalf; ++pFirstY)
			{
				unsigned int pSecondX = secondX - patchHalf;

				for (unsigned int pFirstX = firstX - patchHalf; pFirstX <= firstX + patchHalf; ++pFirstX)
				{
					const int value0 = frame[pFirstY * 3 * width + 3 * pFirstX] - frame[pSecondY * 3 * width + 3 * pSecondX];
					const int value1 = frame[pFirstY * 3 * width + 3 * pFirstX + 1] - frame[pSecondY * 3 * width + 3 * pSecondX + 1];
					const int value2 = frame[pFirstY * 3 * width + 3 * pFirstX + 2] - frame[pSecondY * 3 * width + 3 * pSecondX + 2];

					ssd += value0 * value0 + value1 * value1 + value2 * value2;

					++pSecondX;
				}

				++pSecondY;
			}

			unsigned int ssdCopy = ssd;
			ssd = 0;*/

			const uint8_t* first = frame + (firstY - 3) * width3 + 3 * (firstX - 3);
			const uint8_t* second = frame + (secondY - 3) * width3 + 3 * (secondX - 3);
			const unsigned int offset = width3 - 20; // -(7 * 3 - 1)

			int value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;


			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			//ocean_assert(ssd == ssdCopy);

			break;
		}

		/// patch size 13
		case 6:
		{
			/*unsigned int pSecondY = secondY - patchHalf;

			for (unsigned int pFirstY = firstY - patchHalf; pFirstY <= firstY + patchHalf; pFirstY += 2)
			{
				unsigned int pSecondX = secondX - patchHalf;

				for (unsigned int pFirstX = firstX - patchHalf; pFirstX <= firstX + patchHalf; pFirstX += 2)
				{
					const int value0 = frame[pFirstY * 3 * width + 3 * pFirstX] - frame[pSecondY * 3 * width + 3 * pSecondX];
					const int value1 = frame[pFirstY * 3 * width + 3 * pFirstX + 1] - frame[pSecondY * 3 * width + 3 * pSecondX + 1];
					const int value2 = frame[pFirstY * 3 * width + 3 * pFirstX + 2] - frame[pSecondY * 3 * width + 3 * pSecondX + 2];

					ssd += value0 * value0 + value1 * value1 + value2 * value2;

					pSecondX += 2;
				}

				pSecondY += 2;
			}

			unsigned int ssdCopy = ssd;
			ssd = 0;*/

			const uint8_t* first = frame + (firstY - 6) * width3 + 3 * (firstX - 6);
			const uint8_t* second = frame + (secondY - 6) * width3 + 3 * (secondX - 6);
			const unsigned int offset = (width3 << 1) - 38; // -(13 * 3 - 1)

			int value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 4) - *(second += 4);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			//ocean_assert(ssd == ssdCopy);

			break;
		}

		// patch size 25
		case 12:
		{
			/*unsigned int pSecondY = secondY - patchHalf;

			for (unsigned int pFirstY = firstY - patchHalf; pFirstY <= firstY + patchHalf; pFirstY += 4)
			{
				unsigned int pSecondX = secondX - patchHalf;

				for (unsigned int pFirstX = firstX - patchHalf; pFirstX <= firstX + patchHalf; pFirstX += 4)
				{
					const int value0 = frame[pFirstY * 3 * width + 3 * pFirstX] - frame[pSecondY * 3 * width + 3 * pSecondX];
					const int value1 = frame[pFirstY * 3 * width + 3 * pFirstX + 1] - frame[pSecondY * 3 * width + 3 * pSecondX + 1];
					const int value2 = frame[pFirstY * 3 * width + 3 * pFirstX + 2] - frame[pSecondY * 3 * width + 3 * pSecondX + 2];

					ssd += value0 * value0 + value1 * value1 + value2 * value2;

					pSecondX += 4;
				}

				pSecondY += 4;
			}

			const unsigned int ssdCopy = ssd;
			ssd = 0;*/

			const uint8_t* first = frame + (firstY - 12) * width3 + 3 * (firstX - 12);
			const uint8_t* second = frame + (secondY - 12) * width3 + 3 * (secondX - 12);
			const unsigned int offset = (width3 << 2) - 74;// -(25 * 3 - 1)

			int value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 10) - *(second += 10);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			//ocean_assert(ssd == ssdCopy);
			break;
		}


		// patch size 49
		case 24:
		{
			unsigned int pSecondY = secondY - patchHalf;

			for (unsigned int pFirstY = firstY - patchHalf; pFirstY <= firstY + patchHalf; pFirstY += 8)
			{
				unsigned int pSecondX = secondX - patchHalf;

				for (unsigned int pFirstX = firstX - patchHalf; pFirstX <= firstX + patchHalf; pFirstX += 8)
				{
					const int value0 = frame[pFirstY * 3 * width + 3 * pFirstX] - frame[pSecondY * 3 * width + 3 * pSecondX];
					const int value1 = frame[pFirstY * 3 * width + 3 * pFirstX + 1] - frame[pSecondY * 3 * width + 3 * pSecondX + 1];
					const int value2 = frame[pFirstY * 3 * width + 3 * pFirstX + 2] - frame[pSecondY * 3 * width + 3 * pSecondX + 2];

					ssd += value0 * value0 + value1 * value1 + value2 * value2;

					pSecondX += 8;
				}

				pSecondY += 8;
			}

#ifdef OCEAN_DEBUG
			const unsigned int ssdCopy = ssd;
#endif
			ssd = 0;

			const uint8_t* first = frame + (firstY - 24) * width3 + 3 * (firstX - 24);
			const uint8_t* second = frame + (secondY - 24) * width3 + 3 * (secondX - 24);
			const unsigned int offset = (width3 << 3) - 156; // -(49 * 3 - 1)

			int value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			first += offset;
			second += offset;
			value = *first - *second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *(first += 22) - *(second += 22);
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;
			value = *++first - *++second;
			ssd += value * value;

			ocean_assert(ssd == ssdCopy);

			break;
		}

		default:
		{
			ssd = CV::SumSquareDifferencesBase::patch8BitPerChannel(frame, frame, 3u, patchHalf * 2 + 1, width, width, firstX, firstY, secondX, secondY, framePaddingElements, framePaddingElements);

			break;
		}
	}

	return ssd;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_IMAGE_COMPLETION_LAYER_PATCH_MASK_H
