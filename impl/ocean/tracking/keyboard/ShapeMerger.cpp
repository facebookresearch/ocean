// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/keyboard/ShapeMerger.h"

#include "ocean/base/Median.h"

#include "ocean/geometry/SpatialDistribution.h"

namespace Ocean
{

namespace Tracking
{

namespace Keyboard
{

ShapeMerger::ShapeMerger() :
	maximalDistance_(-1),
	observations_(0)
{
	// nothing to do here
}

ShapeMerger::ShapeMerger(const unsigned int width, const unsigned int height, const Scalar maximalDistance) :
	maximalDistance_(maximalDistance)
{
	ocean_assert(maximalDistance_ >= 0);

	const Scalar maximalDistance2 = maximalDistance_ * Scalar(2);

	unsigned int horizontalBins = 0u;
	unsigned int verticalBins = 0u;
	Geometry::SpatialDistribution::idealBinsNeighborhood9(width, height, maximalDistance2, horizontalBins, verticalBins, 2u, 2u, width, height);

	if (horizontalBins != 0u && verticalBins != 0u)
	{
		distributionArrayLShapes_ = Geometry::SpatialDistribution::DistributionArray(Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins);
		distributionArrayTShapes_ = Geometry::SpatialDistribution::DistributionArray(Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins);
		distributionArrayXShapes_ = Geometry::SpatialDistribution::DistributionArray(Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins);
	}
}

void ShapeMerger::addObservations(const unsigned int poseIndex, const ConstArrayAccessor<LShape>& lShapes, const ConstArrayAccessor<TShape>& tShapes, const ConstArrayAccessor<XShape>& xShapes)
{
	ocean_assert(maximalDistance_ >= 0);
	ocean_assert(distributionArrayLShapes_ && distributionArrayTShapes_ && distributionArrayXShapes_);

	for (int i = 0; i < lShapes.size(); ++i)
	{
		const LShape& lShape = lShapes[i];
		const unsigned int index = distributionArrayLShapes_.index(lShape.position().x(), lShape.position().y());

		distributionArrayLShapes_[index].push_back(Index32(lShapes_.size()));
		lShapes_.push_back(lShape);
	}

	for (int i = 0; i < tShapes.size(); ++i)
	{
		const TShape& tShape = tShapes[i];
		const unsigned int index = distributionArrayTShapes_.index(tShape.position().x(), tShape.position().y());

		distributionArrayTShapes_[index].push_back(Index32(tShapes_.size()));
		tShapes_.push_back(tShape);
	}

	for (int i = 0; i < xShapes.size(); ++i)
	{
		const XShape& xShape = xShapes[i];
		const unsigned int index = distributionArrayXShapes_.index(xShape.position().x(), xShape.position().y());

		distributionArrayXShapes_[index].push_back(Index32(xShapes_.size()));
		xShapes_.push_back(xShape);
	}

	lShapes2poseIndex_.insert(lShapes2poseIndex_.cend(), lShapes_.size() - lShapes2poseIndex_.size(), poseIndex);
	tShapes2poseIndex_.insert(tShapes2poseIndex_.cend(), tShapes_.size() - tShapes2poseIndex_.size(), poseIndex);
	xShapes2poseIndex_.insert(xShapes2poseIndex_.cend(), xShapes_.size() - xShapes2poseIndex_.size(), poseIndex);

	observations_++;
}

void ShapeMerger::mergeObservations(const unsigned int minimalObservations, LShapes& mergedLShapes, TShapes& mergedTShapes, XShapes& mergedXShapes, ObservationPairGroups* mergedLShapesObservationPairGroups, ObservationPairGroups* mergedTShapesObservationPairGroups, ObservationPairGroups* mergedXShapesObservationPairGroups) const
{
	ocean_assert(maximalDistance_ >= 0);
	ocean_assert(distributionArrayLShapes_ && distributionArrayTShapes_ && distributionArrayXShapes_);

	ocean_assert(minimalObservations >= 1u);

	ocean_assert(mergedLShapes.empty() && mergedTShapes.empty() && mergedXShapes.empty());

	ocean_assert(mergedLShapesObservationPairGroups == nullptr || mergedLShapesObservationPairGroups->empty());
	ocean_assert(mergedTShapesObservationPairGroups == nullptr || mergedTShapesObservationPairGroups->empty());
	ocean_assert(mergedXShapesObservationPairGroups == nullptr || mergedXShapesObservationPairGroups->empty());

	constexpr bool mergeXAndTShapes = true;
	constexpr bool mergeTAndLShapes = true;

	const ShapeMatcher shapeMatcher(maximalDistance_, Numeric::deg2rad(20));

	// first we merge all X-shapes

	Indices32 usedXShapeIndices(xShapes_.size(), 0u);
	Indices32 usedTShapeIndices(tShapes_.size(), 0u);
	Indices32 usedLShapeIndices(lShapes_.size(), 0u);

	Indices32 additionalMergedShapeIndices;

	IndexGroups32 mergedXShapeIndexGroups;
	mergedXShapes = mergeShapes<XShape>(xShapes_, usedXShapeIndices, distributionArrayXShapes_, shapeMatcher, minimalObservations, &mergedXShapeIndexGroups);
	ocean_assert(mergedXShapes.size() == mergedXShapeIndexGroups.size());

	if (mergedXShapesObservationPairGroups)
	{
		mergedXShapesObservationPairGroups->resize(mergedXShapes.size());

		for (size_t n = 0; n < mergedXShapes.size(); ++n)
		{
			for (const Index32& mergedXShapeIndex : mergedXShapeIndexGroups[n])
			{
				const Vector2& observation = xShapes_[mergedXShapeIndex].position();
				const Index32& poseIndex = xShapes2poseIndex_[mergedXShapeIndex];

				(*mergedXShapesObservationPairGroups)[n].emplace_back(poseIndex, observation);
			}
		}
	}

	if (mergeXAndTShapes)
	{
		for (size_t n = 0; n < mergedXShapes.size(); ++n)
		{
			XShape& mergedXShape = mergedXShapes[n];

			additionalMergedShapeIndices.clear();
			mergeShapes<XShape, TShape>(mergedXShape, tShapes_, distributionArrayTShapes_, shapeMatcher, usedTShapeIndices, additionalMergedShapeIndices);

			// we do not merge the X-shape with the T-shapes if the X-shape is less dominant
			if (!additionalMergedShapeIndices.empty() && mergedXShapeIndexGroups[n].size() > additionalMergedShapeIndices.size())
			{
				mergedXShape = ShapeMatcher::mergeXAndTShapes(xShapes_.data(), mergedXShapeIndexGroups[n], tShapes_.data(), additionalMergedShapeIndices);

				for (const Index32& mergedTShapeIndex : additionalMergedShapeIndices)
				{
					ocean_assert(usedTShapeIndices[mergedTShapeIndex] == 0u);
					usedTShapeIndices[mergedTShapeIndex] = 1u;

					const Vector2& observation = tShapes_[mergedTShapeIndex].position();
					const Index32& poseIndex = tShapes2poseIndex_[mergedTShapeIndex];

					if (mergedXShapesObservationPairGroups)
					{
						(*mergedXShapesObservationPairGroups)[n].emplace_back(poseIndex, observation);
					}
				}
			}
		}
	}


	// now, we merge all remaining T-shapes

	IndexGroups32 mergedTShapeIndexGroups;
	mergedTShapes = mergeShapes<TShape>(tShapes_, usedTShapeIndices, distributionArrayTShapes_, shapeMatcher, minimalObservations, &mergedTShapeIndexGroups);
	ocean_assert(mergedTShapes.size() == mergedTShapeIndexGroups.size());

	if (mergedTShapesObservationPairGroups)
	{
		mergedTShapesObservationPairGroups->resize(mergedTShapes.size());

		for (size_t n = 0; n < mergedTShapes.size(); ++n)
		{
			for (const Index32& mergedTShapeIndex : mergedTShapeIndexGroups[n])
			{
				const Vector2& observation = tShapes_[mergedTShapeIndex].position();
				const Index32& poseIndex = tShapes2poseIndex_[mergedTShapeIndex];

				(*mergedTShapesObservationPairGroups)[n].emplace_back(poseIndex, observation);
			}
		}
	}

	if (mergeTAndLShapes)
	{
		for (size_t n = 0; n < mergedTShapes.size(); ++n)
		{
			TShape& mergedTShape = mergedTShapes[n];

			additionalMergedShapeIndices.clear();
			mergeShapes<TShape, LShape>(mergedTShape, lShapes_, distributionArrayLShapes_, shapeMatcher, usedLShapeIndices, additionalMergedShapeIndices);

			// we do not merge the T-shape with the L-shapes if the T-shape is less dominant
			if (!additionalMergedShapeIndices.empty() && mergedTShapeIndexGroups[n].size() > additionalMergedShapeIndices.size())
			{
				mergedTShape = ShapeMatcher::mergeTAndLShapes(tShapes_.data(), mergedTShapeIndexGroups[n], lShapes_.data(), additionalMergedShapeIndices);

				for (const Index32& mergedLShapeIndex : additionalMergedShapeIndices)
				{
					ocean_assert(usedLShapeIndices[mergedLShapeIndex] == 0u);
					usedLShapeIndices[mergedLShapeIndex] = 1u;

					const Vector2& observation = lShapes_[mergedLShapeIndex].position();
					const Index32& poseIndex = lShapes2poseIndex_[mergedLShapeIndex];

					if (mergedTShapesObservationPairGroups)
					{
						(*mergedTShapesObservationPairGroups)[n].emplace_back(poseIndex, observation);
					}
				}
			}
		}
	}


	// now, we merge the remaining L-shapes

	IndexGroups32 mergedLShapeIndexGroups;
	mergedLShapes = mergeShapes<LShape>(lShapes_, usedLShapeIndices, distributionArrayLShapes_, shapeMatcher, minimalObservations, &mergedLShapeIndexGroups);

	if (mergedLShapesObservationPairGroups)
	{
		mergedLShapesObservationPairGroups->resize(mergedLShapes.size());

		for (size_t n = 0; n < mergedLShapes.size(); ++n)
		{
			for (const Index32& mergedLShapeIndex : mergedLShapeIndexGroups[n])
			{
				const Vector2& observation = lShapes_[mergedLShapeIndex].position();
				const Index32& poseIndex = lShapes2poseIndex_[mergedLShapeIndex];

				(*mergedLShapesObservationPairGroups)[n].emplace_back(poseIndex, observation);
			}
		}
	}

	ocean_assert(mergedXShapesObservationPairGroups == nullptr || mergedXShapesObservationPairGroups->size() == mergedXShapes.size());
	ocean_assert(mergedTShapesObservationPairGroups == nullptr || mergedTShapesObservationPairGroups->size() == mergedTShapes.size());
	ocean_assert(mergedLShapesObservationPairGroups == nullptr || mergedLShapesObservationPairGroups->size() == mergedLShapes.size());
}

void ShapeMerger::clear()
{
	observations_ = 0;

	lShapes_.clear();
	tShapes_.clear();
	xShapes_.clear();

	lShapes2poseIndex_.clear();
	tShapes2poseIndex_.clear();
	xShapes2poseIndex_.clear();

	distributionArrayLShapes_.clear();
	distributionArrayTShapes_.clear();
	distributionArrayXShapes_.clear();
}

template <typename T>
std::vector<T> ShapeMerger::mergeShapes(const std::vector<T>& shapes, Indices32& usedShapeIndices, const Geometry::SpatialDistribution::DistributionArray& distributionArray, const ShapeMatcher& shapeMatcher, const unsigned int minimalObservations, IndexGroups32* mergedShapeIndexGroups)
{
	const Scalar maximalDistance2Sqr = Numeric::sqr(shapeMatcher.maximalDistance() * Scalar(2));

	ocean_assert(shapes.size() == usedShapeIndices.size());

	Indices32 indicesNeighborhood;
	Indices32 mergedIndices;

	Scalars xPositions;
	xPositions.reserve(shapes.size() / 128);

	Scalars yPositions;
	yPositions.reserve(shapes.size() / 128);

	std::vector<T> mergedShapes;
	IndexGroups32 mergedShapesIndices;

	for (unsigned int yBin = 0u; yBin < distributionArray.verticalBins(); ++yBin)
	{
		for (unsigned int xBin = 0u; xBin < distributionArray.horizontalBins(); ++xBin)
		{
			const Indices32& indices = distributionArray(xBin, yBin);

			if (indices.size() <= 1)
			{
				continue;
			}

			for (const Index32& index : indices)
			{
				if (usedShapeIndices[index])
				{
					continue;
				}

				const Vector2& position = shapes[index].position();

				indicesNeighborhood.clear();
				distributionArray.indicesNeighborhood9(xBin, yBin, indicesNeighborhood);

				xPositions.clear();
				yPositions.clear();

				for (const Index32& neighborhoodIndex : indicesNeighborhood)
				{
					if (usedShapeIndices[neighborhoodIndex] == 0u)
					{
						const Vector2& neighborhoodPosition = shapes[neighborhoodIndex].position();

						if (neighborhoodPosition.sqrDistance(position) <= maximalDistance2Sqr)
						{
							xPositions.push_back(neighborhoodPosition.x());
							yPositions.push_back(neighborhoodPosition.y());
						}
					}
				}

				// a position withine the point cloud of neighboring shapes
				const Vector2 medianPosition(Median::median(xPositions.data(), xPositions.size()), Median::median(yPositions.data(), yPositions.size()));

				// let's determine the shape closest to the median location

				Scalar bestSqrDistance = maximalDistance2Sqr;
				Index32 bestIndex = (unsigned int)(-1);

				for (const Index32& neighborhoodIndex : indicesNeighborhood)
				{
					if (usedShapeIndices[neighborhoodIndex] == 0u)
					{
						const Vector2& neighborhoodPosition = shapes[neighborhoodIndex].position();

						const Scalar sqrDistance = neighborhoodPosition.sqrDistance(medianPosition);

						if (sqrDistance < bestSqrDistance)
						{
							bestIndex = neighborhoodIndex;
							bestSqrDistance = sqrDistance;
						}
					}
				}

				if (bestIndex != (unsigned int)(-1))
				{
					mergedIndices.clear();
					mergeShapes(shapes, bestIndex, distributionArray, shapeMatcher, usedShapeIndices, mergedIndices);

					if (mergedIndices.size() >= minimalObservations)
					{
						for (const Index32& mergedIndex : mergedIndices)
						{
							ocean_assert(usedShapeIndices[mergedIndex] == 0u);
							usedShapeIndices[mergedIndex] = 1u;
						}

						mergedShapes.push_back(ShapeMatcher::mergeShapes(shapes.data(), mergedIndices));
						mergedShapesIndices.push_back(std::move(mergedIndices));
					}
				}
			}
		}
	}

	ocean_assert(mergedShapes.size() == mergedShapesIndices.size());

	// let's see whether we can merge merged shapes
	// **TODO** improve O(n^2)

	bool joinedMergedShapes = true;

	while (joinedMergedShapes)
	{
		joinedMergedShapes = false;

		for (size_t nOuter = 0; nOuter < mergedShapes.size(); ++nOuter)
		{
			for (size_t nInner = nOuter + 1; nInner < mergedShapes.size(); /* noop */)
			{
				if (shapeMatcher.matchShapes<true>(mergedShapes[nOuter], mergedShapes[nInner]) >= ShapeMatcher::MT_PERFECT_MATCH)
				{
					Indices32 joinedIndices = mergedShapesIndices[nOuter];
					joinedIndices.insert(joinedIndices.cend(), mergedShapesIndices[nInner].cbegin(), mergedShapesIndices[nInner].cend());

					mergedShapes[nOuter] = ShapeMatcher::mergeShapes(shapes.data(), joinedIndices);
					mergedShapesIndices[nOuter] = std::move(joinedIndices);

					ocean_assert(nOuter < nInner);

					mergedShapes[nInner] = mergedShapes.back();
					mergedShapesIndices[nInner] = mergedShapesIndices.back();

					mergedShapes.pop_back();
					mergedShapesIndices.pop_back();

					joinedMergedShapes = true;
				}
				else
				{
					++nInner;
				}
			}
		}
	}

	ocean_assert(mergedShapes.size() == mergedShapesIndices.size());

	if (mergedShapeIndexGroups)
	{
		*mergedShapeIndexGroups = std::move(mergedShapesIndices);
	}

	return mergedShapes;
}

template <typename T>
void ShapeMerger::mergeShapes(const std::vector<T>& shapes, const unsigned int shapeIndex, const Geometry::SpatialDistribution::DistributionArray& distributionArray, const ShapeMatcher& shapeMatcher, const Indices32& usedIndices, Indices32& mergedIndices)
{
	ocean_assert(shapes.size() == usedIndices.size());
	ocean_assert(mergedIndices.empty());

	ocean_assert(shapeIndex < shapes.size());
	const T& shape = shapes[shapeIndex];

	const unsigned int xBinCenter = distributionArray.horizontalBin(shape.position().x());
	const unsigned int yBinCenter = distributionArray.verticalBin(shape.position().y());

	for (unsigned int yBin = (unsigned int)std::max(0, int(yBinCenter) - 1); yBin < std::min(yBinCenter + 2u, distributionArray.verticalBins()); ++yBin)
	{
		for (unsigned int xBin = (unsigned int)std::max(0, int(xBinCenter) - 1); xBin < std::min(xBinCenter + 2u, distributionArray.horizontalBins()); ++xBin)
		{
			const Indices32& indices = distributionArray(xBin, yBin);

			for (const Index32& index : indices)
			{
				if (usedIndices[index])
				{
					continue;
				}

				if (shapeIndex == index || shapeMatcher.matchShapes<true>(shape, shapes[index]) >= ShapeMatcher::MT_PARTIAL_MATCH)
				{
					mergedIndices.push_back(index);
				}
			}
		}
	}
}

template <typename T0, typename T1>
void ShapeMerger::mergeShapes(const T0& shape0, const std::vector<T1>& shapes1, const Geometry::SpatialDistribution::DistributionArray& distributionArray1, const ShapeMatcher& shapeMatcher, const Indices32& usedIndices1, Indices32& mergedIndices1)
{
	static_assert(std::is_same<T0, T1>::value == false, "Shape types must be different!");

	ocean_assert(shapes1.size() == usedIndices1.size());
	ocean_assert(mergedIndices1.empty());

	const unsigned int xBinCenter = distributionArray1.horizontalBin(shape0.position().x());
	const unsigned int yBinCenter = distributionArray1.verticalBin(shape0.position().y());

	for (unsigned int yBin = (unsigned int)std::max(0, int(yBinCenter) - 1); yBin < std::min(yBinCenter + 2u, distributionArray1.verticalBins()); ++yBin)
	{
		for (unsigned int xBin = (unsigned int)std::max(0, int(xBinCenter) - 1); xBin < std::min(xBinCenter + 2u, distributionArray1.horizontalBins()); ++xBin)
		{
			const Indices32& indices1 = distributionArray1(xBin, yBin);

			for (const Index32& index1 : indices1)
			{
				if (usedIndices1[index1])
				{
					continue;
				}

				if (shapeMatcher.matchShapes<true>(shape0, shapes1[index1]) >= ShapeMatcher::MT_PARTIAL_MATCH)
				{
					mergedIndices1.push_back(index1);
				}
			}
		}
	}
}

} // namespace Keyboard

} // namespace Tracking

} // namespace Ocean
