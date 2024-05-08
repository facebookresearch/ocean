/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_INITIALIZER_I_1_H
#define META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_INITIALIZER_I_1_H

#include "ocean/test/testcv/testsynthesis/TestCVSynthesis.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/synthesis/MappingI1.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

/**
 * This class implements a test for initializers with one frame.
 * @ingroup testcvsynthesis
 */
class OCEAN_TEST_CV_SYNTHESIS_EXPORT TestInitializerI1
{
	protected:

		/**
		 * Helper function allowing to keep properties of a pixel to be inpainted.
		 */
		class OCEAN_TEST_CV_SYNTHESIS_EXPORT InpaintingPixel final : public CV::PixelPosition
		{
			public:

				/**
				 * Creates a new pixel.
				 * @param pixelPosition The position of the pixel in the frame, with range [0, mask.width() - 1]x[0, mask.height() - 1]
				 * @param mask The mask identifying valid pixels, must be valid
				 * @param sobel The sobel responses of the frame to be inpainted, must be valid
				 */
				InpaintingPixel(const CV::PixelPosition& pixelPosition, const Frame& mask, const Frame& sobel);

				/**
				 * Returns the border direction of this pixel.
				 * @return The pixel's border direction
				 */
				const VectorI2& borderDirection() const;

				/**
				 * Returns whether this pixel has a lower priority of a second pixel.
				 * @param inpaintingPixel The second inpainting pixel to compare
				 * @return True, if so
				 */
				bool operator<(const InpaintingPixel& inpaintingPixel) const;

			protected:

				/**
				 * Determines the border direction of this pixel.
				 * @param mask The mask identifying valid pixels, must be valid
				 * @return The resulting border direction
				 */
				VectorI2 determineBorderDirection(const Frame& mask) const;

				/**
				 * Determines the image orientation of this pixel.
				 * @param mask The mask identifying valid pixels, must be valid
				 * @param sobel The sobel responses of the frame to be inpainted, must be valid
				 * @return The resulting image orientation
				 */
				VectorI2 determineImageOrientation(const Frame& mask, const Frame& sobel) const;

			protected:

				/// The pixel's border direction.
				VectorI2 borderDirection_ = VectorI2(0, 0);

				/// The image orientation.
				VectorI2 imageOrientation_ = VectorI2(0, 0);

				/// The pixel's priority.
				unsigned int priority_ = 0u;
		};

		/**
		 * Definition of a vector holding inpainting pixels.
		 */
		using InpaintingPixels = std::vector<InpaintingPixel>;

	public:

		/**
		 * Invokes all test functions.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the area constrained appearance mapping initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAppearanceMappingAreaConstrained(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the area constrained appearance mapping initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAppearanceMappingAreaConstrained(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the appearance mapping initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAppearanceMapping(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the appearance mapping initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testAppearanceMapping(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the coarser mapping adaption initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testCoarserMappingAdaption(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the coarser mapping adaption initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testCoarserMappingAdaption(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the area constrained coarser mapping adaption initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testCoarserMappingAdaptionAreaConstrained(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the area constrained coarser mapping adaption initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testCoarserMappingAdaptionAreaConstrained(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the spatial cost mask coarser mapping adaption initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testCoarserMappingAdaptionSpatialCostMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the spatial cost mask coarser mapping adaption initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testCoarserMappingAdaptionSpatialCostMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the random mapping initializer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testRandomMapping(const double testDuration, Worker& worker);

		/**
		 * Tests the area constrained random mapping initializer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testRandomMappingAreaConstrained(const double testDuration, Worker& worker);

		/**
		 * Tests the deterministic erosion shrinking initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testShrinkingErosion(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the deterministic erosion shrinking initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testShrinkingErosion(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the randomized erosion shrinking initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testShrinkingErosionRandomized(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the randomized erosion shrinking initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testShrinkingErosionRandomized(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the patch matching shrinking initializer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testShrinkingPatchMatching(const double testDuration, Worker& worker);

		/**
		 * Tests the patch matching shrinking initializer.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testShrinkingPatchMatching(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

	protected:

		/**
		 * Invokes one shrinking iteration.
		 * @param frame The frame to which the shrinking iteration will be applied, will be modified, must be valid
		 * @param mask The mask identifying valid pixels, will be modified, must be valid if 'useMaskForSSD == true'
		 * @param mapping The current mapping, will be modified
		 * @param useHeuristic True, to use a heuristic during shrinking; False, to skip the heuristic
		 * @param maximalRadius The maximal search radius, in pixels, with range [1, infinity)
		 * @param randomGenerator The random generator to use
		 * @param useMaskForSSD True, to use the mask during SSD calculation
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool shrinkPatchMatchingIteration(Frame& frame, Frame& mask, CV::Synthesis::MappingI1& mapping, const bool useHeuristic, const unsigned int maximalRadius, RandomGenerator& randomGenerator, const bool useMaskForSSD, Worker& worker);

		/**
		 * Determines the SSD between two image patches.
		 * @param frame The frame for which the ssd will be determined, must be valid
		 * @param mask The mask identifying valid pixels, must be valid if 'useMaskForSSD == true'
		 * @param patchSize The size of the image patch, with range [1, infinity), must be odd
		 * @param inpaintingPosition The target position which should have an invalid pixel
		 * @param sourcePosition The source position which should have a valid pixel
		 * @param useMaskForSSD True, to use the mask; False, to ignore the mask
		 * @return The resulting SSD, -1 if the SSD could not be determined
		 */
		static unsigned int determineSSD(const Frame& frame, const Frame& mask, const unsigned int patchSize, const CV::PixelPosition& inpaintingPosition, const CV::PixelPosition& sourcePosition, const bool useMaskForSSD);

		/**
		 * Returns whether all mask pixels in a neighborhood have a specific value.
		 * @param mask The mask to be checked, must be valid
		 * @param x The horizontal center location, with range [0, mask.width() - 1]
		 * @param y The vertical center location, with range [0, mask.height() - 1]
		 * @param value The value to check, with range [0, infinity)
		 * @param neighborhood The size of the neighborhood, either 1 or 9
		 * @return True, if so
		 */
		static bool allValueSame(const Frame& mask, const unsigned int x, const unsigned int y, const uint8_t value, const unsigned int neighborhood);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_INITIALIZER_I_1_H
