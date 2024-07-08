/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/Estimator.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/StaticBuffer.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/SparseMatrix.h"
#include "ocean/math/SquareMatrix2.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements the basic functions for least square or robust optimization algorithms for non linear functions.
 * The actual optimization algorithms are located in derived functions.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimization
{
	template <unsigned int, unsigned int, unsigned int> friend class NonLinearUniversalOptimizationDense;

	public:

		/**
		 * This class implements the base class for an accessor of groups of pairs.
		 * Each group of pairs may have an arbitrary number of elements.
		 * @tparam TFirst The data type of the first element of each pair
		 * @tparam TSecond The data type of the second element of each pair
		 */
		template <typename TFirst, typename TSecond>
		class CorrespondenceGroupsAccessor
		{
			protected:

				/**
				 * Definition of a pair combining an object point id with an image point.
				 */
				typedef std::pair<Index32, Vector2> Element;

				/**
				 * Definition of a vector holding elements.
				 */
				typedef std::vector<Element> Elements;

				/**
				 * Definition of a vector holding a group of elements.
				 */
				typedef std::vector<Elements> ElementGroups;

			public:

				/**
				 * Copy constructor.
				 * @param accessor Accessor to copy
				 */
				inline CorrespondenceGroupsAccessor(const CorrespondenceGroupsAccessor<TFirst, TSecond>& accessor);

				/**
				 * Move constructor.
				 * @param accessor Accessor to move
				 */
				inline CorrespondenceGroupsAccessor(CorrespondenceGroupsAccessor<TFirst, TSecond>&& accessor);

				/**
				 * Returns the number of groups of this accessor.
				 * @return The number of groups
				 */
				inline size_t groups() const;

				/**
				 * Returns the number of elements within a specified group.
				 * @param groupIndex The index of the group for which the number of elements is requested, with range [0, groups())
				 * @return The number of elements within the specified group
				 */
				inline size_t groupElements(const size_t groupIndex) const;

				/**
				 * Returns one pair of a specific group of this object.
				 * @param groupIndex The index of the group, with range [0, groups())
				 * @param elementIndex The index of the element within the specified group, with range [0, groupElements(groupIndex))
				 * @param first The resulting first element of the specified pair
				 * @param second The resulting second element of the specified pair
				 */
				inline void element(const size_t groupIndex, const size_t elementIndex, TFirst& first, TSecond& second) const;

				/**
				 * Returns the first element of a pair of a specific group of this object.
				 * @param groupIndex The index of the group, with range [0, groups())
				 * @param elementIndex The index of the element within the specified group, with range [0, groupElements(groupIndex))
				 * @return The first element of the specified pair
				 */
				inline const TFirst& firstElement(const size_t groupIndex, const size_t elementIndex) const;

				/**
				 * Returns the first element of a pair of a specific group of this object.
				 * @param groupIndex The index of the group, with range [0, groups())
				 * @param elementIndex The index of the element within the specified group, with range [0, groupElements(groupIndex))
				 * @return The second element of the specified pair
				 */
				inline const TSecond& secondElement(const size_t groupIndex, const size_t elementIndex) const;

				/**
				 * Copy constructor.
				 * @param accessor Accessor to copy
				 */
				inline CorrespondenceGroupsAccessor<TFirst, TSecond>& operator=(const CorrespondenceGroupsAccessor<TFirst, TSecond>& accessor);

				/**
				 * Move constructor.
				 * @param accessor Accessor to move
				 */
				inline CorrespondenceGroupsAccessor<TFirst, TSecond>& operator=(CorrespondenceGroupsAccessor<TFirst, TSecond>&& accessor);

			protected:

				/**
				 * Creates a new accessor object.
				 */
				inline CorrespondenceGroupsAccessor();

				/**
				 * Creates a new accessor object.
				 * @param elementGroups The element groups of the object
				 */
				inline CorrespondenceGroupsAccessor(const ElementGroups& elementGroups);

				/**
				 * Creates a new accessor object.
				 * @param elementGroups The element groups of the object to move
				 */
				inline CorrespondenceGroupsAccessor(ElementGroups&& elementGroups);

			protected:

				/// The groups of elements of this accessor.
				ElementGroups elementGroups_;
		};

		/**
		 * This class implements an abstract specialization of the accessor for groups of pairs for object points.
		 * The accessor provides one group for each object point while each group holds pairs of correspondences between poses and image points.
		 */
		class ObjectPointGroupsAccessor : public CorrespondenceGroupsAccessor<Index32, Vector2>
		{
			protected:

				/**
				 * Creates a new accessor object.
				 */
				inline ObjectPointGroupsAccessor();

				/**
				 * Creates a new accessor object.
				 * @param elementGroups The element groups of the object
				 */
				inline ObjectPointGroupsAccessor(const ElementGroups& elementGroups);

				/**
				 * Creates a new accessor object.
				 * @param elementGroups The element groups of the object to move
				 */
				inline ObjectPointGroupsAccessor(ElementGroups&& elementGroups);
		};

		/**
		 * This class implements an abstract specialization of the accessor for groups of pairs for poses.
		 * The accessor provides one group for each pose while each group holds pairs of correspondences between object points and image points.
		 */
		class PoseGroupsAccessor : public CorrespondenceGroupsAccessor<Index32, Vector2>
		{
			protected:

				/**
				 * Creates a new accessor object.
				 */
				inline PoseGroupsAccessor();

				/**
				 * Creates a new accessor object.
				 * @param elementGroups The element groups of the object
				 */
				inline PoseGroupsAccessor(const ElementGroups& elementGroups);

				/**
				 * Creates a new accessor object.
				 * @param elementGroups The element groups of the object to move
				 */
				inline PoseGroupsAccessor(ElementGroups&& elementGroups);
		};

		/**
		 * This class implements a group accessor providing access to pairs of poses and image points.
		 * The groups of pairs have the following structure, each object point can have an arbitrary number of pairs:
		 * <pre>
		 * objectpoint_0 -> (pose_0, imagePoint_0)
		 *               -> (pose_1, imagePoint_4)
		 *               -> (pose_5, imagePoint_9)
		 *
		 * objectPoint_1 -> (pose_0, imagePoint_2)
		 *
		 * objectPoint_2 -> (pose_2, imagePoint_3)
		 *                  (pose_1, imagePoint_8)
		 *
		 * objectPoint_3 -> (pose_9, imagePoint_5)
		 * </pre>
		 */
		class ObjectPointToPoseIndexImagePointCorrespondenceAccessor : public ObjectPointGroupsAccessor
		{
			public:

				/**
				 * Default constructor.
				 */
				ObjectPointToPoseIndexImagePointCorrespondenceAccessor() = default;

				/**
				 * Creates a new accessor object for two sets of image points which are the observations of the same object points in two individual camera frames.
				 * @param imagePoints0 The first set of image points which are the observations of the object points in the first pose
				 * @param imagePoints1 The second set of image points which are the observations of the object points in the second pose
				 * @tparam TAccessor The data type of the accessor providing the image points
				 */
				template <typename TAccessor>
				ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const TAccessor& imagePoints0, const TAccessor& imagePoints1);

				/**
				 * Creates a new accessor object for a set of object points all observed by the same number of image points in a set of camera poses.
				 * Thus, the number of corresponding image points for one object points is identical to the number of camera poses.
				 * @param imagePointGroups The group of image points observing the object points, one group for each object point, the indices of the image points are identical to the indices of the corresponding camera poses
				 * @tparam TAccessor The data type of the accessor providing the image points
				 */
				template <typename TAccessor>
				explicit ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const std::vector<TAccessor>& imagePointGroups);

				/**
				 * Creates a new accessor object for a set of object points all observed by the same number of image points in a set of camera poses.
				 * Thus, the number of corresponding image points for one object points is identical to the number of camera poses.<br>
				 * The provided image points have the following pattern for n object points:<br>
				 * <pre>
				 * Group 0 / Object Point 0        , Group 1 / Object Point 1   , Group 2 / Object Point2 ...
				 * i0, i1, i2, i3, i4, i5, ...,  in, in+1, in+2, in+3, in+5, ..., in+n+1, in+n+2, ...
				 * </pre>
				 * where i0, in+1, in+n+1 ... are the image points located in the first camera frame; i1, in+2, in+n+2 ... are image points located in the second frame a.s.o.
				 * @param sequentialImagePointGroups The group of image points observing the object points provided sequentially, one sequential group for each object point, the indices of the image points (within the sequential groups) are identical to the indices of the corresponding camera poses, while imagePointGroups.size() is a multiple of numberObjectPoints
				 * @param numberObjectPoints The number of object points (the number of sequential groups of image points), with range [1, infinity)
				 * @tparam TAccessor The data type of the accessor providing the image points
				 */
				template <typename TAccessor>
				ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const TAccessor& sequentialImagePointGroups, const size_t numberObjectPoints);

				/**
				 * Creates a new accessor object for a set of object points all observed by the same number of image points in a set of camera poses.
				 * Thus, the number of corresponding image points for one object points is identical to the number of camera poses.<br>
				 * The provided image points have the following pattern for n object points:<br>
				 * <pre>
				 * Camera pose 0                   , Camera pose 1              , Camera pose 2 ...
				 * i0, i1, i2, i3, i4, i5, ...,  in, in+1, in+2, in+3, in+5, ..., in+n+1, in+n+2, ...
				 * </pre>
				 * where i0, in+1, in+n+1 ... are the image points observing the first object point; i1, in+2, in+n+2 ... are image points observing the second object point a.s.o.
				 * @param numberObjectPoints The number of object points, with range [1, infinity)
				 * @param imagePoints The set of image points observing the object points provided in sequential groups, each group holds all image points for one camera pose, while imagePoint.size() is a multiple of numberObjectPoints
				 * @tparam TAccessor The data type of the accessor providing the image points
				 */
				template <typename TAccessor>
				ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const size_t numberObjectPoints, const TAccessor& imagePoints);

				/**
				 * Adds the observations of a new object point to this accessor.
				 * @param poseIdImagePointPairs The pairs combining the pose id and the image point locations, at least one
				 * @return The index of the new object point which has been added, with range [0, infinity)
				 */
				inline size_t addObjectPoint(Elements&& poseIdImagePointPairs);
		};

		/**
		 * This class implements the base optimization provider.
		 */
		class OptimizationProvider
		{
			public:

				/**
				 * Returns whether the provider comes with an own solver for the linear equation.
				 * @return True, if so
				 */
				inline bool hasSolver() const;

				/**
				 * Solves the linear equation JTJ * deltas = jErrors.
				 * @param JTJ The Hesse matrix (transposed jacobian multiplied by the jacobian)
				 * @param jErrors The individual error values
				 * @param deltas The resulting individual delta values
				 * @return True, if the equation could be solved
				 */
				inline bool solve(const Matrix& JTJ, const Matrix& jErrors, Matrix& deltas) const;

				/**
				 * Solves the linear equation JTJ * deltas = jErrors.
				 * @param JTJ The Hesse matrix (transposed jacobian multiplied by the jacobian)
				 * @param jErrors The individual error values
				 * @param deltas The resulting individual delta values
				 * @return True, if the equation could be solved
				 */
				inline bool solve(const SparseMatrix& JTJ, const Matrix& jErrors, Matrix& deltas) const;
		};

		/**
		 * This class implements the base for an advanced dense optimization provider.
		 * The advanced optimization provider is able to determine an overall error as well as the entire Hessian matrix and Jacobian-Error vector for any intermediate Levenberg-Marquardt optimization step.
		 * @see OptimizationProvider
		 */
		class AdvancedDenseOptimizationProvider
		{
			public:

				/**
				 * Determines the error for the current model candidate (not the actual model).
				 * Needs to be implemented in the actual provider.
				 * @return The error for the current model candidate
				 */
				inline Scalar determineError();

				/**
				 * Determines the Hessian matrix and the Error-Jacobian vector based on the actual/current model (the transposed jacobian multiplied with the individual errors).
				 * Needs to be implemented in the actual provider.
				 * @param hessian The resulting (n x n) Hessian matrix for a model with dimension n, which is the transposed Jacobian multiplied with the Jacobian, may be the approximated Hessian
				 * @param jacobianError The resulting (n x 1) Error-Jacobian vector
				 * @return True, if succeeded
				 */
				inline bool determineHessianAndErrorJacobian(Matrix& hessian, Matrix& jacobianError);

				/**
				 * Creates a new model candidate by adjusting the current model with delta values.
				 * Needs to be implemented in the actual provider.
				 * @param deltas The delta values to be used applied
				 */
				inline void applyCorrection(const Matrix& deltas);

				/**
				 * Accepts the current model candidate a new (better) model than the previous one.
				 * Needs to be implemented in the actual provider.
				 */
				inline void acceptCorrection();

				/**
				 * Returns whether the optimization process should stop e.g., due to an external event.
				 * Needs to be implemented in the actual provider.
				 * @return True, to stop the optimization process
				 */
				inline bool shouldStop();

				/**
				 * Returns whether the provider comes with an own solver for the linear equation.
				 * @return True, if so
				 */
				inline bool hasSolver() const;

				/**
				 * Solves the linear equation Hessian * deltas = -jacobianError.
				 * @param hessian The Hesse matrix (transposed jacobian multiplied by the jacobian)
				 * @param jacobianError The transposed jacobian multiplied with the individual errors
				 * @param deltas The resulting individual delta values
				 * @return True, if the equation could be solved
				 */
				inline bool solve(const Matrix& hessian, const Matrix& jacobianError, Matrix& deltas) const;

			protected:

				/**
				 * Protected default constructor.
				 */
				inline AdvancedDenseOptimizationProvider();
		};

		/**
		 * This class implements the base class for an advanced sparse optimization provider.
		 * @see AdvancedDenseOptimizationProvider, OptimizationProvider
		 */
		class AdvancedSparseOptimizationProvider
		{
			public:

				/**
				 * Determines the error for the current model candidate (not the actual/actual model).
				 * Needs to be implemented in the actual provider.
				 * @return The error for the current model candidate
				 */
				inline Scalar determineError();

				/**
				 * Determines any kind of (abstract) parameters based on the current/actual model (not the model candidate) e.g., the Jacobian parameters and/or a Hessian matrix.
				 * The provide is responsible for the actual data.<br>
				 * Needs to be implemented in the actual provider.
				 * @return True, if succeeded
				 */
				inline bool determineParameters();

				/**
				 * Creates a new model candidate by adjusting the current/actual model with delta values.
				 * Needs to be implemented in the actual provider.
				 * @param deltas The delta values to be used applied
				 */
				inline void applyCorrection(const Matrix& deltas);

				/**
				 * Accepts the current model candidate a new (better) model than the previous one.
				 * Needs to be implemented in the actual provider.
				 */
				inline void acceptCorrection();

				/**
				 * Returns whether the optimization process should stop e.g., due to an external event.
				 * Needs to be implemented in the actual provider.
				 * @return True, to stop the optimization process
				 */
				inline bool shouldStop();

				/**
				 * Solves the linear equation Hessian * deltas = -jacobianError based on the internal data.
				 * The diagonal of the Hessian matrix will be multiplied by (1 + lambda) before the actual solving starts.<br>
				 * This function may be invoked several times before determineParameters() is invoked again so that the diagonal of the original Hessian matrix should be copied.
				 * @param deltas The resulting individual delta values
				 * @param lambda Optional lambda value for a Levenberg-Marquardt optimization, with range [0, infinity)
				 * @return True, if the equation could be solved
				 */
				inline bool solve(Matrix& deltas, const Scalar lambda = Scalar(0));

			protected:

				/**
				 * Protected default constructor.
				 */
				inline AdvancedSparseOptimizationProvider();
		};

	public:

		/**
		 * Invokes the optimization of a dense (matrix) optimization problem.
		 * @param provider The optimization provider that is used during the optimization
		 * @param iterations Number of optimization iterations
		 * @param estimator Robust estimator to be applied
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust (depending on estimator) pixel error for the given initial parameters
		 * @param finalError Optional resulting averaged robust (depending on estimator) pixel error for the final optimized parameters
		 * @param invertedCovariances Optional set of inverted covariances that define the individual uncertainties of the measurements
		 * @param intermediateErrors Optional resulting intermediate (improving) errors in relation to the defined estimator
		 * @return True, if at least one successful optimization iteration has been executed
		 * @tparam T Data type of the optimization provider
		 */
		template <typename T>
		static inline bool denseOptimization(T& provider, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Invokes the optimization of a dense (matrix) optimization problem.
		 * @param provider The optimization provider that is used during the optimization
		 * @param iterations Number of optimization iterations
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust (depending on estimator) pixel error for the given initial parameters
		 * @param finalError Optional resulting averaged robust (depending on estimator) pixel error for the final optimized parameters
		 * @param invertedCovariances Optional set of inverted covariances that define the individual uncertainties of the measurements
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if at least one successful optimization iteration has been executed
		 * @tparam T Data type of the optimization provider
		 * @tparam tEstimator Type of the robust estimator to be applied
		 */
		template <typename T, Estimator::EstimatorType tEstimator>
		static bool denseOptimization(T& provider, const unsigned int iterations = 5u, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Invokes the optimization of a sparse (matrix) optimization problem.
		 * @param provider The optimization provider that is used during the optimization
		 * @param iterations Number of optimization iterations
		 * @param estimator Robust estimator to be applied
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust (depending on estimator) pixel error for the given initial parameters
		 * @param finalError Optional resulting averaged robust (depending on estimator) pixel error for the final optimized parameters
		 * @param invertedCovariances Optional set of inverted covariances that define the individual uncertainties of the measurements
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if at least one successful optimization iteration has been executed
		 * @tparam T Data type of the optimization provider
		 */
		template <typename T>
		static bool sparseOptimization(T& provider, const unsigned int iterations = 5u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Invokes the optimization of a sparse (matrix) optimization problem.
		 * @param provider The optimization provider that is used during the optimization
		 * @param iterations Number of optimization iterations
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust (depending on estimator) pixel error for the given initial parameters
		 * @param finalError Optional resulting averaged robust (depending on estimator) pixel error for the final optimized parameters
		 * @param invertedCovariances Optional set of inverted covariances that define the individual uncertainties of the measurements
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if at least one successful optimization iteration has been executed
		 * @tparam T Data type of the optimization provider
		 * @tparam tEstimator Type of the robust estimator to be applied
		 */
		template <typename T, Estimator::EstimatorType tEstimator>
		static bool sparseOptimization(T& provider, const unsigned int iterations = 5u, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, const Matrix* invertedCovariances = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Invokes the optimization of a dense (matrix) optimization problem using an advanced optimization provider.
		 * The optimization can use a Levenberg-Marquardt approach or a Gauss-Newton approach.
		 * @param advancedDenseProvider The advanced dense optimization provider that is used during the optimization
		 * @param iterations Number of optimization iterations
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity), 0 to apply a Gauss-Netwton optimization approach
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity), 1 to apply a Gauss-Newton optimization approach
		 * @param initialError Optional resulting averaged robust (depending on estimator) pixel error for the given initial parameters
		 * @param finalError Optional resulting averaged robust (depending on estimator) pixel error for the final optimized parameters
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if at least one successful optimization iteration has been executed
		 * @tparam T Data type of the advanced dense optimization provider, must be derived from AdvancedDenseOptimizationProvider
		 */
		template <typename T>
		static bool advancedDenseOptimization(T& advancedDenseProvider, const unsigned int iterations, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Invokes the optimization of a sparse (matrix) optimization problem using an advanced optimization provider.
		 * The optimization can use a Levenberg-Marquardt approach or a Gauss-Newton approach.
		 * @param advancedSparseProvider The advanced sparse optimization provider that is used during the optimization
		 * @param iterations Number of optimization iterations
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity), 0 to apply a Gauss-Netwton optimization approach
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity), 1 to apply a Gauss-Newton optimization approach
		 * @param initialError Optional resulting averaged robust (depending on estimator) pixel error for the given initial parameters
		 * @param finalError Optional resulting averaged robust (depending on estimator) pixel error for the final optimized parameters
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if at least one successful optimization iteration has been executed
		 * @tparam T Data type of the advanced sparse optimization provider, must be derived from AdvancedSparseOptimizationProvider
		 */
		template <typename T>
		static bool advancedSparseOptimization(T& advancedSparseProvider, const unsigned int iterations, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Translates the n/2 squared errors that correspond to n elements in the error vector to robust errors.
		 * @param sqrErrors The n/2 squared errors
		 * @param modelParameters Number of parameters that define the model that has to be optimized
		 * @param weightedErrors Initial n errors that will be translated to robust errors
		 * @param weightVectors The n individual weights that have been applied to the errors
		 * @param transposedInvertedCovariances Optional transposed 2x2 inverted covariance matrices, one for each pair of errors
		 * @return Resulting averaged robust error of the entire data set
		 * @tparam tEstimator Robust error estimator to be used
		 */
		template <Estimator::EstimatorType tEstimator>
		static Scalar sqrErrors2robustErrors2(const Scalars& sqrErrors, const size_t modelParameters, Vector2* weightedErrors, Vector2* weightVectors, const SquareMatrix2* transposedInvertedCovariances);

		/**
		 * Translates the n/i squared errors that correspond to n elements in the error vector to robust errors.
		 * @param sqrErrors The n/i squared errors
		 * @param modelParameters Number of parameters that define the model that has to be optimized
		 * @param weightedErrors Initial n errors that will be translated to robust errors
		 * @param weightVectors The n individual weights that have been applied to the errors
		 * @param transposedInvertedCovariances Optional transposed ixi inverted covariance matrices, one for each pair of errors
		 * @return Resulting averaged robust error of the entire data set
		 * @tparam tEstimator Robust error estimator to be used
		 * @tparam tDimension Dimension of one error element
		 */
		template <Estimator::EstimatorType tEstimator, size_t tDimension>
		static Scalar sqrErrors2robustErrors(const Scalars& sqrErrors, const size_t modelParameters, StaticBuffer<Scalar, tDimension>* weightedErrors, StaticBuffer<Scalar, tDimension>* weightVectors, const Matrix* transposedInvertedCovariances);

		/**
		 * Translates the n/i squared errors that correspond to n elements in the error vector to robust errors.
		 * @param sqrErrors The n/i squared errors
		 * @param modelParameters Number of parameters that define the model that has to be optimized
		 * @param dimension The dimension of one error element, with range [1, infinity)
		 * @param weightedErrors_i Initial n errors that will be translated to robust errors
		 * @param weightVectors_i The n individual weights that have been applied to the errors
		 * @param transposedInvertedCovariances_i Optional transposed ixi inverted covariance matrices, one for each pair of errors
		 * @return Resulting averaged robust error of the entire data set
		 * @tparam tEstimator Robust error estimator to be used
		 */
		template <Estimator::EstimatorType tEstimator>
		static Scalar sqrErrors2robustErrors_i(const Scalars& sqrErrors, const size_t modelParameters, const size_t dimension, Scalar* weightedErrors_i, Scalar* weightVectors_i, const Matrix* transposedInvertedCovariances_i);

		/**
		 * Translates the n/2 squared errors that correspond to n elements in the error vector to robust errors.
		 * @param estimator Robust error estimator to be used
		 * @param sqrErrors The n/2 squared errors
		 * @param modelParameters Number of parameters that define the model that has to be optimized
		 * @param weightedErrors Initial n errors that will be translated to robust errors
		 * @param weightVectors The n individual weights that have been applied to the errors
		 * @param transposedInvertedCovariances Optional transposed 2x2 inverted covariance matrices, one for each pair of errors
		 * @return Resulting averaged robust error of the entire data set
		 */
		static inline Scalar sqrErrors2robustErrors2(const Estimator::EstimatorType estimator, const Scalars& sqrErrors, const size_t modelParameters, Vector2* weightedErrors, Vector2* weightVectors, const SquareMatrix2* transposedInvertedCovariances);

		/**
		 * Translates the n/i squared errors that correspond to n elements in the error vector to robust errors.
		 * @param estimator Robust error estimator to be used
		 * @param sqrErrors The n/i squared errors
		 * @param modelParameters Number of parameters that define the model that has to be optimized
		 * @param weightedErrors Initial n errors that will be translated to robust errors
		 * @param weightVectors The n individual weights that have been applied to the errors
		 * @param transposedInvertedCovariances Optional transposed ixi inverted covariance matrices, one for each pair of errors
		 * @return Resulting averaged robust error of the entire data set
		 * @tparam tDimension Dimension of one error element
		 */
		template <size_t tDimension>
		static inline Scalar sqrErrors2robustErrors(const Estimator::EstimatorType estimator, const Scalars& sqrErrors, const size_t modelParameters, StaticBuffer<Scalar, tDimension>* weightedErrors, StaticBuffer<Scalar, tDimension>* weightVectors, const Matrix* transposedInvertedCovariances);

		/**
		 * Translates the n/i squared errors that correspond to n elements in the error vector to robust errors.
		 * @param estimator Robust error estimator to be used
		 * @param sqrErrors The n/i squared errors
		 * @param modelParameters Number of parameters that define the model that has to be optimized
		 * @param dimension The dimension of one error element, with range [1, infinity)
		 * @param weightedErrors_i Initial n errors that will be translated to robust errors
		 * @param weightVectors_i The n individual weights that have been applied to the errors
		 * @param transposedInvertedCovariances_i Optional transposed ixi inverted covariance matrices, one for each pair of errors
		 * @return Resulting averaged robust error of the entire data set
		 */
		static inline Scalar sqrErrors2robustErrors_i(const Estimator::EstimatorType estimator, const Scalars& sqrErrors, const size_t modelParameters, const size_t dimension, Scalar* weightedErrors_i, Scalar* weightVectors_i, const Matrix* transposedInvertedCovariances_i);
};

template <typename TFirst, typename TSecond>
inline NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::CorrespondenceGroupsAccessor()
{
	// nothing to do here
}

template <typename TFirst, typename TSecond>
inline NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::CorrespondenceGroupsAccessor(const ElementGroups& elementGroups) :
	elementGroups_(elementGroups)
{
	// nothing to do here
}

template <typename TFirst, typename TSecond>
inline NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::CorrespondenceGroupsAccessor(ElementGroups&& elementGroups) :
	elementGroups_(std::move(elementGroups))
{
	// nothing to do here
}

template <typename TFirst, typename TSecond>
inline NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::CorrespondenceGroupsAccessor(const CorrespondenceGroupsAccessor<TFirst, TSecond>& accessor) :
	elementGroups_(accessor.elementGroups_)
{
	// nothing to do here
}

template <typename TFirst, typename TSecond>
inline NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::CorrespondenceGroupsAccessor(CorrespondenceGroupsAccessor<TFirst, TSecond>&& accessor) :
	elementGroups_(std::move(accessor.elementGroups_))
{
	// nothing to do here
}

template <typename TFirst, typename TSecond>
inline size_t NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::groups() const
{
	return elementGroups_.size();
}

template <typename TFirst, typename TSecond>
inline size_t NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::groupElements(const size_t groupIndex) const
{
	ocean_assert(groupIndex < groups());

	return elementGroups_[groupIndex].size();
}

template <typename TFirst, typename TSecond>
inline void NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::element(const size_t groupIndex, const size_t elementIndex, TFirst& first, TSecond& second) const
{
	ocean_assert(groupIndex < groups());
	ocean_assert(elementIndex < groupElements(groupIndex));

	const Element& element = elementGroups_[groupIndex][elementIndex];

	first = element.first;
	second = element.second;
}

template <typename TFirst, typename TSecond>
inline const TFirst& NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::firstElement(const size_t groupIndex, const size_t elementIndex) const
{
	ocean_assert(groupIndex < groups());
	ocean_assert(elementIndex < groupElements(groupIndex));

	return elementGroups_[groupIndex][elementIndex].first;
}

template <typename TFirst, typename TSecond>
inline const TSecond& NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::secondElement(const size_t groupIndex, const size_t elementIndex) const
{
	ocean_assert(groupIndex < groups());
	ocean_assert(elementIndex < groupElements(groupIndex));

	return elementGroups_[groupIndex][elementIndex].second;
}

template <typename TFirst, typename TSecond>
inline NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>& NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::operator=(const CorrespondenceGroupsAccessor<TFirst, TSecond>& accessor)
{
	elementGroups_ = accessor.elementGroups_;
	return *this;
}

template <typename TFirst, typename TSecond>
inline NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>& NonLinearOptimization::CorrespondenceGroupsAccessor<TFirst, TSecond>::operator=(CorrespondenceGroupsAccessor<TFirst, TSecond>&& accessor)
{
	if (this != &accessor)
	{
		elementGroups_ = std::move(accessor.elementGroups_);
	}

	return *this;
}

inline NonLinearOptimization::ObjectPointGroupsAccessor::ObjectPointGroupsAccessor()
{
	// nothing to do here
}

inline NonLinearOptimization::ObjectPointGroupsAccessor::ObjectPointGroupsAccessor(const ElementGroups& elementGroups) :
	CorrespondenceGroupsAccessor<Index32, Vector2>(elementGroups)
{
	// nothing to do here
}

inline NonLinearOptimization::ObjectPointGroupsAccessor::ObjectPointGroupsAccessor(ElementGroups&& elementGroups) :
	CorrespondenceGroupsAccessor<Index32, Vector2>(std::move(elementGroups))
{
	// nothing to do here
}

inline NonLinearOptimization::PoseGroupsAccessor::PoseGroupsAccessor()
{
	// nothing to do here
}

inline NonLinearOptimization::PoseGroupsAccessor::PoseGroupsAccessor(const ElementGroups& elementGroups) :
	CorrespondenceGroupsAccessor<Index32, Vector2>(elementGroups)
{
	// nothing to do here
}

inline NonLinearOptimization::PoseGroupsAccessor::PoseGroupsAccessor(ElementGroups&& elementGroups) :
	CorrespondenceGroupsAccessor<Index32, Vector2>(std::move(elementGroups))
{
	// nothing to do here
}

inline bool NonLinearOptimization::OptimizationProvider::hasSolver() const
{
	return false;
}

inline bool NonLinearOptimization::OptimizationProvider::solve(const Matrix& /*JTJ*/, const Matrix& /*jErrors*/, Matrix& /*deltas*/) const
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return false;
}

