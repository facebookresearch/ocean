/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/JLinkage.h"
#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimizationHomography.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/base/Utilities.h"

namespace Ocean
{

namespace Geometry
{

std::vector<Indices32> JLinkage::buildingMinimalSampleSet(const ImagePoint* imagePoints, const size_t pointCount, const ImagePoints& pointForInitialModels, const unsigned int testCandidates, const SpatialDistribution::DistributionArray* distributionImagePoints)
{
	ocean_assert(testCandidates > 0u);

	std::vector<Indices32> result;

	for (size_t m = 0; m < pointForInitialModels.size(); ++m)
	{
		const Geometry::ImagePoint& firstPointLocation = pointForInitialModels[m];

		Indices32 minimumSampleSet;

		if (distributionImagePoints == nullptr)
		{
			// get {testCandidates-1} nearest neighbors to first element
			std::vector<DistancePair> distanceToSeed(pointCount);

			for (Index32 i = 0u; i < Index32(pointCount); i++)
			{
				distanceToSeed[i].first = imagePoints[i].sqrDistance(firstPointLocation);
				distanceToSeed[i].second = i;
			}

			std::sort(distanceToSeed.begin(), distanceToSeed.end(), distancePairSortAscending);

			for (unsigned int i = 0; i < testCandidates; i++)
			{
				minimumSampleSet.push_back(distanceToSeed[i].second);
			}
		}
		else
		{
			// get {testCandidates} elements nearby to first element
			const Scalar maxRatio = distributionImagePoints->width() / Scalar(distributionImagePoints->verticalBins());

			minimumSampleSet = Geometry::SpatialDistribution::determineNeighbors(firstPointLocation, imagePoints, pointCount, maxRatio, *distributionImagePoints);

			if (minimumSampleSet.size() < testCandidates)
			{
				continue;
			}
		}

		ocean_assert(minimumSampleSet.size() >= testCandidates);

		result.push_back(minimumSampleSet);
	}

	return result;
}

SquareMatrices3 JLinkage::buildingMinimalSampleSetHomography(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const ImagePoints& leftPointForInitialModels, const unsigned int testCandidates, const SpatialDistribution::DistributionArray* distributionImagePoints, RandomGenerator* randomRansac)
{
	ocean_assert(leftImagePoints && rightImagePoints);
	ocean_assert(testCandidates >= 4u && correspondences >= testCandidates);

	SquareMatrices3 homographies;

	std::vector<Indices32> minimumSampleSets = buildingMinimalSampleSet(leftImagePoints, correspondences, leftPointForInitialModels, testCandidates, distributionImagePoints);

	for (size_t m = 0; m < minimumSampleSets.size(); ++m)
	{
		Indices32& minimumSampleSet = minimumSampleSets[m];

		SquareMatrix3 mssHomography;
		if (randomRansac == nullptr || minimumSampleSet.size() < 2 * testCandidates)
		{
			//resize to testCandidates, if larger
			minimumSampleSet.resize(testCandidates);

			// determine homography per minimal sample set
			const ImagePoints mssLeftImagePoints(Subset::subset(leftImagePoints, correspondences, minimumSampleSet));
			const ImagePoints mssRightImagePoints(Subset::subset(rightImagePoints, correspondences, minimumSampleSet));

			if (Homography::homographyMatrix(mssLeftImagePoints.data(), mssRightImagePoints.data(), testCandidates, mssHomography))
			{
				if (mssHomography.data()[0] < 0)
				{
					for (unsigned int i = 0; i < 9; i++)
					{
						mssHomography.data()[i] = -mssHomography.data()[i];
					}
				}

				homographies.push_back(mssHomography);
			}
		}
		else
		{
			// determine homography per minimal sample set using RANSAC
			const ImagePoints mssLeftImagePoints(Subset::subset(leftImagePoints, correspondences, minimumSampleSet));
			const ImagePoints mssRightImagePoints(Subset::subset(rightImagePoints, correspondences, minimumSampleSet));

			if (RANSAC::homographyMatrix(mssLeftImagePoints.data(), mssRightImagePoints.data(), mssLeftImagePoints.size(), *randomRansac, mssHomography, testCandidates, false, (unsigned int)(mssLeftImagePoints.size()) / testCandidates))
			{
				homographies.push_back(mssHomography);
			}
		}
	}

	return homographies;
}

Lines2 JLinkage::buildingMinimalSampleSetLine(const ImagePoint* imagePoints, const size_t pointCount, const ImagePoints& pointForInitialModels, const unsigned int testCandidates, const SpatialDistribution::DistributionArray* distributionImagePoints)
{
	ocean_assert(imagePoints);
	ocean_assert(testCandidates >= 2u && pointCount >= testCandidates);

	const std::vector<Indices32> minimumSampleSets(buildingMinimalSampleSet(imagePoints, pointCount, pointForInitialModels, testCandidates, distributionImagePoints));

	Lines2 lines;
	lines.reserve(minimumSampleSets.size());

	Line2 line;
	for (size_t m = 0; m < minimumSampleSets.size(); ++m)
	{
		const Indices32& minimumSampleSet = minimumSampleSets[m];

		const ImagePoints mssImagePoints(Subset::subset(imagePoints, pointCount, minimumSampleSet));

		if (Line2::fitLineLeastSquare(mssImagePoints.data(), mssImagePoints.size(), line))
		{
			lines.push_back(line);
		}
	}

	return lines;
}

bool JLinkage::homographyMatrices(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const unsigned int width, const unsigned int height, SquareMatrices3& homographies, const unsigned int testCandidates, const ImagePoints& leftPointForInitialModels, const Scalar squarePixelErrorAssignmentThreshold, std::vector<IndexSet32>* usedIndicesPerHomography,  bool refineHomographies, bool approximatedNeighborSearch, Ocean::RandomGenerator* randomGenerator)
{
	ocean_assert(squarePixelErrorAssignmentThreshold > 0);
	ocean_assert(leftImagePoints && rightImagePoints);
	ocean_assert(testCandidates >= 4u && correspondences >= testCandidates);
	ocean_assert(leftPointForInitialModels.size() < correspondences);

	if (approximatedNeighborSearch)
	{
		unsigned int grids = 16u;

		do
		{
			const SpatialDistribution::DistributionArray distributionImagePoints(SpatialDistribution::distributeToArray(leftImagePoints, correspondences, Scalar(0), Scalar(0), Scalar(width), Scalar(height), grids, grids));
			homographies = buildingMinimalSampleSetHomography(leftImagePoints, rightImagePoints, correspondences, leftPointForInitialModels, testCandidates, &distributionImagePoints, randomGenerator);
			grids = grids >> 1u;
		}
		while (grids > 0u && homographies.size() < 1);
	}
	else
	{
		homographies = buildingMinimalSampleSetHomography(leftImagePoints, rightImagePoints, correspondences, leftPointForInitialModels, testCandidates);
	}

	size_t modelCounts = homographies.size();

	if (modelCounts == 0u)
	{
		SquareMatrix3 globalHomography;
		if (Homography::homographyMatrix(leftImagePoints, rightImagePoints, correspondences, globalHomography))
		{
			homographies.push_back(globalHomography);
		}
		modelCounts = 1u;
	}

	// determining consensus/ preference set matrix
	std::vector<IndexSet32> preferenceSets(modelCounts);

	for (size_t p = 0; p < correspondences; p++)
	{
		const Vector2& leftPoint = leftImagePoints[p];
		const Vector2& rightPoint = rightImagePoints[p];

		for (Index32 m = 0; m < Index32(modelCounts); m++)
		{
			const SquareMatrix3& mssHomography = homographies[m];
			const Scalar squareError = (mssHomography * leftPoint).sqrDistance(rightPoint);

			if (squareError < squarePixelErrorAssignmentThreshold)
			{
				preferenceSets[m].insert((unsigned int)(p));
			}
		}
	}

	// perform agglomerative clustering
	while (true)
	{
		// go thru all preference set and merge set with minimal jaccard distance

		Scalar minJaccardDistance(1);
		IndexPair32 minIndex(0, 0);
		Index32 len = Index32(preferenceSets.size());

		for (Index32 a = 0; a < len; a++)
		{
			for (Index32 b = a + 1u; b < len; b++)
			{
				const Scalar currentJaccardDistance(jaccardDistance(preferenceSets[a], preferenceSets[b]));
				if (currentJaccardDistance < minJaccardDistance)
				{
					minJaccardDistance = currentJaccardDistance;
					minIndex.first = a;
					minIndex.second = b;
				}
			}
		}

		if (Numeric::isEqual(minJaccardDistance, Scalar(1)))
		{
			break;
		}

		// merge clusters/ models with minimal jaccard distance
		for (IndexSet32::const_iterator it = preferenceSets[minIndex.second].cbegin(); it != preferenceSets[minIndex.second].cend(); ++it)
		{
			preferenceSets[minIndex.first].insert(*it);
		}

		preferenceSets.erase(preferenceSets.begin() + minIndex.second);
		homographies.erase(homographies.begin() + minIndex.second);

#ifdef OCEAN_DEBUG
		MatrixT<Scalar> preferenceConsensusMatrixDebug(correspondences, preferenceSets.size()); // MatrixT_<unsigned int>
		memset(preferenceConsensusMatrixDebug.data(), 0, preferenceConsensusMatrixDebug.elements() * sizeof(Scalar));

		for (size_t i = 0; i < preferenceSets.size(); i++)
		{
			for (IndexSet32::const_iterator it = preferenceSets[i].cbegin(); it != preferenceSets[i].cend(); ++it)
			{
				++preferenceConsensusMatrixDebug[*it][i];
			}
		}
#endif
	}

	// decline set with less then {testCandidates} members
	for (size_t m = 0; m < preferenceSets.size();)
	{
		if (preferenceSets[m].size() < testCandidates)
		{
			preferenceSets.erase(preferenceSets.begin() + m);
			homographies.erase(homographies.begin() + m);
		}
		else
		{
			++m;
		}
	}

	// **TODO** **SM** add constraints

	if (preferenceSets.empty())
	{
		return false;
	}

	if (refineHomographies)
	{
		for (size_t m = 0; m < preferenceSets.size(); ++m)
		{
			const IndexSet32& set = preferenceSets[m];

			ocean_assert(!set.empty());

			if (set.size() >= 8)
			{
				const ImagePoints setLeftImagePoints(Subset::subset(leftImagePoints, correspondences, set));
				const ImagePoints setRightImagePoints(Subset::subset(rightImagePoints, correspondences, set));

				if (randomGenerator != nullptr)
				{
					SquareMatrix3 homography;
					if (RANSAC::homographyMatrix(setLeftImagePoints.data(), setRightImagePoints.data(), set.size(), *randomGenerator, homography, testCandidates, true, 20u, squarePixelErrorAssignmentThreshold))
					{
						homographies[m] = homography;
					}
				}
				else
				{
					SquareMatrix3 optimizedHomography;
					if (NonLinearOptimizationHomography::optimizeHomography(homographies[m], setLeftImagePoints.data(), setRightImagePoints.data(), set.size(), 9u, optimizedHomography, 20u, Geometry::Estimator::ET_SQUARE))
					{
						homographies[m] = optimizedHomography;
					}
				}
			}
		}
	}

	if (usedIndicesPerHomography)
	{
		*usedIndicesPerHomography = preferenceSets;
	}

	return true;
}

bool JLinkage::fitLines(const ImagePoint* imagePoints, const size_t pointCount, const unsigned int width, const unsigned int height, Lines2& lines, const unsigned int testCandidates, const ImagePoints& pointForInitialModels, const Scalar pixelErrorAssignmentThreshold, std::vector<IndexSet32>* usedIndicesPerHomography, bool approximatedNeighborSearch)
{
	ocean_assert(pixelErrorAssignmentThreshold > 0);
	ocean_assert(imagePoints);
	ocean_assert(testCandidates >= 2u && pointCount >= testCandidates);
	ocean_assert(pointForInitialModels.size() < pointCount);

	if (approximatedNeighborSearch)
	{
		unsigned int grids = 16u;

		do
		{
			const SpatialDistribution::DistributionArray distributionImagePoints(SpatialDistribution::distributeToArray(imagePoints, pointCount, Scalar(0), Scalar(0), Scalar(width), Scalar(height), grids, grids));
			lines = buildingMinimalSampleSetLine(imagePoints, pointCount, pointForInitialModels, testCandidates, &distributionImagePoints);

			grids /= 2u;
		}
		while (grids > 0u && lines.size() < 1);
	}
	else
	{
		lines = buildingMinimalSampleSetLine(imagePoints, pointCount, pointForInitialModels, testCandidates);
	}

	const size_t modelCounts = lines.size();

	if (modelCounts == 0)
	{
		Line2 globalLine;
		if (Line2::fitLineLeastSquare(imagePoints, pointCount, globalLine))
		{
			lines.push_back(globalLine);
		}
	}

	// determining consensus/ preference set matrix
	std::vector<IndexSet32> preferenceSets(modelCounts);

	for (size_t p = 0; p < pointCount; p++)
	{
		const Vector2& point = imagePoints[p];

		for (Index32 m = 0; m < Index32(modelCounts); m++)
		{
			const Line2& mssLine = lines[m];

			const Scalar error = mssLine.distance(point);

			if (error < pixelErrorAssignmentThreshold)
			{
				preferenceSets[m].insert((unsigned int)(p));
			}
		}
	}

	// perform agglomerative clustering

	while (true)
	{
		// go thru all preference set and merge set with minimal jaccard distance
		Scalar minJaccardDistance(1);
		IndexPair32 minIndex(0, 0);
		Index32 len = Index32(preferenceSets.size());

		for (Index32 a = 0; a < len; a++)
		{
			for (Index32 b = a + 1u; b < len; b++)
			{
				const Scalar currentJaccardDistance(jaccardDistance(preferenceSets[a], preferenceSets[b]));
				if (currentJaccardDistance < minJaccardDistance)
				{
					minJaccardDistance = currentJaccardDistance;
					minIndex.first = a;
					minIndex.second = b;
				}
			}
		}

		if (Numeric::isEqual(minJaccardDistance, Scalar(1)))
		{
			break;
		}

		// merge clusters/ models with minimal jaccard distance
		for (IndexSet32::const_iterator it = preferenceSets[minIndex.second].cbegin(); it != preferenceSets[minIndex.second].cend(); ++it)
		{
			preferenceSets[minIndex.first].insert(*it);
		}
		preferenceSets.erase(preferenceSets.begin() + minIndex.second);
		lines.erase(lines.begin() + minIndex.second);

#ifdef OCEAN_DEBUG
		MatrixT<Scalar> preferenceConsensusMatrixDebug(pointCount,  preferenceSets.size()); // MatrixT_<unsigned int>
		memset(preferenceConsensusMatrixDebug.data(), 0, preferenceConsensusMatrixDebug.elements() * sizeof(Scalar));

		for (size_t i = 0; i < preferenceSets.size(); i++)
		{
			for (IndexSet32::const_iterator it = preferenceSets[i].cbegin(); it != preferenceSets[i].cend(); ++it)
			{
				++preferenceConsensusMatrixDebug[*it][i];
			}
		}
#endif
	}

	// decline set with less then {testCandidates} members
	for (size_t m = 0; m < preferenceSets.size();)
	{
		if (preferenceSets[m].size() < testCandidates)
		{
			preferenceSets.erase(preferenceSets.begin() + m);
			lines.erase(lines.begin() + m);
		}
		else
		{
			++m;
		}
	}

	if (preferenceSets.empty())
	{
		return false;
	}

	if (usedIndicesPerHomography)
	{
		*usedIndicesPerHomography = preferenceSets;
	}

	return true;
}

bool TLinkage::homographyMatrices(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, SquareMatrices3& homographies, const unsigned int testCandidates, const ImagePoints& leftPointForInitialModels, const Scalar pixelAssignmentRatio, std::vector<IndexSet32>* usedIndicesPerHomography, bool refineHomographies, Ocean::RandomGenerator* randomGenerator)
{
	ocean_assert(pixelAssignmentRatio > 0);
	ocean_assert(leftImagePoints && rightImagePoints);
	ocean_assert(testCandidates >= 8u && correspondences >= testCandidates);
	ocean_assert(leftPointForInitialModels.size() < correspondences);

	const Scalar invertedInlierThreshold_5 = (Scalar(5) / pixelAssignmentRatio);

	homographies = buildingMinimalSampleSetHomography(leftImagePoints, rightImagePoints, correspondences, leftPointForInitialModels, testCandidates);

	const unsigned int modelCounts = (unsigned int)homographies.size();

	// determining preference function matrix and initial tanimoto distance matrix

	Matrix preferenceMatrix(correspondences, modelCounts);
	for (size_t p = 0; p < correspondences; p++)
	{
		const Vector2& leftPoint = leftImagePoints[p];
		const Vector2& rightPoint = rightImagePoints[p];

		for (unsigned int m = 0u; m < modelCounts; m++)
		{
			const SquareMatrix3& mssHomography = homographies[m];
			const Scalar error = (mssHomography * leftPoint).distance(rightPoint);

			if (error < pixelAssignmentRatio)
			{
				preferenceMatrix(p, m) = Numeric::exp(-error * invertedInlierThreshold_5);
			}
			else
			{
				preferenceMatrix(p, m) = 0;
			}
		}
	}
	preferenceMatrix.transpose();

	Matrix tanimotoMatrix(modelCounts, modelCounts);
	for (unsigned int i = 0u; i < modelCounts; i++)
	{
		tanimotoMatrix(i, i) = 0;
		Matrix rowI = preferenceMatrix.row(i);

		for (unsigned int j = i + 1u; j < modelCounts; j++)
		{
			const Scalar distance = tanimotoDistance(rowI, preferenceMatrix.row(j));
			tanimotoMatrix(i, j) = distance;
			tanimotoMatrix(j, i) = distance;
		}
	}

	// performance agglomerative clustering

	Indices32 deleteModels;

	while (true)
	{
		// go thru preference function matrix and merge columns with minimal tanimoto distance

		Scalar minTanimotoDistance(2);
		IndexPair32 minIndex(0, 0);

		for (unsigned int m = 0u; m < modelCounts; m++)
		{
			for (unsigned int n = m + 1u; n < modelCounts; n++)
			{
				const Scalar& tanimotoDistance(tanimotoMatrix[m][n]);
				if (tanimotoDistance < minTanimotoDistance)
				{
					minTanimotoDistance = tanimotoDistance;
					minIndex.first = m;
					minIndex.second = n;
				}
			}
		}

		if (minTanimotoDistance > Scalar(0.9999))
		{
			break;
		}

		ocean_assert(!hasElement(deleteModels, minIndex.second));

		// merge second with first

		for (size_t p = 0; p < correspondences; p++)
		{
			preferenceMatrix(minIndex.first, p) = Scalar(0.5) * (preferenceMatrix(minIndex.first, p) + preferenceMatrix(minIndex.second, p));
		}

		// clear  second
		memset(preferenceMatrix.data() + preferenceMatrix.columns() * minIndex.second, 0, sizeof(Scalar) * preferenceMatrix.columns());

		deleteModels.push_back(minIndex.second);

		// update tanimoto distance matrix
		Matrix row = preferenceMatrix.row(minIndex.first);

		for (unsigned int i = 0u; i < modelCounts; i++)
		{
			if (!hasElement(deleteModels, i) && i != minIndex.first)
			{
				const Scalar distance = tanimotoDistance(row, preferenceMatrix.row(i));
				tanimotoMatrix(i, minIndex.first) = distance;
				tanimotoMatrix(minIndex.first, i) = distance;
			}

			tanimotoMatrix(i, minIndex.second) = 2;
			tanimotoMatrix(minIndex.second, i) = 2;
		}

	}

	std::vector<IndexSet32> preferenceSets(modelCounts);

	unsigned int i = 0u;
	for (unsigned int m = 0u; m < modelCounts; m++)
	{
		for (unsigned int p = 0; p < (unsigned int)correspondences; p++)
		{
			if (preferenceMatrix(m, p) > 0)
			{
				preferenceSets[i].insert(p);
			}
		}

		if (preferenceSets[i].empty())
		{
			homographies.erase(homographies.begin() + i);
			preferenceSets.erase(preferenceSets.end() - 1u);
		}
		else
		{
			++i;
		}
	}

	if (preferenceSets.empty())
	{
		return false;
	}

	if (refineHomographies)
	{
		for (size_t m = 0; m < preferenceSets.size(); ++m)
		{
			const IndexSet32& set = preferenceSets[m];

			ocean_assert(!set.empty());
			if (set.size() >= 8)
			{
				const ImagePoints setLeftImagePoints(Subset::subset(leftImagePoints, correspondences, set));
				const ImagePoints setRightImagePoints(Subset::subset(rightImagePoints, correspondences, set));

				if (randomGenerator != nullptr)
				{
					SquareMatrix3 homography;

					if (RANSAC::homographyMatrix(setLeftImagePoints.data(), setRightImagePoints.data(), set.size(), *randomGenerator, homography, testCandidates, true, 20u, Numeric::sqr(Scalar(0.2) * pixelAssignmentRatio)))
					{
						homographies[m] = homography;
					}
				}
				else
				{
					SquareMatrix3 optimizedHomography;

					if (NonLinearOptimizationHomography::optimizeHomography(homographies[m], setLeftImagePoints.data(), setRightImagePoints.data(), set.size(), 9u, optimizedHomography, 20u, Geometry::Estimator::ET_SQUARE))
					{
						homographies[m] = optimizedHomography;
					}
				}
			}
		}
	}

	if (usedIndicesPerHomography)
	{
		*usedIndicesPerHomography = preferenceSets;
	}

	return true;
}

bool TLinkage::fitLines(const ImagePoint* imagePoints, const size_t pointCount, Lines2& lines, const unsigned int testCandidates, const ImagePoints& pointForInitialModels, const Scalar pixelErrorAssignmentThreshold, std::vector<IndexSet32>* usedIndicesPerHomography)
{
	ocean_assert(pixelErrorAssignmentThreshold > 0);
	ocean_assert(imagePoints);
	ocean_assert(testCandidates >= 2u && pointCount >= testCandidates);
	ocean_assert(pointForInitialModels.size() < pointCount);

	const Scalar invertedInlierThreshold_5 = (Scalar(5) / pixelErrorAssignmentThreshold);

	lines = buildingMinimalSampleSetLine(imagePoints, pointCount, pointForInitialModels, testCandidates);

	size_t modelCounts = lines.size();

	// determining preference function matrix and initial tanimoto distance matrix

	Matrix preferenceMatrix(pointCount, modelCounts);
	for (size_t p = 0; p < pointCount; p++)
	{
		const Vector2& point = imagePoints[p];

		for (unsigned int m = 0u; m < modelCounts; m++)
		{
			const Line2& mssLine = lines[m];

			const Scalar error = mssLine.distance(point);

			if (error < pixelErrorAssignmentThreshold)
			{
				preferenceMatrix(p, m) = Numeric::exp(-error * invertedInlierThreshold_5);
			}
			else
			{
				preferenceMatrix(p, m) = 0;
			}
		}
	}
	preferenceMatrix.transpose();

	Matrix tanimotoMatrix(modelCounts, modelCounts);
	for (unsigned int i = 0; i < modelCounts; i++)
	{
		tanimotoMatrix(i, i) = 0;
		Matrix rowI = preferenceMatrix.row(i);
		for (unsigned int j = i + 1u; j < modelCounts; j++)
		{
			const Scalar distance = tanimotoDistance(rowI, preferenceMatrix.row(j));
			tanimotoMatrix(i, j) = distance;
			tanimotoMatrix(j, i) = distance;
		}
	}

	// perform agglomerative clustering
	Indices32 deleteModels;

	while (true)
	{
		// go thru preference function matrix and merge columns with minimal tanimoto distance
		Scalar minTanimotoDistance(2);
		IndexPair32 minIndex(0, 0);

		for (unsigned int m = 0u; m < modelCounts; m++)
		{
			for (unsigned int n = m + 1u; n < modelCounts; n++)
			{
				const Scalar& tanimotoDistance(tanimotoMatrix[m][n]);
				if (tanimotoDistance < minTanimotoDistance)
				{
					minTanimotoDistance = tanimotoDistance;
					minIndex.first = m;
					minIndex.second = n;
				}
			}
		}

		if (minTanimotoDistance > Scalar(0.9999))
		{
			break;
		}

		ocean_assert(!hasElement(deleteModels, minIndex.second));

		// merge second with first
		for (size_t p = 0; p < pointCount; p++)
		{
			preferenceMatrix(minIndex.first, p) = Scalar(0.5) * (preferenceMatrix(minIndex.first, p) + preferenceMatrix(minIndex.second, p));
		}

		// clear  second
		memset(preferenceMatrix.data() + preferenceMatrix.columns() * minIndex.second, 0, sizeof(Scalar) * preferenceMatrix.columns());

		deleteModels.push_back(minIndex.second);

		// update tanimoto distance matrix
		Matrix row = preferenceMatrix.row(minIndex.first);

		for (unsigned int i = 0; i < modelCounts; i++)
		{
			if (!hasElement(deleteModels, i) && i != minIndex.first)
			{
				const Scalar distance = tanimotoDistance(row, preferenceMatrix.row(i));

				tanimotoMatrix(i, minIndex.first) = distance;
				tanimotoMatrix(minIndex.first, i) = distance;
			}

			tanimotoMatrix(i, minIndex.second) = 2;
			tanimotoMatrix(minIndex.second, i) = 2;
		}
	}

	std::vector<IndexSet32> preferenceSets(modelCounts);

	unsigned int i = 0u;
	for (unsigned int m = 0u; m < modelCounts; m++)
	{
		for (unsigned int p = 0; p < (unsigned int)(pointCount); p++)
		{
			if (preferenceMatrix(m, p) > 0)
			{
				preferenceSets[i].insert(p);
			}
		}

		if (preferenceSets[i].empty())
		{
			lines.erase(lines.begin() + i);
			preferenceSets.erase(preferenceSets.end() - 1u);
		}
		else
		{
			++i;
		}
	}

	if (preferenceSets.empty())
	{
		return false;
	}

	if (usedIndicesPerHomography)
	{
		*usedIndicesPerHomography = preferenceSets;
	}

	return true;
}

} // namespace Geometry

} // namespace Ocean
