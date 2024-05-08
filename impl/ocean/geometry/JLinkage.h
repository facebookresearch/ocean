/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_JLINKAGE_H
#define META_OCEAN_GEOMETRY_JLINKAGE_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Subset.h"

#include "ocean/math/Line2.h"
#include "ocean/math/Matrix.h"

#include <iterator>

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements several J-linkage functions for pose/homography determination.
 * J-linkage aims to fit multiple models from data given
 * The J-linkage algorithm is an agglomerative clustering that proceeds by linking elements with Jaccard distance smaller than 1 and stop as soon as there are no such elements left.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT JLinkage
{
	protected:

		/**
		 * Definition of a pair combining a point index with the distance to a reference.
		 */
		typedef std::pair<Scalar, Index32> DistancePair;

	public:

		/**
		 * Calculates multiple homographies between two images transforming the projected planar object points between the two images using J-linkage
		 * The resulting homographies transform its image points defined in the left image to image points defined in the right image (rightPoint_i = H_i * leftPoint_i).<br>
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera
		 * @param correspondences Number of points correspondences
		 * @param width The width of the left image in pixel
		 * @param height The height of the left image in pixel
		 * @param homographies Resulting valid homographies for each image plane
		 * @param testCandidates Number of candidates used in for minimum sample set, with range [4, correspondences]
		 * @param leftPointForInitialModels Initial image points, one per each sample set
		 * @param squarePixelErrorAssignmentThreshold Maximal square pixel error of a valid projection of a 3D point onto the 2D image plane, with range (0, infinity)
		 * @param usedIndicesPerHomography Optional set of indices which will receive the indices of the used image correspondences per model, if defined
		 * @param refineHomographies Determines whether a not linear least square algorithm is used to increase the pose accuracies after J-linkage
		 * @param approximatedNeighborSearch Defines if speeded up spatial neighbor search is used
		 * @param randomGenerator Random number generator. If is not nullptr, RANSAC is used for homography determination within initial minimum sample set
		 * @return True, if successfully completed
		 */
		static bool homographyMatrices(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const unsigned int width, const unsigned int height, SquareMatrices3& homographies, const unsigned int testCandidates, const ImagePoints& leftPointForInitialModels, const Scalar squarePixelErrorAssignmentThreshold, std::vector<IndexSet32>* usedIndicesPerHomography = nullptr, bool refineHomographies = true, bool approximatedNeighborSearch = true, Ocean::RandomGenerator* randomGenerator = nullptr);

		/**
		 * Calculates multiple homographies between two images transforming the projected planar object points between the two images using J-linkage
		 * The resulting homographies transform its image points defined in the left image to image points defined in the right image (rightPoint_i = H_i * leftPoint_i).<br>
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera
		 * @param correspondences Number of points correspondences
		 * @param width The width of the left image in pixel
		 * @param height The height of the left image in pixel
		 * @param homographies Resulting valid homographies for each image plane
		 * @param testCandidates Number of candidates used in for minimum sample set, with range [4, correspondences]
		 * @param leftPointIndicesForInitialModels Initial indices of image points, one per each sample set
		 * @param squarePixelErrorAssignmentThreshold Maximal square pixel error of a valid projection of a 3D point onto the 2D image plane, with range (0, infinity)
		 * @param usedIndicesPerHomography Optional set of indices which will receive the indices of the used image correspondences per model, if defined
		 * @param refineHomographies Determines whether a not linear least square algorithm is used to increase the pose accuracies after J-linkage
		 * @param approximatedNeighborSearch Defines if speeded up spatial neighbor search is used
		 * @param randomGenerator Random number generator. If is not nullptr, RANSAC is used for homography determination within initial minimum sample set
		 * @return True, if successfully completed
		 */
		static inline bool homographyMatrices(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const unsigned int width, const unsigned int height, SquareMatrices3& homographies, const unsigned int testCandidates, const Indices32& leftPointIndicesForInitialModels, const Scalar squarePixelErrorAssignmentThreshold, std::vector<IndexSet32>* usedIndicesPerHomography = nullptr, bool refineHomographies = true, bool approximatedNeighborSearch = true, Ocean::RandomGenerator* randomGenerator = nullptr);

		/**
		 * Multiple line detector using J-linkage
		 * @param imagePoints image points
		 * @param pointCount Number of points
		 * @param width The width of the image in pixel
		 * @param height The height of the image in pixel
		 * @param lines Resulting valid line model
		 * @param testCandidates Number of candidates used in for minimum sample set, with range [2, correspondences]
		 * @param pointForInitialModels Initial image points, one per each sample set
		 * @param pixelErrorAssignmentThreshold Maximal pixel error of a point-line corresspodance (0, infinity)
		 * @param usedIndicesPerHomography Optional set of indices which will receive the indices of the used image correspondences per model, if defined
		 * @param approximatedNeighborSearch Defines if speeded up spatial neighbor search is used
		 * @return True, if successfully completed
		 */
		static bool fitLines(const ImagePoint* imagePoints, const size_t pointCount, const unsigned int width, const unsigned int height, Lines2& lines, const unsigned int testCandidates, const ImagePoints& pointForInitialModels, const Scalar pixelErrorAssignmentThreshold, std::vector<IndexSet32>* usedIndicesPerHomography = nullptr, bool approximatedNeighborSearch = true);

	protected:

		/**
		 * Generates minimal sample set for J-/T-Linkage
		 * @param imagePoints Image points
		 * @param pointCount Number of points
		 * @param pointForInitialModels Initial points of minimal sample sets. Note: some maybe be discarded
		 * @param testCandidates Number of candidates used for each minimal sampling set, with range [1, correspondences]
		 * @param distributionImagePoints Spatial distribution for approximate search (if nullptr, brute force methode is used)
		 * @return List of minimal sample sets (represented by indices)
		 */
		static std::vector<Indices32> buildingMinimalSampleSet(const ImagePoint* imagePoints, const size_t pointCount, const ImagePoints& pointForInitialModels, const unsigned int testCandidates, const SpatialDistribution::DistributionArray* distributionImagePoints = nullptr);

		/**
		 * Generates a homography per minimal sample set (for J-/T-Linkage)
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera
		 * @param correspondences Number of points correspondences
		 * @param leftPointForInitialModels Initial points of minimal sample sets. Note: some maybe be discarded
		 * @param testCandidates Number of candidates used for each minimal sampling set, with range [4, correspondences]
		 * @param distributionImagePoints Spatial distribution for approximate search (if nullptr, brute force methode is used)
		 * @param randomRansac Random generator object to be used for creating random numbers, RANSAC is used if this is not null
		 * @return Resulting homographies for each minimal sample set (its count is possibly smaller then the given image points)
		 */
		static SquareMatrices3 buildingMinimalSampleSetHomography(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const ImagePoints& leftPointForInitialModels, const unsigned int testCandidates, const SpatialDistribution::DistributionArray* distributionImagePoints = nullptr, RandomGenerator* randomRansac = nullptr);

		/**
		 * Generates a line model per minimal sample set (for J-/T-Linkage)
		 * @param imagePoints Image points
		 * @param pointCount Number of points
		 * @param pointForInitialModels Initial points of minimal sample sets. Note: some maybe be discarded
		 * @param testCandidates Number of candidates used for each minimal sampling set, with range [2, correspondences]
		 * @param distributionImagePoints Spatial distribution for approximate search (if nullptr, brute force methode is used)
		 * @return Resulting line for each minimal sample set (its count is possibly smaller then the given image points)
		 */
		static Lines2 buildingMinimalSampleSetLine(const ImagePoint* imagePoints, const size_t pointCount, const ImagePoints& pointForInitialModels, const unsigned int testCandidates, const SpatialDistribution::DistributionArray* distributionImagePoints = nullptr);

		/**
		 * Calculates the jaccard distance
		 * d(A, B) = ( |union(A, B)| - |intersection(A, B)| ) / |union(A, B)|.
		 * @param setA first set of indices to compare
		 * @param setB second set of indices to compare
		 * @return Jaccard distance [0, 1]
		 */
		static inline Scalar jaccardDistance(const IndexSet32& setA, const IndexSet32& setB);

		/**
		 * Sorts pairs of indices and their corresponding distances in ascending order regarding the distance values.
		 * @param firstPair First pair of distance and index
		 * @param seccondPair Second pair of distance and index
		 * @return True, if the distance value of the first pair is smaller than distance value of the second pair
		 */
		static inline bool distancePairSortAscending(const DistancePair& firstPair, const DistancePair& seccondPair);
};

