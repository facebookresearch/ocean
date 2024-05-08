/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_FRAME_RECTIFICATION_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_FRAME_RECTIFICATION_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Box3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements a frame rectification test.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestFrameRectification
{
	public:

		/**
		 * Tests the frame rectification functions.
		 * @param width The width of the test frame in pixel, with range [1u, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the rectification function for a planar rectangle object.
		 * @param width The width of the test frame in pixel, with range [1u, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testPlanarRectangleObject(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the rectification function for an arbitrary rectangle object.
		 * @param width The width of the test frame in pixel, with range [1u, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testArbitraryRectangleObject(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the rectification function for triangles.
		 * @param width The width of the test frame in pixel, with range [1u, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testTriangleObject(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the mask rectification function for a planar rectangle object.
		 * @param width The width of the test frame in pixel, with range [1u, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testPlanarRectangleObjectMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the mask rectification function for an arbitrary rectangle object.
		 * @param width The width of the test frame in pixel, with range [1u, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testArbitraryRectangleObjectMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the mask rectification function for triangles.
		 * @param width The width of the test frame in pixel, with range [1u, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testTriangleObjectMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	protected:

		/**
		 * Determines the average color intensity error between two frames while excluding a two pixel thick border at the frame boundaries.
		 * @param frame0 The first frame to be checked, must be valid
		 * @param frame1 The second frame to be checked, must be valid
		 * @param skipColor0 Optional pixel color to skip error calculation for pixels with this color in the first frame, nullptr otherwise
		 * @param skipColor1 Optional pixel color to skip error calculation for pixels with this color in the second frame, nullptr otherwise
		 * @return The average pixel-wise color difference between both frame
		 */
		static double determineFrameError(const Frame& frame0, const Frame& frame1, const uint8_t* skipColor0, const uint8_t* skipColor1);

		/**
		 * Renders a planar rectangular object with given camera profile and camera pose.
		 * @param objectFrame The square object frame to be rendered, must be valid
		 * @param targetFrame The resulting rendered frame, must be valid
		 * @param camera The camera profile to be used for rendering
		 * @param world_T_camera The transformation between camera and world, must be valid
		 * @param objectDimension The dimension of the object frame to be rendered, with range (0, infinity)x(0, infinity)
		 * @return True, if succeeded
		 */
		static bool renderPlanarRectangleObject(const Frame& objectFrame, Frame& targetFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector2& objectDimension);

		/**
		 * Determines a camera pose so that all corners of a given bounding box are visible in the camera.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param boundingBox The 3D bounding box, must be valid
		 * @param world_R_camera The rotation of the camera to be used, must be valid
		 * @return The transformation between camera and world (world_T_camera)
		 */
		static HomogenousMatrix4 determineCameraPose(const AnyCamera& camera, const Box3& boundingBox, const Quaternion& world_R_camera);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_FRAME_RECTIFICATION_H
