// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_PYRAMID_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_PYRAMID_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"
#include "ocean/base/Frame.h"

#include "ocean/cv/FramePyramid.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the functionality of frame pyramids.
 * @see Ocean::CV::FramePyramid
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFramePyramid
{
	protected:

		/**
		 * Default value to create as many layers as possible.
		 */
		static constexpr unsigned int ALL_LAYERS = (unsigned int)(-1);

	public:

		/**
		 * Tests the functionality of frame pyramids
		 * @param testDuration Number of seconds for each test
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the ideal layers functions.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testIdealLayers(const double testDuration);

		/**
		 * Tests the isOwner() function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsOwner(const double testDuration);

		/**
		 * Tests the creation of a frame pyramid.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testCreationFramePyramidWithFrame(const double testDuration, Worker& worker);

		/**
		 * Tests the creation of a frame pyramid.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testCreationFramePyramid(const double testDuration, Worker& worker);

		/**
		 * Tests the creation of a frame pyramid with extreme parameters.
		 * @return True, if the test succeeded; otherwise, false is returned.
		 */
		static bool testCreateFramePyramidExtreme();

		/**
		 * Tests the creation of a frame pyramid for a specific frame with specific number of resulting layers.
		 * @param width The width of the test frame in pixel, width range [1, infinity)
		 * @param height The width of the test frame in pixel, height range [1, infinity)
		 * @param channels The width of the test frame in pixel, channels range [1, infinity)
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testCreationFramePyramidWithFrame(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int layers, const double testDuration, Worker& worker);

		/**
		 * Tests the creation of a frame pyramid for a specific frame with specific number of resulting layers.
		 * @param width The width of the test frame in pixel, width range [1, infinity)
		 * @param height The width of the test frame in pixel, height range [1, infinity)
		 * @param channels The width of the test frame in pixel, channels range [1, infinity)
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testCreationFramePyramid(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int layers, const double testDuration, Worker& worker);

		/**
		 * Tests the construction of a new frame pyramid from an existing frame.
		 * @param testDuration Requested duration of test loop in seconds
		 * @param worker Optional worker instance
		 * @return True, if the test succeeded; otherwise, false is returned
		 */
		static bool testConstructFromFrameMultiLayer(const double testDuration, Worker& worker);

		/**
		 * Tests the construction of a new frame pyramid from an existing frame.
		 * @param width The width of the source image in pixel, with range [1, infinity)
		 * @param height The height of the source image in pixel, with range [1, infinity)
		 * @param layerCount Number of layers to create, with range [1, infinity)
		 * @param testDuration Requested duration of test loop in seconds
		 * @param worker The worker object to distribute the computation
		 * @return True, if the test succeeded; otherwise, false is returned
		 */
		static bool testConstructFromFrameMultiLayer(const unsigned int width, const unsigned int height, const unsigned int layerCount, const double testDuration, Worker& worker);

		/**
		 * Tests the construction of a new frame pyramid from an existing source pyramid.
		 * @param testDuration Requested duration of test loop in seconds
		 * @param worker The worker object to distribute the computation
		 * @return True, if the test succeeded; otherwise, false is returned.
		 */
		static bool testConstructFromPyramid(const double testDuration, Worker& worker);

		/**
		 * Tests the construction of a new frame pyramid from an existing source pyramid.
		 * @param sourcePyramid Source frame pyramid that is used to generate the frame pyramid
		 * @param copyData True, if frame data is copied; otherwise, a reference to the frame data is created
		 * @param layerIndex Index of the first source pyramid layer that is used
		 * @param layerCount Number of layers to create
		 * @param testDuration Requested duration of test loop in seconds
		 * @param worker The worker object to distribute the computation
		 * @return True, if the test succeeded; otherwise, false is returned.
		 */
		static bool testConstructFromPyramid(const CV::FramePyramid& sourcePyramid, const bool copyData, const unsigned int  layerIndex, const unsigned int layerCount, const double testDuration, Worker& worker);

		/**
		 * Tests the replacement of a frame pyramid using a 1-1 downsampling.
		 * @param testDuration Requested duration of test loop in seconds
		 * @param worker The worker object to distribute the computation
		 * @return True, if the test succeeded; otherwise, false is returned.
		 */
		static bool testReplace11(const double testDuration, Worker& worker);

		/**
		 * Tests the reduceLayers() function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testReduceLayers(const double testDuration);

	protected:

		/**
		 * Validates the visual content of a frame pyramid.
		 * @param frame The frame from which the frame pyramid has been created, must be valid
		 * @param framePyramid The frame pyramid to be validated, must be valid
		 * @param downsamplingMode The downsampling mode that has been applied when creating the pyramid, either DM_FILTER_11 or DM_FILTER_14641
		 * @param layers The explicit number of pyramid layers to be checked, with range [1, infinity)
		 * @param allowCompatibleFrameType True, to allow a compatible frame type; False, to expect a perfectly matching frame type
		 * @return True, if succeeded
		 */
		static bool validateFramePyramid(const Frame& frame, const CV::FramePyramid& framePyramid, const CV::FramePyramid::DownsamplingMode downsamplingMode, const unsigned int layers, const bool allowCompatibleFrameType = false);

		/**
		 * Validates if the frame pyramid was created correctly with the specified settings.
		 * @param framePyramid Created frame pyramid
		 * @param frame The frame that was used to generate the frame pyramid
		 * @param copyData True, if frame data was copied; otherwise, a reference to the frame data was created
		 * @param layerCount Number of layers that were to be created
		 * @return True, if the generated frame pyramid is valid; otherwise, false is returned.
		 */
		static bool validateConstructFromFrame(const CV::FramePyramid& framePyramid, const Frame& frame, const bool copyData, const unsigned int layerCount);

		/**
		 * Validates if the frame pyramid was created correctly with the specified settings.
		 * @param framePyramid Created frame pyramid
		 * @param sourcePyramid Frame pyramid that was used as source
		 * @param copyData True, if frame data was copied; otherwise, a reference to the frame data was created
		 * @param layerIndex Index of the first source pyramid layer
		 * @param layerCount Number of layers that should have been created
		 * @return True, if the generated frame pyramid is valid; otherwise, false is returned.
		 */
		static bool validateConstructFromPyramid(const CV::FramePyramid& framePyramid, const CV::FramePyramid& sourcePyramid, const bool copyData, const unsigned int  layerIndex, const unsigned int layerCount);

		/**
		 * Returns the maximum layer count for the given frame width and height
		 * @param width Frame width in pixel
		 * @param height Frame height in pixel
		 * @return The maximum number of pyramid layers that can be generate for this frame
		 */
		static unsigned int determineMaxLayerCount(unsigned int width, unsigned int height);

		/**
		 * Verifies the ownership properties of a frame pyramid.
		 * @param framePyramid The frame pyramid to verify, can be invalid
		 * @param isValid True, if the frame pyramid is expected to be valid
		 * @param isOwner True, if the entire frame pyramid is expected to own the memory of all pyramid layers; False, if at least one layer is not owned
		 * @param layerIsOwner The explicit indices of all pyramid layers for which the pyramid is owning the memory, all other layers don't own the memory, empty to skip this check
		 * @return True, if all properties match
		 */
		static bool verifyPyramidOwnership(const CV::FramePyramid& framePyramid, const bool isValid, const bool isOwner, const Indices32& layerIsOwner = Indices32());
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_PYRAMID_H
