/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_OCCUPANCY_ARRAY_H
#define META_OCEAN_TRACKING_SLAM_OCCUPANCY_ARRAY_H

#include "ocean/tracking/slam/SLAM.h"

#include "ocean/geometry/SpatialDistribution.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class implements an occupancy array allowing to keep track of occupied and unoccupied bins in a camera image.
 * The bins are sized to work with a configurable neighborhood - when checking if a bin is empty, the neighboring bins are also checked.
 * The neighborhood size can be 1 (only the center bin), 3 (3x3 neighborhood), 5 (5x5 neighborhood), etc.
 * Use SpatialDistribution::idealBinsNeighborhood9() to calculate the optimal bin count for a 3x3 neighborhood with a given distance threshold.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT OccupancyArray : public Geometry::SpatialDistribution::Array
{
	public:

		/**
		 * Default constructor to create an invalid array.
		 */
		OccupancyArray() = default;

		/**
		 * Creates a new occupancy array.
		 * The bins should be sized appropriately for the specified neighborhood size.
		 * For a 3x3 neighborhood, use SpatialDistribution::idealBinsNeighborhood9().
		 * @param left The left position of the distribution area, with range (-infinity, infinity)
		 * @param top The top position of the distribution area, with range (-infinity, infinity)
		 * @param width The width of the distribution area in pixels, with range [1, infinity)
		 * @param height The height of the distribution area in pixels, with range [1, infinity)
		 * @param horizontalBins The number of horizontal distribution bins, with range [1, infinity)
		 * @param verticalBins The number of vertical distribution bins, with range [1, infinity)
		 * @param neighborhoodSize The size of the neighborhood to check (must be odd), e.g., 1 for single bin, 3 for 3x3, 5 for 5x5, with range [1, infinity), must be odd
		 * @param minCoverageThreshold The minimal coverage threshold (fraction of bins that should be occupied, normalized by neighborhood size), with range [0, 1]
		 */
		OccupancyArray(const Scalar left, const Scalar top, const unsigned int width, const unsigned int height, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int neighborhoodSize = 3u, const float minCoverageThreshold = 0.8f);

		/**
		 * Adds a new point.
		 * @param imagePoint The image point to be added
		 */
		inline void addPoint(const Vector2& imagePoint);

		/**
		 * Adds a new point.
		 * @param xBin The horizontal bin in which the point is located, with range [0, horizontalBins() - 1]
		 * @param yBin The vertical bin in which the point is located, with range [0, verticalBins() - 1]
		 */
		void addPoint(const unsigned int xBin, const unsigned int yBin);

		/**
		 * Adds a new point only if the neighborhood around the bin is empty (not yet occupied).
		 * The neighborhood size is determined by the constructor parameter.
		 * @param imagePoint The image point to be added
		 * @return True, if the neighborhood was empty and if the point was added
		 */
		inline bool addPointIfEmpty(const Vector2& imagePoint);

		/**
		 * Adds a new point only if the neighborhood will not exceed the specified number of points after adding the point.
		 * The neighborhood size is determined by the constructor parameter.
		 * @param imagePoint The image point to be added
		 * @param maxPoints The maximum number of points that the neighborhood may contain after adding the point, with range [1, infinity)
		 * @return True if the neighborhood contained fewer than maxPoints before adding, and the point was added
		 */
		inline bool addPointIfWithinLimit(const Vector2& imagePoint, const unsigned int maxPoints);

		/**
		 * Adds a new point only if the neighborhood around the bin is empty (not yet occupied).
		 * The neighborhood size is determined by the constructor parameter.
		 * @param xBin The horizontal bin in which the point is located, with range [0, horizontalBins() - 1]
		 * @param yBin The vertical bin in which the point is located, with range [0, verticalBins() - 1]
		 * @return True, if the neighborhood was empty and if the point was added
		 */
		bool addPointIfEmpty(const unsigned int xBin, const unsigned int yBin);

		/**
		 * Adds a new point only if the neighborhood will not exceed the specified number of points after adding the point.
		 * The neighborhood size is determined by the constructor parameter.
		 * @param xBin The horizontal bin in which the point is located, with range [0, horizontalBins() - 1]
		 * @param yBin The vertical bin in which the point is located, with range [0, verticalBins() - 1]
		 * @param maxPoints The maximum number of points that the neighborhood may contain after adding the point, with range [1, infinity)
		 * @return True if the neighborhood contained fewer than maxPoints before adding, and the point was added
		 */
		bool addPointIfWithinLimit(const unsigned int xBin, const unsigned int yBin, const unsigned int maxPoints);

		/**
		 * Returns whether the neighborhood around the given image point is empty (not yet occupied).
		 * The neighborhood size is determined by the constructor parameter.
		 * @param imagePoint The image point to check
		 * @return True if the neighborhood is empty
		 */
		inline bool isEmpty(const Vector2& imagePoint) const;

		/**
		 * Returns whether the neighborhood around the given bin is empty (not yet occupied).
		 * The neighborhood size is determined by the constructor parameter.
		 * @param xBin The horizontal bin to check, with range [0, horizontalBins() - 1]
		 * @param yBin The vertical bin to check, with range [0, verticalBins() - 1]
		 * @return True if the neighborhood is empty
		 */
		bool isEmpty(const unsigned int xBin, const unsigned int yBin) const;

		/**
		 * Removes all points from the occupancy array.
		 */
		inline void removePoints();

		/**
		 * Returns whether more points are needed to ensure a good distribution of the points in the image.
		 * This compares the current coverage against the minimum coverage threshold specified in the constructor.
		 * @return True, if so
		 */
		inline bool needMorePoints() const;

		/**
		 * Returns whether this array is valid.
		 * @return True, if valid
		 */
		inline bool isValid() const;

		/**
		 * Returns the width of the distribution area.
		 * @return The width in pixels
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the distribution area.
		 * @return The height in pixels
		 */
		inline unsigned int height() const;

		/**
		 * Returns the fraction of bins that are occupied, normalized by neighborhood size.
		 * This value accounts for the fact that each point effectively covers a neighborhood of bins.
		 * @return Coverage ratio, with range [0, infinity), typically in range [0, 1] but can exceed 1 with overlapping neighborhoods
		 */
		inline float coverage() const;

	protected:

		/// The vector holding occupancy counters for all bins.
		Indices32 bins_;

		/// The number of points added to the occupancy array.
		size_t numberPoints_ = 0;

		/// The coverage threshold to determine if more points are needed.
		float coverageThreshold_ = -1.0f;

		/// The neighborhood radius (offset from center bin), e.g., 1 for 3x3 neighborhood, 2 for 5x5, etc.
		unsigned int neighborhoodRadius_ = 0u;
};