inline bool NonLinearOptimization::OptimizationProvider::solve(const SparseMatrix& /*JTJ*/, const Matrix& /*jErrors*/, Matrix& /*deltas*/) const
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return false;
}

inline NonLinearOptimization::AdvancedDenseOptimizationProvider::AdvancedDenseOptimizationProvider()
{
	// nothing to do here
}

inline Scalar NonLinearOptimization::AdvancedDenseOptimizationProvider::determineError()
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return Numeric::maxValue();
}

inline bool NonLinearOptimization::AdvancedDenseOptimizationProvider::determineHessianAndErrorJacobian(Matrix& /*hessian*/, Matrix& /*jacobianError*/)
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return false;
}

inline void NonLinearOptimization::AdvancedDenseOptimizationProvider::applyCorrection(const Matrix& /*deltas*/)
{
	ocean_assert(false && "Must be implemented in a derived class!");
}

inline void NonLinearOptimization::AdvancedDenseOptimizationProvider::acceptCorrection()
{
	ocean_assert(false && "Must be implemented in a derived class!");
}

inline bool NonLinearOptimization::AdvancedDenseOptimizationProvider::shouldStop()
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return true;
}

inline bool NonLinearOptimization::AdvancedDenseOptimizationProvider::hasSolver() const
{
	return false;
}

