/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/PointTrack.h"

#include "ocean/base/Median.h"

#include "ocean/math/Line3.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

size_t PointTrack::determineTracksLengthUntil(const Index32 frameIndex, const PointTrackMap& pointTrackMap, const size_t minimalTracks, const double percentile)
{
	ocean_assert(minimalTracks >= 1);
	ocean_assert(percentile >= 0.0 && percentile <= 1.0);

	std::vector<size_t> tracksLengths; // TODO make re-usable
	tracksLengths.reserve(pointTrackMap.size());

	for (const PointTrackMap::value_type& pointPair : pointTrackMap)
	{
		const PointTrack& pointTrack = pointPair.second;
		ocean_assert(pointTrack.isValid());

		ocean_assert(pointTrack.lastFrameIndex() >= frameIndex);

		const size_t trackLength = pointTrack.numberObservationsUntil(frameIndex);

		if (trackLength != 0)
		{
			tracksLengths.push_back(trackLength);
		}
	}

	if (tracksLengths.size() < minimalTracks)
	{
		return 0;
	}

	const size_t percentileTracksLength = Median::percentile(tracksLengths.data(), tracksLengths.size(), percentile);

	return percentileTracksLength;
}

void PointTrack::extractCorrespondences(const Index32 firstFrameIndex, const Index32 lastFrameIndex, const PointTrackMap& pointTrackMap, Vectors2& firstImagePoints, Vectors2& lastImagePoints, Indices32& objectPointIds)
{
	ocean_assert(firstFrameIndex < lastFrameIndex);

	ocean_assert(firstImagePoints.empty());
	firstImagePoints.clear();

	ocean_assert(lastImagePoints.empty());
	lastImagePoints.clear();

	ocean_assert(objectPointIds.empty());
	objectPointIds.clear();

	for (const PointTrackMap::value_type& pointPair : pointTrackMap)
	{
		const Index32 objectPointId = pointPair.first;
		const PointTrack& pointTrack = pointPair.second;

		if (pointTrack.firstFrameIndex() <= firstFrameIndex && lastFrameIndex <= pointTrack.lastFrameIndex())
		{
			firstImagePoints.push_back(pointTrack.observation(firstFrameIndex));
			lastImagePoints.push_back(pointTrack.observation(lastFrameIndex));
			objectPointIds.push_back(objectPointId);
		}
	}
}

Scalar PointTrack::determineViewingAngle(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera0, const HomogenousMatrix4& world_T_camera1, const Vectors2& imagePoints0, const Vectors2& imagePoints1, const Indices32& validIndices, const double percentile)
{
	ocean_assert(world_T_camera0.isValid());
	ocean_assert(world_T_camera1.isValid());

	ocean_assert(!imagePoints0.empty());
	ocean_assert(!imagePoints1.empty());
	ocean_assert(imagePoints0.size() == imagePoints1.size());
	ocean_assert(!validIndices.empty());

	ocean_assert(percentile >= 0.0 && percentile <= 1.0);

	std::vector<Scalar> dotProducts; // TODO make re-usable
	dotProducts.reserve(validIndices.size());

	for (const Index32& validIndex : validIndices)
	{
		ocean_assert(validIndex < imagePoints0.size());
		const Vector2& imagePoint0 = imagePoints0[validIndex];

		ocean_assert(validIndex < imagePoints1.size());
		const Vector2& imagePoint1 = imagePoints1[validIndex];

		const Line3 ray0 = camera.ray(imagePoint0, world_T_camera0);
		const Line3 ray1 = camera.ray(imagePoint1, world_T_camera1);

		const Vector3& direction0 = ray0.direction();
		const Vector3& direction1 = ray1.direction();

		ocean_assert(direction0.isUnit());
		ocean_assert(direction1.isUnit());

		const Scalar dotProduct = direction0 * direction1;

		dotProducts.push_back(dotProduct);
	}

	const Scalar dotProductPercentile = Median::percentile(dotProducts.data(), dotProducts.size(), 1.0 - percentile); // dot product is smaller for larger angles

	const Scalar anglePercentile = Numeric::acos(dotProductPercentile);

	return anglePercentile;
}

}

}

}
