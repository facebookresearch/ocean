/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestGeometry.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Messenger.h"

#include "ocean/math/Euler.h"
#include "ocean/math/ExponentialMap.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line3.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Sphere3.h"
#include "ocean/math/Vector3.h"

#include <opencv2/calib3d.hpp>

#include <numeric>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

typedef cv::Point3f ObjectPoint;
typedef cv::Point2f ImagePoint;
typedef std::vector<ObjectPoint> ObjectPoints;
typedef std::vector<cv::Point2f> ImagePoints;

namespace
{

// Clone of Ocean::Test::TestGeometry::Utilities::viewPosition()
HomogenousMatrix4 viewPosition(const PinholeCamera& camera, const Sphere3& boundingSphere, const Vector3& viewingDirection)
{
	ocean_assert(camera.isValid());
	ocean_assert(boundingSphere.isValid());

	const Scalar fov_2 = min(min(camera.fovXLeft(), camera.fovXRight()), min(camera.fovYTop(), camera.fovYBottom()));
	ocean_assert(fov_2 >= 0);

	const Scalar sideDistance = boundingSphere.radius() / Numeric::sin(fov_2);
	const Scalar viewDistance = Numeric::sqrt(Numeric::sqr(sideDistance) + Numeric::sqr(boundingSphere.radius()));

	ocean_assert(Numeric::isEqual(viewingDirection.length(), 1));

	const Vector3 newViewPosition = boundingSphere.center() - viewingDirection * viewDistance;
	const HomogenousMatrix4 pose(newViewPosition, Rotation(Vector3(0, 0, -1), viewingDirection));

	return pose;
}

// Clone of Ocean::Test::TestGeometry::Utilities::viewPosition()
HomogenousMatrix4 viewPosition(const PinholeCamera& camera, const Vectors3& objectPoints, const Vector3& viewingDirection, const bool circumcircle = true)
{
	ocean_assert(camera.isValid());
	ocean_assert(!objectPoints.empty());

	const Box3 boundingBox(objectPoints);

	const Sphere3 boundingSphere(circumcircle ? Sphere3(boundingBox) : Sphere3(boundingBox.center(), max(boundingBox.xDimension(), max(boundingBox.yDimension(), boundingBox.zDimension())) * Scalar(0.75)));

	return viewPosition(camera, boundingSphere, viewingDirection);
}

/**
 * Compute the distance between two (N-dimensional) points
 *
 * @note Point should only be used with cv::Point2? and cv::Point3?
 *
 * @todo Add a type traits check that @c PointType has a function dot()
 *
 * @param p0   The first point
 * @param p1   The second point
 *
 * @return The distance between the first and the second point.
 */
template <typename PointType>
Scalar computeDistance(const PointType& p0, const PointType& p1)
{
	PointType diff = p0 - p1;
	return std::sqrt(static_cast<Scalar>(diff.dot(diff)));
}

/**
 * Convert Ocean vectors into OpenCV points (object points, i.e. 3D)
 *
 * @param imagePoints  The Ocean vectors to be converted to OpenCV points
 *
 * @return The converted vectors
 */
inline ObjectPoints toOpenCV(const Vectors3& objectPoints)
{
	const size_t pointCount = objectPoints.size();
	ObjectPoints op(pointCount);

	for (size_t i = 0; i < pointCount; ++i)
	{
		// Note: OpeCV uses a right-handed coordinate system flipped around the x-axis (if), cf.
		// http://docs.opencv.org/3.2.0/d9/d0c/group__calib3d.html#details
		op[i] = ObjectPoint(static_cast<float>(objectPoints[i].x()), static_cast<float>(-objectPoints[i].y()), static_cast<float>(-objectPoints[i].z()));
	}

	return op;
}

/**
 * Convert Ocean vectors into OpenCV points (image points, i.e. 2D)
 *
 * @param imagePoints  The Ocean vectors to be converted to OpenCV points
 *
 * @return The converted vectors
 */
inline ImagePoints toOpenCV(const Vectors2& imagePoints)
{
	const size_t pointCount = imagePoints.size();
	ImagePoints ip(pointCount);

	for (size_t i = 0; i < pointCount; ++i)
	{
		ip[i] = ImagePoint(static_cast<float>(imagePoints[i].x()), static_cast<float>(imagePoints[i].y()));
	}

	return ip;
}

/**
 * Generation of randomized data for the PnP tests
 *
 * @param cvObjectPoints  Randomized object points (output)
 * @param cvImagePoints   Corresponding image points (output)
 * @param cvRotation      The random rotation vector of the camera (exponential map, output) used to generate the image points
 * @param numberOfPoints  The number of points to be generated (must be >= 3)
 * @param camera          The definition camera (to be as close to the Ocean P3P/PnP tests, the @c Ocean::PinholeCamera is used from which the necessary information is extracted for OpenCV)
 *
 * @return True if the random data has been successfully generated, otherwise false
 */
bool generateRandomizedPnPData(ObjectPoints& cvObjectPoints, ImagePoints& cvImagePoints, cv::Vec3d& cvRotation, const size_t numberOfPoints, const PinholeCamera& camera)
{
	ocean_assert(camera.isValid());

	Vectors3 objectPoints;
	Vectors2 imagePoints;
	HomogenousMatrix4 groundtruthPose;

	if (numberOfPoints < 3u)
	{
		return false;
	}
	if (numberOfPoints == 3u)
	{
		// Generate 3 random object points inside a unit circle, that are not colinear
		objectPoints.resize(3);
		imagePoints.resize(3);
		const Scalar minimumDistance(0.01);

		bool imagePointsColinear = false;

		do
		{
			objectPoints[0] = Vector3(Random::scalar(-1, 1), 0, Random::scalar(-1, 1));
			objectPoints[1] = Vector3(Random::scalar(-1, 1), 0, Random::scalar(-1, 1));

			while (objectPoints[0].distance(objectPoints[1]) < minimumDistance)
				objectPoints[1] = Vector3(Random::scalar(-1, 1), 0, Random::scalar(-1, 1));

			objectPoints[2] = Vector3(Random::scalar(-1, 1), 0, Random::scalar(-1, 1));

			while (Line3(objectPoints[0], (objectPoints[1] - objectPoints[0]).normalized()).distance(objectPoints[2]) < minimumDistance)
				objectPoints[2] = Vector3(Random::scalar(-1, 1), 0, Random::scalar(-1, 1));

			const Euler euler(Random::euler(Numeric::deg2rad(0), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);
			groundtruthPose = HomogenousMatrix4(viewPosition(camera, objectPoints, quaternion * Vector3(0, -1, 0)));

			// determine the perfectly projected image points

			camera.projectToImage<true>(groundtruthPose, objectPoints.data(), 3, true, imagePoints.data());

			for (unsigned int n = 0u; n < 3u; ++n)
			{
				ocean_assert(camera.isInside(imagePoints[n]));

				const unsigned int n1 = (n + 1u) % 3u;
				const unsigned int n2 = (n + 2u) % 3u;

				const Line2 line(imagePoints[n], (imagePoints[n1] - imagePoints[n]).normalized());

				if (line.distance(imagePoints[n2]) < 5)
					imagePointsColinear = true;
			}

		}
		while (imagePointsColinear);
	}
	else if (numberOfPoints == 4u)
	{
		// Generate 4 random in the image plane and back-project them into 3D space by using rays from the projection center through the image points. The 3D points are
		// placed at the intersection with the xz-plane.
		const Plane3 planeY(Vector3(0, 0, 0), Vector3(0, 1, 0));

		const Vector3 randomTranslation(Random::scalar(-1, 1), Random::scalar(Scalar(0.1), 1), Random::scalar(-1, 1));
		const Euler randomEuler(Random::scalar(-Numeric::pi_2(), Numeric::pi_2()), Random::scalar(Numeric::deg2rad(-85), Numeric::deg2rad(-50)), Random::scalar(-Numeric::pi_2(), Numeric::pi_2()));
		groundtruthPose = HomogenousMatrix4(randomTranslation, randomEuler);

		objectPoints.resize(numberOfPoints);
		imagePoints.resize(numberOfPoints);

		for (size_t n = 0u; n < numberOfPoints; ++n)
		{
			imagePoints[n] = Vector2(Random::scalar(0, Scalar(camera.width() - 1u)), Random::scalar(0, Scalar(camera.height() - 1u)));

			const Line3 ray = camera.ray(imagePoints[n], groundtruthPose);
			Vector3 objectPoint(0, 0, 0);

			const bool result = planeY.intersection(ray, objectPoint);
			ocean_assert_and_suppress_unused(result, result);

			objectPoints[n] = objectPoint;
		}
	}
	else if (numberOfPoints > 4u)
	{
		// Generate N random in the image plane and back-project them into 3D space by using rays from the projection center through the image points. The
		// points are projected such that they are (somewhere) in front of the camera (distanceToObject).
		const Vector3 randomTranslation(Random::scalar(-1, 1), Random::scalar(Scalar(0.1), 1), Random::scalar(-1, 1));
		const Euler randomEuler(Random::scalar(-Numeric::pi_2(), Numeric::pi_2()), Random::scalar(Numeric::deg2rad(-85), Numeric::deg2rad(-50)), Random::scalar(-Numeric::pi_2(), Numeric::pi_2()));
		groundtruthPose = HomogenousMatrix4(randomTranslation, randomEuler);

		for (unsigned int n = 0; n < numberOfPoints; ++n)
		{
			imagePoints.push_back(Vector2(Random::scalar(0, Scalar(camera.width() - 1u)), Random::scalar(0, Scalar(camera.height() - 1u))));

			const Vector3 direction = groundtruthPose.rotationMatrix() * camera.vector(imagePoints[n]);
			ocean_assert(Numeric::isEqual(direction.length(), 1));

			ocean_assert(Numeric::isNotEqualEps(direction.y()));
			const Scalar distanceToObject = -randomTranslation.y() / direction.y();
			ocean_assert(distanceToObject > 0);

			objectPoints.push_back(randomTranslation + direction * distanceToObject);
			ocean_assert(Numeric::isEqualEps(objectPoints[n].y()));
		}
	}

	// Convert the image and object points into a "OpenCV-format"
	cvObjectPoints = toOpenCV(objectPoints);
	cvImagePoints  = toOpenCV(imagePoints);

	// Extract and convert the rotation and translation of the camera from the groundtruth pose
	ocean_assert(groundtruthPose.isValid());
	if (!groundtruthPose.isValid())
		return false;

	const ExponentialMap rotationEM(groundtruthPose.rotation());
	const Vector3 axis = rotationEM.axis().normalized();
	cvRotation         = cv::Vec3d(axis.x(), axis.y(), axis.z());

	const Vector3 translation = groundtruthPose.translation();
	cvRotation                = cv::Vec3d(translation.x(), translation.y(), translation.z());

	return true;
}

/**
 * Computation of the projection error of the OpenCV PnP solution
 *
 * @param objectPoints  The groundtruth object points
 * @param imagePoints   The groundtruth image points
 * @param cameraMatrix  The camera matrix, must be 3x3
 * @param rotation      The rotation vector (exponential map)
 * @param translation   The 3D translation vector
 * @param distCoeff     Distortion coefficients (default: 0 == no distortion)
 *
 * @return The sum of distances of corresponding image points (true and estimated ones)
 */
Scalar computeProjectionError(const ObjectPoints& objectPoints, const ImagePoints& imagePoints, const cv::Mat1f& cameraMatrix, const cv::Vec3d& rotation, const cv::Vec3d& translation, const cv::Mat distCoeff = cv::Mat())
{
	ImagePoints projectedPoints;
	cv::projectPoints(objectPoints, rotation, translation, cameraMatrix, distCoeff, projectedPoints);

	Scalar error(0);
	ocean_assert(imagePoints.size() == projectedPoints.size());
	for (ImagePoints::const_iterator iter0 = imagePoints.begin(), iter1 = projectedPoints.begin(); iter0 != imagePoints.end() && iter1 != projectedPoints.end(); ++iter0, ++iter1)
		error += computeDistance(*iter0, *iter1);

	return error;
}

/**
 * Extract the camera matrix from a camera (Ocean)
 *
 * @param camera  An instance of a camera from the Ocean framework
 *
 * @return A 3x3 camera matrix
 */
cv::Mat1f extractCameraMatrix(const Ocean::PinholeCamera& camera)
{
	const Scalar fx              = camera.focalLengthX();
	const Scalar fy              = camera.focalLengthY();
	const Scalar cx              = camera.principalPointX();
	const Scalar cy              = camera.principalPointY();
	const cv::Mat1f cameraMatrix = (cv::Mat1f(3, 3) << fx, 0.f, cx, 0.f, fy, cy, 0.f, 0.f, 1.f);

	return cameraMatrix;
}

/**
 * Helper functor for the OpenCV PnP function
 *
 * This functions helps to avoid having multiple bodies of test functions with 99% identical code.
 *
 * @sa template <typename PnPFunctor> bool runTestPnP(const double testDuration, const PinholeCamera& camera, const size_t numberOfPoints)
 */
struct OpenCVPnP
{
	virtual ~OpenCVPnP() = default;

	/**
	 * Wrapper of the actual OpenCV function to solve the PnP problem
	 *
	 * @param objectPoints  Observations of the object points
	 * @param imagePoints   Observations of the projections of the object points
	 * @param cameraMatrix  A camera matrix, must be 3x3
	 * @param distCoeffs    Distortion coefficients (in OpenCV format)
	 * @param rvec          Estimated rotation of the camera in 3D (exponential map)
	 * @param tvec          Estimated translation of the camera in 3D
	 *
	 */
	inline virtual bool solve(cv::InputArray objectPoints, cv::InputArray imagePoints, cv::InputArray cameraMatrix, cv::InputArray distCoeffs, cv::OutputArray rvec, cv::OutputArray tvec) const
	{
		return cv::solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);
	}

	/**
	 * Returns a simple identifier to be displayed along with the results
	 *
	 * @return The identifier/name of the PnP variant
	 */
	inline virtual std::string name() const
	{
		return std::string("OpenCV PnP");
	}
};

/**
 * Helper functor for the OpenCV PnP function (with RANSAC)
 *
 * This functions helps to avoid having multiple bodies of test functions with 99% identical code.
 *
 * @sa template <typename PnPFunctor> bool runTestPnP(const double testDuration, const PinholeCamera& camera, const size_t numberOfPoints)
 */
struct OpenCVPnPRansac : public OpenCVPnP
{
	/**
	 * Wrapper of the actual OpenCV function to solve the PnP problem
	 *
	 * @param objectPoints  Observations of the object points
	 * @param imagePoints   Observations of the projections of the object points
	 * @param cameraMatrix  A camera matrix, must be 3x3
	 * @param distCoeffs    Distortion coefficients (in OpenCV format)
	 * @param rvec          Estimated rotation of the camera in 3D (exponential map)
	 * @param tvec          Estimated translation of the camera in 3D
	 *
	 */
	inline bool solve(cv::InputArray objectPoints, cv::InputArray imagePoints, cv::InputArray cameraMatrix, cv::InputArray distCoeffs, cv::OutputArray rvec, cv::OutputArray tvec) const override
	{
		return cv::solvePnPRansac(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);
	}

