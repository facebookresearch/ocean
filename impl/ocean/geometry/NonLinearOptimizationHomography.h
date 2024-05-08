/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_HOMOGRAPHY_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_HOMOGRAPHY_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements non linear optimization algorithms for homographies.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimizationHomography : protected NonLinearOptimization
{
	public:

		/**
		 * Definition of a pair holding to sets of corresponding image points.
		 */
		typedef std::pair<Vectors2, Vectors2> ImagePointsPair;

		/**
		 * Definition of a vector holding pairs of corresponding image points.
		 */
		typedef std::vector<ImagePointsPair> ImagePointsPairs;

	protected:

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION
		/**
		 * Forward declaration of a class implementing a data object allowing to optimize a homography.
		 */
		class HomographyData;
#endif

		/**
		 * Forward declaration of a class implementing a provider allowing to optimize a homography matrix.
		 */
		class HomographyOptimizationProvider;

		/**
		 * Forward declaration of a class implementing a provider allowing to optimize a similarity matrix.
		 */
		class SimilarityOptimizationProvider;

		/**
		 * Forward declaration of a class implementing a data object allowing to optimize a normalized homography.
		 */
		class NormalizedHomographyData;

		/**
		 * Forward declaration of a class implementing a data object allowing to optimize a homography concurrently with a camera profile (without distortion parameters).
		 */
		class HomographyCameraData;

		/**
		 * Forward declaration of a class implementing a data object allowing to optimize several homographies concurrently.
		 */
		class CameraHomographiesData;

		/**
		 * Forward declaration of a class implementing a data object allowing to optimize a homography concurrently with a camera profile (including distortion parameters).
		 */
		class DistortionCameraHomographiesData;

	public:

		/**
		 * Optimizes the homography defining the transformation between two sets of corresponding image points which are projections of 3D object points (lying on a common plane) and visible in two individual camera frames.
		 * This function can use 8 or 9 parameters to represent the optimization model of the homography.<br>
		 * A homography has 8 degrees of freedom so that 8 model parameters should be the correct choice.<br>
		 * However, due to numerical stability the application of 9 model parameters often provides a more robust/stable solution, especially for image point correspondences with complex distributions/locations.
		 * Beware: In case binary size matters, and in case you know which robust estimator to be used for optimizing the homography, please use the corresponding function using a template-parameter for the estimator.
		 * @param homography Initial homography that will be optimized, rightPoint = homography * leftPoint, must be valid
		 * @param imagePointsLeft Projected 3D plane points visible in the left camera frame
		 * @param imagePointsRight Projected 3D plane points visible in the right camera frame, each point corresponds the one point in the left camera frame
		 * @param correspondences Number given image correspondences, with range [4, infinity)
		 * @param modelParameters The number of parameters defining the model (of the homography) to be optimize, with range [8, 9]
		 * @param optimizedHomography Resulting optimized homography, normalized so that the lower right element of the Homography is 1, rightPoint = optimizedHomography * leftPoint
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator The robust error estimator to be used for calculating the accuracy/error of a homography
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @param intermediates Optional resulting intermediate (improving) errors in relation to the defined estimator
		 * @return True, if the homography could be optimized (at least within one iteration)
		 * @see optimizeHomography<tEstimator>().
		 */
		static bool optimizeHomography(const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, const unsigned int modelParameters, SquareMatrix3& optimizedHomography, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediates = nullptr);

		/**
		 * Optimizes the homography defining the transformation between two sets of corresponding image points which are projections of 3D object points (lying on a common plane) and visible in two individual camera frames.
		 * This function can use 8 or 9 parameters to represent the optimization model of the homography.<br>
		 * A homography has 8 degrees of freedom so that 8 model parameters should be the correct choice.<br>
		 * However, due to numerical stability the application of 9 model parameters often provides a more robust/stable solution, especially for image point correspondences with complex distributions/locations.
		 * @param homography Initial homography that will be optimized, rightPoint = homography * leftPoint, must be valid
		 * @param imagePointsLeft Projected 3D plane points visible in the left camera frame
		 * @param imagePointsRight Projected 3D plane points visible in the right camera frame, each point corresponds the one point in the left camera frame
		 * @param correspondences Number given image correspondences, with range [4, infinity)
		 * @param modelParameters The number of parameters defining the model (of the homography) to be optimize, with range [8, 9]
		 * @param optimizedHomography Resulting optimized homography, normalized so that the lower right element of the Homography is 1, rightPoint = optimizedHomography * leftPoint
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @param intermediates Optional resulting intermediate (improving) errors in relation to the defined estimator
		 * @return True, if the homography could be optimized (at least within one iteration)
		 * @tparam tEstimator The robust error estimator to be used for calculating the accuracy/error of a homography
		 * @see optimizeHomography().
		 */
		template <Estimator::EstimatorType tEstimator>
		static bool optimizeHomography(const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, const unsigned int modelParameters, SquareMatrix3& optimizedHomography, const unsigned int iterations = 20u, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediates = nullptr);

		/**
		 * Optimizes a similarity transformation defining the transformation between two sets of corresponding image points visible in two individual camera frames.
		 * Beware: In case binary size matters, and in case you know which robust estimator to be used for optimizing the similarity, please use the corresponding function using a template-parameter for the estimator.
		 * @param similarity Initial similarity that will be optimized, rightPoint = similarity * leftPoint, must be valid
		 * @param imagePointsLeft Projected 3D plane points visible in the left camera frame
		 * @param imagePointsRight Projected 3D plane points visible in the right camera frame, each point corresponds the one point in the left camera frame
		 * @param correspondences Number given image correspondences, with range [2, infinity)
		 * @param optimizedSimilarity Resulting optimized similarity, rightPoint = optimizedSimilarity * leftPoint
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator The robust error estimator to be used for calculating the accuracy/error of a similarity
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @param intermediates Optional resulting intermediate (improving) errors in relation to the defined estimator
		 * @return True, if the similarity could be optimized (at least within one iteration)
		 * @see optimizeSimilarity<tEstimator>().
		 */
		static bool optimizeSimilarity(const SquareMatrix3& similarity, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, SquareMatrix3& optimizedSimilarity, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediates = nullptr);

		/**
		 * Optimizes a similarity defining the transformation between two sets of corresponding image points visible in two individual camera frames.
		 * @param similarity Initial similarity that will be optimized, rightPoint = similarity * leftPoint, must be valid
		 * @param imagePointsLeft Projected 3D plane points visible in the left camera frame
		 * @param imagePointsRight Projected 3D plane points visible in the right camera frame, each point corresponds the one point in the left camera frame
		 * @param correspondences Number given image correspondences, with range [2, infinity)
		 * @param optimizedSimilarity Resulting optimized similarity, rightPoint = optimizedSimilarity * leftPoint
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @param invertedCovariances Optional set of 2x2 inverted covariance matrices that represent the uncertainties of the image points (a 2*n x 2 matrix)
		 * @param intermediates Optional resulting intermediate (improving) errors in relation to the defined estimator
		 * @return True, if the similarity could be optimized (at least within one iteration)
		 * @tparam tEstimator The robust error estimator to be used for calculating the accuracy/error of a similarity
		 * @see optimizeSimilarity().
		 */
		template <Estimator::EstimatorType tEstimator>
		static bool optimizeSimilarity(const SquareMatrix3& similarity, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, SquareMatrix3& optimizedSimilarity, const unsigned int iterations = 20u, const Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediates = nullptr);

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION
		/**
		 * Optimizes the planar homography defining the transformation between projected 3D plane points in two cameras frames.
		 * @param homography Initial homography that will be optimized, rightPoint = homography * leftPoint, must be valid
		 * @param imagePointsLeft Projected 3D plane points visible in the left camera frame
		 * @param imagePointsRight Projected 3D plane points visible in the right camera frame, each point corresponds the one point in the left camera frame
		 * @param correspondences Number given image correspondences
		 * @param optimizedHomography Resulting optimized homography, rightPoint = optimizedHomography * leftPoint, must be valid
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return Result of the optimization
		 */
		static bool optimizeHomography(const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, SquareMatrix3& optimizedHomography, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);
#endif

		/**
		 * Optimizes homography defining the transformation between projected 3D plane points in two cameras frames.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param homography Initial homography that will be optimized, rightPoint = homography * leftPoint, must be valid
		 * @param imagePointsLeft Projected 3D plane points visible in the left camera frame
		 * @param imagePointsRight Projected 3D plane points visible in the right camera frame, each point corresponds the one point in the left camera frame
		 * @param correspondences Number given image correspondences
		 * @param optimizedHomography Resulting optimized homography, rightPoint = optimizedHomography * leftPoint
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return Result of the optimization
		 */
		static bool optimizeHomography(const PinholeCamera& pinholeCamera, const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, SquareMatrix3& optimizedHomography, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * **TODO** seems to be not correct **TODO** **VALIDATE**
		 * Optimizes the camera profile and the planar homography defining the transformation between projected 3D plane points in two cameras frames.
		 * @param pinholeCamera The initial pinhole camera profile
		 * @param homography Initial homography that will be optimized
		 * @param imagePointsLeft Projected 3D plane points visible in the left camera frame
		 * @param imagePointsRight Projected 3D plane points visible in the right camera frame, each point corresponds the one point in the left camera frame
		 * @param correspondences Number given image correspondences
		 * @param optimizedCamera Optimized camera profile
		 * @param optimizedHomography Resulting optimized homography
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 */
		static bool optimizeCameraHomography(const PinholeCamera& pinholeCamera, const SquareMatrix3& homography, const Vector2* imagePointsLeft, const Vector2* imagePointsRight, const size_t correspondences, PinholeCamera& optimizedCamera, SquareMatrix3& optimizedHomography, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes the camera profile and concurrently a set of homographies which come with corresponding pairs of image point correspondences.
		 * The given homographies transform image points defined in the 'first' camera frame into image points defined in the 'second' camera frame: p_second = H * p_first.<br>
		 * However, as more than one homography can be provided, each individual homography has an individual 'first' and 'second' camera frame.<br>
		 * The given pairs of points correspondences define the points in the 'first' camera frame (pair.first) and the corresponding points in the 'second' camera frame (pair.second).<br>
		 * @param pinholeCamera The pinhole camera profile that has been used to create the initial homographies
		 * @param homographies The individual homographies each defines a transformation between two camera frames, the number of homographies must match the number of pairs of image point correspondences
		 * @param imagePointsPairs The individual pairs of image points correspondences, each paint pair corresponds with one homography
		 * @param optimizedCamera The resulting optimized camera profile
		 * @param optimizedHomographies The set of optimized homographies
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 * @see optimizeDistortionCameraHomographies().
		 */
		static bool optimizeCameraHomographies(const PinholeCamera& pinholeCamera, const SquareMatrices3& homographies, const ImagePointsPairs& imagePointsPairs, PinholeCamera& optimizedCamera, SquareMatrices3& optimizedHomographies, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);

		/**
		 * Optimizes only the distortion parameters of a camera profile and concurrently a set of homographies which come with corresponding pairs of image point correspondences.
		 * The given homographies transform image points defined in the 'first' camera frame into image points defined in the 'second' camera frame: p_second = H * p_first.<br>
		 * However, as more than one homography can be provided, each individual homography has an individual 'first' and 'second' camera frame.<br>
		 * The given pairs of points correspondences define the points in the 'first' camera frame (pair.first) and the corresponding points in the 'second' camera frame (pair.second).<br>
		 * @param pinholeCamera The pinhole camera profile that has been used to create the initial homographies
		 * @param homographies The individual homographies each defines a transformation between two camera frames, the number of homographies must match the number of pairs of image point correspondences
		 * @param imagePointsPairs The individual pairs of image points correspondences, each paint pair corresponds with one homography
		 * @param optimizedCamera The resulting optimized camera profile
		 * @param optimizedHomographies The set of optimized homographies
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 * @see optimizeCameraHomographies().
		 */
		static bool optimizeDistortionCameraHomographies(const PinholeCamera& pinholeCamera, const SquareMatrices3& homographies, const ImagePointsPairs& imagePointsPairs, PinholeCamera& optimizedCamera, SquareMatrices3& optimizedHomographies, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);
};

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_HOMOGRAPHY_H
