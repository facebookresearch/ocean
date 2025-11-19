/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_CALIBRATION_BOARD_H
#define META_OCEAN_CV_CALIBRATION_CALIBRATION_BOARD_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/Marker.h"
#include "ocean/cv/calibration/MarkerCandidate.h"

#include "ocean/base/StaticVector.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements a basic calibration board.
 * The calibration board is defined by a unique id, a number of horizontal and vertical markers, and the actual board markers with their positions and orientations.
 * @see MetricCalibrationBoard.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT CalibrationBoard
{
	public:

		/**
		 * Definition of a coordinate for markers.
		 */
		using MarkerCoordinate = CV::PixelPosition;

		/**
		 * This class implements a marker in a calibration board.
		 */
		class BoardMarker : public Marker
		{
			public:

				/**
				 * Creates a new valid board marker.
				 * @param markerId The id of the marker, with range [0, Marker::numberMarkerIds() - 1]
				 * @param sign True, if the marker has positive sign (mainly black dot's on white background); False, if the marker has negative sign (mainly white dot's on black background)
				 * @param orientation The orientation of this marker in relation to the calibration board, must be valid
				 * @param coordinate The coordinate of the marker in the calibration board, with range [0, CalibrationBoard::xMarkers() - 1]x[0, CalibrationBoard::yMarkers() - 1]
				 */
				inline BoardMarker(const size_t markerId, const bool sign, const CV::PixelDirection orientation, const MarkerCoordinate& coordinate);

				/**
				 * Returns the orientation of this marker in relation to the calibration board.
				 * @return The marker's orientation
				 */
				inline CV::PixelDirection orientation() const;

				/**
				 * Returns the coordinate of this marker within the calibration board.
				 * @return The marker's coordinate, with range [0, CalibrationBoard::xMarkers() - 1]x[0, CalibrationBoard::yMarkers() - 1]
				 */
				inline const MarkerCoordinate& coordinate() const;

				/**
				 * Returns the sign of point in this marker.
				 * @param x The horizontal position of the point in the marker, with range [0, Marker::numberRowsColumns() - 1]
				 * @param y The vertical position of the point in the marker, with range [0, Marker::numberRowsColumns() - 1]
				 * @return True, if the point has a positive sign (black dot on white background); False, if the point has a negative sign (white dot on black background)
				 * @tparam tOriented True, if the specified point is defined within the marker's orientation; False, if the specified point is defined within the marker's unoriented layout
				 */
				template <bool tOriented>
				inline bool pointSign(const size_t x, const size_t y) const;

				/**
				 * Returns the sign of point in this marker.
				 * @param indexInMarkerUnoriented The index of the point in the marker, with range [0, numberPoints() - 1]
				 * @return True, if the point has a positive sign (black dot on white background); False, if the point has a negative sign (white dot on black background)
				 * @tparam tOriented True, if the specified point is defined within the marker's orientation; False, if the specified point is defined within the marker's unoriented layout
				 */
				template <bool tOriented>
				inline bool pointSign(const size_t indexInMarker) const;

				/**
				 * Returns the 3D object point of a marker point of this board marker in the coordinate system of the calibration board.
				 * @param markerPosition The position of the marker in the calibration board, defined in the board's xz-plane
				 * @param xMarkerSize The horizontal size of the marker in the calibration board, with range (0, infinity)
				 * @param zMarkerSize The vertical size of the marker in the calibration board, with range (0, infinity)
				 * @param indexInMarker The index of the marker point, with range [0, Marker::numberPoints() - 1]
				 * @return The 3D object point of the marker point, defined in the calibration board
				 */
				Vector3 objectPoint(const Vector3& markerPosition, const Scalar xMarkerSize, const Scalar zMarkerSize, const size_t indexInMarker) const;

				/**
				 * Returns the direction to a neighbor marker in relation to this marker's orientation.
				 * @param neighborBoardMarker The neighbor marker to which the direction will be determined, must be valid
				 * @return The direction to the neighbor marker, in relation to this marker's orientation
				 */
				CV::PixelDirection neighborDirection(const BoardMarker& neighborBoardMarker) const;

				/**
				 * Returns the marker coordinate for a neighbor marker.
				 * @param neighborDirection The direction to the neighbor marker, in relation to this marker's orientation, must be valid
				 * @param xMarkers The number of horizontal markers in the calibration board, with range [1, CalibrationBoard::xMarkers() - 1]
				 * @param yMarkers The number of vertical markers in the calibration board, with range [1, CalibrationBoard::yMarkers() - 1]
				 * @return The marker coordinate of the neighbor marker, if the neighbor marker exists, otherwise an invalid marker coordinate
				 */
				MarkerCoordinate boardMarkerNeighborCoordinate(const CV::PixelDirection& neighborDirection, const size_t xMarkers, const size_t yMarkers) const;

				/**
				 * Returns whether this board maker is valid.
				 * A valid board marker has a valid marker id, a valid sign, a valid orientation, and a valid coordinate.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/// The orientation of this marker in relation to the calibration board.
				CV::PixelDirection orientation_ = CV::PD_INVALID;

				/// The rotation between the marker and the calibration board, a rotation around the y-axis with 90 deg steps.
				Quaternion board_R_marker_ = Quaternion(false);

				/// The coordinate of the marker in the calibration board, with range [0, CalibrationBoard::xMarkers() - 1]x[0, CalibrationBoard::yMarkers() - 1]
				MarkerCoordinate coordinate_;
		};

		/**
		 * Definition of a vector holding board markers.
		 */
		using BoardMarkers = std::vector<BoardMarker>;

		/**
		 * Definition of a static vector allowing to store up to four marker coordinates.
		 */
		using MarkerCoordinates = StaticVector<MarkerCoordinate, 4u>;

		/**
		 * Definition of a map mapping neighbor directions to (up to four) marker coordinates.
		 */
		using NeighborMarkerCoordinateMap = std::unordered_map<CV::PixelDirection, MarkerCoordinate>;

		/**
		 * Definition of an object point id composed of a marker coordinate and marker point index.
		 */
		class ObjectPointId
		{
			public:

				/**
				 * Definition of a hash object for object point ids.
				 */
				struct Hash
				{
					/**
					 * Hash function for object point ids.
					 * @param objectPointId The object point id to be hashed
					 * @return The hash value
					 */
					inline size_t operator()(const ObjectPointId& objectPointId) const;
				};

			public:

				/**
				 * Default constructor creating an invalid object point id.
				 */
				ObjectPointId() = default;

				/**
				 * Creates a new object point id.
				 * @param markerCoordinate The coordinate of the marker to which the object point belongs
				 * @param indexInMarker The index of the marker point, with range [0, 24]
				 */
				inline ObjectPointId(const MarkerCoordinate& markerCoordinate, const size_t indexInMarker);

				/**
				 * Returns the coordinate of the marker to which the object point belongs.
				 * @return The marker coordinate
				 */
				inline const MarkerCoordinate& markerCoordinate() const;

				/**
				 * Returns the index of the marker point, with range [0, 24].
				 * @return The index of the marker point
				 */
				inline size_t indexInMarker() const;

				/**
				 * Returns whether this object point id is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns whether two object point ids are identical.
				 * @param objectPointId The second object point id to compare
				 * @return True, if so
				 */
				inline bool operator==(const ObjectPointId& objectPointId) const;

			protected:

				/// The coordinate of the marker to which the object point belongs.
				MarkerCoordinate markerCoordinate_;

				/// The index of the marker point, with range [0, 24].
				size_t indexInMarker_ = size_t(-1);
		};

		/**
		 * Definition of a vector holding object point ids.
		 */
		using ObjectPointIds = std::vector<ObjectPointId>;

		/**
		 * Definition of an unordered set holding object point ids.
		 */
		using ObjectPointIdSet = std::unordered_set<ObjectPointId, ObjectPointId::Hash>;

	protected:

		/**
		 * Definition of an unordered map mapping unique values for pairs of markers to counters.
		 */
		using NeighborMap = std::unordered_map<uint64_t, unsigned int>;

		/**
		 * Definition of an unordered map mapping marker types to marker coordinates.
		 */
		using MarkerIdMap = std::unordered_map<BoardMarker::MarkerType, MarkerCoordinates>;

	public:

		/**
		 * Default constructor creating an invalid calibration board.
		 */
		CalibrationBoard() = default;

		/**
		 * Default copy constructor.
		 * @param calibrationBoard The calibration board to copy
		 */
		CalibrationBoard(const CalibrationBoard& calibrationBoard) = default;

		/**
		 * Creates a new calibration board.
		 * @param xMarkers The number of horizontal markers the calibration board will have, with range [1, infinity)
		 * @param yMarkers The number of vertical markers the calibration board will have, with range [1, infinity)
		 * @param boardMarkers The board markers of the calibration board, must be xMarkers * yMarkers markers
		 */
		CalibrationBoard(const size_t xMarkers, const size_t yMarkers, BoardMarkers&& boardMarkers);

		/**
		 * Returns the hash of this calibration board.
		 * @return The calibration board's hash
		 */
		inline uint64_t hash() const;

		/**
		 * Returns the board marker at a specific position.
		 * @param markerCoordinate The coordinate of the marker to return, with range [0, xMarkers() - 1]x[0, yMarkers() - 1]
		 * @return The board marker at the specified position
		 */
		inline const BoardMarker& marker(const MarkerCoordinate& markerCoordinate) const;

		/**
		 * Returns the board marker at a specific position.
		 * @param xMarker The horizontal position of the marker, with range [0, xMarkers() - 1]
		 * @param yMarker The vertical position of the marker, with range [0, yMarkers() - 1]
		 * @return The board marker at the specified position
		 */
		inline const BoardMarker& marker(const size_t xMarker, const size_t yMarker) const;

		/**
		 * Returns the number of horizontal markers of this calibration board.
		 * @return The calibration board's number of horizontal markers, with range [1, infinity)
		 */
		inline size_t xMarkers() const;

		/**
		 * Returns the number of vertical markers of this calibration board.
		 * @return The calibration board's number of vertical markers, with range [1, infinity)
		 */
		inline size_t yMarkers() const;

		/**
		 * Returns the number of markers of this calibration board.
		 * @return The calibration board's number of markers, which is xMarkers() * yMarkers(), with range [1, infinity)
		 */
		inline size_t markers() const;

		/**
		 * Returns the number of points of this calibration board.
		 * @return The calibration board's number of points, which is markers() * Marker::numberPoints(), with range [1, infinity)
		 */
		inline size_t numberPoints() const;

		/**
		 * Checks whether this calibration board contains a marker candidate with it's specified neighboring marker candidates.
		 * @param markerCandidates All marker candidates, at least one
		 * @param markerCandidateIndex The index of the marker candidate to check, with range [0, markerCandidates.size() - 1]
		 * @param markerCoordinate Optional resulting marker coordinate of the marker candidate, if the marker candidate is part of this calibration board; nullptr if not of interest
		 * @param neighborMarkerCoordinateMap Optional resulting neighbor map of the marker candidate; nullptr if not of interest
		 * @return True, if so
		 */
		bool containsMarkerCandidateWithNeighborhood(const MarkerCandidates& markerCandidates, const size_t markerCandidateIndex, MarkerCoordinate* markerCoordinate = nullptr, NeighborMarkerCoordinateMap* neighborMarkerCoordinateMap = nullptr) const;

		/**
		 * Returns whether this calibration board is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Creates a unique calibration board based on a unique seed value and the number of markers the board has.
		 * In case the underlying random generator implementation is changing, the resulting calibration board may change as well.
		 * @param seed The seed value which is used for the random number generator to create a unique board, with range [0, infinity)
		 * @param xMarkers The number of horizontal markers the calibration board will have, with range [1, infinity)
		 * @param yMarkers The number of vertical markers the calibration board will have, with range [1, infinity)
		 * @param calibrationBoard The resulting calibration board
		 * @return True, if succeeded
		 */
		static bool createCalibrationBoard(const unsigned int seed, const size_t xMarkers, const size_t yMarkers, CalibrationBoard& calibrationBoard);

		/**
		 * Determines the optimal marker grid for a calibration board with specific aspect ratio and number of markers.
		 * @param aspectRatio The aspect ratio of the calibration board (width/height), with range (0, infinity)
		 * @param minimalNumberMarkers The minimal number of markers of the calibration board will have, with range [1, infinity)
		 * @param xMarkers The resulting number of horizontal markers, with range [1, infinity)
		 * @param yMarkers The resulting number of vertical markers, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool determineOptimalMarkerGrid(const double aspectRatio, const size_t minimalNumberMarkers, size_t& xMarkers, size_t& yMarkers);

		/**
		 * Determines the optimal marker grid for a calibration board with specific aspect ratio and number of horizontal markers.
		 * @param aspectRatio The aspect ratio of the calibration board (width/height), with range (0, infinity)
		 * @param xMarkers The number of horizontal markers, with range [1, infinity)
		 * @param yMarkers The resulting number of vertical markers, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool determineOptimalMarkerGrid(const double aspectRatio, const size_t xMarkers, size_t& yMarkers);

		/**
		 * Determines the uniqueness between two calibration board.
		 * @param calibrationBoardA The first calibration board, must be valid
		 * @param calibrationBoardB The second calibration board, must be valid
		 * @param oneIdenticalNeighborCounter The number of identical 1-neighbor pairs both calibration boards have in common, with range [0, infinity)
		 * @param twoIdenticalNeighborCounter The number of identical 2-neighbor pairs both calibration boards have in common, with range [0, infinity)
		 * @param threeIdenticalNeighborCounter The number of identical 3-neighbor pairs both calibration boards have in common, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool determineUniqueness(const CalibrationBoard& calibrationBoardA, const CalibrationBoard& calibrationBoardB, size_t& oneIdenticalNeighborCounter, size_t& twoIdenticalNeighborCounter, size_t& threeIdenticalNeighborCounter);

		/**
		 * Returns a representative hash based on the properties of the calibration board.
		 * The hash is computed from the board dimensions and all marker properties (id, sign, orientation, coordinate).
		 * @param calibrationBoard The calibration board for which the hash will be computed, must be valid
		 * @return The resulting hash value
		 */
		static uint64_t hash(const CalibrationBoard& calibrationBoard);

	protected:

		/**
		 * Checks whether this calibration board contains a board marker with specific marker type.
		 * @param markerCandidate The marker candidate to check, must have a valid marker id and sign
		 * @param markerCoordinates Optional resulting marker coordinates of all markers with same id and sign, at most four; nullptr if not of interest
		 * @return True, if so
		 */
		bool containsMarkerTypeIgnoringNeighborhood(const MarkerCandidate& markerCandidate, MarkerCoordinates* markerCoordinates = nullptr) const;

		/**
		 * Creates unique board markers for a calibration board with specific id and number of markers.
		 * The resulting markers are unique in the sense that the 4-neighborhood of each marker is unique in the board and cannot be found again in the same board.
		 * The provided id is used as a seed for the random number generator.
		 * @param id The unique id (the seed) of the calibration board, with range [0, infinity)
		 * @param xMarkers The number of horizontal markers the calibration board will have, with range [1, infinity)
		 * @param yMarkers The number of vertical markers the calibration board will have, with range [1, infinity)
		 * @param boardMarkers The resulting board markers of the calibration board
		 * @return True, if succeeded; False, if the provided id and number of markers does not allow to create a unique board
		 */
		static bool createUniqueBoardMarkers(const unsigned int id, const size_t xMarkers, const size_t yMarkers, BoardMarkers& boardMarkers);

		/**
		 * Returns a unique value for a pairs of neighboring board markers.
		 * The unique id contains the marker types, and the mutual orientation of the markers.
		 * @param boardMarker The first board marker, must be valid
		 * @param neighborBoardMarker The second (neighbor) board marker, must be valid
		 * @return The unique value for the pair of board markers
		 */
		static uint64_t uniqueValueOrientedNeighborhood(const BoardMarker& boardMarker, const BoardMarker& neighborBoardMarker);

		/**
		 * Combines two unique values to one unique value.
		 * @param valueA The first unique value, must be valid
		 * @param valueB The second unique value, must be valid
		 * @return The combined unique value
		 */
		static uint64_t uniqueValue(const uint32_t valueA, const uint32_t valueB);

	protected:

		/// The number of horizontal markers of this calibration board, with range [1, infinity).
		size_t xMarkers_ = 0;

		/// The number of vertical markers of this calibration board, with range [1, infinity).
		size_t yMarkers_ = 0;

		/// The board markers of this calibration board, with xMarkers_ * yMarkers_ markers.
		BoardMarkers boardMarkers_;

		/// The map mapping marker types to marker coordinates (allows to determine the marker coordinates of all markers with specific id and sign).
		MarkerIdMap markerIdMap_;

		/// The hash of this calibration board.
		uint64_t hash_ = 0ull;
};

inline CalibrationBoard::BoardMarker::BoardMarker(const size_t markerId, const bool sign, const CV::PixelDirection orientation, const MarkerCoordinate& coordinate) :
	Marker(markerId, sign),
	orientation_(orientation),
	board_R_marker_(Vector3(0, 1, 0), Numeric::deg2rad(Scalar(orientation))),
	coordinate_(coordinate)
{
	ocean_assert(isValid());
}

inline CV::PixelDirection CalibrationBoard::BoardMarker::orientation() const
{
	return orientation_;
}

inline const CalibrationBoard::MarkerCoordinate& CalibrationBoard::BoardMarker::coordinate() const
{
	return coordinate_;
}

template <bool tOriented>
inline bool CalibrationBoard::BoardMarker::pointSign(const size_t x, const size_t y) const
{
	ocean_assert(isValid());

	if constexpr (tOriented)
	{
		// the defined point coordinates x and y are defined within the marker's orientation, so we don't need to rotate the coordinates

		return LayoutManager::layoutPointSign(markerId(), sign(), CV::PD_NORTH, x, y);
	}
	else
	{
		// the defined point coordinates x and y are defined within the marker's unoriented layout

		return LayoutManager::layoutPointSign(markerId(), sign(), orientation(), x, y);
	}
}

template <bool tOriented>
inline bool CalibrationBoard::BoardMarker::pointSign(const size_t indexInMarker) const
{
	ocean_assert(isValid());

	if constexpr (tOriented)
	{
		// the defined point index is defined within the marker's orientation, so we don't need to rotate the index

		return LayoutManager::layoutPointSign(markerId(), sign(), CV::PD_NORTH, indexInMarker);
	}
	else
	{
		// the defined point index is defined within the marker's unoriented layout

		return LayoutManager::layoutPointSign(markerId(), sign(), orientation(), indexInMarker);
	}
}

inline bool CalibrationBoard::BoardMarker::isValid() const
{
	ocean_assert(orientation_ == CV::PD_INVALID || board_R_marker_.isValid());

	return Marker::isValid() && orientation_ != CV::PD_INVALID && coordinate_.isValid();
}

inline size_t CalibrationBoard::ObjectPointId::Hash::operator()(const ObjectPointId& objectPointId) const
{
	size_t seed = std::hash<unsigned int>{}(objectPointId.markerCoordinate_.x());
	seed ^= std::hash<unsigned int>{}(objectPointId.markerCoordinate_.y()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<size_t>{}(objectPointId.indexInMarker_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

inline CalibrationBoard::ObjectPointId::ObjectPointId(const MarkerCoordinate& markerCoordinate, const size_t indexInMarker) :
	markerCoordinate_(markerCoordinate),
	indexInMarker_(indexInMarker)
{
	ocean_assert(isValid());
}

inline const CalibrationBoard::MarkerCoordinate& CalibrationBoard::ObjectPointId::markerCoordinate() const
{
	return markerCoordinate_;
}

inline size_t CalibrationBoard::ObjectPointId::indexInMarker() const
{
	return indexInMarker_;
}

inline bool CalibrationBoard::ObjectPointId::isValid() const
{
	return markerCoordinate_.isValid() && indexInMarker_ < 25;
}

inline bool CalibrationBoard::ObjectPointId::operator==(const ObjectPointId& objectPointId) const
{
	return objectPointId.markerCoordinate_ == markerCoordinate_ && objectPointId.indexInMarker_ == indexInMarker_;
}

inline uint64_t CalibrationBoard::hash() const
{
	return hash_;
}

inline const CalibrationBoard::BoardMarker& CalibrationBoard::marker(const MarkerCoordinate& markerCoordinate) const
{
	ocean_assert(markerCoordinate.x() < xMarkers_);
	ocean_assert(markerCoordinate.y() < yMarkers_);

	return boardMarkers_[xMarkers_ * markerCoordinate.y() + markerCoordinate.x()];
}

inline const CalibrationBoard::BoardMarker& CalibrationBoard::marker(const size_t xMarker, const size_t yMarker) const
{
	return marker(MarkerCoordinate((unsigned int)(xMarker), (unsigned int)(yMarker)));
}

inline size_t CalibrationBoard::xMarkers() const
{
	return xMarkers_;
}

inline size_t CalibrationBoard::yMarkers() const
{
	return yMarkers_;
}

inline size_t CalibrationBoard::markers() const
{
	ocean_assert(isValid());
	ocean_assert(boardMarkers_.size() == xMarkers_ * yMarkers_);

	return boardMarkers_.size();
}

inline size_t CalibrationBoard::numberPoints() const
{
	return markers() * Marker::numberPoints();
}

bool CalibrationBoard::isValid() const
{
	return xMarkers_ >= 1 && yMarkers_ >= 1;
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_CALIBRATION_BOARD_H