	/**
	 * Returns a simple identifier to be displayed along with the results
	 *
	 * @return The identifier/name of the PnP variant
	 */
	inline std::string name() const override
	{
		return std::string("OpenCV PnP (RANSAC)");
	}
};

/**
 * Test function for the OpenCV PnP solvers
 *
 * @param testDuration    Duration of this test in seconds
 * @param camera          A camera model; all information required by OpenCV will be extracted from this model
 * @param numberOfPoints  Number of random points to be generated and used for this test (must be >= 3)
 *
 * @tparam PnPFunction
 *
 * @sa struct OpenCVPnp
 * @sa struct OpenCVPnpRansac
 *
 * @return True if the test succeeded, otherwise false
 */
template <typename PnPFunctor>
bool runTestPnP(const double testDuration, const PinholeCamera& camera, const size_t numberOfPoints)
{
	ocean_assert(numberOfPoints >= 3);

	if (camera.hasDistortionParameters())
	{
		Log::error() << "In test " << TestGeometry::testName() << " the Ocean camera uses distortion, which - at the time of writing - isn't translateable into to the OpenCV format. Please switch it off or implement the conversion. Currently, this test is likely to fail.";
		return false;
	}

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);
	const cv::Mat1f cameraMatrix = extractCameraMatrix(camera);
	const cv::Mat1f distortionCoeffs; // Intentionally left empty, b/c we'll assume a camera w/o distortion.
	PnPFunctor pnp;