inline void OccupancyArray::addPoint(const Vector2& imagePoint)
{
	const unsigned int xBin = horizontalBin(imagePoint.x());
	const unsigned int yBin = verticalBin(imagePoint.y());

	ocean_assert(xBin < horizontalBins());
	ocean_assert(yBin < verticalBins());

	addPoint(xBin, yBin);
}

inline bool OccupancyArray::addPointIfEmpty(const Vector2& imagePoint)
{
	const unsigned int xBin = horizontalBin(imagePoint.x());
	const unsigned int yBin = verticalBin(imagePoint.y());

	ocean_assert(xBin < horizontalBins());
	ocean_assert(yBin < verticalBins());

	return addPointIfEmpty(xBin, yBin);
}

inline bool OccupancyArray::addPointIfWithinLimit(const Vector2& imagePoint, const unsigned int maxPoints)
{
	const unsigned int xBin = horizontalBin(imagePoint.x());
	const unsigned int yBin = verticalBin(imagePoint.y());

	ocean_assert(xBin < horizontalBins());
	ocean_assert(yBin < verticalBins());

	return addPointIfWithinLimit(xBin, yBin, maxPoints);
}

inline bool OccupancyArray::isEmpty(const Vector2& imagePoint) const
{
	const unsigned int xBin = horizontalBin(imagePoint.x());
	const unsigned int yBin = verticalBin(imagePoint.y());

	ocean_assert(xBin < horizontalBins());
	ocean_assert(yBin < verticalBins());

	return isEmpty(xBin, yBin);
}

inline void OccupancyArray::removePoints()
{
	std::fill(bins_.begin(), bins_.end(), 0u);

	numberPoints_ = 0;
}

inline bool OccupancyArray::needMorePoints() const
{
	return coverage() < coverageThreshold_;
}

inline bool OccupancyArray::isValid() const
{
	return Array::isValid() && coverageThreshold_ >= 0.0f && coverageThreshold_ <= 1.0f;
}

inline unsigned int OccupancyArray::width() const
{
	return (unsigned int)(Array::width());
}

inline unsigned int OccupancyArray::height() const
{
	return (unsigned int)(Array::height());
}

inline float OccupancyArray::coverage() const
{
	const unsigned int neighborhoodSize = neighborhoodRadius_ * 2u + 1u;

	return float(numberPoints_ * neighborhoodSize * neighborhoodSize) / float(bins());
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_OCCUPANCY_ARRAY_H
