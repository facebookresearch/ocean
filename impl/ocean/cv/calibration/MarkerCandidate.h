/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_MARKER_CANDIDATE_H
#define META_OCEAN_CV_CALIBRATION_MARKER_CANDIDATE_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/Marker.h"
#include "ocean/cv/calibration/Point.h"

#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/// Forward declaration.
class MarkerCandidate;

/**
 * Definition of a vector holding marker candidates.
 * @ingroup cvcalibration
 */
using MarkerCandidates = std::vector<MarkerCandidate>;

/**
 * This class implements a candidate of a marker.
 * The candidate of a marker can have individual levels of certainty:
 * <pre>
 * 1. The 16 border points of the marker are known, and the sign.
 * 2. The 25 points of the marker are known.
 * 3. The marker has a known id.
 * 4. The marker has known neighbors.
 * 5. The marker has a known coordinate, and thus is actually not a candidate anymore but a unique marker associated with a calibration board.
 * </pre>
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT MarkerCandidate : public Marker
{
	public:

		/**
		 * Definition of an unordered map mapping pixel directions to marker candidate indices.
		 */
		using NeighborMap = std::unordered_map<CV::PixelDirection, size_t>;

	public:

		/**
		 * Creates an invalid marker candidate.
		 */
		MarkerCandidate() = default;

		/**
		 * Creates a new marker candidate object with indices of the 16 border points of the marker.
		 * The border indices in the marker are defined as follows:
		 * <pre>
		 *  ----------------
		 * |  0  1  2  3  4 |
		 * | 15           5 |
		 * | 14           6 |
		 * | 13           7 |
		 * | 12  11 10 9  8 |
		 *  ----------------
		 * </pre>
		 * The provided border indices can be provided in clockwise or counter-clockwise order.<br>
		 * Further, the indices can start at any marker corner, the correct order and correct start corner will be determined automatically once more and more information is known/provided.
		 * @param borderIndices The indices of the border points of the marker, the signals of all border points must be identical, must be 16 indices
		 * @param points The points from which the border indices are defined, must be valid
		 */
		MarkerCandidate(const Indices32& borderIndices, const Points& points);

		/**
		 * Returns the index of a point of the marker.
		 * The indices in the marker are defined as follows:
		 * <pre>
		 *  ----------------
		 * |  0  1  2  3  4 |
		 * |  5  6  7  8  9 |
		 * | 10 11 12 13 14 |
		 * | 15 16 17 18 19 |
		 * | 20 21 22 23 24 |
		 *  ----------------
		 * </pre>
		 * @param indexInMarker The index of the point in the marker, with range [0, 24]
		 * @return The index of the point as defined in the points vector, -1 if the point is not known
		 */
		Index32 pointIndex(const size_t indexInMarker) const;

		/**
		 * Returns the index of a point in the border of the marker.
		 * The border indices in the marker are defined as follows:
		 * <pre>
		 *  ----------------
		 * |  0  1  2  3  4 |
		 * | 15           5 |
		 * | 14           6 |
		 * | 13           7 |
		 * | 12  11 10 9  8 |
		 *  ----------------
		 * </pre>
		 * @param indexInBorder The index of the point in the border of the marker, with range [0, 15]
		 * @return The index of the point as defined in the points vector, -1 if the point is not known
		 */
		Index32 borderIndex(const size_t indexInBorder) const;

		/**
		 * Sets the index of a point of the marker.
		 * @param indexInMarker The index of the point in the marker, with range [0, 24]
		 * @param pointIndex The index of the point as defined in the points vector, -1 if the point is not known
		 */
		void setPointIndex(const size_t indexInMarker, const Index32 pointIndex);

		/**
		 * Sets the coordinate of the marker.
		 * The coordinate of the marker describes the position of the marker within a calibration board.
		 * @param markerCoordinate The coordinate of the marker to set
		 */
		inline void setMarkerCoordinate(const CV::PixelPosition& markerCoordinate);

		/**
		 * Returns the position of this marker within a calibration board.
		 * The coordinate of the marker describes the position of the marker within a calibration board.
		 * @return The marker's coordinate, invalid if unknown
		 * @see hasMarkerCoordinate().
		 */
		inline const CV::PixelPosition& markerCoordinate() const;

		/**
		 * Returns the marker candidate rotated by a multiple of 90 degree.
		 * @param degree The number of 90 degree to rotate, in clockwise order, possible values are {-360, -270, ..., 0, ..., 270, 360}
		 * @return The rotated marker candidate
		 */
		MarkerCandidate rotatedClockWise(const int degree) const;

		/**
		 * Rotates this marker candidate by a multiple of 90 degree.
		 * Rotating the marker candidate will also rotate the directions of the neighbors accordingly.
		 * @param degree The number of 90 degree to rotate, in clockwise order, possible values are {-360, -270, ..., 0, ..., 270, 360}
		 */
		void rotateClockWise(const int degree);

		/**
		 * Returns whether this marker candidate is a neighbor of a given marker candidate.
		 * The neighboring marker candidate must have a different sign (as neighboring markers have do not have the same sign/color).
		 * @param markerCandidate The marker candidate to check, must be valid
		 * @param points The points associated with both marker candidates
		 * @param localEdge The resulting local edge of this marker candidate at which the neighbor marker candidate is located, in case the marker candidate is a neighbor
		 * @param neighborEdge The resulting edge of the neighbor marker candidate at which this marker candidate is located, in case the marker candidate
		 * @param maxDistancePercentage The maximal distance between the predicted corner and the actual corner of a neighboring marker candidate, in relation to the distance between the two marker candidate corners, with range (0, infinity)
		 * @return True, if the provided marker candidate is a neighbor of this marker candidate
		 */
		bool isNeighbor(const MarkerCandidate& markerCandidate, const Points& points, CV::PixelDirection& localEdge, CV::PixelDirection& neighborEdge, const Scalar maxDistancePercentage) const;

		/**
		 * Returns whether this marker candidate has an edge which is close to a predicted edge.
		 * This marker candidate must have valid border indices for this check.
		 * @param predictedCornerA The first corner of the predicted edge
		 * @param predictedCornerB The second corner of the predicted edge
		 * @param points The points associated with this marker candidate
		 * @param maxSqrDistanceA The maximal square distance between the predicted first corner and the actual first corner of this marker candidate, with range [0, infinity)
		 * @param maxSqrDistanceB The maximal square distance between the predicted second corner and the actual second corner of this marker candidate, with range [0, infinity)
		 * @return The direction in which the predicted edge is located in this marker candidate, PD_INVALID if the predicted edge is not close to an edge of this marker candidate
		 */
		CV::PixelDirection hasEdge(const Vector2& predictedCornerA, const Vector2& predictedCornerB, const Points& points, const Scalar maxSqrDistanceA, const Scalar maxSqrDistanceB) const;

		/**
		 * Adds a marker candidate as neighbor to this marker candidate.
		 * The neighbor direction is defined as the direction from this marker candidate to the neighbor marker candidate.
		 * @param neighborDirection The direction in which the neighbor marker candidate is located, must not be a diagonal direction
		 * @param neighborMarkerCandidateIndex The index of the neighboring marker candidate
		 */
		inline void addNeighbor(const CV::PixelDirection neighborDirection, const size_t neighborMarkerCandidateIndex);

		/**
		 * Returns the known neighbors of this marker candidate.
		 * @return The marker candidate's neighbors
		 */
		inline const NeighborMap& neighbors() const;

		/**
		 * Returns the center position of this marker candidate.
		 * The center position is defined as the average of all known points of the marker candidate.
		 * @param points The points associated with this marker candidate, must be valid
		 * @return The center position of this marker candidate
		 */
		Vector2 center(const Points& points) const;

		/**
		 * Returns whether this marker candidate has 16 valid border point indices.
		 * Having 16 valid border indices is the minimal requirement for a marker candidate.
		 * @return True, if so
		 * @see hasValidIndices().
		 */
		bool hasValidBorderIndices() const;

		/**
		 * Returns whether this marker candidate has 25 valid point indices.
		 * Having 25 valid point indices means that all points of the marker are known but does not yet mean that the marker has a valid id.
		 * @return True, if so
		 */
		bool hasValidIndices() const;

		/**
		 * Returns whether this marker candidate has at least one known neighbor candidate marker.
		 * @return True, if so
		 */
		inline bool hasNeighbor() const;

		/**
		 * Returns whether this marker candidate has at least one known neighbor with a known valid marker id.
		 * @param markerCandidates The marker candidates from which the neighbors are checked, must be valid
		 * @return True, if so
		 */
		bool hasNeighborWithMarkerId(const MarkerCandidates& markerCandidates) const;

		/**
		 * Returns whether a specific marker candidate is a known neighbor of this marker candidate.
		 * @param markerCandidateIndex The index of the marker candidate to check, with range [0, markerCandidates.size())
		 * @return The direction in which the marker candidate is located, PD_INVALID if the marker candidate is not a known neighbor
		 */
		CV::PixelDirection neighborDirection(const size_t markerCandidateIndex) const;

		/**
		 * Returns whether this marker candidate has a known marker coordinate.
		 * Having a valid marker coordinate means that this candidate has been associated with a unique position within a calibration board.
		 * @return True, if so
		 */
		inline bool hasMarkerCoordinate() const;

		/**
		 * Determines the id of the marker candidate.
		 * The marker id is determined based on the 25 known points of the marker candidate.<br>
		 * @param points The points associated with this marker candidate, must be valid
		 * @return True, if succeeded; False, if the marker candidate does not have 25 valid point indices or if the points do not match with any known marker layout
		 */
		bool determineMarkerId(const Points& points);

		/**
		 * Returns whether this marker candidate has been initialized with valid border indices.
		 * A valid marker candidate does not yet mean that the candidate is actually a valid marker with known id, markers coordinate etc.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the 25 marker indices for given 16 border indices.
		 * @param borderIndices The border indices of the marker candidate, must be 16 indices
		 * @param clockWise True, to receive the indices in clockwise order; False, to receive the indices in counter-clockwise order
		 * @return The resulting 25 marker indices, the inner indices will be invalid
		 */
		static Indices32 borderIndicesToMarkerIndices(const Indices32& borderIndices, const bool clockWise);

		/**
		 * Rotates 25 marker indices clockwise by 90 degree.
		 * @param indices The 25 marker indices to rotate
		 * @return The resulting rotated 25 marker indices
		 */
		static Indices32 rotateIndicesClockWise90(const Indices32& indices);

		/**
		 * Rotates the 25 marker indices by 180 degree.
		 * @param indices The 25 marker indices to rotate
		 * @return The resulting rotated 25 marker indices
		 */
		static Indices32 rotateIndices180(const Indices32& indices);

		/**
		 * Rotates 25 marker indices counter-clockwise by 90 degree.
		 * @param indices The 25 marker indices to rotate
		 * @return The resulting rotated 25 marker indices
		 */
		static Indices32 rotateIndicesCounterClockWise90(const Indices32& indices);

		/**
		 * Removes a marker candidate from a vector of marker candidates.
		 * Further, the marker candidate will be removed as known neighbor from all other marker candidates.
		 * @param markerCandidates The vector of marker candidates from which the marker candidate will be removed, must be valid
		 * @param index The index of the marker candidate to be removed, with range [0, markerCandidates.size())
		 */
		static void removeMarkerCandidate(MarkerCandidates& markerCandidates, const size_t index);

	protected:

		/// The 25 indices of the points of the marker, empty if invalid.
		Indices32 pointIndices_;

		/// The map mapping the four neighbor directions to other marker candidates.
		NeighborMap neighborMap_;

		/// The coordinate of the marker, invalid if unknown.
		CV::PixelPosition markerCoordinate_;
};

inline void MarkerCandidate::setMarkerCoordinate(const CV::PixelPosition& markerCoordinate)
{
	ocean_assert(!markerCoordinate_.isValid());
	markerCoordinate_ = markerCoordinate;
}

inline const CV::PixelPosition& MarkerCandidate::markerCoordinate() const
{
	ocean_assert(markerCoordinate_.isValid());
	return markerCoordinate_;
}

inline void MarkerCandidate::addNeighbor(const CV::PixelDirection neighborDirection, const size_t neighborMarkerCandidateIndex)
{
	ocean_assert(neighborDirection == CV::PD_NORTH || neighborDirection == CV::PD_SOUTH || neighborDirection == CV::PD_WEST || neighborDirection == CV::PD_EAST);
	ocean_assert(neighborMap_.find(neighborDirection) == neighborMap_.cend());

	neighborMap_[neighborDirection] = neighborMarkerCandidateIndex;
}

inline const MarkerCandidate::NeighborMap& MarkerCandidate::neighbors() const
{
	return neighborMap_;
}

bool MarkerCandidate::hasNeighbor() const
{
	return !neighborMap_.empty();
}

inline bool MarkerCandidate::hasMarkerCoordinate() const
{
	return markerCoordinate_.isValid();
}

inline bool MarkerCandidate::isValid() const
{
	return pointIndices_.size() == 25;
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_MARKER_CANDIDATE_H