	std::vector<Scalar> projectionErrors;
	size_t succeeded  = 0;
	size_t iterations = 0;

	do
	{
		ObjectPoints objectPoints;
		ImagePoints imagePoints;
		cv::Vec3d trueRotation;
		cv::Vec3d estimatedRotation;
		cv::Vec3d estimatedTranslation;

		bool generationSuccessful = generateRandomizedPnPData(objectPoints, imagePoints, trueRotation, numberOfPoints, camera);

		if (!generationSuccessful)
			continue;

		if (numberOfPoints != objectPoints.size() || numberOfPoints != imagePoints.size())
		{
			Log::warning() << "Wrong number of image/object points";
			return false;
		}

		performance.start();
		pnp.solve(objectPoints, imagePoints, cameraMatrix, cv::noArray(), estimatedRotation, estimatedTranslation);
		performance.stop();

		Scalar error = computeProjectionError(objectPoints, imagePoints, cameraMatrix, estimatedRotation, estimatedTranslation);
		projectionErrors.push_back(error);

		if (error < Scalar(0.9))
			succeeded++;

		iterations++;
	} while (startTimestamp + testDuration > Timestamp(true));

	std::sort(projectionErrors.begin(), projectionErrors.end());

	ocean_assert(iterations != 0ull);
	const double percent = static_cast<double>(succeeded) / static_cast<double>(iterations);