/**
 * This class implements L-linkage functions for pose/homography determination.
 * T-linkage aims to fit multiple models from data given
 * The T-linkage algorithm is an agglomerative clustering that proceeds by linking elements with tanimoto distance smaller than 1 and stop as soon as there are no such elements left.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT TLinkage : JLinkage
{
	public:

		/**
		 * Calculates multiple homographies between two images transforming the projected planar object points between the two images using T-linkage
		 * The resulting homographies transform its image points defined in the left image to image points defined in the right image (rightPoint_i = H_i * leftPoint_i).<br>
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera
		 * @param correspondences Number of points correspondences
		 * @param homographies Resulting valid homographies for each image plane
		 * @param testCandidates Number of candidates used in for minimum sample set, with range [8, correspondences]
		 * @param leftPointForInitialModels Initial image points, one per each sample set
		 * @param pixelAssignmentRadius Maximal pixel radius of image points being considered [5, image width)
		 * @param usedIndicesPerHomography Optional set of indices which will receive the indices of the used image correspondences per model, if defined
		 * @param refineHomographies Determines whether a not linear least square algorithm is used to increase the pose accuracies after J-linkage
		 * @param randomGenerator Random number generator. If is not nullptr, RANSAC is used for homography refinement
		 * @return True, if successfully completed
		 */
		static bool homographyMatrices(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, SquareMatrices3& homographies, const unsigned int testCandidates, const ImagePoints& leftPointForInitialModels, const Scalar pixelAssignmentRadius, std::vector<IndexSet32>* usedIndicesPerHomography = nullptr, bool refineHomographies = true, Ocean::RandomGenerator* randomGenerator = nullptr);

		/**
		 * Multiple line detector using T-linkage
		 * @param imagePoints image points
		 * @param pointCount Number of points
		 * @param lines Resulting valid line model
		 * @param testCandidates Number of candidates used in for minimum sample set, with range [2, correspondences]
		 * @param pointForInitialModels Initial image points, one per each sample set
		 * @param pixelErrorAssignmentThreshold Maximal pixel error of a point-line corresspodance (0, infinity)
		 * @param usedIndicesPerHomography Optional set of indices which will receive the indices of the used image correspondences per model, if defined
		 * @return True, if successfully completed
		 */
		static bool fitLines(const ImagePoint* imagePoints, const size_t pointCount, Lines2& lines, const unsigned int testCandidates, const ImagePoints& pointForInitialModels, const Scalar pixelErrorAssignmentThreshold, std::vector<IndexSet32>* usedIndicesPerHomography = nullptr);

	private:

		/**
		 * Calculates the tanimoto distance
		 * d(A, B) = 1 - A*B / (A*A + B*B - A*B).<BR>
		 * A and B must have the same amount of elements
		 * @param vectorA first vector to compare
		 * @param vectorB second vector to compare
		 * @return Tanimoto distance [0, 1]
		 */
		static inline Scalar tanimotoDistance(const Matrix& vectorA, const Matrix& vectorB);
};

