/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_SPATIAL_DISTRIBUTION_H
#define META_OCEAN_GEOMETRY_SPATIAL_DISTRIBUTION_H

#include "ocean/geometry/Geometry.h"

#include "ocean/base/Utilities.h"

#include "ocean/math/Box2.h"

#include <algorithm>

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements spatial distribution function for 2D geometric data.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT SpatialDistribution
{
	public:

		/**
		 * This class implements a base class for data arrays.
		 */
		class OCEAN_GEOMETRY_EXPORT Array
		{
			public:

				/**
				 * Returns the left position of the distribution area.
				 * @return Left position
				 */
				inline Scalar left() const;

				/**
				 * Returns the top position of the distribution area.
				 * @return Top position
				 */
				inline Scalar top() const;

				/**
				 * Returns the width of the distribution area.
				 * @return Area width
				 */
				inline Scalar width() const;

				/**
				 * Returns the height of the distribution area.
				 * @return Area height
				 */
				inline Scalar height() const;

				/**
				 * Returns the number of horizontal distribution bins.
				 * @return Number of bins
				 */
				inline unsigned int horizontalBins() const;

				/**
				 * Returns the number of vertical distribution bins.
				 * @return Number of bins
				 */
				inline unsigned int verticalBins() const;

				/**
				 * Returns the number of bins this distribution holds.
				 * @return Bin number
				 */
				inline unsigned int bins() const;

				/**
				 * Returns the bin index for a given position.
				 * Beware: Make sure that the given position is inside the specified range!
				 * @param x Horizontal position, with range [0, width())
				 * @param y Vertical position, with range [0, height())
				 * @return Resulting bin index
				 */
				inline unsigned int index(const Scalar x, const Scalar y) const;

				/**
				 * Returns the horizontal bin of a given horizontal position.
				 * Beware: The resulting bin can exceed the bin-ranges of the array.<br>
				 * @param x Horizontal position, with range (-infinity, infinity)
				 * @return Horizontal bin, with range (-infinity, infinity)
				 * @see clampedHorizontalBin().
				 */
				inline int horizontalBin(const Scalar x) const;

				/**
				 * Returns the vertical bin of a given vertical position.
				 * Beware: The resulting bin can exceed the bin-ranges of the array.<br>
				 * @param y Vertical position, with range (-infinity, infinity)
				 * @return Vertical bin, with range (-infinity, infinity)
				 * @see clampedVerticalBin().
				 */
				inline int verticalBin(const Scalar y) const;

				/**
				 * Returns the horizontal bin of a given horizontal position.
				 * Beware: The resulting bin is clamped into the range [0, number of bins - 1).<br>
				 * @param x Horizontal position, with range (-infinity, infinity)
				 * @return Horizontal bin, with range [0, horizontalBins() - 1]
				 * @see horizontalBin().
				 */
				inline int clampedHorizontalBin(const Scalar x) const;

				/**
				 * Returns the vertical bin of a given vertical position.
				 * Beware: The resulting bin is clamped into the range [0, number of bins - 1).<br>
				 * @param y Vertical position, with range (-infinity, infinity)
				 * @return Vertical bin, with range [0, verticalBins() - 1]
				 * @see clampedVerticalBin().
				 */
				inline int clampedVerticalBin(const Scalar y) const;

				/**
				 * Returns whether this object holds a valid distribution.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns whether two Array objects are identical.
				 * @param right The second array object to compare
				 * @return True, if so
				 */
				inline bool operator==(const Array& right) const;

				/**
				 * Returns whether two Array objects are not identical.
				 * @param right The second array object to compare
				 * @return True, if so
				 */
				inline bool operator!=(const Array& right) const;

				/**
				 * Returns whether this object holds a valid distribution.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/**
				 * Creates an empty array object.
				 */
				Array() = default;

				/**
				 * Copy constructor.
				 * @param object The array object to be copied
				 */
				Array(const Array& object) = default;

				/**
				 * Move constructor.
				 * @param object The array object to be moved
				 */
				inline Array(Array&& object) noexcept;

				/**
				 * Creates a new array object.
				 * @param left The left area position, with range (-infinity, infinity)
				 * @param top The top area position, with range (-infinity, infinity)
				 * @param width The width of the distribution area, with range (0, infinity)
				 * @param height The height of the distribution area, with range (0, infinity)
				 * @param horizontalBins Number of horizontal distribution bins, with range [1, infinity)
				 * @param verticalBins Number of vertical distribution bins, with range [1, infinity)
				 */
				inline Array(const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

				/**
				 * Assign operator.
				 * @param object The object to be assigned
				 * @return Reference to this object
				 */
				inline Array& operator=(const Array& object);

				/**
				 * Move operator.
				 * @param object The object to be moved
				 * @return Reference to this object
				 */
				inline Array& operator=(Array&& object) noexcept;

			protected:

				/// Left position of the distribution area.
				Scalar areaLeft_ = Scalar(0);

				/// Top position of the distribution area.
				Scalar areaTop_ = Scalar(0);

				/// Width of the distribution area.
				Scalar areaWidth_ = Scalar(0);

				/// Height of the distribution area.
				Scalar areaHeight_ = Scalar(0);

				/// Number of horizontal distribution bins.
				unsigned int horizontalBins_ = 0u;

				/// Number of vertical distribution bins.
				unsigned int verticalBins_ = 0u;

				/// Horizontal position to bin factor.
				Scalar horizontalPoint2Bin_ = Scalar(0);

				/// Vertical position to bin factor.
				Scalar verticalPoint2Bin_ = Scalar(0);
		};

		/**
		 * This class implements a distribution array.
		 */
		class OCEAN_GEOMETRY_EXPORT DistributionArray : public Array
		{
			public:

				/**
				 * Creates an empty distribution array object.
				 */
				DistributionArray() = default;

				/**
				 * Copy constructor.
				 * @param object The object to be copied
				 */
				DistributionArray(const DistributionArray& object) = default;

				/**
				 * Move constructor.
				 * @param object The object to be copied
				 */
				inline DistributionArray(DistributionArray&& object) noexcept;

				/**
				 * Creates a new distribution array object.
				 * @param left The left area position, with range (-infinity, infinity)
				 * @param top The top area position, with range (-infinity, infinity)
				 * @param width The width of the distribution area, with range (0, infinity)
				 * @param height The height of the distribution area, with range (0, infinity)
				 * @param horizontalBins Number of horizontal distribution bins, with range [1, infinity)
				 * @param verticalBins Number of vertical distribution bins, with range [1, infinity)
				 */
				inline DistributionArray(const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

				/**
				 * Copies a given distribution array and optional copies the indices from the 8-neighborhood of each individual bin to the bin in the center of the neighborhood.
				 * The resulting distribution array can be used to speed-up the search for nearest neighbors if the distribution array is reused several times.<br>
				 * Instead of gathering neighboring indices via indicesNeighborhood9(), each bin contains all indices of the 9 neighborhood already.
				 * @param distributionArray The distribution array to be copied
				 * @param copyNeighborhood8 True, to copy the 8-neighborhood of each individual bin to the bin in the center of the neighborhood; False to create a simple copy of the distribution array
				 * @see hasCopiedNeighborhood8().
				 */
				DistributionArray(const DistributionArray& distributionArray, const bool copyNeighborhood8);

				/**
				 * Returns the indices of the 8-neighborhood and the specified bin itself.
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @return Indices of the 9 neighboring bins
				 * @see hasCopiedNeighborhood8().
				 */
				Indices32 indicesNeighborhood9(const unsigned int horizontal, const unsigned int vertical) const;

				/**
				 * Returns the indices of the 8-neighborhood and the specified bin itself.
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @param indices Resulting indices of the 9 neighboring bins
				 * @see hasCopiedNeighborhood8().
				 */
				void indicesNeighborhood9(const unsigned int horizontal, const unsigned int vertical, Indices32& indices) const;

				/**
				 * Returns whether this distribution array contains copies of indices within the 8-neighborhood of each individual bin.
				 * @return True, if so
				 */
				inline bool hasCopiedNeighborhood8() const;

				/**
				 * Removes all elements form this array.
				 */
				void clear();

				/**
				 * Returns the distribution indices of a specified bin.
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @return Specified distribution indices
				 */
				inline const Indices32& operator()(const unsigned int horizontal, const unsigned int vertical) const;

				/**
				 * Returns the distribution indices of a specified bin.
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @return Specified distribution indices
				 */
				inline Indices32& operator()(const unsigned int horizontal, const unsigned int vertical);

				/**
				 * Returns the distribution indices of a specified bin.
				 * The bins are stored in a row aligned order.
				 * @param index The index of the specified bin, with range [0, horizontalBins() * verticalBins() - 1]
				 * @return Specified distribution indices
				 */
				inline const Indices32& operator[](const unsigned int index) const;

				/**
				 * Returns the distribution indices of a specified bin.
				 * The bins are stored in a row aligned order.
				 * @param index The index of the specified bin, with range [0, horizontalBins() * verticalBins() - 1]
				 * @return Specified distribution indices
				 */
				inline Indices32& operator[](const unsigned int index);

				/**
				 * Assign operator.
				 * @param object The object to be assigned
				 * @return Reference to this object
				 */
				inline DistributionArray& operator=(const DistributionArray& object);

				/**
				 * Move operator.
				 * @param object The object to be moved
				 */
				inline DistributionArray& operator=(DistributionArray&& object) noexcept;

				/**
				 * Returns whether two DistributionArray objects are identical.
				 * @param distributionArray The second array object to compare
				 * @return True, if so
				 */
				inline bool operator==(const DistributionArray& distributionArray) const;

				/**
				 * Returns whether two DistributionArray objects are not identical.
				 * @param distributionArray The second array object to compare
				 * @return True, if so
				 */
				inline bool operator!=(const DistributionArray& distributionArray) const;

			private:

				/// Distribution array with point indices.
				IndexGroups32 indexGroups_;

				/// True, if the array contains copies of indices within the 8-neighborhood of each individual bin; False, if each bin contains the indices of the actual bin only
				bool hasCopiedNeighborhood8_ = false;
		};

		/**
		 * This class implements an occupancy array.
		 */
		class OCEAN_GEOMETRY_EXPORT OccupancyArray : public Array
		{
			public:

				/**
				 * Creates an empty distribution array object.
				 */
				OccupancyArray() = default;

				/**
				 * Copy constructor.
				 * @param object Occupancy array object to be copied
				 */
				OccupancyArray(const OccupancyArray& object) = default;

				/**
				 * Move constructor.
				 * @param object Occupancy array object to be move
				 */
				inline OccupancyArray(OccupancyArray&& object) noexcept;

				/**
				 * Creates a new distribution array object.
				 * @param boundingBox The bounding box defining the position and dimension of the area, must be valid
				 * @param horizontalBins Number of horizontal distribution bins, with range [1, infinity)
				 * @param verticalBins Number of vertical distribution bins, with range [1, infinity)
				 * @param allFree True, to set all bins as free; False, to set all bins as occupied
				 */
				inline OccupancyArray(const Box2& boundingBox, const unsigned int horizontalBins, const unsigned int verticalBins, const bool allFree = true);

				/**
				 * Creates a new distribution array object.
				 * @param left The left area position, with range (-infinity, infinity)
				 * @param top The top area position, with range (-infinity, infinity)
				 * @param width The width of the distribution area, with range (0, infinity)
				 * @param height The height of the distribution area, with range (0, infinity)
				 * @param horizontalBins Number of horizontal distribution bins, with range [1, infinity)
				 * @param verticalBins Number of vertical distribution bins, with range [1, infinity)
				 * @param allFree True, to set all bins as free; False, to set all bins as occupied
				 */
				inline OccupancyArray(const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins, const bool allFree = true);

				/**
				 * Returns whether at least one bin in the 8-neighborhood or the specified bin itself is occupied.
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @return True, if so
				 */
				inline bool isOccupiedNeighborhood9(const unsigned int horizontal, const unsigned int vertical) const;

				/**
				 * Returns whether at least one bin in the 8-neighborhood or the specified bin itself is occupied.
				 * @param point The point for that the 9-neighborhood bins have to be checked, with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if so
				 */
				inline bool isOccupiedNeighborhood9(const Vector2& point) const;

				/**
				 * Returns whether at least one bin in the 8-neighborhood or the specified bin itself is not occupied.
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @return True, if so
				 */
				inline bool isNotOccupiedNeighborhood9(const unsigned int horizontal, const unsigned int vertical) const;

				/**
				 * Returns whether at least one bin in the 8-neighborhood or the specified bin itself is not occupied.
				 * @param point The point for that the 9-neighborhood bins have to be checked, with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if so
				 */
				inline bool isNotOccupiedNeighborhood9(const Vector2& point) const;

				/**
				 * Returns the number of occupied bins in the 9-neighborhood (so the specified bin is included).
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @return The number of occupied bins
				 */
				inline unsigned int countOccupiedNeighborhood9(const unsigned int horizontal, const unsigned int vertical) const;

				/**
				 * Returns the number of occupied bins.
				 * @return Bin number
				 */
				inline unsigned int occupiedBins() const;

				/**
				 * Returns the number of free bins.
				 * @return Bin number
				 */
				inline unsigned int freeBins() const;

				/**
				 * Adds an image point and returns whether the corresponding bin was not occupied before (was free before).
				 * If the point lies outside the array False is returned.<br>
				 * The bin (if valid) which belongs to the given point is occupied after the function returns.
				 * @param point The point to be added, with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if so
				 */
				inline bool addPoint(const Vector2& point);

				/**
				 * Adds an image point and returns whether the occupancy counter of the corresponding bin was equal or below a specified value.
				 * If the point lies outside the array False is returned.<br>
				 * The occupancy counter of the bin (if valid) to which the given point belongs will be incremented.<br>
				 * Beware: This function should not be used in conjunction with any other addPoint(), removePoint() function not leveraging a occupancy counter.
				 * @param point The point to be added, with range (-infinity, infinity)x(-infinity, infinity)
				 * @param maximalOccupancyCounter The maximal occupancy counter the corresponding bin may have to add the point.
				 * @return True, if so
				 */
				inline bool addPointWithCounter(const Vector2& point, const unsigned int maximalOccupancyCounter);

				/**
				 * Removes an image point and returns whether the corresponding bin was occupied before.
				 * If the point lies outside the array False is returned.<br>
				 * The bin (if valid) which belongs to the given point is free (not occupied) after the function returns.
				 * @param point The point to be removed, with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if so
				 */
				inline bool removePoint(const Vector2& point);

				/**
				 * Resets all occupied bins so that all bins a free afterwards.
				 */
				inline void reset();

				/**
				 * Adds an image point to this occupancy array.
				 * The bin (if valid) which belongs to the given point is occupied after the function returns.
				 * @param point The point to be added, with range (-infinity, infinity)x(-infinity, infinity)
				 * @return Reference to this object
				 */
				inline OccupancyArray& operator+=(const Vector2& point);

				/**
				 * Removes an image point from this occupancy array.
				 * The bin (if valid) which belongs to the given point is free (not occupied) after the function returns.
				 * @return Reference to this object
				 */
				inline OccupancyArray& operator-=(const Vector2& point);

				/**
				 * Returns whether a specified bin is occupied.
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @return True, if so
				 */
				inline bool operator()(const unsigned int horizontal, const unsigned int vertical) const;

				/**
				 * Returns whether the bin corresponding to a given point is occupied.
				 * @param point The point for which the bin has to be checked, with range (-infinity, infinity)x(-infinity, infinity)
				 * @return True, if so
				 */
				inline bool operator()(const Vector2& point) const;

				/**
				 * Returns whether a specified bin is occupied.
				 * @param horizontal The horizontal distribution bin, with range [0, horizontalBins() - 1]
				 * @param vertical The vertical distribution bin, with range [0, verticalBins() - 1]
				 * @return Non-zero, if so
				 */
				inline unsigned int& operator()(const unsigned int horizontal, const unsigned int vertical);

				/**
				 * Returns whether a specified bin is occupied.
				 * @param index The index of the specified bin, with range [0, horizontalBins() * verticalBins() - 1]
				 * @return True, if so
				 */
				inline bool operator[](const unsigned int index) const;

				/**
				 * Returns whether a specified bin is occupied.
				 * @param index The index of the specified bin, with range [0, horizontalBins() * verticalBins() - 1]
				 * @return Non-zero, if so
				 */
				inline unsigned int& operator[](const unsigned int index);

				/**
				 * Assign operator.
				 * @param object Occupancy array object to be assigned
				 * @return Reference to this object
				 */
				inline OccupancyArray& operator=(const OccupancyArray& object);

				/**
				 * Move operator.
				 * @param object Occupancy array object to be moved
				 * @return Reference to this object
				 */
				inline OccupancyArray& operator=(OccupancyArray&& object) noexcept;

				/**
				 * Returns whether two OccupancyArray objects are identical.
				 * @param occupancyArray The second array object to compare
				 * @return True, if so
				 */
				inline bool operator==(const OccupancyArray& occupancyArray) const;

				/**
				 * Returns whether two OccupancyArray objects are not identical.
				 * @param occupancyArray The second array object to compare
				 * @return True, if so
				 */
				inline bool operator!=(const OccupancyArray& occupancyArray) const;

			private:

				/// Occupancy array.
				Indices32 occupancy_;
		};

		/**
		 * Definition of a class holding an index and a distance.
		 */
		class OCEAN_GEOMETRY_EXPORT DistanceElement
		{
			public:

				/**
				 * Creates a new distance element.
				 * @param index Interest index to be stored
				 * @param candidateIndex Index of the candidate to be stored
				 * @param distance The distance to be stored
				 */
				inline DistanceElement(const unsigned int index, const unsigned int candidateIndex, const Scalar distance);

				/**
				 * Returns the interest index of this element.
				 * @return Stored index
				 */
				inline unsigned int index() const;

				/**
				 * Returns the candidate index of this element.
				 * @return Stored candidate index
				 */
				inline unsigned int candidateIndex() const;

				/**
				 * Returns the distance of this element.
				 * @return Stored distance
				 */
				inline Scalar distance() const;

				/**
				 * Compares two distance elements.
				 * @param left The left element to compare
				 * @param right The right element to compare
				 * @return True, if the left element has a smaller distance value than the right one
				 */
				static inline bool compareLeftSmaller(const DistanceElement& left, const DistanceElement& right);

				/**
				 * Compares two distance elements.
				 * @param left The left element to compare
				 * @param right The right element to compare
				 * @return True, if the left element has a higher distance value than the right one
				 */
				static inline bool compareLeftHigher(const DistanceElement& left, const DistanceElement& right);

				/**
				 * Returns whether the left element holds a higher distance than the right one.
				 * @return True, if so
				 */
				inline bool operator<(const DistanceElement& element) const;

			private:

				/// Interest index.
				unsigned int index_ = 0u;

				/// Candidate index.
				unsigned int candidateIndex_ = 0u;

				/// Distance value.
				Scalar distance_ = Scalar(-1);
		};

		/**
		 * Definition of a vector holding distance elements.
		 */
		typedef std::vector<DistanceElement> DistanceElements;

	public:

		/**
		 * Calculates the ideal number of horizontal and vertical bins for an array if the overall number of bins is known.
		 * This function tries to get bins with almost identical horizontal and vertical size.
		 * @param width The width of the area the array covers, with range [1, infinity)
		 * @param height The height of the area the array covers, with range [1, infinity)
		 * @param numberBins The number of bins that would be perfect, with range [1, infinity)
		 * @param horizontalBins The resulting number of horizontal bins that fit best with the given parameters, with range [minimalHorizontalBins, width]
		 * @param verticalBins The resulting number of vertical bins that fit best with the given parameters, with range [minimalVerticalBins, height]
		 * @param minimalHorizontalBins The minimal number of horizontal bins, with range [1, width]
		 * @param minimalVerticalBins The minimal number of vertical bins, with range [1, height]
		 */
		static void idealBins(const unsigned int width, const unsigned int height, const size_t numberBins, unsigned int& horizontalBins, unsigned int& verticalBins, const unsigned int minimalHorizontalBins = 2u, const unsigned int minimalVerticalBins = 2u);

		/**
		 * Calculates the ideal number of horizontal and vertical bins for an array if bin elements within a certain distance should be guaranteed to be located in the 9 neighborhood of that bin
		 * This function tries to get bins with almost identical horizontal and vertical size.
		 * @param width The width of the area the array covers, with range [1, infinity)
		 * @param height The height of the area the array covers, with range [1, infinity)
		 * @param distance The distance to other bin elements that should be guaranteed to be located in the 9 neighborhood, with range [1, infinity)
		 * @param horizontalBins The resulting number of horizontal bins that fit best with the given parameters, with range [minimalHorizontalBins, width]
		 * @param verticalBins The resulting number of vertical bins that fit best with the given parameters, with range [minimalVerticalBins, height]
		 * @param minimalHorizontalBins The minimal number of horizontal bins, with range [1, maximalHorizontalBins]
		 * @param minimalVerticalBins The minimal number of vertical bins, with range [1, maximalVerticalBins]
		 * @param maximalHorizontalBins The maximal number of horizontal bins, with range [minimalHorizontalBins, width]
		 * @param maximalVerticalBins The maximal number of vertical bins, with range [minimalVerticalBins, height]
		 */
		static void idealBinsNeighborhood9(const unsigned int width, const unsigned int height, const Scalar distance, unsigned int& horizontalBins, unsigned int& verticalBins, const unsigned int minimalHorizontalBins = 2u, const unsigned int minimalVerticalBins = 2u, const unsigned int maximalHorizontalBins = 20u, const unsigned int maximalVerticalBins = 20u);

		/**
		 * Distributes the given 2D image points into a spatial array.
		 * The number of used horizontal and vertical bins are calculated automatically.
		 * Instead of creating a 2D array a 1D array is returned with the first n elements for the top bins and the second n elements for the second top bins and so on.<br>
		 * Image points not fitting into the array bins are discarded.<br>
		 * @param imagePoints Image points to be distributed, may be nullptr if number==0
		 * @param number The number of given image points, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the elements, with range (0, infinity)
		 * @param height The height of the area holding the elements, with range (0, infinity)
		 * @param averagePointsPerBin Average number of points per bin, with range [1, infinity)
		 * @param maxHorizontalBins Maximal number of horizontal bins, with range [1, infinity)
		 * @param maxVerticalBins Maximal number of vertical bins, with range [1, infinity)
		 * @param horizontalBins Resulting horizontal bins, with range [1, infinity)
		 * @param verticalBins Resulting vertical bins, with range [1, infinity)
		 * @return Resulting array holding the indices of the distributed image points, will be cleared before usage
		 */
		static inline DistributionArray distributeToArray(const ImagePoint* imagePoints, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int averagePointsPerBin, const unsigned int maxHorizontalBins, const unsigned int maxVerticalBins, unsigned int& horizontalBins, unsigned int& verticalBins);

		/**
		 * Distributes a set of given 2D image points into a spatial array.
		 * Image points not fitting into the array bins are discarded.
		 * @param imagePoints Image points to be distributed, with range (-infinity, infinity)x(-infinity, infinity), may be nullptr if number==0
		 * @param number The number of given image points, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the points, with range (0, infinity)
		 * @param height The height of the area holding the points, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to distribute the image points into, with range [1, infinity)
		 * @param verticalBins Number of vertical bins to distribute the image points into, with range [1, infinity)
		 * @return Resulting array holding the indices of the distributed image points
		 */
		static DistributionArray distributeToArray(const ImagePoint* imagePoints, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

		/**
		 * Distributes the given elements into a spatial array.
		 * Elements not fitting into the array bins are discarded.
		 * @param elements The elements (e.g. holding image points) to be distributed
		 * @param number The number of given image points
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the elements, with range (0, infinity)
		 * @param height The height of the area holding the elements, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to distribute the image points into, with range [1, width]
		 * @param verticalBins Number of vertical bins to distribute the image points into, with range [1, height]
		 * @return Resulting array holding the indices of the distributed elements
		 * @tparam T Data type of the given elements
		 * @tparam tFunction Function pointer type of a function returning the 2D position of any element
		 */
		template <typename T, const Vector2& (*tFunction)(const T&)>
		static DistributionArray distributeToArray(const T* elements, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

		/**
		 * Distributes the given 2D image points into a spatial array.
		 * Instead of creating a 2D array a 1D array is returned with the first n elements for the top bins and the second n elements for the second top bins and so on.
		 * Image points not fitting into the array bins are discarded.<br>
		 * @param imagePoints Image points to be distributed
		 * @param number The number of given image points
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the points, with range (0, infinity)
		 * @param height The height of the area holding the points, with range (0, infinity)
		 * @param searchDistance Search distance that will be applied (minimal size of each bin)
		 * @return Resulting array holding the indices of the distributed image points
		 * @tparam tMaximalBins Number of maximal bins in each direction (horizontal and vertical), width range [1, infinity)
		 */
		template <unsigned int tMaximalBins>
		static inline DistributionArray distributeToArray(const ImagePoint* imagePoints, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const Scalar searchDistance);

		/**
		 * Distributes the given 2D image points into a spatial array.
		 * Instead of creating a 2D array a 1D array is returned with the first n elements for the top bins and the second n elements for the second top bins and so on.
		 * Image points not fitting into the array bins are discarded.<br>
		 * @param imagePoints Image points to be distributed, must be valid if the specified number is not zero
		 * @param number The number of given image points, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the points, with range (0, infinity)
		 * @param height The height of the area holding the points, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to distribute the image points into, with range [1, width]
		 * @param verticalBins Number of vertical bins to distribute the image points into, with range [1, height]
		 * @return Resulting array holding the indices of the distributed image points
		 */
		static OccupancyArray createOccupancyArray(const ImagePoint* imagePoints, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

		/**
		 * Filters the given 2D image points according to their distance to neighboring image points.
		 * @param imagePoints Image points (e.g. holding image points) to be filtered
		 * @param number The number of given image points
		 * @param width The width of the area holding the points in pixel, with range [1, infinity)
		 * @param height The height of the area holding the points in pixel, with range [1, infinity)
		 * @param distance The distance threshold to be used for filtering
		 * @return Resulting vector holding the indices of the found image points
		 */
		static Indices32 filterAccordingDistance(const ImagePoint* imagePoints, const size_t number, const unsigned int width, const unsigned int height, const Scalar distance);

		/**
		 * Filters the given 2D candidate points according to the distance to the given image points.
		 * @param imagePoints Image points for that the filtered candidate points have to be found
		 * @param numberImagePoints Number of image points
		 * @param candidatePoints Candidate points that will be filtered
		 * @param numberCandidatePoints Number of candidate points
		 * @param width The width of the area holding the image points, with range [1, infinity)
		 * @param height The height of the area holding the image points, with range [1, infinity)
		 * @param filterDistance Distance threshold that define whether a candidate point belongs to one image point
		 * @param filterSize Number of candidate points that can belong to one image point
		 * @param filteredIndices Resulting indices of the filtered candidate points, if defined
		 * @param filteredCandidates Resulting filtered candidate points, if defined
		 */
		static void filterCandidatePoint(const ImagePoint* imagePoints, const size_t numberImagePoints, const ImagePoint* candidatePoints, const size_t numberCandidatePoints, const unsigned int width, const unsigned int height, const Scalar filterDistance, const unsigned int filterSize, Indices32* filteredIndices = nullptr, ImagePoints* filteredCandidates = nullptr);

		/**
		 * Sorts the given 2D image points according to their minimal distance to neighboring image points.
		 * Points with higher distances will be returned before points with lower distances.
		 * @param imagePoints Image points (e.g. holding image points) to be sorted
		 * @param number The number of given image points
		 * @param minimalDistanceFirst True, to sort minimal distance to the front
		 * @return Resulting vector holding the indices and distances of the sorted image points
		 */
		static DistanceElements sortAccordingDistance(const ImagePoint* imagePoints, const size_t number, const bool minimalDistanceFirst);

		/**
		 * Sorts the given 2D image points according to their minimal distance to neighboring image points.
		 * Points with higher distances will be returned before points with lower distances.
		 * This function first distributes all points to array bins to speed up the computation.<br>
		 * @param imagePoints Image points (e.g. holding image points) to be sorted
		 * @param number The number of given image points
		 * @param width The width of the area holding the image points in pixel, with range [1, infinity)
		 * @param height The height of the area holding the image points in pixel, with range [1, infinity)
		 * @param bins Number of bins in each direction used for image point distribution
		 * @param minimalDistanceFirst True, to sort minimal distance to the front
		 * @return Resulting vector holding the indices and distances of the sorted image points
		 */
		static DistanceElements sortAccordingDistance(const ImagePoint* imagePoints, const size_t number, const unsigned int width, const unsigned int height, const unsigned int bins, const bool minimalDistanceFirst);

		/**
		 * Determines the minimal square distance for one given 2D image point to all other points in the same set.
		 * This function uses an already create point distribution to speed up the computation.<br>
		 * Candidates for nearest neighbors are all points lying in the 9-bin-neighborhood of a point, thus the result technically is an approximation and not correct in any case.
		 * @param imagePoints Image points to calculate the distances for
		 * @param numberImagePoints Number of given image points
		 * @param index The index of the image point to find the minimal square distance for, with range [0, numberImagePoints)
		 * @param distributionImagePoints Already created distribution array of the image points
		 * @return Resulting square distances calculated for the given image point
		 */
		static Scalar determineMinimalSqrDistance(const ImagePoint* imagePoints, const size_t numberImagePoints, const unsigned int index, const DistributionArray& distributionImagePoints);

		/**
		 * Determines the minimal square distances for each given 2D image point to all other points in the same set.
		 * This function first distributes all points to array bins to speed up the computation.<br>
		 * Candidates for nearest neighbors are all points lying in the 9-bin-neighborhood of a point, thus the result technically is an approximation and not correct in any case.
		 * @param imagePoints Image points to calculate the distances for, may be nullptr if numberImagePoints==0
		 * @param numberImagePoints Number of given image points, with range [0, infinity)
		 * @param width The width of the area holding the image points in pixel, with range [1, infinity)
		 * @param height The height of the area holding the image points in pixel, with range [1, infinity)
		 * @param bins Number of bins in each direction used for image point distribution, with range [1, infinity)
		 * @param sqrDistances Resulting square distances calculated for each given image point, make sure that this buffer holds enough space
		 */
		static void determineMinimalSqrDistances(const ImagePoint* imagePoints, const size_t numberImagePoints, const unsigned int width, const unsigned int height, const unsigned int bins, Scalar* sqrDistances);

		/**
		 * Determines the minimal square distances for each given 2D image point to another given set of 2D image points.
		 * This function first distributes all points to array bins to speed up the computation.<br>
		 * Candidates for nearest neighbors are all points lying in the 9-bin-neighborhood of a point, thus the result technically is an approximation and not correct in any case.
		 * @param imagePoints Image points to calculate the distances for, may be nullptr if numberImagePoints==0
		 * @param numberImagePoints Number of given image points, with range [0, infinity)
		 * @param candidates Image points being candidates as nearest neighbors for the given image points
		 * @param numberCandidates Number of given candidates
		 * @param width The width of the area holding the image points in pixel, with range [1, infinity)
		 * @param height The height of the area holding the image points in pixel, with range [1, infinity)
		 * @param bins Number of bins in each direction used for image point distribution
		 * @param sqrDistances Resulting square distances calculated for each given image point, make sure that this buffer holds enough space
		 */
		static void determineMinimalSqrDistances(const ImagePoint* imagePoints, const size_t numberImagePoints, const ImagePoint* candidates, const size_t numberCandidates, const unsigned int width, const unsigned int height, const unsigned int bins, Scalar* sqrDistances);

		/**
		 * Determines the minimal square distances for each given image point to another given set of image points.
		 * Candidates for nearest neighbors are all points lying in the 9-bin-neighborhood of a point, thus the result technically is an approximation and not correct in any case.
		 * @param imagePoints Image points to calculate the distances for, may be nullptr if numberImagePoints==0
		 * @param numberImagePoints Number of given image points, with range [0, infinity)
		 * @param candidates Image points being candidates as nearest neighbors for the given image points
		 * @param numberCandidates Number of given candidates
		 * @param distributionCandidates Already created distribution array of the candidates
		 * @param sqrDistances Resulting square distances calculated for each given element, make sure that this buffer holds enough space
		 * @param candidateIndices Optional resulting indices of the candidate with minimal distance, make sure that this buffer holds enough space (if defined)
		 */
		static void determineMinimalSqrDistances(const ImagePoint* imagePoints, const size_t numberImagePoints, const ImagePoint* candidates, const size_t numberCandidates, const DistributionArray& distributionCandidates, Scalar* sqrDistances, unsigned int* candidateIndices = nullptr);

		/**
		 * Determines the minimal square distances for each specified image point inside their neighborhood.
		 * This function first distributes all image points to array bins to speed up the computation.<br>
		 * Candidates for nearest neighbors are all points lying in the 9-bin-neighborhood of a point, thus the result technically is an approximation and not correct in any case.
		 * @param imagePoints Image points holding the entire set, may be nullptr if numberImagePoints==0
		 * @param numberImagePoints Number of given image points, with range [0, infinity)
		 * @param interestIndices Indices of all interest image points to calculate the minimal distance for
		 * @param numberInterestIndices Number of given interest indices
		 * @param width The width of the area holding the elements in pixel, with range [1, infinity)
		 * @param height The height of the area holding the elements in pixel, with range [1, infinity)
		 * @param bins Number of bins in each direction used for element distribution
		 * @param sqrDistances Resulting square distances calculated for each given interest index, make sure that this buffer holds enough space
		 */
		static void determineMinimalSqrDistances(const ImagePoint* imagePoints, const size_t numberImagePoints, const unsigned int* interestIndices, const size_t numberInterestIndices, const unsigned int width, const unsigned int height, const unsigned int bins, Scalar* sqrDistances);

		/**
		 * Determines all candidate points for a given image point (interest point) lying inside a specified circle around the interest point.
		 * @param imagePoint The interest image point to find the neighboring candidate points for, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param candidatePoints Candidate points to find the neighbors in
		 * @param numberCandidatePoints Number of given candidate points
		 * @param radius The radius specifying the circle around the interest point, with range [0, infinity)
		 * @param distributionCandidatePoints Already created distribution array of the candidate points
		 * @return Resulting indices of all neighbors out the candidate set lying within the specified circle.
		 */
		static Indices32 determineNeighbors(const ImagePoint& imagePoint, const ImagePoint* candidatePoints, const size_t numberCandidatePoints, const Scalar radius, const DistributionArray& distributionCandidatePoints);

		/**
		 * Determines the nearest image point between an interest point and a set of given image point lying inside a specified circle around the interest point.
		 * @param interestPoint The interest image point for which the nearest neighbor will be determined, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param imagePoints The set of image points from which the nearest neighbor will be determined
		 * @param numberImagePoints Number of given image points, with range [0, infinity)
		 * @param radius The radius specifying the circle around the interest point, with range [0, infinity)
		 * @param distributionImagePoints Already created distribution array of the image points
		 * @param sqrDistance Optional resulting square distance of the nearest image point, if any
		 * @return Resulting index of the nearest neighbor image point, -1 if no image point lies within the specified radius
		 */
		static Index32 determineNearestNeighbor(const ImagePoint& interestPoint, const ImagePoint* imagePoints, const size_t numberImagePoints, const Scalar radius, const DistributionArray& distributionImagePoints, Scalar* sqrDistance = nullptr);

		/**
		 * Distributes the given image points into an array of specified size and returns (at most) one point from each bin.
		 * @param imagePoints Image points to be distributed and filtered, may be nullptr if numberImagePoints==0
		 * @param numberImagePoints Number of given image points, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the elements, with range (0, infinity)
		 * @param height The height of the area holding the elements, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to be used for distribution, with range [1, infinity)
		 * @param verticalBins Number of vertical bins to be used for distribution, with range [1, infinity)
		 * @return Resulting filtered elements
		 */
		static inline ImagePoints distributeAndFilter(const ImagePoint* imagePoints, const size_t numberImagePoints, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

		/**
		 * Distributes the given image points into an array of specified size and returns as much points as requested by first selecting the first point from each bin, then the second point from each bin, and so on.
		 * @param imagePoints Image points to be distributed and filtered, may be nullptr if numberImagePoints==0
		 * @param numberImagePoints Number of given image points, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the elements, with range (0, infinity)
		 * @param height The height of the area holding the elements, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to be used for distribution, with range [1, infinity)
		 * @param verticalBins Number of vertical bins to be used for distribution, with range [1, infinity)
		 * @param size The number of requested feature points, with range [0, numberImagePoints]
		 * @return Resulting filtered elements
		 */
		static ImagePoints distributeAndFilter(const ImagePoint* imagePoints, const size_t numberImagePoints, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins, const size_t size);

		/**
		 * Distributes the given image points into an array of specified size and returns (at most) one point index from each bin.
		 * The resulting indices can be used to filter the actual set of image points.<br>
		 * @param imagePoints Image points to be distributed and filtered, may be nullptr if numberImagePoints==0
		 * @param numberImagePoints Number of given image points, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the elements, with range (0, infinity)
		 * @param height The height of the area holding the elements, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to be used for distribution, with range [1, infinity)
		 * @param verticalBins Number of vertical bins to be used for distribution, with range [1, infinity)
		 * @return Resulting indices of the filtered elements
		 * @tparam TIndex The data type of the indices (e.g., may be Index32 or Index64)
		 */
		template <typename TIndex>
		static inline std::vector<TIndex> distributeAndFilterIndices(const ImagePoint* imagePoints, const size_t numberImagePoints, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

		/**
		 * Distributes the given elements into an array of specified size and returns (at most) one element from each bin.
		 * @param elements The elements to be distributed and filtered, may be nullptr if numberImagePoints==0
		 * @param numberElements Number of given elements, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the elements, with range (0, infinity)
		 * @param height The height of the area holding the elements, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to be used for distribution, with range [1, infinity)
		 * @param verticalBins Number of vertical bins to be used for distribution, with range [1, infinity)
		 * @return Resulting filtered elements
		 * @tparam T The data type of the elements to be distributed
		 * @tparam tFunction The function pointer that returns the 2D position of each element
		 */
		template <typename T, Vector2 (*tFunction)(const T&)>
		static std::vector<T> distributeAndFilter(const T* elements, const size_t numberElements, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

		/**
		 * Distributes the given elements into an array of specified size but returns as much elements per bin necessary to reach the specified amount of desired elements.
		 * This function should be used whenever the given elements are not distributed equally while a specified amount of feature points is desired.
		 * The function applies several filter iterations until the desired number of elements is reached.<br>
		 * After the first iteration each bin will contain at most one element, after the second iteration at most two elements, and so on.
		 * @param elements The elements to be distributed and filtered, may be nullptr if numberImagePoints==0
		 * @param numberElements Number of given elements, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the elements, with range (0, infinity)
		 * @param height The height of the area holding the elements, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to be used for distribution, with range [1, infinity)
		 * @param verticalBins Number of vertical bins to be used for distribution, with range [1, infinity)
		 * @param numberDesiredFilteredElements The desired number of filtered elements, with range [1, numberElements]
		 * @param indices Optional resulting indices of the selected keypoints, if defined indices will be pushed at the back.
		 * @return Resulting filtered elements
		 * @tparam T The data type of the elements to be distributed
		 * @tparam tFunction The function pointer that returns the 2D position of each element
		 */
		template <typename T, Vector2 (*tFunction)(const T&)>
		static std::vector<T> distributeAndFilter(const T* elements, const size_t numberElements, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins, const size_t numberDesiredFilteredElements, Indices32 *indices = nullptr);

		/**
		 * Distributes the given elements into an array of specified size and returns (at most) one point index from each bin.
		 * The resulting indices can be used to filter the actual set of image points.<br>
		 * @param elements The elements to be distributed and filtered, may be nullptr if numberImagePoints==0
		 * @param numberElements Number of given elements, with range [0, infinity)
		 * @param left Horizontal position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param top Vertical position of the area, may be 0 for e.g. an entire image, with range (-infinity, infinity)
		 * @param width The width of the area holding the elements, with range (0, infinity)
		 * @param height The height of the area holding the elements, with range (0, infinity)
		 * @param horizontalBins Number of horizontal bins to be used for distribution, with range [1, infinity)
		 * @param verticalBins Number of vertical bins to be used for distribution, with range [1, infinity)
		 * @return Resulting indices of the filtered elements
		 * @tparam T The data type of the elements to be distributed
		 * @tparam TIndex The data type of the indices (e.g., may be Index32 or Index64)
		 * @tparam tFunction The function pointer that returns the 2D position of each element
		 */
		template <typename T, typename TIndex, Vector2 (*tFunction)(const T&)>
		static std::vector<TIndex> distributeAndFilterIndices(const T* elements, const size_t numberElements, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins);

	protected:

		/**
		 * This function simply returns the given object (actually a copy of the object).
		 * @param value The value to be returned
		 * @return The given value
		 * @tparam T The data type of the value
		 */
		template <typename T>
		static inline T identity(const T& value);
};

inline SpatialDistribution::Array::Array(Array&& object) noexcept :
	areaLeft_(object.areaLeft_),
	areaTop_(object.areaTop_),
	areaWidth_(object.areaWidth_),
	areaHeight_(object.areaHeight_),
	horizontalBins_(object.horizontalBins_),
	verticalBins_(object.verticalBins_),
	horizontalPoint2Bin_(object.horizontalPoint2Bin_),
	verticalPoint2Bin_(object.verticalPoint2Bin_)
{
	object.areaLeft_ = Scalar(0);
	object.areaTop_ = Scalar(0);
	object.areaWidth_ = Scalar(0);
	object.areaHeight_ = Scalar(0);
	object.horizontalBins_ = 0u;
	object.verticalBins_ = 0u;
	object.horizontalPoint2Bin_ = Scalar(0);
	object.verticalPoint2Bin_ = Scalar(0);
}

inline SpatialDistribution::Array::Array(const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins) :
	areaLeft_(left),
	areaTop_(top),
	areaWidth_(width),
	areaHeight_(height),
	horizontalBins_(horizontalBins),
	verticalBins_(verticalBins),
	horizontalPoint2Bin_(width > Numeric::eps() ? (Scalar(horizontalBins) / (Scalar(width) + Numeric::eps())) : Scalar(0)),
	verticalPoint2Bin_(height > Numeric::eps() ? (Scalar(verticalBins) / (Scalar(height) + Numeric::eps())) : Scalar(0))
{
	ocean_assert(width > Numeric::eps() && height > Numeric::eps());
}

inline Scalar SpatialDistribution::Array::left() const
{
	return areaLeft_;
}

inline Scalar SpatialDistribution::Array::top() const
{
	return areaTop_;
}

inline Scalar SpatialDistribution::Array::width() const
{
	return areaWidth_;
}

inline Scalar SpatialDistribution::Array::height() const
{
	return areaHeight_;
}

inline unsigned int SpatialDistribution::Array::horizontalBins() const
{
	return horizontalBins_;
}

inline unsigned int SpatialDistribution::Array::verticalBins() const
{
	return verticalBins_;
}

inline unsigned int SpatialDistribution::Array::bins() const
{
	return horizontalBins_ * verticalBins_;
}

inline unsigned int SpatialDistribution::Array::index(const Scalar x, const Scalar y) const
{
	const int xBin = horizontalBin(x);
	const int yBin = verticalBin(y);

	ocean_assert(xBin >= 0 && xBin < int(horizontalBins_));
	ocean_assert(yBin >= 0 && yBin < int(verticalBins_));

	return yBin * horizontalBins_ + xBin;
}

inline int SpatialDistribution::Array::horizontalBin(const Scalar x) const
{
	// for positive values we could avoid to use floor(), however for negative values we e.g., need -0.2 to be -1
	return int(Numeric::floor((x - areaLeft_) * horizontalPoint2Bin_));
}

inline int SpatialDistribution::Array::verticalBin(const Scalar y) const
{
	// for positive values we could avoid to use floor(), however for negative values we e.g., need -0.2 to be -1
	return int(Numeric::floor((y - areaTop_) * verticalPoint2Bin_));
}

inline int SpatialDistribution::Array::clampedHorizontalBin(const Scalar x) const
{
	ocean_assert(isValid());
	return minmax<int>(0, horizontalBin(x), horizontalBins_ - 1);
}

inline int SpatialDistribution::Array::clampedVerticalBin(const Scalar y) const
{
	ocean_assert(isValid());
	return minmax<int>(0, verticalBin(y), verticalBins_ - 1);
}

inline bool SpatialDistribution::Array::isValid() const
{
	return horizontalBins_ != 0u && verticalBins_ != 0u;
}

inline bool SpatialDistribution::Array::operator==(const Array& right) const
{
	return areaLeft_ == right.areaLeft_
				&& areaTop_ == right.areaTop_
				&& areaWidth_ == right.areaWidth_
				&& areaHeight_ == right.areaHeight_
				&& horizontalBins_ == right.horizontalBins_
				&& verticalBins_ == right.verticalBins_
				&& horizontalPoint2Bin_ == right.horizontalPoint2Bin_
				&& verticalPoint2Bin_ == right.verticalPoint2Bin_;
}

inline bool SpatialDistribution::Array::operator!=(const Array& right) const
{
	return !(*this == right);
}

inline SpatialDistribution::Array::operator bool() const
{
	return isValid();
}

inline SpatialDistribution::Array& SpatialDistribution::Array::operator=(const Array& object)
{
	if (this != &object)
	{
		areaLeft_ = object.areaLeft_;
		areaTop_ = object.areaTop_;
		areaWidth_ = object.areaWidth_;
		areaHeight_ = object.areaHeight_;
		horizontalBins_ = object.horizontalBins_;
		verticalBins_ = object.verticalBins_;
		horizontalPoint2Bin_ = object.horizontalPoint2Bin_;
		verticalPoint2Bin_ = object.verticalPoint2Bin_;
	}

	return *this;
}

inline SpatialDistribution::Array& SpatialDistribution::Array::operator=(Array&& object) noexcept
{
	if (this != &object)
	{
		areaLeft_ = object.areaLeft_;
		areaTop_ = object.areaTop_;
		areaWidth_ = object.areaWidth_;
		areaHeight_ = object.areaHeight_;
		horizontalBins_ = object.horizontalBins_;
		verticalBins_ = object.verticalBins_;
		horizontalPoint2Bin_ = object.horizontalPoint2Bin_;
		verticalPoint2Bin_ = object.verticalPoint2Bin_;

		object.areaLeft_ = Scalar(0);
		object.areaTop_ = Scalar(0);
		object.areaWidth_ = Scalar(0);
		object.areaHeight_ = Scalar(0);
		object.horizontalBins_ = 0u;
		object.verticalBins_ = 0u;
		object.horizontalPoint2Bin_ = Scalar(0);
		object.verticalPoint2Bin_ = Scalar(0);
	}

	return *this;
}

inline SpatialDistribution::DistributionArray::DistributionArray(DistributionArray&& object) noexcept :
	Array(object),
	indexGroups_(std::move(object.indexGroups_)),
	hasCopiedNeighborhood8_(object.hasCopiedNeighborhood8_)
{
	object = DistributionArray();
}

inline SpatialDistribution::DistributionArray::DistributionArray(const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins) :
	Array(left, top, width, height, horizontalBins, verticalBins),
	indexGroups_(horizontalBins * verticalBins),
	hasCopiedNeighborhood8_(false)
{
	// nothing to do here
}

inline bool SpatialDistribution::DistributionArray::hasCopiedNeighborhood8() const
{
	return hasCopiedNeighborhood8_;
}

inline const Indices32& SpatialDistribution::DistributionArray::operator()(const unsigned int horizontal, const unsigned int vertical) const
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	return indexGroups_[vertical * horizontalBins_ + horizontal];
}

inline Indices32& SpatialDistribution::DistributionArray::operator()(const unsigned int horizontal, const unsigned int vertical)
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	return indexGroups_[vertical * horizontalBins_ + horizontal];
}

inline const Indices32& SpatialDistribution::DistributionArray::operator[](const unsigned int index) const
{
	ocean_assert(index < horizontalBins_ * verticalBins_);
	return indexGroups_[index];
}

inline Indices32& SpatialDistribution::DistributionArray::operator[](const unsigned int index)
{
	ocean_assert(index < horizontalBins_ * verticalBins_);
	return indexGroups_[index];
}

inline SpatialDistribution::DistributionArray& SpatialDistribution::DistributionArray::operator=(const DistributionArray& object)
{
	Array::operator=((Array&)object);
	indexGroups_ = object.indexGroups_;
	hasCopiedNeighborhood8_ = object.hasCopiedNeighborhood8_;

	return *this;
}

inline SpatialDistribution::DistributionArray& SpatialDistribution::DistributionArray::operator=(DistributionArray&& object) noexcept
{
	if (this != &object)
	{
		Array::operator=(std::move((Array&)object));
		indexGroups_ = std::move(object.indexGroups_);
		hasCopiedNeighborhood8_ = object.hasCopiedNeighborhood8_;

		object.hasCopiedNeighborhood8_ = false;
	}

	return *this;
}

inline bool SpatialDistribution::DistributionArray::operator==(const DistributionArray& distributionArray) const
{
	if (!Array::operator==(distributionArray))
	{
		return false;
	}

	return indexGroups_ == distributionArray.indexGroups_ && hasCopiedNeighborhood8_ && distributionArray.hasCopiedNeighborhood8_;
}

inline bool SpatialDistribution::DistributionArray::operator!=(const DistributionArray& distributionArray) const
{
	return !(*this == distributionArray);
}

inline SpatialDistribution::OccupancyArray::OccupancyArray(OccupancyArray&& object) noexcept :
	Array(std::move((Array&)object)),
	occupancy_(std::move(object.occupancy_))
{
	// nothing to do here
}

inline SpatialDistribution::OccupancyArray::OccupancyArray(const Box2& boundingBox, const unsigned int horizontalBins, const unsigned int verticalBins, const bool allFree) :
	Array(boundingBox.left(), boundingBox.top(), boundingBox.width(), boundingBox.height(), horizontalBins, verticalBins),
	occupancy_(horizontalBins * verticalBins, allFree ? 0u : 1u)
{
	ocean_assert(NumericT<unsigned int>::isInsideValueRange(uint64_t(horizontalBins) * uint64_t(verticalBins)));
}

inline SpatialDistribution::OccupancyArray::OccupancyArray(const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins, const bool allFree) :
	Array(left, top, width, height, horizontalBins, verticalBins),
	occupancy_(horizontalBins * verticalBins, allFree ? 0u : 1u)
{
	ocean_assert(NumericT<unsigned int>::isInsideValueRange(uint64_t(horizontalBins) * uint64_t(verticalBins)));
}

inline unsigned int SpatialDistribution::OccupancyArray::countOccupiedNeighborhood9(const unsigned int horizontal, const unsigned int vertical) const
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	unsigned int number = 0u;

	for (unsigned int y = max(0, int(vertical) - 1); y < min(vertical + 2u, verticalBins_); ++y)
	{
		for (unsigned int x = max(0, int(horizontal) - 1); x < min(horizontal + 2u, horizontalBins_); ++x)
		{
			if ((*this)(x, y))
			{
				number++;
			}
		}
	}

	return number;
}

inline bool SpatialDistribution::OccupancyArray::isOccupiedNeighborhood9(const unsigned int horizontal, const unsigned int vertical) const
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	for (unsigned int y = max(0, int(vertical) - 1); y < min(vertical + 2u, verticalBins_); ++y)
	{
		for (unsigned int x = max(0, int(horizontal) - 1); x < min(horizontal + 2u, horizontalBins_); ++x)
		{
			if ((*this)(x, y))
			{
				return true;
			}
		}
	}

	return false;
}

inline bool SpatialDistribution::OccupancyArray::isOccupiedNeighborhood9(const Vector2& point) const
{
	const unsigned int horizontal = horizontalBin(point.x());
	const unsigned int vertical = verticalBin(point.y());

	if (horizontal < horizontalBins_ && vertical < verticalBins_)
	{
		for (unsigned int y = max(0, int(vertical) - 1); y < min(vertical + 2u, verticalBins_); ++y)
		{
			for (unsigned int x = max(0, int(horizontal) - 1); x < min(horizontal + 2u, horizontalBins_); ++x)
			{
				if ((*this)(x, y))
				{
					return true;
				}
			}
		}
	}

	return false;
}

inline bool SpatialDistribution::OccupancyArray::isNotOccupiedNeighborhood9(const unsigned int horizontal, const unsigned int vertical) const
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	for (unsigned int y = max(0, int(vertical) - 1); y < min(vertical + 2u, verticalBins_); ++y)
	{
		for (unsigned int x = max(0, int(horizontal) - 1); x < min(horizontal + 2u, horizontalBins_); ++x)
		{
			if (!(*this)(x, y))
			{
				return true;
			}
		}
	}

	return false;
}

inline bool SpatialDistribution::OccupancyArray::isNotOccupiedNeighborhood9(const Vector2& point) const
{
	const unsigned int horizontal = horizontalBin(point.x());
	const unsigned int vertical = verticalBin(point.y());

	if (horizontal < horizontalBins_ && vertical < verticalBins_)
	{
		for (unsigned int y = max(0, int(vertical) - 1); y < min(vertical + 2u, verticalBins_); ++y)
		{
			for (unsigned int x = max(0, int(horizontal) - 1); x < min(horizontal + 2u, horizontalBins_); ++x)
			{
				if (!(*this)(x, y))
				{
					return true;
				}
			}
		}
	}

	return false;
}

inline unsigned int SpatialDistribution::OccupancyArray::occupiedBins() const
{
	unsigned int count = 0u;

	for (const Index32& bin : occupancy_)
	{
		if (bin != 0u)
		{
			++count;
		}
	}

	return count;
}

inline unsigned int SpatialDistribution::OccupancyArray::freeBins() const
{
	unsigned int count = 0u;

	for (const Index32& bin : occupancy_)
	{
		if (bin == 0u)
		{
			++count;
		}
	}

	return count;
}

inline bool SpatialDistribution::OccupancyArray::addPoint(const Vector2& point)
{
	const unsigned int horizontal = (unsigned int)(horizontalBin(point.x()));
	const unsigned int vertical = (unsigned int)(verticalBin(point.y()));

	if (horizontal < horizontalBins_ && vertical < verticalBins_)
	{
		const unsigned int index = vertical * horizontalBins_ + horizontal;

		if (!occupancy_[index])
		{
			occupancy_[index] = 1u;
			return true;
		}
	}

	return false;
}

inline bool SpatialDistribution::OccupancyArray::addPointWithCounter(const Vector2& point, const unsigned int maximalOccupancyCounter)
{
	const unsigned int horizontal = (unsigned int)(horizontalBin(point.x()));
	const unsigned int vertical = (unsigned int)(verticalBin(point.y()));

	if (horizontal < horizontalBins_ && vertical < verticalBins_)
	{
		const unsigned int index = vertical * horizontalBins_ + horizontal;

		if (occupancy_[index] <= maximalOccupancyCounter)
		{
			occupancy_[index]++;
			return true;
		}
	}

	return false;
}

inline bool SpatialDistribution::OccupancyArray::removePoint(const Vector2& point)
{
	const unsigned int horizontal = (unsigned int)(horizontalBin(point.x()));
	const unsigned int vertical = (unsigned int)(verticalBin(point.y()));

	if (horizontal < horizontalBins_ && vertical < verticalBins_)
	{
		const unsigned int index = vertical * horizontalBins_ + horizontal;

		if (occupancy_[index])
		{
			occupancy_[index] = 0u;
			return true;
		}
	}

	return false;
}

inline void SpatialDistribution::OccupancyArray::reset()
{
	for (size_t n = 0; n < occupancy_.size(); ++n)
	{
		occupancy_[n] = 0u;
	}
}

inline SpatialDistribution::OccupancyArray& SpatialDistribution::OccupancyArray::operator+=(const Vector2& point)
{
	const unsigned int horizontal = (unsigned int)(horizontalBin(point.x()));
	const unsigned int vertical = (unsigned int)(verticalBin(point.y()));

	if (horizontal < horizontalBins_ && vertical < verticalBins_)
	{
		occupancy_[vertical * horizontalBins_ + horizontal] = 1u;
	}

	return *this;
}

inline SpatialDistribution::OccupancyArray& SpatialDistribution::OccupancyArray::operator-=(const Vector2& point)
{
	const unsigned int horizontal = (unsigned int)(horizontalBin(point.x()));
	const unsigned int vertical = (unsigned int)(verticalBin(point.y()));

	if (horizontal < horizontalBins_ && vertical < verticalBins_)
	{
		occupancy_[vertical * horizontalBins_ + horizontal] = 0u;
	}

	return *this;
}

inline bool SpatialDistribution::OccupancyArray::operator()(const unsigned int horizontal, const unsigned int vertical) const
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	return occupancy_[vertical * horizontalBins_ + horizontal] != 0u;
}

inline unsigned int& SpatialDistribution::OccupancyArray::operator()(const unsigned int horizontal, const unsigned int vertical)
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	return occupancy_[vertical * horizontalBins_ + horizontal];
}

inline bool SpatialDistribution::OccupancyArray::operator()(const Vector2& point) const
{
	const unsigned int xBin = horizontalBin(point.x());
	const unsigned int yBin = verticalBin(point.y());

	return xBin < horizontalBins_ && yBin < horizontalBins_ && occupancy_[yBin * horizontalBins_ + xBin] != 0u;
}

inline bool SpatialDistribution::OccupancyArray::operator[](const unsigned int index) const
{
	ocean_assert(index < horizontalBins_ * verticalBins_);
	return occupancy_[index] != 0u;
}

inline unsigned int& SpatialDistribution::OccupancyArray::operator[](const unsigned int index)
{
	ocean_assert(index < horizontalBins_ * verticalBins_);
	return occupancy_[index];
}

inline SpatialDistribution::OccupancyArray& SpatialDistribution::OccupancyArray::operator=(const OccupancyArray& object)
{
	Array::operator=((Array&)object);
	occupancy_ = object.occupancy_;

	return *this;
}

inline SpatialDistribution::OccupancyArray& SpatialDistribution::OccupancyArray::operator=(OccupancyArray&& object) noexcept
{
	if (this != &object)
	{
		Array::operator=(std::move((Array&)object));
		occupancy_ = std::move(object.occupancy_);
	}

	return *this;
}

inline bool SpatialDistribution::OccupancyArray::operator==(const OccupancyArray& occupancyArray) const
{
	if (!Array::operator==(occupancyArray))
	{
		return false;
	}

	return occupancy_ == occupancyArray.occupancy_;
}

inline bool SpatialDistribution::OccupancyArray::operator!=(const OccupancyArray& occupancyArray) const
{
	return !(*this == occupancyArray);
}

inline SpatialDistribution::DistanceElement::DistanceElement(const unsigned int index, const unsigned int candidateIndex, const Scalar distance) :
	index_(index),
	candidateIndex_(candidateIndex),
	distance_(distance)
{
	// nothing to do here
}

inline unsigned int SpatialDistribution::DistanceElement::index() const
{
	return index_;
}

inline unsigned int SpatialDistribution::DistanceElement::candidateIndex() const
{
	return candidateIndex_;
}

inline Scalar SpatialDistribution::DistanceElement::distance() const
{
	return distance_;
}

inline bool SpatialDistribution::DistanceElement::compareLeftSmaller(const DistanceElement& left, const DistanceElement& right)
{
	return left.distance_ < right.distance_;
}

inline bool SpatialDistribution::DistanceElement::compareLeftHigher(const DistanceElement& left, const DistanceElement& right)
{
	return left.distance_ > right.distance_;
}

inline bool SpatialDistribution::DistanceElement::operator<(const DistanceElement& element) const
{
	// ** TODO** this mismatch between comparison operators should be fixed

	return distance_ > element.distance_;
}

template <unsigned int tMaximalBins>
inline SpatialDistribution::DistributionArray SpatialDistribution::distributeToArray(const ImagePoint* imagePoints, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const Scalar searchDistance)
{
	static_assert(tMaximalBins > 0u, "Invalid maximal bin parameter!");

	const unsigned int horizontalBins = min(tMaximalBins, (unsigned int)Numeric::ceil(width / max(searchDistance, Scalar(2))));
	const unsigned int verticalBins = min(tMaximalBins, (unsigned int)Numeric::ceil(height / max(searchDistance, Scalar(2))));

	return distributeToArray(imagePoints, number, left, top, width, height, horizontalBins, verticalBins);
}

template <typename T, const Vector2& (*tFunction)(const T&)>
SpatialDistribution::DistributionArray SpatialDistribution::distributeToArray(const T* elements, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	ocean_assert(elements);
	ocean_assert(width > 0 && height > 0);

	ocean_assert(horizontalBins > 0);
	ocean_assert(verticalBins > 0);

	ocean_assert(Scalar(horizontalBins) <= width);
	ocean_assert(Scalar(verticalBins) <= width);

	// reserve enough array elements
	DistributionArray indexArray(left, top, width, height, horizontalBins, verticalBins);

	for (size_t n = 0; n < number; ++n)
	{
		const Vector2& point = tFunction(*elements);

		const unsigned int horizontal = (unsigned int)(indexArray.horizontalBin(point.x()));
		const unsigned int vertical = (unsigned int)(indexArray.verticalBin(point.y()));

		// discard points with negative bin value or bin value larger than the smallest allowed one (due to unsigned int negative values are discarded as well)
		if (horizontal < indexArray.horizontalBins() && vertical < indexArray.verticalBins())
		{
			indexArray(horizontal, vertical).push_back((unsigned int)n);
		}

		++elements;
	}

	return indexArray;
}

inline SpatialDistribution::DistributionArray SpatialDistribution::distributeToArray(const ImagePoint* imagePoints, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int averagePointsPerBin, const unsigned int maxHorizontalBins, const unsigned int maxVerticalBins, unsigned int& horizontalBins, unsigned int& verticalBins)
{
	ocean_assert(imagePoints || number == 0);

	ocean_assert(width > 0 && height > 0);
	ocean_assert(averagePointsPerBin > 0);

	ocean_assert(maxHorizontalBins >= 1u);
	ocean_assert(maxVerticalBins >= 1u);

	if (number == 0)
	{
		return DistributionArray(left, top, width, height, horizontalBins, verticalBins);
	}

	/**
	 * averagePointsPerBin * horizontalBins * verticalBins = number
	 * horizontalBins / verticalBins = width / height
	 */

	const Scalar sqr = max(Scalar(1), (Scalar(number) * width) / (Scalar(averagePointsPerBin) * height));

	horizontalBins = min(maxHorizontalBins, max(1u, (unsigned int)(Numeric::sqrt(sqr))));
	verticalBins = min(maxVerticalBins, max(1u, (unsigned int)(Scalar(horizontalBins) * height / width)));

	return distributeToArray(imagePoints, number, left, top, width, height, horizontalBins, verticalBins);
}

inline ImagePoints SpatialDistribution::distributeAndFilter(const ImagePoint* imagePoints, const size_t numberImagePoints, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	return distributeAndFilter<ImagePoint, &SpatialDistribution::identity>(imagePoints, numberImagePoints, left, top, width, height, horizontalBins, verticalBins);
}

template <typename TIndex>
inline std::vector<TIndex> SpatialDistribution::distributeAndFilterIndices(const ImagePoint* imagePoints, const size_t numberImagePoints, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	return distributeAndFilterIndices<ImagePoint, TIndex, &SpatialDistribution::identity>(imagePoints, numberImagePoints, left, top, width, height, horizontalBins, verticalBins);
}

template <typename T, Vector2 (*tFunction)(const T&)>
typename std::vector<T> SpatialDistribution::distributeAndFilter(const T* elements, const size_t numberElements, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	ocean_assert(elements || numberElements == 0);
	ocean_assert(width >= 0 && height >= 0);
	ocean_assert(horizontalBins >= 1u && verticalBins >= 1u);

	if (numberElements == 0)
	{
		return std::vector<T>();
	}

	OccupancyArray occupancyArray(left, top, width, height, horizontalBins, verticalBins);

	const size_t bins = occupancyArray.bins();

	std::vector<T> result;
	result.reserve(bins);

	for (size_t n = 0; n < numberElements && result.size() < bins; ++n)
	{
		if (occupancyArray.addPoint(tFunction(elements[n])))
		{
			result.push_back(elements[n]);
		}
	}

	return result;
}

template <typename T, Vector2 (*tFunction)(const T&)>
typename std::vector<T> SpatialDistribution::distributeAndFilter(const T* elements, const size_t numberElements, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins, const size_t numberDesiredFilteredElements, Indices32* indices)
{
	ocean_assert(elements != nullptr || numberElements == 0);
	ocean_assert(width >= 0 && height >= 0);
	ocean_assert(horizontalBins >= 1u && verticalBins >= 1u);
	ocean_assert(numberDesiredFilteredElements <= numberElements);

	if (numberElements == 0 || numberDesiredFilteredElements == 0)
	{
		return std::vector<T>();
	}

	OccupancyArray occupancyArray(left, top, width, height, horizontalBins, verticalBins);

	std::vector<unsigned char> usedElements(numberElements, 0u);

	const size_t bins = occupancyArray.bins();

	std::vector<T> result;
	result.reserve(bins);

	unsigned int filterIteration = 0u;

	while (result.size() < numberDesiredFilteredElements && filterIteration < (unsigned int)numberDesiredFilteredElements)
	{
		for (size_t n = 0; n < numberElements && result.size() < numberDesiredFilteredElements; ++n)
		{
			if (usedElements[n] == 0u && occupancyArray.addPointWithCounter(tFunction(elements[n]), filterIteration))
			{
				result.push_back(elements[n]);
				usedElements[n] = 1u;
			}
		}

		filterIteration++;
	}

	if (indices)
	{
		for (size_t n = 0; n < numberElements; ++n)
		{
			if(usedElements[n])
			{
				indices->push_back(Index32(n));
			}
		}
	}

	return result;
}

template <typename T, typename TIndex, Vector2 (*tFunction)(const T&)>
typename std::vector<TIndex> SpatialDistribution::distributeAndFilterIndices(const T* elements, const size_t numberElements, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	ocean_assert(elements || numberElements == 0);
	ocean_assert(width >= 0 && height >= 0);
	ocean_assert(horizontalBins >= 1u && verticalBins >= 1u);

	if (numberElements == 0)
	{
		return std::vector<TIndex>();
	}

	ocean_assert((unsigned long long)(numberElements) <= (unsigned long long)(NumericT<TIndex>::maxValue()));

	OccupancyArray occupancyArray(left, top, width, height, horizontalBins, verticalBins);

	const size_t bins = occupancyArray.bins();

	std::vector<TIndex> result;
	result.reserve(bins);

	for (size_t n = 0; n < numberElements && result.size() < bins; ++n)
	{
		if (occupancyArray.addPoint(tFunction(elements[n])))
		{
			result.push_back(TIndex(n));
		}
	}

	return result;
}

template <typename T>
inline T SpatialDistribution::identity(const T& value)
{
	return value;
}

}

}

#endif // META_OCEAN_GEOMETRY_SPATIAL_DISTRIBUTION_H
