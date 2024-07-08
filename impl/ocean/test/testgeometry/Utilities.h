/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_UTILITIES_H
#define META_OCEAN_TEST_TESTGEOMETRY_UTILITIES_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/geometry/Estimator.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Box3.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Sphere3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements utility functions for the geometry test library.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT Utilities
{
	public:

		/**
		 * Creates a set of image points that are located inside a camera frame.
		 * @param pinholeCamera The pinhole camera profile that is applied
		 * @param number The number of the resulting image points, with range [1, infinity)
		 * @param randomGenerator Optional random generator
		 * @return Resulting 2D image points
		 */
		static Vectors2 imagePoints(const PinholeCamera& pinholeCamera, const size_t number, RandomGenerator* randomGenerator = nullptr);

		/**
		 * Creates a set of object points that are located inside a specified bounding box.
		 * @param boundingBox 3D bounding box
		 * @param number The number of the resulting object points, with range [1, infinity)
		 * @param randomGenerator Optional random generator
		 * @return Resulting 3D object points
		 */
		static Vectors3 objectPoints(const Box3& boundingBox, const size_t number, RandomGenerator* randomGenerator = nullptr);

		/**
		 * Deprecated.
		 *
		 * Determines a viewing position for a given camera for a set of 3D object points so that all point project into the camera frame.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param boundingSphere Bounding sphere that has to be visible in the final view, can have radius 0, must be valid
		 * @param viewingDirection Viewing direction of the resulting camera with unit length
		 * @return Resulting camera pose, transforming camera to world (world_T_camera)
		 */
		static HomogenousMatrix4 viewPosition(const PinholeCamera& pinholeCamera, const Sphere3& boundingSphere, const Vector3& viewingDirection);

		/**
		 * Determines a viewing position for a given camera for a set of 3D object points so that all point project into the camera frame.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param boundingSphere Bounding sphere that has to be visible in the final view, can have radius 0, must be valid
		 * @param viewingDirection Viewing direction of the resulting camera with unit length
		 * @return Resulting camera pose, transforming camera to world (world_T_camera)
		 * @tparam T The data type of the scalar to use, either 'float' or 'double'
		 */
		template <typename T>
		static HomogenousMatrixT4<T> viewPosition(const AnyCameraT<T>& anyCamera, const SphereT3<T>& boundingSphere, const VectorT3<T>& viewingDirection);

		/**
		 * Deprecated.
		 *
		 * Determines a viewing position for a given camera for a set of 3D object points so that all point project into the camera frame.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param objectPoints Object points for that the camera position will be determined
		 * @param viewingDirection Viewing direction of the resulting camera with unit length
		 * @param circumcircle True, to ensure that all object points will project into the frame, otherwise some sole points will project outside the image frame
		 * @return Resulting camera pose, transforming camera to world (world_T_camera)
		 */
		static HomogenousMatrix4 viewPosition(const PinholeCamera& pinholeCamera, const Vectors3& objectPoints, const Vector3& viewingDirection, const bool circumcircle = true);

		/**
		 * Determines a viewing position for a given camera for a set of 3D object points so that all point project into the camera frame.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param objectPoints Object points for that the camera position will be determined
		 * @param viewingDirection Viewing direction of the resulting camera with unit length
		 * @param circumcircle True, to ensure that all object points will project into the frame, otherwise some sole points will project outside the image frame
		 * @return Resulting camera pose, transforming camera to world (world_T_camera)
		 * @tparam T The data type of the scalar to use, either 'float' or 'double'
		 */
		template <typename T>
		static HomogenousMatrixT4<T> viewPosition(const AnyCameraT<T>& anyCamera, const VectorsT3<T>& objectPoints, const VectorT3<T>& viewingDirection, const bool circumcircle = true);

		/**
		 * Deprecated.
		 *
		 * Determines a viewing position for a given camera for a set of 3D object points so that all point project into the camera frame.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param objectPoints Object points for that the camera position will be determined
		 * @param circumcircle True, to ensure that all object points will project into the frame, otherwise some sole points will project outside the image frame
		 * @param randomGenerator Optional random generator
		 * @return Resulting camera pose, transforming camera to world (world_T_camera)
		 */
		static HomogenousMatrix4 viewPosition(const PinholeCamera& pinholeCamera, const Vectors3& objectPoints, const bool circumcircle = true, RandomGenerator* randomGenerator = nullptr);

		/**
		 * Determines a viewing position for a given camera for a set of 3D object points so that all point project into the camera frame.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param objectPoints Object points for that the camera position will be determined
		 * @param circumcircle True, to ensure that all object points will project into the frame, otherwise some sole points will project outside the image frame
		 * @param randomGenerator Optional random generator
		 * @return Resulting camera pose, transforming camera to world (world_T_camera)
		 * @tparam T The data type of the scalar to use, either 'float' or 'double'
		 */
		template <typename T>
		static HomogenousMatrixT4<T> viewPosition(const AnyCameraT<T>& anyCamera, const VectorsT3<T>& objectPoints, const bool circumcircle = true, RandomGenerator* randomGenerator = nullptr);

		/**
		 * Modifies a provided camera and adds random radial and tangential distortion.
		 * @param pinholeCamera The pinhole camera object that will be modified
		 * @param principalPointDistortion True, to modify the principal point
		 * @param radialDistortion True, to modify the radial distortion
		 * @param tangentialDistortion True, to modify the tangential distortion
		 * @param randomGenerator Optional random generator, nullptr to use a default random generator
		 * @param maximalPrincipalPointOffset Maximal offset of the principal point, with range [0, infinity)
		 * @param maximalRadialDistortion Maximal radial distortion, with range [0, infinity)
		 * @param maximalTangentialDistortion Maximal tangential distortion, with range [0, infinity)
		 * @return Resulting modified camera
		 */
		static PinholeCamera distortedCamera(const PinholeCamera& pinholeCamera, const bool principalPointDistortion = true, const bool radialDistortion = true, const bool tangentialDistortion = true, RandomGenerator* randomGenerator = nullptr, const Scalar maximalPrincipalPointOffset = Scalar(20), const Scalar maximalRadialDistortion = Scalar(0.1), const Scalar maximalTangentialDistortion = Scalar(0.01));

		/**
		 * Returns one of two realistic pinhole camera profiles.
		 * @param index The index of the camera profile to be returned, with range [0, 1]
		 * @return The realistic camera profile
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T = Scalar>
		static PinholeCameraT<T> realisticPinholeCamera(const unsigned int index = 0u);

		/**
		 * Returns one of two realistic fisheye camera profiles.
		 * @param index The index of the camera profile to be returned, with range [0, 1]
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 * @return The realistic camera profile
		 */
		template <typename T = Scalar>
		static FisheyeCameraT<T> realisticFisheyeCamera(const unsigned int index = 0u);

		/**
		 * Returns one of two realistic camera profiles provided as an AnyCamera object.
		 * @param anyCameraType The type of the camera to be created, must be valid
		 * @param index The index of the camera profile to be returned, with range [0, 1]
		 * @return The realistic camera profile
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T = Scalar>
		static SharedAnyCameraT<T> realisticAnyCamera(const AnyCameraType anyCameraType, const unsigned int index = 0u);

		/**
		 * Returns all camera types for which a realistic camera object can be accessed.
		 * @return The available camera types
		 */
		static std::vector<AnyCameraType> realisticCameraTypes();

		/**
		 * Creates a set of random indices.
		 * @param size The size of the entire element, with range [1, infinity)
		 * @param randomIndices Number of random indices, with range [1, size]
		 * @param randomGenerator Optional random generator
		 */
		static IndexSet32 randomIndices(const size_t size, const size_t randomIndices, RandomGenerator* randomGenerator = nullptr);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_UTILITIES_H