inline bool NonLinearOptimization::AdvancedDenseOptimizationProvider::solve(const Matrix& /*hessian*/, const Matrix& /*jacobianError*/, Matrix& /*deltas*/) const
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return false;
}

inline NonLinearOptimization::AdvancedSparseOptimizationProvider::AdvancedSparseOptimizationProvider()
{
	// nothing to do here
}

inline Scalar NonLinearOptimization::AdvancedSparseOptimizationProvider::determineError()
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return Numeric::maxValue();
}

inline bool NonLinearOptimization::AdvancedSparseOptimizationProvider::determineParameters()
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return false;
}

inline void NonLinearOptimization::AdvancedSparseOptimizationProvider::applyCorrection(const Matrix& /*deltas*/)
{
	ocean_assert(false && "Must be implemented in a derived class!");
}

inline void NonLinearOptimization::AdvancedSparseOptimizationProvider::acceptCorrection()
{
	ocean_assert(false && "Must be implemented in a derived class!");
}

inline bool NonLinearOptimization::AdvancedSparseOptimizationProvider::shouldStop()
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return true;
}

inline bool NonLinearOptimization::AdvancedSparseOptimizationProvider::solve(Matrix& /*deltas*/, const Scalar /*lambda*/)
{
	ocean_assert(false && "Must be implemented in a derived class!");
	return true;
}

template <typename TAccessor>
NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const TAccessor& imagePoints0, const TAccessor& imagePoints1) :
	ObjectPointGroupsAccessor(ElementGroups(imagePoints0.size(), Elements(2)))
{
	ocean_assert(imagePoints0.size() == imagePoints1.size());

	for (size_t n = 0; n < imagePoints0.size(); ++n)
	{
		elementGroups_[n][0].first = 0;
		elementGroups_[n][0].second = imagePoints0[n];

		elementGroups_[n][1].first = 1;
		elementGroups_[n][1].second = imagePoints1[n];
	}
}

template <typename TAccessor>
NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const std::vector<TAccessor>& imagePointGroups) :
	ObjectPointGroupsAccessor(ElementGroups(imagePointGroups.size()))
{
	ocean_assert(elementGroups_.size() == imagePointGroups.size());

	for (size_t n = 0; n < imagePointGroups.size(); ++n)
	{
		const ConstIndexedAccessor<Vector2>& imagePoints = imagePointGroups[n];
		ocean_assert(imagePoints.size() >= 2);

		Elements poseIndexImagePoints;
		poseIndexImagePoints.reserve(imagePoints.size());

		for (size_t i = 0; i < imagePoints.size(); ++i)
		{
			poseIndexImagePoints.emplace_back(Index32(i), imagePoints[i]);
		}

		elementGroups_[n] = std::move(poseIndexImagePoints);
	}
}

template <typename TAccessor>
NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const TAccessor& sequentialImagePointGroups, const size_t numberObjectPoints) :
	ObjectPointGroupsAccessor(ElementGroups(numberObjectPoints))
{
	ocean_assert(elementGroups_.size() == numberObjectPoints);
	ocean_assert(sequentialImagePointGroups.size() % numberObjectPoints == 0);

	const unsigned int numberImagePoints = (unsigned int)(sequentialImagePointGroups.size() / numberObjectPoints);

	unsigned int index = 0u;

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		Elements poseIndexImagePoints;
		poseIndexImagePoints.reserve(numberImagePoints);

		for (unsigned int i = 0u; i < numberImagePoints; ++i)
		{
			poseIndexImagePoints.emplace_back(Index32(i), sequentialImagePointGroups[index++]);
		}

		elementGroups_[n] = std::move(poseIndexImagePoints);
	}
}

template <typename TAccessor>
NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const size_t numberObjectPoints, const TAccessor& imagePoints) :
	ObjectPointGroupsAccessor(ElementGroups(numberObjectPoints, Elements(numberObjectPoints == 0 ? 0 : (imagePoints.size() / numberObjectPoints))))
{
	ocean_assert(elementGroups_.size() == numberObjectPoints);
	ocean_assert(imagePoints.size() % numberObjectPoints == 0);

	ocean_assert(numberObjectPoints != 0);
	const unsigned int numberImagePoints = (unsigned int)(imagePoints.size() / numberObjectPoints);

	unsigned int poseIndex = 0u;
	unsigned int index = 0u;

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		for (size_t i = 0; i < numberObjectPoints; ++i)
		{
			ocean_assert(elementGroups_[i].size() == numberImagePoints);
			elementGroups_[i][poseIndex] = Element(poseIndex, imagePoints[index++]);
		}

		poseIndex++;
	}
}

inline size_t NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::addObjectPoint(Elements&& poseIdImagePointPairs)
{
	ocean_assert(!poseIdImagePointPairs.empty());

	const size_t objectPointIndex = elementGroups_.size();

	elementGroups_.emplace_back(std::move(poseIdImagePointPairs));

	return objectPointIndex;
}

template <typename T>
inline bool NonLinearOptimization::denseOptimization(T& provider, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediateErrors)
{
	switch (estimator)
	{
		case Estimator::ET_SQUARE:
			return denseOptimization<T, Estimator::ET_SQUARE>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		case Estimator::ET_LINEAR:
			return denseOptimization<T, Estimator::ET_LINEAR>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		case Estimator::ET_HUBER:
			return denseOptimization<T, Estimator::ET_HUBER>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		case Estimator::ET_TUKEY:
			return denseOptimization<T, Estimator::ET_TUKEY>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		case Estimator::ET_CAUCHY:
			return denseOptimization<T, Estimator::ET_CAUCHY>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		default:
			ocean_assert(false && "Invalid estimator!");
			return false;
	}
}

template <typename T, Estimator::EstimatorType tEstimator>
bool NonLinearOptimization::denseOptimization(T& provider, const unsigned int iterations, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediateErrors)
{
	constexpr Scalar maxLambda = Scalar(1e8);

	ocean_assert(lambda >= Numeric::eps() && lambda <= maxLambda);

	Matrix jacobian;

	// the error vectors are weighted for non-square estimators only
	Matrix weightedErrorVector, swapWeightedErrorVector;
	// the weight vectors are used for non-square estimators only
	Matrix weightVector, swapWeightVector;

	Matrix JTJ, jErrors, deltas;

	// intermediate matrices that are used if a covariance matrix is provided
	Matrix intermediateCovarianceJacobian;
	Matrix intermediateWeightedErrorVector;

	Scalar bestError = provider.template determineRobustError<tEstimator>(weightedErrorVector, weightVector, invertedCovariances);

	if (bestError == Numeric::maxValue())
	{
		ocean_assert(false && "The initial model was invalid and thus the optimization cannot be applied!");
		return false;
	}

	if (initialError != nullptr)
	{
		*initialError = bestError;
	}

	if (intermediateErrors != nullptr)
	{
		ocean_assert(intermediateErrors->empty());
		intermediateErrors->push_back(bestError);
	}

	bool oneValidIteration = false;

	unsigned int i = 0u;
	while (i < iterations)
	{
		provider.determineJacobian(jacobian);

		// if inverted covariance matrices are provided a further matrix multiplication must be respected
		if (invertedCovariances)
		{
			if constexpr (tEstimator == Estimator::ET_SQUARE)
			{
				// J^+ = (J^T * iCV * J + lambda * diag(J^T * J))^-1 * J^T, dimension: m x dn

				invertedCovariances->selfSquareDiagonalMatrixMultiply(jacobian, intermediateCovarianceJacobian);
				jacobian.transposedMultiply(intermediateCovarianceJacobian, JTJ);
				ocean_assert(JTJ.columns() == JTJ.rows());
			}
			else
			{
				// J^+ = (J^T * iCV * diag(weights) * J + lambda * diag(J^T * J))^-1 * J^T, dimension: m x dn

				invertedCovariances->selfSquareDiagonalMatrixMultiply(weightVector, jacobian, intermediateCovarianceJacobian);
				jacobian.transposedMultiply(intermediateCovarianceJacobian, JTJ);
				ocean_assert(JTJ.columns() == JTJ.rows());
			}

			// J^T * iCV * diag(weights) * error

			invertedCovariances->selfSquareDiagonalMatrixMultiply(weightedErrorVector, intermediateWeightedErrorVector);
			jacobian.transposedMultiply(intermediateWeightedErrorVector, jErrors);

			ocean_assert(jErrors.rows() == JTJ.rows() && jErrors.columns() == 1);
		}
		else
		{
			if constexpr (tEstimator == Estimator::ET_SQUARE)
			{
				// J^+ = (J^T * J + lambda * diag(J^T * J))^-1 * J^T, dimension: m x dn
				jacobian.selfTransposedSquareMatrix(JTJ);
				ocean_assert(JTJ.columns() == JTJ.rows());
			}
			else
			{
				// J^+ = (J^T * diag(weights) * J + lambda * diag(J^T * J))^-1 * J^T, dimension: m x dn
				jacobian.weightedSelfTransposedSquareMatrix(weightVector, JTJ);
				ocean_assert(JTJ.columns() == JTJ.rows());
			}

			// error = J^T * diag(weights)
			jacobian.transposedMultiply(weightedErrorVector, jErrors);
			ocean_assert(jErrors.rows() == JTJ.rows() && jErrors.columns() == 1);
		}

		const Matrix JTJdiagonal(JTJ.diagonal());

		while (i < iterations)
		{
			++i;

			// J^T * J = J^T * J + lambda * diag(J^T * J)
			if (lambda > Numeric::eps())
			{
				for (unsigned int n = 0; n < JTJ.columns(); ++n)
				{
					JTJ(n, n) = JTJdiagonal(n, 0) * (Scalar(1) + lambda);
				}
			}

			// JTJ * deltas = -J^T * error
			// however, we determine JTJ * deltas = J^T * error and thus receive negative deltas (which then need to be subracted from the current model/parameter configuration)

			bool solved = false;

			if (provider.hasSolver())
			{
				solved = provider.solve(JTJ, jErrors, deltas);
			}
			else
			{
				solved = JTJ.solve<Matrix::MP_SYMMETRIC>(jErrors, deltas);
			}

			if (solved)
			{
				oneValidIteration = true;

				// check whether the offset has been converged
				if (Numeric::isEqualEps(deltas.norm() / Scalar(deltas.elements())))
				{
					i = iterations;
				}

				// we apply the deltas by: new = old - deltas (due to the solved equation: JTJ * deltas = J^T * error)
				provider.applyCorrection(deltas);

				const Scalar iterationError = provider.template determineRobustError<tEstimator>(swapWeightedErrorVector, swapWeightVector, invertedCovariances);

				// check whether the new error is not better than the best one
				if (iterationError >= bestError)
				{
					// modify the lambda parameter and start a new optimization, as long as the lambda is not zero already or too large
					if (lambdaFactor > Numeric::eps() && lambda > 0 && lambda <= maxLambda)
					{
						lambda *= lambdaFactor;
					}
					else
					{
						ocean_assert(oneValidIteration && "At this moment we should have at least one valid iteration!");

						// no further improvement can be applied
						i = iterations;
					}

					continue;
				}

				// we have an improvement
				bestError = iterationError;

				if (intermediateErrors != nullptr)
				{
					intermediateErrors->push_back(bestError);
				}

				provider.acceptCorrection();

				std::swap(swapWeightedErrorVector, weightedErrorVector);
				std::swap(swapWeightVector, weightVector);

				if (Numeric::isNotEqualEps(lambdaFactor))
				{
					// we do not decrease lambda if lambda is already near to zero so that we simply should stop optimization if we fail to reduce the error
					if (lambda > Numeric::eps())
					{
						lambda /= lambdaFactor;
					}
				}

				//  skip this inner loop here as a new jacobian has to be calculated
				break;
			}
			else if (lambda > Numeric::eps() && lambda <= maxLambda)
			{
				lambda *= lambdaFactor;
			}
			else
			{
				ocean_assert(oneValidIteration && "At this moment we should have at least one valid iteration!");

				// no further improvement can be applied
				i = iterations;
			}
		}
	}

	if (finalError)
	{
		*finalError = bestError;
	}

	return oneValidIteration;
}