inline bool JLinkage::homographyMatrices(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const unsigned int width, const unsigned int height, SquareMatrices3 & homographies, const unsigned int testCandidates, const Indices32& leftPointIndicesForInitialModels, const Scalar squarePixelErrorAssignmentThreshold, std::vector<IndexSet32>* usedIndicesPerHomography, bool refineHomographies, bool approximatedNeighborSearch, Ocean::RandomGenerator* randomGenerator)
{
	const ImagePoints leftPointForInitialModels(Subset::subset(leftImagePoints, correspondences, leftPointIndicesForInitialModels));

	return homographyMatrices(leftImagePoints, rightImagePoints, correspondences, width, height, homographies, testCandidates, leftPointForInitialModels, squarePixelErrorAssignmentThreshold, usedIndicesPerHomography, refineHomographies, approximatedNeighborSearch, randomGenerator);
}

inline Scalar JLinkage::jaccardDistance(const IndexSet32& setA, const IndexSet32& setB)
{
	if (setA.empty() || setB.empty())
	{
		return 0;
	}

	Indices32 intersectionSet;
	std::set_intersection(setA.cbegin(), setA.cend(), setB.cbegin(), setB.cend(), std::back_inserter(intersectionSet));

	Indices32 unionSet;
	std::set_union(setA.cbegin(), setA.cend(), setB.cbegin(), setB.cend(), std::back_inserter(unionSet));

	const Scalar numberIntersections = Scalar(intersectionSet.size());
	const Scalar numberUnion = Scalar(unionSet.size());

	ocean_assert(unionSet.size() >= 1);
	return (numberUnion - numberIntersections) / numberUnion;
}

inline bool JLinkage::distancePairSortAscending(const DistancePair& firstPair, const DistancePair& seccondPair)
{
	return firstPair.first < seccondPair.first;
}

inline Scalar TLinkage::tanimotoDistance(const Matrix& vectorA, const Matrix& vectorB)
{
	ocean_assert((vectorA.columns() == 1u || vectorA.rows() == 1u) && (vectorB.columns() == 1u || vectorB.rows() == 1u));
	ocean_assert(vectorA.elements() == vectorB.elements());

	size_t length = vectorA.elements();

	Scalar squaredNormAB(0);
	Scalar squaredNormA(0);
	Scalar squaredNormB(0);

	for (size_t n = 0u; n < length; ++n)
	{
		squaredNormA += Numeric::sqr(vectorA.data()[n]);
		squaredNormB += Numeric::sqr(vectorB.data()[n]);
		squaredNormAB += vectorA.data()[n] * vectorB.data()[n];
	}

	return (1 - Numeric::ratio(squaredNormAB, squaredNormA + squaredNormB - squaredNormAB));
}

}

}

#endif // META_OCEAN_GEOMETRY_JLINKAGE_H
