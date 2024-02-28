// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FREAK_DESCRIPTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FREAK_DESCRIPTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

/// Forward-declaration of the statistics class
class HighPerformanceStatistic;

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/// Forward-declaration of the test class for FREAK descriptors
template <size_t tSize>
class TestFREAKDescriptorT;

/// Typedef for the test class of the 32-bytes long FREAK descriptors
typedef TestFREAKDescriptorT<32> TestFREAKDescriptor32;

/// Typedef for the test class of the 64-bytes long FREAK descriptors
typedef TestFREAKDescriptorT<64> TestFREAKDescriptor64;

/**
 * This class implements a test for the FREAK descriptor.
 * @ingroup testcvdetector
 */
template <size_t tSize>
class OCEAN_TEST_CV_DETECTOR_EXPORT TestFREAKDescriptorT : public CV::Detector::FREAKDescriptorT<tSize>
{
	public:

		/**
		 * Invokes all test for the FREAK descriptor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Test for the computation of FREAK descriptors on synthetic data for single points
		 * @param testDuration Number of seconds that this test will be run, range: (0, infinity)
		 * @param width The width of the synthetic images that will be used for testing, range: [1, infinity)
		 * @param height The height of the synthetic images that will be used for testing, range: [1, infinity)
		 * @param hammingDistanceThreshold Maximum  allowed Hamming distance between descriptors generated with the original implementation and those from Ocean, range: [0, infinity)
		 * @param worker The worker object
		 * @return True if the test has passed, otherwise false
		 */
		static bool testComputeDescriptor(const double testDuration, const unsigned int width, const unsigned int height, const unsigned int hammingDistanceThreshold, Worker& worker);

		/**
		 * Test for the computation of FREAK descriptors on synthetic data for vectors of points
		 * @param testDuration Number of seconds that this test will be run, range: (0, infinity)
		 * @param width The width of the synthetic images that will be used for testing, range: [1, infinity)
		 * @param height The height of the synthetic images that will be used for testing, range: [1, infinity)
		 * @param maxAllowedHammingDistance Maximum  allowed Hamming distance between descriptors generated with the original implementation and those from Ocean, range: [0, infinity)
		 * @param worker The worker object
		 * @return True if the test has passed, otherwise false
		 */
		static bool testComputeDescriptors(const double testDuration, const unsigned int width, const unsigned int height, const unsigned int maxAllowedHammingDistance, Worker& worker);

	protected:

		/**
		 * Compute a FREAK descriptor for a single point
		 * This is a wrapper for the original FRL-FREAK implementation
		 * @param pyramid Frame pyramid in which the location `point` has been defined, must be valid
		 * @param point Point defined at level `pointPyramidLevel` in `framePyramid` for which a descriptor will be computed, must be valid
		 * @param pointPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, framePyramid.layers() - 1)
		 * @param freakDescriptor The FREAK descriptor that will be computed for the input point, will be valid only if this function returns true
		 * @param unprojectRay This is the 3D vector that connects the projection center of the camera with image point `point` in the image plane, must be valid
		 * @param inverseFocalLengthX The inverse focal length (assumes identical vertical and horizontal focal lengths), range: (0, infinity)
		 * @param pointJacobianMatrix2x3 The 2-by-3 Jacobian of the camera projection matrix, cf. `Geometry::Jacobian::calculatePointJacobian2x3()`, must be valid
		 * @return True if the descriptor was successfully computed, otherwise false
		 */
		static bool computeOriginalDescriptor(const CV::FramePyramid& pyramid, const Eigen::Vector2f& point, const unsigned int pointPyramidLevel, CV::Detector::FREAKDescriptorT<tSize>& freakDescriptor, const Eigen::Vector3f& unprojectRay, const float inverseFocalLengthX, const Eigen::Matrix<float, 2, 3>& pointJacobianMatrix2x3);

		/**
		 * Compute a FREAK descriptor for a vector of points
		 * This is a wrapper for the original FRL-FREAK implementation
		 * @param pyramid Frame pyramid in which the location `point` has been defined, must be valid
		 * @param points The 2D image points defined at level `pointPyramidLevel` in `framePyramid` for which descriptors will be computed, must be valid and non-empty
		 * @param pointPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, framePyramid.layers() - 1)
		 * @param freakDescriptors The FREAK descriptors that will be computed for each of the input points. Elements may be invalid if the corresponding points are too close to the image border. Will be initialized to the same number of elements as `points`
		 * @param inverseFocalLength The inverse focal length (assumes identical vertical and horizontal focal lengths), range: (0, infinity)
		 * @param cameraDerivativeFunctor A functor that is called for each input point and which must return its corresponding 2x3 Jacobian of the projection matrix and normalized unprojection ray
		 */
		static void computeOriginalDescriptors(const CV::FramePyramid& pyramid, const std::vector<Eigen::Vector2f>& points, const unsigned int pointPyramidLevel, std::vector<CV::Detector::FREAKDescriptorT<tSize>>& freakDescriptors, const float inverseFocalLength, const typename CV::Detector::FREAKDescriptorT<tSize>::CameraDerivativeFunctor& cameraDerivativeFunctor);

		/**
		 * Validation of the computation of FREAK descriptors
		 * @param descriptor0 The first descriptor that will be validated
		 * @param descriptor1 The second descriptor that will be validated
		 * @param maxHammingDistanceThreshold The maximum allowed Hamming distance (per level of the multi-level descriptor) before the validation is considered  to have failed, range: [0, infinity), default: 1
		 * @param hammingDistances The distances at each level measured between the first and the second input descriptor; will be ignored if not specified
		 * @return True if the validation was successful, otherwise false
		 */
		static bool validateFREAKDescriptor(const CV::Detector::FREAKDescriptorT<tSize>& descriptor0, const CV::Detector::FREAKDescriptorT<tSize>& descriptor1, const unsigned int maxHammingDistanceThreshold = 1u, unsigned int* hammingDistances = nullptr);
};

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namepace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FREAK_DESCRIPTOR_H