template <typename T>
bool NonLinearOptimization::sparseOptimization(T& provider, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediateErrors)
{
	switch (estimator)
	{
		case Estimator::ET_SQUARE:
			return sparseOptimization<T, Estimator::ET_SQUARE>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		case Estimator::ET_LINEAR:
			return sparseOptimization<T, Estimator::ET_LINEAR>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		case Estimator::ET_HUBER:
			return sparseOptimization<T, Estimator::ET_HUBER>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		case Estimator::ET_TUKEY:
			return sparseOptimization<T, Estimator::ET_TUKEY>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		case Estimator::ET_CAUCHY:
			return sparseOptimization<T, Estimator::ET_CAUCHY>(provider, iterations, lambda, lambdaFactor, initialError, finalError, invertedCovariances, intermediateErrors);

		default:
			ocean_assert(false && "Invalid estimator!");
			return false;
	}
}

template <typename T, Estimator::EstimatorType tEstimator>
bool NonLinearOptimization::sparseOptimization(T& provider, const unsigned int iterations, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, const Matrix* invertedCovariances, Scalars* intermediateErrors)
{
	constexpr Scalar maxLambda = Scalar(1e8);

	ocean_assert(lambda >= Numeric::eps() && lambda <= maxLambda);

	SparseMatrix jacobian;

	// the error vectors are weighted for non-square estimators only
	Matrix weightedErrorVector, swapWeightedErrorVector;
	// the weight vectors are used for non-square estimators only
	Matrix weightVector, swapWeightVector;

	const SparseMatrix invertedCovarianceMatrix = invertedCovariances ? SparseMatrix(invertedCovariances->rows(), invertedCovariances->rows(), *invertedCovariances) : SparseMatrix();

	SparseMatrix JTJ;
	Matrix jErrors, deltas;

	Scalar bestError = provider.template determineRobustError<tEstimator>(weightedErrorVector, weightVector, invertedCovariances);

	if (bestError == Numeric::maxValue())
	{
		ocean_assert(false && "The initial model was invalid and thus the optimization cannot be applied!");
		return false;
	}

	if (initialError)
	{
		*initialError = bestError;
	}

	if (intermediateErrors != nullptr)
	{
		ocean_assert(intermediateErrors->empty());
		intermediateErrors->push_back(bestError);
	}

	bool oneValidIteration = false;

	unsigned int i = 0u;
	while (i < iterations)
	{
		provider.determineJacobian(jacobian);

		// if inverted covariance matrices are provided a further matrix multiplication must be respected
		if (invertedCovariances)
		{
			//                                      delta  =  -(J^T * iCV * diag(weights) * J + lambda * diag(J^T * J)^-1 * J^T * iCV * diag(weights) * error
			// (J^T * iCV * diag(weights) * J + lambda * diag(J^T * J) *  delta  =  -J^T * iCV * diag(weights) * error
			// (J^T * iCV * diag(weights) * J + lambda * diag(J^T * J) * -delta  =   J^T * iCV * diag(weights) * error

			// J^T
			const SparseMatrix jacobianTransposed(jacobian.transposed());

			if constexpr (tEstimator == Estimator::ET_SQUARE)
			{
				// J^T * iCV * J
				JTJ = jacobianTransposed * (invertedCovarianceMatrix * jacobian);
				ocean_assert(JTJ.columns() == JTJ.rows());
			}
			else
			{
				// J^T * ICV * diag(weights) * J
				JTJ = jacobianTransposed * (invertedCovarianceMatrix * (SparseMatrix(weightVector.rows(), weightVector.rows(), weightVector, true) * jacobian));
				ocean_assert(JTJ.columns() == JTJ.rows());
			}

			// J^T * iCV * diag(weights) * error
			jErrors = jacobianTransposed * (invertedCovarianceMatrix * weightedErrorVector);
			ocean_assert(jErrors.rows() == JTJ.rows() && jErrors.columns() == 1);
		}
		else
		{
			//                                      delta  =  -(J^T * diag(weights) * J + lambda * diag(J^T * J)^-1 * J^T * diag(weights) * error
			// (J^T * diag(weights) * J + lambda * diag(J^T * J) *  delta  =  -J^T * diag(weights) * error
			// (J^T * diag(weights) * J + lambda * diag(J^T * J) * -delta  =   J^T * diag(weights) * error

			// J^T
			const SparseMatrix jacobianTransposed(jacobian.transposed());

			if constexpr (tEstimator == Estimator::ET_SQUARE)
			{
				// J^T * J
				JTJ = jacobianTransposed * jacobian;
				ocean_assert(JTJ.columns() == JTJ.rows());
			}
			else
			{
				// J^T * diag(weights) * J
				JTJ = jacobianTransposed * SparseMatrix(weightVector.rows(), weightVector.rows(), weightVector, true) * jacobian;
				ocean_assert(JTJ.columns() == JTJ.rows());
			}

			// J^T * diag(weights) * error
			jErrors = jacobianTransposed * weightedErrorVector;
			ocean_assert(jErrors.rows() == JTJ.rows() && jErrors.columns() == 1);
		}

		const Matrix JTJdiagonal(JTJ.diagonal());

		while (i < iterations)
		{
			++i;

			// J^T * J = J^T * J + lambda * diag(J^T * J)
			if (lambda > Numeric::eps())
			{
				for (unsigned int n = 0; n < JTJ.columns(); ++n)
				{
					if (JTJdiagonal(n, 0) != Scalar(0))
					{
						ocean_assert(!JTJ.isZero(n, n));
						JTJ(n, n) = JTJdiagonal(n, 0) * (Scalar(1) + lambda);
					}
				}
			}

			// JTJ * deltas = -J^T * error
			// however, we determine JTJ * deltas = J^T * error and thus receive negative deltas (which then need to be subracted from the current model/parameter configuration)

			bool solved = false;

			if (provider.hasSolver())
			{
				solved = provider.solve(JTJ, jErrors, deltas);
			}
			else
			{
				solved = JTJ.solve(jErrors, deltas);
			}

			if (solved)
			{
				oneValidIteration = true;

				// check whether the offset has been converted
				if (Numeric::isEqual(deltas.norm() / Scalar(deltas.elements()), 0, Numeric::weakEps() * Scalar(0.01)))
				{
					i = iterations;
				}

				// we apply the deltas by: new = old - deltas (due to the solved equation: JTJ * deltas = J^T * error)
				provider.applyCorrection(deltas);

				const Scalar iterationError = provider.template determineRobustError<tEstimator>(swapWeightedErrorVector, swapWeightVector, invertedCovariances);

				// check whether the new error is not better than the best one
				if (iterationError >= bestError)
				{
					// modify the lambda parameter and start a new optimization, as long as the lambda is not zero already or too large
					if (lambdaFactor > Numeric::eps() && lambda > 0 && lambda <= maxLambda)
					{
						lambda *= lambdaFactor;
					}
					else
					{
						ocean_assert(oneValidIteration && "At this moment we should have at least one valid iteration!");

						// no further improvement can be applied
						i = iterations;
					}

					continue;
				}

				// we have an improvement
				bestError = iterationError;

				if (intermediateErrors != nullptr)
				{
					intermediateErrors->push_back(bestError);
				}

				provider.acceptCorrection();

				std::swap(swapWeightedErrorVector, weightedErrorVector);
				std::swap(swapWeightVector, weightVector);

				if (Numeric::isNotEqualEps(lambdaFactor))
				{
					// we do not decrease lambda if lambda is already near to zero so that we simply should stop optimization if we fail to reduce the error
					if (lambda > Numeric::eps())
					{
						lambda /= lambdaFactor;
					}
				}

				//  skip this inner loop here as a new jacobian has to be calculated
				break;
			}
			else if (lambda > Numeric::eps() && lambda <= maxLambda)
			{
				lambda *= lambdaFactor;
			}
			else
			{
				ocean_assert(oneValidIteration && "At this moment we should have at least one valid iteration!");

				// no further improvement can be applied
				i = iterations;
			}
		}
	}

	if (finalError)
	{
		*finalError = bestError;
	}

	return oneValidIteration;
}