	Log::info() << pnp.name() << " with n = " << numberOfPoints << " points";
	Log::info() << "";
	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "Iterations: " << iterations;
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	Log::info() << "Projection errors: ";
	Log::info() << "   min: " << projectionErrors[0];
	Log::info() << "   max: " << projectionErrors.back();
	Log::info() << "   avg: " << (std::accumulate(projectionErrors.begin(), projectionErrors.end(), Scalar(0.f)) / Scalar(projectionErrors.size()));
	Log::info() << "   med: " << projectionErrors[projectionErrors.size() / 2];

	bool testSucceeded = false;
	if (std::is_same<Scalar, float>::value)
	{
		testSucceeded = (percent >= 0.85);
	}
	else
	{
		testSucceeded = (percent >= 0.95);
	}

	Log::info() << "Test " << (testSucceeded ? "succeeded" : "FAILED");

	return testSucceeded;
}

} // namespace

const std::string& TestGeometry::testName()
{
	static const std::string testName = "OpenCV calib3d/geometry tests";
	return testName;
}

bool TestGeometry::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "--- " << testName() << " ---";
	Log::info() << "  ";

	bool allSucceeded = true;

	const PinholeCamera goodCamera(640, 480, Numeric::deg2rad(58));
	const PinholeCamera badCamera(640, 480, goodCamera.focalLengthX(), goodCamera.focalLengthY(), 300, 200); // Bad principal point

	allSucceeded = testPnP(testDuration, goodCamera, 3) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, badCamera, 3) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, goodCamera, 4) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, badCamera, 4) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, goodCamera, 5) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, badCamera, 5) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, goodCamera, 10) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, badCamera, 10) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, goodCamera, 20) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, badCamera, 20) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, goodCamera, 30) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnP(testDuration, badCamera, 30) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnPRANSAC(testDuration, goodCamera, 3) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnPRANSAC(testDuration, goodCamera, 4) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnPRANSAC(testDuration, goodCamera, 5) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnPRANSAC(testDuration, goodCamera, 10) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnPRANSAC(testDuration, goodCamera, 20) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPnPRANSAC(testDuration, goodCamera, 30) && allSucceeded;

	Log::info() << " ";

	Log::info() << testName() << (allSucceeded ? " succeeded" : " FAILED!") << ".";

	return allSucceeded;
}

bool TestGeometry::testPnP(const double testDuration, const PinholeCamera& camera, const size_t numberOfPoints)
{
	return runTestPnP<OpenCVPnP>(testDuration, camera, numberOfPoints);
}

bool TestGeometry::testPnPRANSAC(const double testDuration, const PinholeCamera & camera, const size_t numberOfPoints)
{
	return runTestPnP<OpenCVPnPRansac>(testDuration, camera, numberOfPoints);
}

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean
