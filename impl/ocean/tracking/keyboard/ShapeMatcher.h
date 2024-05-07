// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_KEYBOARD_SHAPE_MATCHER_H
#define META_OCEAN_TRACKING_KEYBOARD_SHAPE_MATCHER_H

#include "ocean/tracking/keyboard/Keyboard.h"

#include "ocean/cv/detector/ShapeDetector.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Tracking
{

namespace Keyboard
{

/**
 * This class implements a matcher for shape objects.
 * The matcher is able to match individual shapes with each other based on their location and e.g., orientation.
 * Further, the matcher implements the logic for partial matches (e.g., between an L-shape and a T-shape).
 * @ingroup trackingkeyboard
 */
class OCEAN_TRACKING_KEYBOARD_EXPORT ShapeMatcher
{
	public:

		using TwoLineShape = CV::Detector::ShapeDetector::TwoLineShape;

		using LShape = CV::Detector::ShapeDetector::LShape;
		using LShapes = CV::Detector::ShapeDetector::LShapes;

		using TShape = CV::Detector::ShapeDetector::TShape;
		using TShapes = CV::Detector::ShapeDetector::TShapes;

		using XShape = CV::Detector::ShapeDetector::XShape;
		using XShapes = CV::Detector::ShapeDetector::XShapes;

		/**
		 * Definition of individual match types.
		 */
		enum MatchType : uint32_t
		{
			/// Unknown match type.
			MT_UNKNOWN = 0u,
			/// No match due to a wrong position.
			MT_WRONG_POSTION = 1u,
			/// No match due to not aligning shapes.
			MT_NO_MATCH = 2u,
			/// A partial match e.g., a T-shape matching with an L-shape.
			MT_PARTIAL_MATCH = 3u,
			/// A perfect match e.g., a T-shape with a T-shape.
			MT_PERFECT_MATCH = 4u
		};

	public:

		/**
		 * Creates a new matcher object with given thresholds.
		 * @param maximalDistance The maximal distance between two shapes to be considered as match, with range [0, infinity)
		 * @param angleThreshold The maximal angle between two shape directions to be considered as match, in radian, with range [0, PI_2)
		 * @param reference_O_candidate An optional explicit position offset between candidate shapes and reference shapes e.g., to determine matches with constant translational offset, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		inline ShapeMatcher(const Scalar maximalDistance, const Scalar angleThreshold, const Vector2& reference_O_candidate = Vector2(0, 0));

		/**
		 * Returns the maximal distance between two shapes to be considered as match.
		 * @return The matcher's maximal distance, with range [0, infinity)
		 */
		inline Scalar maximalDistance() const;

		/**
		 * Returns the explicit position offset between candidate shapes and reference shapes.
		 * @return The matcher's explicit position offset, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		 inline const Vector2& reference_O_candidate() const;

		/**
		 * Sets or changes the explicit position offset between candidate shapes and reference shapes.
		 * @param reference_O_candidate The matcher's explicit position offset, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		inline void setReference_O_candidate(const Vector2& reference_O_candidate);

		/**
		 * Returns the match type for two individual shapes.
		 * In general, the matching hierarchy is:
		 * <pre>
		 * X-shape >= T-shape >= L-shape
		 * </pre>
		 * That means that e.g., a candidate L-shape can be matched to a reference T-shape (or X-shape).
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tCheckDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tCheckDistance, bool tUseTransformation>
		MatchType matchShapes(const TwoLineShape& shapeReference, const TwoLineShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Returns the match type for two individual shapes.
		 * In general, the matching hierarchy is:
		 * <pre>
		 * X-shape >= T-shape >= L-shape
		 * </pre>
		 * That means that e.g., a candidate L-shape can be matched to a reference T-shape (or X-shape).
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param checkDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tUseTransformation>
		MatchType matchShapes(const TwoLineShape& shapeReference, const TwoLineShape& shapeCandidate, const bool checkDistance = true, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Returns the match type between two L-shapes.
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tCheckDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tCheckDistance = true, bool tUseTransformation = false>
		inline MatchType matchShapes(const LShape& shapeReference, const LShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Returns the match type between two T-shapes.
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tCheckDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tCheckDistance = true, bool tUseTransformation = false>
		inline MatchType matchShapes(const TShape& shapeReference, const TShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Returns the match type between a T-shape and a L-shape.
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tCheckDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tCheckDistance = true, bool tUseTransformation = false>
		inline MatchType matchShapes(const TShape& shapeReference, const LShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Returns the match type between a T-shape and a X-shape.
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tCheckDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tCheckDistance = true, bool tUseTransformation = false>
		inline MatchType matchShapes(const TShape& shapeReference, const XShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Returns the match type between two X-shapes.
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tCheckDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tCheckDistance = true, bool tUseTransformation = false>
		inline MatchType matchShapes(const XShape& shapeReference, const XShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Returns the match type between a X-shape and a L-shape.
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tCheckDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tCheckDistance = true, bool tUseTransformation = false>
		inline MatchType matchShapes(const XShape& shapeReference, const LShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Returns the match type between a X-shape and a T-shape.
		 * @param shapeReference The reference shape, must be valid
		 * @param shapeCandidate The candidate shape, must be valid
		 * @param reference_H_shiftedCandidate Optional homography transformation between (potentially shifted) candidate and reference, nullptr otherwise
		 * @return The resulting match type
		 * @tparam tCheckDistance True, to check the distance between both shapes; False, to skip the check (e.g., as the distance has been checked already)
		 * @tparam tUseTransformation True, to use `reference_H_shiftedCandidate` which must be valid; False, to skip using the transformation
		 */
		template <bool tCheckDistance = true, bool tUseTransformation = false>
		inline MatchType matchShapes(const XShape& shapeReference, const TShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Determines groups of positions located on the same horizontal lines (positions having the same vertical coordinate).
		 * @param height The height of the frame in which the positions are located, with range [1, infinity)
		 * @param positions The postions to be grouped, with range (-infinity, infinity)x[0, height), at least one
		 * @param size The number of given positions, with range [1, infinity)
		 * @param minimalGroupSize The minimal number of positions for a valid group, with range [1, infinity)
		 * @param maximalGroupRadius The maximal distance between the center of the group and any element of the group, with range [0, height)
		 * @param groupPositions Optional resulting positions (vertical coordinates) of the groups, one for each resulting group
		 * @return The groups of given positions
		 */
		static std::vector<Indices32> determineHorizontalGroups(const unsigned int height, const Vector2* positions, const size_t size, const unsigned int minimalGroupSize = 10u, const Scalar maximalGroupRadius = Scalar(2), Scalars* groupPositions = nullptr);

		/**
		 * Merges several given L-shapes which represent the same L-shape to one L-shape.
		 * @param lShapes All L-shapes from which some will be merged together, must be valid
		 * @param indices The indices of all L-shapes which will be used for merging, at least one
		 * @return The resulting new L-shape, with position and direction best fitting with all L-shapes used for merging
		 */
		static LShape mergeShapes(const LShape* lShapes, const Indices32& indices);

		/**
		 * Merges several given T-shapes which represent the same T-shape to one T-shape.
		 * @param tShapes All T-shapes from which some will be merged together, must be valid
		 * @param indices The indices of all T-shapes which will be used for merging, at least one
		 * @return The resulting new T-shape, with position and direction best fitting with all T-shapes used for merging
		 */
		static TShape mergeShapes(const TShape* tShapes, const Indices32& indices);

		/**
		 * Merges several given X-shapes which represent the same X-shape to one X-shape.
		 * @param xShapes All X-shapes from which some will be merged together, must be valid
		 * @param indices The indices of all X-shapes which will be used for merging, at least one
		 * @return The resulting new X-shape, with position and direction best fitting with all X-shapes used for merging
		 */
		static XShape mergeShapes(const XShape* xShapes, const Indices32& indices);

		/**
		 * Merges several given T-shapes and L-shapes which represent the same T-shape to one T-shape.
		 * @param tShapes All T-shapes from which some will be merged together, must be valid
		 * @param tIndices The indices of all T-shapes which will be used for merging, at least one
		 * @param lShapes All X-shapes from which some will be merged together, must be valid
		 * @param lIndices The indices of all L-shapes which will be used for merging, at least one
		 * @return The resulting new T-shape, with position and direction best fitting with all T-shapes and L-shapes used for merging
		 */
		static TShape mergeTAndLShapes(const TShape* tShapes, const Indices32& tIndices, const LShape* lShapes, const Indices32& lIndices);

		/**
		 * Merges several given X-shapes and T-shapes which represent the same X-shape to one X-shape.
		 * @param xShapes All X-shapes from which some will be merged together, must be valid
		 * @param xIndices The indices of all X-shapes which will be used for merging, at least one
		 * @param tShapes All T-shapes from which some will be merged together, must be valid
		 * @param tIndices The indices of all T-shapes which will be used for merging, at least one
		 * @return The resulting new X-shape, with position and direction best fitting with all X-shapes and T-shapes used for merging
		 */
		static XShape mergeXAndTShapes(const XShape* xShapes, const Indices32& xIndices, const TShape* tShapes, const Indices32& tIndices);

		/**
		 * Determines the 2D/2D matching between shapes defined in two almost aligned coordinate system.
		 * Two aligned 2D coordinate systems can be based on e.g., two rectified images showing the same planar object.<br>
		 * The positions of candidates shapes can be transformed before matching via:
		 * <pre>
		 * shapePosition = shapes_H_shiftedCandidates * (shiftedCandidates_O_candidates + candidatePosition)
		 * </pre>
		 * @param shapes The (database) shapes for which matches from the set of candidate shapes will be determined, at least one
		 * @param shapeDistributionArrayWithCopiedNeighborhood8 The distribution array which has been created (and filled) for the given shapes, the distribution array must contain copies of the 8-neighborhood, must be valid
		 * @param shapeCandidates The candidate shapes for which matches from the (database) shapes will be determined, at least one
		 * @param matches The resulting pairs of shape indices of matching shapes
		 * @param maximalDistance The maximal distance between two matching shapes, defined in the domain of the coordinate system, e.g., pixels if the coordinate system is based on rectified images, with range [0, infinity)
		 * @param angleThreshold The maximal angle between the direction of two shapes so that they count as match, with range [0, PI]
		 * @param shiftedCandidates_O_candidates Optional explicit offset between candidate shapes and shifted candidate shapes
		 * @param shapes_H_shiftedCandidates Optional explicit homography transformation between shifted candidates and (database) shapes, must be valid
		 * @param unmatchedReferenceShapeIndices Optional resulting indices of all shapes which have not been matched
		 * @param unmatchedCandidateShapeIndices Optional resulting indices of all shapes which have not been matched
		 * @param averageSqrDistance Optional resulting average square distance of all matching shapes, in pixel, with range [0, infinity)
		 */
		static void determineAlignedMatching(const std::vector<const TwoLineShape*>& shapes, const Geometry::SpatialDistribution::DistributionArray& shapeDistributionArrayWithCopiedNeighborhood8, const std::vector<const TwoLineShape*>& shapeCandidates, IndexPairs32& matches, const Scalar maximalDistance, const Scalar angleThreshold = Numeric::deg2rad(20), const Vector2& shiftedCandidates_O_candidates = Vector2(0, 0), const SquareMatrix3& shapes_H_shiftedCandidates = SquareMatrix3(true), Indices32* unmatchedReferenceShapeIndices = nullptr, Indices32* unmatchedCandidateShapeIndices = nullptr, Scalar* averageSqrDistance = nullptr);

		/**
		 * Determines shapes which are located along a line and for which the shape's directions are perpendicular to this line.
		 * @param width The width of the image in which the shapes were detected, in pixel, with range [1, infinity)
		 * @param height The height of the image in which the shapes were detected, in pixel, with range [1, infinity)
		 * @param shapes The detect shapes which will be used to determine lines for
		 * @param lines Optional resulting lines which match with the determined shapes, one line for each group of resulting shape indices
		 * @param minimalShapes Minimal number of shapes which need to contribute to a line, with range [2, infinity)
		 * @return Resulting groups of indices of shapes along the individual lines, one group for each line, sorted so that the first group contains the most shapes, and so on
		 */
		static IndexGroups32 determineShapesAlongLines(const unsigned int width, const unsigned int height, const std::vector<const TwoLineShape*>& shapes, Lines2* lines = nullptr, const unsigned int minimalShapes = 4u);

		/**
		 * Groups lines into sets with almost parallel lines.
		 * This function has a complexity of O(n^2) and thus should not be used to group too many lines.
		 * @param lines The lines to be grouped into subsets, sorted with most dominant/important lines coming first, can be empty
		 * @param maximalAngle The maximal angle between two parallel lines, in radian, with range [0, PI_2)
		 * @param minimalParallelLines The minimal number of lines a group of parallel lines must contain, with range [1, infinity)
		 * @return The groups of indices with parallel lines
		 */
		static IndexGroups32 determineParallelLineGroups(const Lines2& lines, const Scalar maximalAngle = Numeric::deg2rad(5), const unsigned int minimalParallelLines = 2u);

		/**
		 * Returns a vector containing pointers to the given x-shapes, t-shapes, and l-shapes.
		 * Beware: The pointers are valid as long as the given shapes are untouched, once the given shapes are modified, the pointers must not be used anymore.
		 * @param xShapes The X-shapes to be serialized
		 * @param tShapes The T-shapes to be serialized
		 * @param lShapes The L-shapes to be serialized
		 * @param firstXShapeIndex Optional resulting index of the first X-shape, -1 if now X-shape exists
		 * @param firstTShapeIndex Optional resulting index of the first T-shape, -1 if now T-shape exists
		 * @param firstLShapeIndex Optional resulting index of the first :-shape, -1 if now L-shape exists
		 * @return The vector containing pointers to all given shape object, with order x-shapes, t-shapes, l-shapes
		 */
		static std::vector<const TwoLineShape*> serializeShapes(const XShapes& xShapes, const TShapes& tShapes, const LShapes& lShapes, size_t* firstXShapeIndex = nullptr, size_t* firstTShapeIndex = nullptr, size_t* firstLShapeIndex = nullptr);

		/**
		 * Determines whether the position of two shapes are close to each other.
		 * Optional, a homography can be used to transform the position of the candidate shape.
		 * @param referencePosition The position of the reference shape
		 * @param candidatePosition The position of the candidate shape
		 * @param reference_H_shiftedCandidate Optional homography transformation to transform the (shifted) candidate position, must be valid if `tUseTransformation == true`, otherwise nullptr
		 * @return True, if so
		 * @tparam tUseTransformation True, if the parameter reference_H_shiftedCandidate is defined; False, otherwise
		 */
		template <bool tUseTransformation>
		inline bool arePositionsClose(const Vector2& referencePosition, const Vector2& candidatePosition, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

		/**
		 * Determines whether the direction of a shape is similar with a given direction.
		 * @param direction The direction to which the shape's direction must be similar, must be a unit vector
		 * @param twoLineShape The shape to be checked
		 * @param minimalAngleParallelCos The cos value of the angle between two directions to be considered parallel, the dot product of parallel lines will fall into [minimalAngleParallelCos_, 1].
		 * @return True, if so
		 */
		static bool areDirectionsSimilar(const Vector2& direction, const TwoLineShape& twoLineShape, const Scalar minimalAngleParallelCos);

		/**
		 * Determines whether the direction of a shape is parallel with a given direction.
		 * @param direction The direction to which the shape's direction must be parallel, must be a unit vector
		 * @param twoLineShape The shape to be checked
		 * @param minimalAngleParallelCos The cos value of the angle between two directions to be considered parallel, the dot absolute product of parallel lines will fall into [minimalAngleParallelCos_, 1].
		 * @return True, if so
		 */
		static bool areDirectionsParallel(const Vector2& direction, const TwoLineShape& twoLineShape, const Scalar minimalAngleParallelCos);

		/**
		 * Determines the cosine value (the dot product) of two shape directions.
		 * Optional, a homography can be used to transform the direction of the candidate shape.
		 * @param referenceDirection The direction of the reference shape, must be a unit vector
		 * @param candidateDirection The direction of the candidate shape, must be a unit vector
		 * @param candidatePosition The position of the candidate shape, must be valid if `tUseTransformation == true`, otherwise nullptr
		 * @param reference_H_shiftedCandidate Optional homography transformation to transform the (shifted) candidate position, must be valid if `tUseTransformation == true`, otherwise nullptr
		 * @return The resulting cosine value (dot product)
		 * @tparam tUseTransformation True, if the parameter reference_H_shiftedCandidate is defined; False, otherwise
		 */
		template <bool tUseTransformation>
		inline Scalar cosValueDirections(const Vector2& referenceDirection, const Vector2& candidateDirection, const Vector2& candidatePosition, const SquareMatrix3* reference_H_shiftedCandidate = nullptr) const;

	protected:

		/**
		 * Compares the size of two vectors and returns whether the left vector is smaller than the right vector.
		 * @param left The left vector to compare
		 * @param right The right vector to compare
		 * @return True, if so
		 */
		static inline bool compareVectorSize(const Indices32& left, const Indices32& right);

	protected:

		/// The maximal distance between two shapes to be considered as match, with range [0, infinity).
		Scalar maximalDistance_;

		/// The maximal (squared) distance between two shapes to be considered as match, with range [0, infinity).
		Scalar maximalSqrDistance_;

		// The cos value of the angle between two directions to be considered parallel, the absolute dot product of parallel lines will fall into [minimalAngleParallelCos_, 1].
		Scalar minimalAngleParallelCos_;

		// The minimal cos value of the angle between two directions to be considered diagonal, the dot product of diagonal lines will fall into [minimalAngleDiagonalCos_, maximalAngleDiagonalCos_].
		Scalar minimalAngleDiagonalCos_;

		// The maximal cos value of the angle between two directions to be considered diagonal, the dot product of diagonal lines will fall into [minimalAngleDiagonalCos_, maximalAngleDiagonalCos_].
		Scalar maximalAngleDiagonalCos_;

		// The cos value of the angle between two directions to be considered perpendicular, the dot product of perpendicular lines will fall into [0, maximalAnglePerpendicularCos_]
		Scalar maximalAnglePerpendicularCos_;

		/// An optional explicit position offset between candidate shapes and reference shapes e.g., to determine matches with constant translational offset.
		Vector2 reference_O_candidate_;
};

inline ShapeMatcher::ShapeMatcher(const Scalar maximalDistance, const Scalar angleThreshold, const Vector2& reference_O_candidate) :
	maximalDistance_(maximalDistance),
	maximalSqrDistance_(maximalDistance * maximalDistance),
	minimalAngleParallelCos_(Numeric::cos(angleThreshold)),
	minimalAngleDiagonalCos_(Numeric::cos(Numeric::pi_4() + angleThreshold)),
	maximalAngleDiagonalCos_(Numeric::cos(Numeric::pi_4() - angleThreshold)),
	maximalAnglePerpendicularCos_(Numeric::cos(Numeric::pi_2() - angleThreshold)),
	reference_O_candidate_(reference_O_candidate)
{
	ocean_assert(maximalDistance_ >= 0);
	ocean_assert(angleThreshold >= 0 && angleThreshold < Numeric::pi_2());
}

inline Scalar ShapeMatcher::maximalDistance() const
{
	return maximalDistance_;
}

inline const Vector2& ShapeMatcher::reference_O_candidate() const
{
	return reference_O_candidate_;
}

inline void ShapeMatcher::setReference_O_candidate(const Vector2& reference_O_candidate)
{
	reference_O_candidate_ = reference_O_candidate;
}

template <bool tCheckDistance, bool tUseTransformation>
ShapeMatcher::MatchType ShapeMatcher::matchShapes(const TwoLineShape& shapeReference, const TwoLineShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	ocean_assert(shapeReference.type() != TwoLineShape::ST_INVALID && shapeCandidate.type() != TwoLineShape::ST_INVALID);

	// let's first check the distance between both shapes

	if (tCheckDistance && !arePositionsClose<tUseTransformation>(shapeReference.position(), shapeCandidate.position(), reference_H_shiftedCandidate))
	{
		return MT_WRONG_POSTION;
	}

	switch (shapeReference.type() | (shapeCandidate.type() << 16u))
	{
		// an L-shape can not partially match with some other shape

		// L-shape -> L-shape
		case TwoLineShape::ST_SHAPE_L | (TwoLineShape::ST_SHAPE_L << 16u):
			return matchShapes<false, tUseTransformation>(static_cast<const LShape&>(shapeReference), static_cast<const LShape&>(shapeCandidate), reference_H_shiftedCandidate);


		// a T-shape can partially match with an L-shape, and with an X-shape

		// T-shape -> T-shape
		case TwoLineShape::ST_SHAPE_T | (TwoLineShape::ST_SHAPE_T << 16u):
			return matchShapes<false, tUseTransformation>(static_cast<const TShape&>(shapeReference), static_cast<const TShape&>(shapeCandidate), reference_H_shiftedCandidate);

		// T-shape -> L-shape
		case TwoLineShape::ST_SHAPE_T | (TwoLineShape::ST_SHAPE_L << 16u):
			return matchShapes<false, tUseTransformation>(static_cast<const TShape&>(shapeReference), static_cast<const LShape&>(shapeCandidate), reference_H_shiftedCandidate);

		// T-shape -> X-shape
		case TwoLineShape::ST_SHAPE_T | (TwoLineShape::ST_SHAPE_X << 16u):
			return matchShapes<false, tUseTransformation>(static_cast<const TShape&>(shapeReference), static_cast<const XShape&>(shapeCandidate), reference_H_shiftedCandidate);


		// a X-shape can partially match with an L-shape and a T-shape

		// X-shape -> X-shape
		case TwoLineShape::ST_SHAPE_X | (TwoLineShape::ST_SHAPE_X << 16u):
			return matchShapes<false, tUseTransformation>(static_cast<const XShape&>(shapeReference), static_cast<const XShape&>(shapeCandidate), reference_H_shiftedCandidate);

		// X-shape -> L-shape
		case TwoLineShape::ST_SHAPE_X | (TwoLineShape::ST_SHAPE_L << 16u):
			return matchShapes<false, tUseTransformation>(static_cast<const XShape&>(shapeReference), static_cast<const LShape&>(shapeCandidate), reference_H_shiftedCandidate);

		// X-shape -> T-shape
		case TwoLineShape::ST_SHAPE_X | (TwoLineShape::ST_SHAPE_T << 16u):
			return matchShapes<false, tUseTransformation>(static_cast<const XShape&>(shapeReference), static_cast<const TShape&>(shapeCandidate), reference_H_shiftedCandidate);
	}

	return MT_NO_MATCH;
}

template <bool tUseTransformation>
ShapeMatcher::MatchType ShapeMatcher::matchShapes(const TwoLineShape& shapeReference, const TwoLineShape& shapeCandidate, const bool checkDistance, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	ocean_assert(shapeReference.type() != TwoLineShape::ST_INVALID && shapeCandidate.type() != TwoLineShape::ST_INVALID);

	if (checkDistance)
	{
		return matchShapes<true, tUseTransformation>(shapeReference, shapeCandidate, reference_H_shiftedCandidate);
	}
	else
	{
		return matchShapes<false, tUseTransformation>(shapeReference, shapeCandidate, reference_H_shiftedCandidate);
	}
}

template <bool tCheckDistance, bool tUseTransformation>
inline ShapeMatcher::MatchType ShapeMatcher::matchShapes(const LShape& shapeReference, const LShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	// two L-shapes match if both shapes have the same direction (and same location)

	if (tCheckDistance && !arePositionsClose<tUseTransformation>(shapeReference.position(), shapeCandidate.position(), reference_H_shiftedCandidate))
	{
		return MT_WRONG_POSTION;
	}

	ocean_assert(shapeReference.direction().isUnit() && shapeCandidate.direction().isUnit());

	const Scalar cosValue = cosValueDirections<tUseTransformation>(shapeReference.direction(), shapeCandidate.direction(), shapeCandidate.position(), reference_H_shiftedCandidate);

	if (cosValue < minimalAngleParallelCos_)
	{
		return MT_NO_MATCH;
	}

	return MT_PERFECT_MATCH;
}

template <bool tCheckDistance, bool tUseTransformation>
inline ShapeMatcher::MatchType ShapeMatcher::matchShapes(const TShape& shapeReference, const TShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	// two T-shapes match if both shapes have the same direction (and same location)

	if (tCheckDistance && !arePositionsClose<tUseTransformation>(shapeReference.position(), shapeCandidate.position(), reference_H_shiftedCandidate))
	{
		return MT_WRONG_POSTION;
	}

	ocean_assert(shapeReference.direction().isUnit() && shapeCandidate.direction().isUnit());

	const Scalar cosValue = cosValueDirections<tUseTransformation>(shapeReference.direction(), shapeCandidate.direction(), shapeCandidate.position(), reference_H_shiftedCandidate);

	if (cosValue < minimalAngleParallelCos_)
	{
		return MT_NO_MATCH;
	}

	return MT_PERFECT_MATCH;
}

template <bool tCheckDistance, bool tUseTransformation>
inline ShapeMatcher::MatchType ShapeMatcher::matchShapes(const TShape& shapeReference, const LShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	// we have a T-shape as reference and an L-shape as candidate

	if (tCheckDistance && !arePositionsClose<tUseTransformation>(shapeReference.position(), shapeCandidate.position(), reference_H_shiftedCandidate))
	{
		return MT_WRONG_POSTION;
	}

	// we have a partial match if the direction of the L-shape is 45 degree to the direction of the T-shape
	//
	// T-shape with direction downwards,         two possible L-shapes
	//  -----------                              ------           ------
	//       |                                       / |         | `
	//       |                                      /  |         |  `
	//       v                                     /   |         |   `

	ocean_assert(shapeReference.direction().isUnit() && shapeCandidate.direction().isUnit());

	const Scalar cosValue = cosValueDirections<tUseTransformation>(shapeReference.direction(), shapeCandidate.direction(), shapeCandidate.position(), reference_H_shiftedCandidate);

	if (cosValue < minimalAngleDiagonalCos_ || cosValue > maximalAngleDiagonalCos_)
	{
		return MT_NO_MATCH;
	}

	return MT_PARTIAL_MATCH;
}

template <bool tCheckDistance, bool tUseTransformation>
inline ShapeMatcher::MatchType ShapeMatcher::matchShapes(const TShape& shapeReference, const XShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	// we have a T-shape as reference and a X-shape as candidate

	if (tCheckDistance && !arePositionsClose<tUseTransformation>(shapeReference.position(), shapeCandidate.position(), reference_H_shiftedCandidate))
	{
		return MT_WRONG_POSTION;
	}

	// we have a partial match if one direction of the X-shape parallel with the direction of the T-shape
	//
	// X-shape with two directions,         two of four possible T-shapes
	//       |                                                   |
	//       |                                                   |
	//  ----------->                             -------------   |------>
	//       |                                         |         |
	//       |                                         |         |
	//       v                                         v         |

	ocean_assert(shapeReference.direction().isUnit());
	ocean_assert(shapeCandidate.direction0().isUnit() && shapeCandidate.direction1().isUnit());

	const Scalar absCosValue0 = Numeric::abs(cosValueDirections<tUseTransformation>(shapeReference.direction(), shapeCandidate.direction0(), shapeCandidate.position(), reference_H_shiftedCandidate));
	const Scalar absCosValue1 = Numeric::abs(cosValueDirections<tUseTransformation>(shapeReference.direction(), shapeCandidate.direction1(), shapeCandidate.position(), reference_H_shiftedCandidate));

	if (absCosValue0 >= minimalAngleParallelCos_ || absCosValue1 >= minimalAngleParallelCos_)
	{
		return MT_PARTIAL_MATCH;
	}

	return MT_NO_MATCH;
}

template <bool tCheckDistance, bool tUseTransformation>
inline ShapeMatcher::MatchType ShapeMatcher::matchShapes(const XShape& shapeReference, const XShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	// two X-shapes match if both shapes have the same direction (and same location)

	if (tCheckDistance && !arePositionsClose<tUseTransformation>(shapeReference.position(), shapeCandidate.position(), reference_H_shiftedCandidate))
	{
		return MT_WRONG_POSTION;
	}

	ocean_assert(shapeReference.direction0().isUnit() && shapeReference.direction1().isUnit());
	ocean_assert(shapeCandidate.direction0().isUnit() && shapeCandidate.direction1().isUnit());

	const Scalar absDirectionCos0 = Numeric::abs(cosValueDirections<tUseTransformation>(shapeReference.direction0(), shapeCandidate.direction0(), shapeCandidate.position(), reference_H_shiftedCandidate));
	const Scalar absDirectionCos1 = Numeric::abs(cosValueDirections<tUseTransformation>(shapeReference.direction1(), shapeCandidate.direction1(), shapeCandidate.position(), reference_H_shiftedCandidate));

	if ((absDirectionCos0 >= minimalAngleParallelCos_ || absDirectionCos0 <= maximalAnglePerpendicularCos_) && (absDirectionCos1 >= minimalAngleParallelCos_ || absDirectionCos1 <= maximalAnglePerpendicularCos_))
	{
		return MT_PERFECT_MATCH;
	}

	return MT_NO_MATCH;
}

template <bool tCheckDistance, bool tUseTransformation>
inline ShapeMatcher::MatchType ShapeMatcher::matchShapes(const XShape& shapeReference, const LShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	// we have a X-shape as reference and an L-shape as candidate

	if (tCheckDistance && !arePositionsClose<tUseTransformation>(shapeReference.position(), shapeCandidate.position(), reference_H_shiftedCandidate))
	{
		return MT_WRONG_POSTION;
	}

	// we have a partial match if the direction of the L-shape is 45 degree to all four directions of the X-shape
	//
	// X-shape with two directions,         two of four possible L-shapes
	//       |
	//       |
	//  ----------->                             ------           ------
	//       |                                       / |         | `
	//       |                                      /  |         |  `
	//       v                                     /   |         |   `

	ocean_assert(shapeReference.direction0().isUnit() && shapeReference.direction1().isUnit());
	ocean_assert(shapeCandidate.direction().isUnit());

	const Scalar absCosValue0 = Numeric::abs(cosValueDirections<tUseTransformation>(shapeReference.direction0(), shapeCandidate.direction(), shapeCandidate.position(), reference_H_shiftedCandidate));
	const Scalar absCosValue1 = Numeric::abs(cosValueDirections<tUseTransformation>(shapeReference.direction1(), shapeCandidate.direction(), shapeCandidate.position(), reference_H_shiftedCandidate));

	if (minimalAngleDiagonalCos_ <= absCosValue0 && absCosValue0 <= maximalAngleDiagonalCos_ && minimalAngleDiagonalCos_ <= absCosValue1 && absCosValue1 <= maximalAngleDiagonalCos_)
	{
		return MT_PARTIAL_MATCH;
	}

	return MT_NO_MATCH;
}

template <bool tCheckDistance, bool tUseTransformation>
inline ShapeMatcher::MatchType ShapeMatcher::matchShapes(const XShape& shapeReference, const TShape& shapeCandidate, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	// we have a X-shape as reference and a T-shape as candidate

	if (tCheckDistance && !arePositionsClose<tUseTransformation>(shapeReference.position(), shapeCandidate.position(), reference_H_shiftedCandidate))
	{
		return MT_WRONG_POSTION;
	}

	// we have a partial match if the direction of the T-shape is parallel with one of the directions of the X-shape
	//
	// X-shape with two directions,         two of four possible T-shapes
	//       |                                                   |
	//       |                                                   |
	//  ----------->                             -------------   |------>
	//       |                                         |         |
	//       |                                         |         |
	//       v                                         v         |

	ocean_assert(shapeReference.direction0().isUnit() && shapeReference.direction1().isUnit());
	ocean_assert(shapeCandidate.direction().isUnit());

	const Scalar absCosValue0 = Numeric::abs(cosValueDirections<tUseTransformation>(shapeReference.direction0(), shapeCandidate.direction(), shapeCandidate.position(), reference_H_shiftedCandidate));
	const Scalar absCosValue1 = Numeric::abs(cosValueDirections<tUseTransformation>(shapeReference.direction1(), shapeCandidate.direction(), shapeCandidate.position(), reference_H_shiftedCandidate));

	if (absCosValue0 >= minimalAngleParallelCos_ || absCosValue1 >= minimalAngleParallelCos_)
	{
		return MT_PARTIAL_MATCH;
	}

	return MT_NO_MATCH;
}

template <bool tUseTransformation>
inline bool ShapeMatcher::arePositionsClose(const Vector2& referencePosition, const Vector2& candidatePosition, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	if constexpr (tUseTransformation)
	{
		ocean_assert(reference_H_shiftedCandidate != nullptr);

		return referencePosition.sqrDistance(*reference_H_shiftedCandidate * (reference_O_candidate_ + candidatePosition)) <= maximalSqrDistance_;
	}
	else
	{
		ocean_assert(reference_H_shiftedCandidate == nullptr);

		return referencePosition.sqrDistance(candidatePosition + reference_O_candidate_) <= maximalSqrDistance_;
	}
}

template <bool tUseTransformation>
inline Scalar ShapeMatcher::cosValueDirections(const Vector2& referenceDirection, const Vector2& candidateDirection, const Vector2& candidatePosition, const SquareMatrix3* reference_H_shiftedCandidate) const
{
	ocean_assert(referenceDirection.isUnit());
	ocean_assert(candidateDirection.isUnit());

	if constexpr (tUseTransformation)
	{
		ocean_assert(reference_H_shiftedCandidate != nullptr);

		const Vector2 transformedTail = *reference_H_shiftedCandidate * (reference_O_candidate_ + candidatePosition);
		const Vector2 transformedHead = *reference_H_shiftedCandidate * (reference_O_candidate_ + candidatePosition + candidateDirection);

		const Vector2 transformedDirection = (transformedHead - transformedTail).normalized();
		ocean_assert(transformedDirection.isUnit());

		return referenceDirection * transformedDirection;
	}
	else
	{
		ocean_assert(reference_H_shiftedCandidate == nullptr);

		return referenceDirection * candidateDirection;
	}
}

inline bool ShapeMatcher::compareVectorSize(const Indices32& left, const Indices32& right)
{
	return left.size() < right.size();
}

}

}

}

#endif // META_OCEAN_TRACKING_KEYBOARD_SHAPE_MATCHER_H