template <typename T>
bool NonLinearOptimization::advancedDenseOptimization(T& advancedDenseProvider, const unsigned int iterations, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	constexpr Scalar maxLambda = Scalar(1e8);

	ocean_assert(lambda >= Scalar(0) && lambda <= maxLambda);
	ocean_assert((lambda == Scalar(0) && lambdaFactor == Scalar(1)) || (lambda > Scalar(0) && lambdaFactor > Scalar(1)));

	const bool useLevenbergMarquardt = (lambda > 0 && lambdaFactor > 1);

	// we determine the initial error only for the Levenberg-Marquardt optimization of if the user explicitly requests that value
	Scalar bestError = (useLevenbergMarquardt || initialError) ? advancedDenseProvider.determineError() : Numeric::minValue();

	if (bestError == Numeric::maxValue())
	{
		ocean_assert(false && "The initial model was invalid and thus the optimization cannot be applied!");
		return false;
	}

	if (initialError)
	{
		*initialError = bestError;
	}

	if (intermediateErrors != nullptr)
	{
		ocean_assert(intermediateErrors->empty());
		intermediateErrors->push_back(bestError);
	}

	bool oneValidIteration = false;

	Matrix hessian, jacobianError, deltas;

	unsigned int i = 0u;
	while (!advancedDenseProvider.shouldStop() && i < iterations)
	{
		/**
		 * Now the provider will determine the (n x n) Hessian matrix and the (n x 1) Jacobian-Error matrix, may be the approximated Hessian.
		 * The Hessian matrix is calculated by J^T * J.
		 * The Jacobian-Error matrix is calculated by J^T * E, with E the error vector.
		 * The model has dimension n.
		 */
		if (!advancedDenseProvider.determineHessianAndErrorJacobian(hessian, jacobianError))
		{
			ocean_assert(false && "The Hessian and the Jacobian with combined error values could not be determined!");
			return false;
		}

		ocean_assert(hessian.rows() == hessian.columns());
		ocean_assert(hessian.rows() == jacobianError.rows() && jacobianError.columns() == 1);

		// now we store the diagonal of the Hessian allowing to apply a Levenberg-Marquardt later

		const Matrix hessianDiagonal(hessian.diagonal());
		ocean_assert(hessianDiagonal.rows() == hessian.rows() && hessianDiagonal.columns() == 1);

		while (!advancedDenseProvider.shouldStop() && i++ < iterations)
		{
			// we apply the lambda value for the Levenberg-Marquardt:
			// Hessian + lambda * diag(Hessian)
			if (lambda > Numeric::eps())
			{
				for (unsigned int n = 0; n < hessian.columns(); ++n)
				{
					hessian(n, n) = hessianDiagonal(n, 0) * (Scalar(1) + lambda);
				}
			}

			// now we solve: Hessian * deltas = -JacobianErrors, (we will subtract the deltas later so that we now can solve Hessian * deltas = +JacobianErrors)

			bool solved = false;

			if (advancedDenseProvider.hasSolver())
			{
				solved = advancedDenseProvider.solve(hessian, jacobianError, deltas);
			}
			else
			{
				solved = hessian.solve<Matrix::MP_SYMMETRIC>(jacobianError, deltas);
			}

			if (solved)
			{
				oneValidIteration = true;

				// check whether the offset has been converted
				if (Numeric::isWeakEqualEps(deltas.norm() / Scalar(deltas.elements())))
				{
					i = iterations;
				}

				// we apply the deltas by: new = old - deltas
				advancedDenseProvider.applyCorrection(deltas);

				const Scalar iterationError = (useLevenbergMarquardt || intermediateErrors) ? advancedDenseProvider.determineError() : Numeric::minValue();

				// check whether a Levenberg-Marquardt approach is intended and then check whether the new error is not better than the best one
				if (useLevenbergMarquardt && iterationError >= bestError)
				{
					// modify the lambda parameter and start a new optimization
					if (lambda > 0 && lambda <= maxLambda)
					{
						lambda *= lambdaFactor;
					}
					else
					{
						ocean_assert(oneValidIteration && "At this moment we should have at least one valid iteration!");

						// no further improvement can be applied
						i = iterations;
					}

					continue;
				}

				// we have an improvement, or we have a Gauss-Newton optimization so that we do not store the previous iteration
				bestError = iterationError;

				if (intermediateErrors != nullptr)
				{
					intermediateErrors->push_back(bestError);
				}

				advancedDenseProvider.acceptCorrection();

				if (useLevenbergMarquardt && Numeric::isNotEqualEps(lambdaFactor))
				{
					// we do not decrease lambda if lambda is already near to zero so that we simply should stop optimization if we fail to reduce the error
					if (lambda > Numeric::eps())
					{
						lambda /= lambdaFactor;
					}
				}

				//  skip this inner loop here as a new jacobian has to be calculated
				break;
			}
			else if (lambda > Numeric::eps() && lambda <= maxLambda)
			{
				lambda *= lambdaFactor;
			}
			else
			{
				ocean_assert(oneValidIteration && "At this moment we should have at least one valid iteration!");

				// no further improvement can be applied
				i = iterations;
			}
		}
	}

	if (finalError)
	{
		// if Levenberg-Marquardt is not intended we have to determine the error as we did not do it before
		if (!useLevenbergMarquardt && !intermediateErrors)
		{
			bestError = advancedDenseProvider.determineError();
		}

		*finalError = bestError;
	}

	return oneValidIteration;
}

template <typename T>
bool NonLinearOptimization::advancedSparseOptimization(T& advancedSparseProvider, const unsigned int iterations, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError, Scalars* intermediateErrors)
{
	constexpr Scalar maxLambda = Scalar(1e8);

	ocean_assert(lambda >= Scalar(0) && lambda <= maxLambda);
	ocean_assert((lambda == Scalar(0) && lambdaFactor == Scalar(1)) || (lambda > Scalar(0) && lambdaFactor > Scalar(1)));

	const bool useLevenbergMarquardt = (lambda > 0 && lambdaFactor > 1);

	// we determine the initial error only for the Levenberg-Marquardt optimization of if the user explicitly requests that value
	Scalar bestError = (useLevenbergMarquardt || initialError) ? advancedSparseProvider.determineError() : Numeric::minValue();

	if (bestError == Numeric::maxValue())
	{
		ocean_assert(false && "The initial model was invalid and thus the optimization cannot be applied!");
		return false;
	}

	if (initialError)
	{
		*initialError = bestError;
	}

	if (intermediateErrors != nullptr)
	{
		ocean_assert(intermediateErrors->empty());
		intermediateErrors->push_back(bestError);
	}

	bool oneValidIteration = false;

	Matrix deltas;

	unsigned int i = 0u;
	while (!advancedSparseProvider.shouldStop() && i < iterations)
	{
		/**
		 * Now the provider will determine all parameters like the Jacobian, the transposed Jacobian, the Hessian and the Jacobian-Error matrix.
		 * However, the provider may also determine parts of the values or abstract information as long as the provider is able to solve the linear equation later.
		 */
		if (!advancedSparseProvider.determineParameters())
		{
			ocean_assert(false && "The provider failed to determine the abstract parameters.");
			return false;
		}

		while (!advancedSparseProvider.shouldStop() && i++ < iterations)
		{
			ocean_assert(lambda >= Scalar(0));
			if (advancedSparseProvider.solve(deltas, lambda))
			{
				oneValidIteration = true;

				// check whether the offset has been converted
				if (Numeric::isEqualEps(deltas.norm() / Scalar(deltas.elements())))
				{
					i = iterations;
				}

				// we apply the deltas by: new = old - deltas
				advancedSparseProvider.applyCorrection(deltas);

				const Scalar iterationError = (useLevenbergMarquardt || intermediateErrors) ? advancedSparseProvider.determineError() : Numeric::minValue();

				// check whether a Levenberg-Marquardt approach is intended and then check whether the new error is not better than the best one
				if (useLevenbergMarquardt && iterationError >= bestError)
				{
					// modify the lambda parameter and start a new optimization
					if (lambda > 0 && lambda <= maxLambda)
					{
						lambda *= lambdaFactor;
					}
					else
					{
						ocean_assert(oneValidIteration && "At this moment we should have at least one valid iteration!");

						// no further improvement can be applied
						i = iterations;
					}

					continue;
				}

				// we have an improvement, or we have a Gauss-Newton optimization so that we do not store the previous iteration
				bestError = iterationError;

				if (intermediateErrors != nullptr)
				{
					intermediateErrors->push_back(bestError);
				}

				advancedSparseProvider.acceptCorrection();

				if (useLevenbergMarquardt && Numeric::isNotEqualEps(lambdaFactor))
				{
					// we do not decrease lambda if lambda is already near to zero so that we simply should stop optimization if we fail to reduce the error
					if (lambda > Numeric::eps())
					{
						lambda /= lambdaFactor;
					}
				}

				//  skip this inner loop here as a new jacobian has to be calculated
				break;
			}
			else if (lambda > Numeric::eps() && lambda <= maxLambda)
			{
				lambda *= lambdaFactor;
			}
			else
			{
				ocean_assert(oneValidIteration && "At this moment we should have at least one valid iteration!");

				// no further improvement can be applied
				i = iterations;
			}
		}
	}

	if (finalError)
	{
		// if Levenberg-Marquardt is not intended we have to determine the error as we did not do it before
		if (!useLevenbergMarquardt && !intermediateErrors)
		{
			bestError = advancedSparseProvider.determineError();
		}

		*finalError = bestError;
	}

	return oneValidIteration;
}

template <Estimator::EstimatorType tEstimator>
Scalar NonLinearOptimization::sqrErrors2robustErrors2(const Scalars& sqrErrors, const size_t modelParameters, Vector2* weightedErrors, Vector2* weightVectors, const SquareMatrix2* transposedInvertedCovariances)
{
	// determine the sigma ideal for the square errors
	const Scalar sqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(sqrErrors.data(), sqrErrors.size(), modelParameters)) : 0;

	Scalar robustError = 0;

	for (size_t n = 0; n < sqrErrors.size(); ++n)
	{
		ocean_assert(Numeric::isEqual(weightedErrors[n].sqr(), sqrErrors[n]));

		// determine the weight for each individual image point, however as e.g., the tukey estimator may return a weight of 0 we have to clamp the weight to ensure that we still can solve the equation
		// **NOTE** the much better way would be to remove the entry from the equation and to solve it
		const Scalar weight = max(Numeric::weakEps(), Estimator::robustWeightSquare<tEstimator>(sqrErrors[n], sqrSigma));

		// increase the total robust error
		if (transposedInvertedCovariances)
		{
			robustError += (transposedInvertedCovariances[n].transposed() * weightedErrors[n]).sqr() * weight;
		}
		else
		{
			robustError += sqrErrors[n] * weight;
		}

		weightedErrors[n] *= weight;

		weightVectors[n] = Vector2(weight, weight);
	}

	// return the averaged robust error
	return robustError / Scalar(sqrErrors.size());
}

template <Estimator::EstimatorType tEstimator, size_t tDimension>
Scalar NonLinearOptimization::sqrErrors2robustErrors(const Scalars& sqrErrors, const size_t modelParameters, StaticBuffer<Scalar, tDimension>* weightedErrors, StaticBuffer<Scalar, tDimension>* weightVectors, const Matrix* transposedInvertedCovariances)
{
	ocean_assert(transposedInvertedCovariances == nullptr && "Currently not implemenated");
	OCEAN_SUPPRESS_UNUSED_WARNING(transposedInvertedCovariances);

	// determine the sigma ideal for the square errors
	const Scalar sqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(sqrErrors.data(), sqrErrors.size(), modelParameters)) : 0;

	Scalar robustError = 0;

	for (size_t n = 0; n < sqrErrors.size(); ++n)
	{
		StaticBuffer<Scalar, tDimension>& weightedErrorsPointer = weightedErrors[n];
		StaticBuffer<Scalar, tDimension>& weightVectorsPointer = weightVectors[n];

		ocean_assert(Numeric::isEqual(Numeric::summedSqr(weightedErrorsPointer.data(), tDimension), sqrErrors[n]));

		// determine the weight for each individual image point, however as e.g., the tukey estimator may return a weight of 0 we have to clamp the weight to ensure that we still can solve the equation
		// **NOTE** the much better way would be to remove the entry from the equation and to solve it
		const Scalar weight = max(Numeric::weakEps(), Estimator::robustWeightSquare<tEstimator>(sqrErrors[n], sqrSigma));

		// increase the total robust error
		robustError += sqrErrors[n] * weight;

		for (size_t d = 0; d < tDimension; ++d)
		{
			weightedErrorsPointer[d] *= weight;
			weightVectorsPointer[d] = weight;
		}
	}

	// return the averaged robust error
	return robustError / Scalar(sqrErrors.size());
}

template <Estimator::EstimatorType tEstimator>
Scalar NonLinearOptimization::sqrErrors2robustErrors_i(const Scalars& sqrErrors, const size_t modelParameters, const size_t dimension, Scalar* weightedErrors_i, Scalar* weightVectors_i, const Matrix* transposedInvertedCovariances_i)
{
	ocean_assert(transposedInvertedCovariances_i == nullptr && "Currently not implemenated");
	OCEAN_SUPPRESS_UNUSED_WARNING(transposedInvertedCovariances_i);

	// determine the sigma ideal for the square errors
	const Scalar sqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(sqrErrors.data(), sqrErrors.size(), modelParameters)) : 0;

	Scalar robustError = 0;

	for (size_t n = 0; n < sqrErrors.size(); ++n)
	{
		Scalar* const weightedErrorsPointer = weightedErrors_i + n * dimension;
		Scalar* const weightVectorsPointer = weightVectors_i + n * dimension;

		ocean_assert(Numeric::isEqual(Numeric::summedSqr(weightedErrorsPointer, dimension), sqrErrors[n]));

		// determine the weight for each individual image point
		const Scalar weight = Estimator::robustWeightSquare<tEstimator>(sqrErrors[n], sqrSigma);

		// increase the total robust error
		robustError += sqrErrors[n] * weight;

		for (size_t d = 0; d < dimension; ++d)
		{
			weightedErrorsPointer[d] *= weight;
			weightVectorsPointer[d] = weight;
		}
	}

	// return the averaged robust error
	return robustError / Scalar(sqrErrors.size());
}

inline Scalar NonLinearOptimization::sqrErrors2robustErrors2(const Estimator::EstimatorType estimator, const Scalars& sqrErrors, const size_t modelParameters, Vector2* weightedErrors, Vector2* weightVectors, const SquareMatrix2* transposedInvertedCovariances)
{
	switch (estimator)
	{
		case Estimator::ET_SQUARE:
			return sqrErrors2robustErrors2<Estimator::ET_SQUARE>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		case Estimator::ET_LINEAR:
			return sqrErrors2robustErrors2<Estimator::ET_LINEAR>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		case Estimator::ET_HUBER:
			return sqrErrors2robustErrors2<Estimator::ET_HUBER>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		case Estimator::ET_TUKEY:
			return sqrErrors2robustErrors2<Estimator::ET_TUKEY>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		case Estimator::ET_CAUCHY:
			return sqrErrors2robustErrors2<Estimator::ET_CAUCHY>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		default:
			ocean_assert(false && "Invalid estimator!");
			return Numeric::maxValue();
	}
}

template <size_t tDimension>
inline Scalar NonLinearOptimization::sqrErrors2robustErrors(const Estimator::EstimatorType estimator, const Scalars& sqrErrors, const size_t modelParameters, StaticBuffer<Scalar, tDimension>* weightedErrors, StaticBuffer<Scalar, tDimension>* weightVectors, const Matrix* transposedInvertedCovariances)
{
	switch (estimator)
	{
		case Estimator::ET_SQUARE:
			return sqrErrors2robustErrors<Estimator::ET_SQUARE, tDimension>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		case Estimator::ET_LINEAR:
			return sqrErrors2robustErrors<Estimator::ET_LINEAR, tDimension>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		case Estimator::ET_HUBER:
			return sqrErrors2robustErrors<Estimator::ET_HUBER, tDimension>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		case Estimator::ET_TUKEY:
			return sqrErrors2robustErrors<Estimator::ET_TUKEY, tDimension>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		case Estimator::ET_CAUCHY:
			return sqrErrors2robustErrors<Estimator::ET_CAUCHY, tDimension>(sqrErrors, modelParameters, weightedErrors, weightVectors, transposedInvertedCovariances);

		default:
			ocean_assert(false && "Invalid estimator!");
			return Numeric::maxValue();
	}
}

inline Scalar NonLinearOptimization::sqrErrors2robustErrors_i(const Estimator::EstimatorType estimator, const Scalars& sqrErrors, const size_t modelParameters, const size_t dimension, Scalar* weightedErrors_i, Scalar* weightVectors_i, const Matrix* transposedInvertedCovariances_i)
{
	switch (estimator)
	{
		case Estimator::ET_SQUARE:
			return sqrErrors2robustErrors_i<Estimator::ET_SQUARE>(sqrErrors, modelParameters, dimension, weightedErrors_i, weightVectors_i, transposedInvertedCovariances_i);

		case Estimator::ET_LINEAR:
			return sqrErrors2robustErrors_i<Estimator::ET_LINEAR>(sqrErrors, modelParameters, dimension, weightedErrors_i, weightVectors_i, transposedInvertedCovariances_i);

		case Estimator::ET_HUBER:
			return sqrErrors2robustErrors_i<Estimator::ET_HUBER>(sqrErrors, modelParameters, dimension, weightedErrors_i, weightVectors_i, transposedInvertedCovariances_i);

		case Estimator::ET_TUKEY:
			return sqrErrors2robustErrors_i<Estimator::ET_TUKEY>(sqrErrors, modelParameters, dimension, weightedErrors_i, weightVectors_i, transposedInvertedCovariances_i);

		case Estimator::ET_CAUCHY:
			return sqrErrors2robustErrors_i<Estimator::ET_CAUCHY>(sqrErrors, modelParameters, dimension, weightedErrors_i, weightVectors_i, transposedInvertedCovariances_i);

		default:
			ocean_assert(false && "Invalid estimator!");
			return Numeric::maxValue();
	}
}

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_H
