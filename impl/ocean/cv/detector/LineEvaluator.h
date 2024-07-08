/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_LINE_EVALUATOR_H
#define META_OCEAN_CV_DETECTOR_LINE_EVALUATOR_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Median.h"
#include "ocean/base/SegmentUnion.h"

#include "ocean/math/FiniteLine2.h"

#include <unordered_map>
#include <unordered_set>

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements an evalutator for line segments.
 * @ingroup cvdetector
 */
class LineEvaluator
{
	public:

		/**
		 * Definition of an id identifying e.g., a specific line.
		 */
		typedef unsigned int Id;

		/**
		 * Definition of an unordered set of ids.
		 */
		typedef std::unordered_set<Id> IdSet;

		/**
		 * Definition of an unordered map mapping ids to sets of ids.
		 */
		typedef std::unordered_map<Id, IdSet> IdToIdSetMap;

		/**
		 * This class is the base class for all line matches.
		 * Each match is composed of at least a match type and an id of the source line.
		 */
		class LineMatch
		{
			public:

				/**
				 * Definition of individual match types.
				 */
				enum MatchType
				{
					/// An invalid type.
					MT_INVALID,
					/// A perfect match, @see PerfectLineMath.
					MT_PERFECT,
					/// A partial match, @see PartialLineMatch.
					MT_PARTIAL,
					/// A complex match, @see ComplexLineMatch.
					MT_COMPLEX
				};

			public:

				/**
				 * Destructs a match object.
				 */
				virtual ~LineMatch();

				/**
				 * Returns the type of the match.
				 * @return The match's type
				 */
				virtual MatchType matchType() const = 0;

				/**
				 * Returns the id of the source line.
				 * @return The match's source line id
				 */
				inline Id sourceId() const;

			protected:

				/**
				 * Creates a new match object.
				 * @param sourceId The id of the source line, must be valid
				 */
				explicit inline LineMatch(const Id sourceId);

			protected:

				/// The id of the source line.
				Id sourceId_;
		};

		/**
		 * Definition of a shared pointer for a LineMatch object.
		 * @see LineMatch.
		 */
		typedef std::shared_ptr<LineMatch> LineMatchRef;

		/**
		 * Definition of an unordered multi map mapping ids to match objects.
		 */
		typedef std::unordered_multimap<Id, LineMatchRef> LineMatchMap;

		/**
		 * This class implements a perfect match between a source line and a target line.
		 * A perfect match is given whenever two lines are almost equal (end points and direction).<br>
		 * A source line may have several perfect matches to several individual target lines.<br>
		 * Below, the scheme of a perfect match is depicted:
		 * <pre>
		 * ground truth lines: ++++++++++++++
		 *   evaluation lines: --------------
		 * </pre>
		 * @see PartialLineMatch, ComplexLineMatch.
		 */
		class PerfectLineMatch : public LineMatch
		{
			public:

				/**
				 * Creates a new match object.
				 * @param sourceId The id of the source line, must be valid
				 * @param targetId The id of the target line, must be valid
				 * @param angle The (absolute) angle between source and target lines in radian, with range [0, PI/2]
				 * @param maximalDistance The maximal distance between infinite source line and target line, with range [0, infinity)
				 */
				inline PerfectLineMatch(const Id sourceId, const Id targetId, const double angle, const double maximalDistance);

				/**
				 * Returns the type of the match.
				 * @return The match's type
				 */
				MatchType matchType() const override;

				/**
				 * Returns the id of the target line.
				 * @return The target line's id
				 */
				inline Id targetId() const;

				/**
				 * Returns the angle between the source and the target line.
				 * @return The (absolute) angle between both lines in radian, with range [0, PI/2]
				 */
				inline double angle() const;

				/**
				 * Returns maximal distance between infinite source line and target line.
				 * @return The maximal distance, with range [0, infinity)
				 */
				inline double maximalDistance() const;

			protected:

				/// The id of the target line.
				unsigned int targetId_;

				/// The absolute angle between both lines, with range [0, PI/2]
				double angle_;

				/// The  maximal distance between infinite source line and target line, with range [0, infinity)
				double maximalDistance_;
		};

		/**
		 * This class implements a partial match between one source line and several target lines.
		 * A partial match is given whenever several target lines can be combined to cover a source line.<br>
		 * Below, the scheme of a partial match is depicted:
		 * <pre>
		 * ground truth lines:  ++++++++++++++++++++++
		 *   evaluation lines: -------- -------- ----
		 * </pre>
		 * @see PerfectLineMatch, ComplexLineMatch.
		 */
		class PartialLineMatch : public LineMatch
		{
			public:

				/**
				 * Creates a new partial match object.
				 * @param sourceId The id of the soruce line, must be valid
				 * @param targetIds The ids of the target line matching with the source line, at least one valid id
				 * @param coverage The coverage of the source line (the amout the target lines cover the source line), with range (0, 1 + borderEps]
				 * @param medianAngle The median angle of between the source line and all target lines in radian, with range [0, PI/2]
				 * @param medianDistance The median distance from all target lines to the source lines, with range [0, infinity)
				 */
				inline PartialLineMatch(const Id sourceId, const IdSet& targetIds, const double coverage, const double medianAngle, const double medianDistance);

				/**
				 * Creates a new partial match object.
				 * @param sourceId The id of the soruce line, must be valid
				 * @param targetIds The ids of the target line matching with the source line, at least one valid id, will be moved
				 * @param coverage The coverage of the source line (the amout the target lines cover the source line), with range (0, 1 + borderEps]
				 * @param medianAngle The median angle of between the source line and all target lines in radian, with range [0, PI/2]
				 * @param medianDistance The median distance from all target lines to the source lines, with range [0, infinity)
				 */
				inline PartialLineMatch(const Id sourceId, IdSet&& targetIds, const double coverage, const double medianAngle, const double medianDistance);

				/**
				 * Returns the type of the match.
				 * @return The match's type
				 */
				MatchType matchType() const override;

				/**
				 * Returns the ids of all target lines belonging to the partial match.
				 * @return The target ids, at least one
				 */
				inline const IdSet& targetIds() const;

				/**
				 * The amout the target lines cover the source line.
				 * @return The coverage amout, with range (0, 1 + borderEps]
				 */
				inline double coverage() const;

				/**
				 * Returns the median angle between the source line and all target lines.
				 * @return The median angle in radian, with range [0, PI/2]
				 */
				inline double medianAngle() const;

				/**
				 * Returns the median distance between the source line and all target lines.
				 * @return The median distance, with range [0, infinity)
				 */
				inline double medianDistance() const;

			protected:

				/// The ids of all target lines.
				IdSet targetIds_;

				/// The coverage of the source line (the amout the target lines cover the source line), with range (0, 1 + borderEps]
				double coverage_;

				/// The median angle between the source line and all target lines, in radian, with range [0, PI/2]
				double medianAngle_;

				/// The median distance between the source line and all target lines, with range [0, infinity)
				double medianDistance_;
		};

		/**
		 * This class implements a complex match between one source line and several target lines.
		 * A complex match is given whenever several source lines match to portions of several target lines.<br>
		 * The complex match is still defined for one source line, in combination with all portions of target lines.<br>
		 * Below, the scheme of a complex match is depicted:
		 * <pre>
		 * ground truth lines:  ++++++++++++++++++++++ +++++++++++++ ++++++++++
		 *   evaluation lines: -------- ----------------------- -------- -----
		 * </pre>
		 * @see PerfectLineMatch.
		 */
		class ComplexLineMatch : public PartialLineMatch
		{
			public:

				/**
				 * Creates a new match object.
				 * @param sourceId The id of the soruce line, must be valid
				 * @param targetIds The ids of the target line matching with the source line, at least one valid id
				 * @param coverage The coverage of the source line (the amout the target lines cover the source line), with range (0, 1]
				 * @param medianAngle The median angle of between the source line and all target lines in radian, with range [0, PI/2]
				 * @param medianDistance The median distance from all target lines to the source lines, with range [0, infinity)
				 * @param connectedSourceIds The ids of all sibling/connected source lines which have been investigated during the match creation
				 * @param connectedTargetIds The ids of all sibling/connected target lines which have been investigated during the match creation, at least one
				 */
				inline ComplexLineMatch(const Id sourceId, const IdSet& targetIds, const double coverage, const double medianAngle, const double medianDistance, const IdSet& connectedSourceIds, const IdSet& connectedTargetIds);

				/**
				 * Returns the type of the match.
				 * @return The match's type
				 */
				MatchType matchType() const override;

				/**
				 * Returns the ids of all sibling/connected source lines which have been investigated during the match creation.
				 * @return The source lines' ids
				 */
				inline const IdSet& connectedSourceIds() const;

				/**
				 * Returns the ids of all sibling/connected target lines which have been investigated during the match creation.
				 * @return The target lines' ids
				 */
				inline const IdSet& connectedTargetIds() const;

			protected:

				/// The ids of all sibling/connected source lines which have been investigated during the match creation.
				IdSet connectedSourceIds_;

				/// The ids of all sibling/connected target lines which have been investigated during the match creation.
				IdSet connectedTargetIds_;
		};

		/**
		 * Definition of individual strategies to determine the distance between two line segments.
		 */
		enum DistanceMeasure
		{
			/**
			 * The end points of the evaluation line are projected onto the infinite ground truth line,
			 * and the maximal distance between end points and projected points is determined.<br>
			 * This measure has the property that very long evaluation lines and very short ground truth lines
			 * may result in a large distance although the overlapping area seems to be quite close.
			 */
			DM_PROJECTED_ONTO_GROUND_TRUTH,

			/**
			 * The end points of the ground truth lines are projected onto the infinite evaluation line,
			 * and the maximal distance between end points and projected points is determined.<br>
			 * This measure has the property that very long ground truth lines and very short evaluation lines
			 * may result in a large distance although the overlapping area seems to be quite close.
			 */
			DM_PROJECTED_ONTO_EVALUATION_LINE,

			/**
			 * This measure combines 'DM_PROJECTED_ONTO_GROUND_TRUTH' and 'DM_PROJECTED_ONTO_EVALUATION_LINE'.
			 * The distance is the minimal distance of both measures.<br>
			 * Thus, this measure has the property that combinations of long and small lines end up with smaller distances.
			 */
			DM_PROJECTED_ONTO_EACH_OTHER
		};

	protected:

		/**
		 * Definition of a set holding a pair of ids.
		 */
		typedef std::unordered_set<unsigned long long> Id64Set;

	public:

		/**
		 * Checks whether two given lines are overlapping up to some extend and determines some overlapping metrics.
		 * This function can determine out-of-border distances for the projected end points of the evaluation line, see FiniteLineT2::nearestPointOnInfiniteLine() for more details.
		 * The paris of resulting distance values are sorted so that the following holds: outOfBorderDistance0 <= outOfBorderDistance1, locationOnLine0 <= locationOnLine1.
		 * @param lineGroundTruth The ground truth line (actually one of both lines) while all optional resulting location values are determined in relation to this line, must be valid
		 * @param lineEvaluation The evaluation line (actually the second line), must be valid
		 * @param angleThresholdCos The angle between two lines so that both lines count as similar, given in the cosine value of the angle: angleThresholdCos = cos(angleThreshold), with range [0, 1]
		 * @param distanceThresholdPixels The maixmal distance between two lines so that both lines count as similar, depending on the defined distance measure 'distanceMeasure', with range [0, infinity)
		 * @param distanceMeasure The distance measure to be applied when comparing with the provided distance theshold 'distanceThresholdPixels'
		 * @param projectedLength Optional resulting length of the projected evaluation line on the ground truth line, with range [0, infinity)
		 * @param outOfBorderDistance0 Optional resulting distance between the projected point (of the first end point of the evaluation line) on the infinite ground truth line and the closest end point of the (finite) ground truth line, with range (-infinity, 0]
		 * @param outOfBorderDistance1 Optional resulting distance between the projected point (of the second end point of the evaluation line) on the infinite ground truth line and the closest end point of the (finite) ground truth line, with range [0, infinity)
		 * @param locationOnLine0 Optional resulting location of the projected point (of the first end point of the evaluation line) on the infinite ground truth line in relation to the finite ground truth line so that the following holds lineGroundTruth.point0() + lineGroundTruth.direction() * locationOnLine0, with range (-infinity, infinity)
		  * @param locationOnLine1 Optional resulting location of the projected point (of the second end point of the evaluation line) on the infinite ground truth line in relation to the finite ground truth line so that the following holds lineGroundTruth.point1() + lineGroundTruth.direction() * locationOnLine1, with range (-infinity, infinity)
		 * @return True, if so
		 * @tparam T The data type of a scalar, either 'float' or 'double'
		 * @see determineOverlappingAmount(), FiniteLineT2::nearestPointOnInfiniteLine().
		 */
		template <typename T>
		static bool areLinesOverlapping(const FiniteLineT2<T>& lineGroundTruth, const FiniteLineT2<T>& lineEvaluation, const T angleThresholdCos, const T distanceThresholdPixels, const DistanceMeasure distanceMeasure, T* projectedLength = nullptr, T* outOfBorderDistance0 = nullptr, T* outOfBorderDistance1 = nullptr, T* locationOnLine0 = nullptr, T* locationOnLine1 = nullptr);

		/**
		 * Determines the overlapping metrics between two lines that are known to overlapping (and to be similar) already.
		 * This function can determine out-of-border distances for the projected end points of the evaluation line, see FiniteLineT2::nearestPointOnInfiniteLine() for more details.
		 * The paris of resulting distance values are sorted so that the following holds: outOfBorderDistance0 <= outOfBorderDistance1, locationOnLine0 <= locationOnLine1.
		 * @param lineGroundTruth The ground truth line (actually one of both lines) while all optional resulting location values are determined in relation to this line, must be valid
		 * @param lineEvaluation The evaluation line (actually the second line), must be valid
		 * @param projectedLength Optional resulting length of the projected evaluation line on the ground truth line, with range [0, infinity)
		 * @param outOfBorderDistance0 Optional resulting distance between the projected point (of the first end point of the evaluation line) on the infinite ground truth line and the closest end point of the (finite) ground truth line, with range (-infinity, 0]
		 * @param outOfBorderDistance1 Optional resulting distance between the projected point (of the second end point of the evaluation line) on the infinite ground truth line and the closest end point of the (finite) ground truth line, with range [0, infinity)
		 * @param locationOnLine0 Optional resulting location of the projected point (of the first end point of the evaluation line) on the infinite ground truth line in relation to the finite ground truth line so that the following holds lineGroundTruth.point0() + lineGroundTruth.direction() * locationOnLine0, with range (-infinity, infinity)
		  * @param locationOnLine1 Optional resulting location of the projected point (of the second end point of the evaluation line) on the infinite ground truth line in relation to the finite ground truth line so that the following holds lineGroundTruth.point1() + lineGroundTruth.direction() * locationOnLine1, with range (-infinity, infinity)
		 * @tparam T The data type of a scalar, either 'float' or 'double'
		 * @see areLinesOverlapping(), FiniteLineT2::nearestPointOnInfiniteLine().
		 */
		template <typename T>
		static void determineOverlappingAmount(const FiniteLineT2<T>& lineGroundTruth, const FiniteLineT2<T>& lineEvaluation, T* projectedLength = nullptr, T* outOfBorderDistance0 = nullptr, T* outOfBorderDistance1 = nullptr, T* locationOnLine0 = nullptr, T* locationOnLine1 = nullptr);

		/**
		 * Determines the similarity between two lines known to be overlapping.
		 * @param lineGroundTruth The ground truth line (actually one of both lines) while all optional resulting location values are determined in relation to this line, must be valid
		 * @param lineEvaluation The evaluation line (actually the second line), must be valid
		 * @param distanceMeasure The distance measure to be used to determine the distance between both lines
		 * @param angle The resulting angle between both lines in radian, with range [0, infinity)
		 * @param distance The distance between both lines based on the specified distance measure
		 * @tparam T The data type of a scalar, either 'float' or 'double'
		 * @see areLinesOverlapping().
		 */
		template <typename T>
		static void determineSimilarity(const FiniteLineT2<T>& lineGroundTruth, const FiniteLineT2<T>& lineEvaluation, const DistanceMeasure distanceMeasure, T& angle, T& distance);

		/**
		 * Evaluates two sets of finite lines.
		 * The given ground truth lines should be accurate and should contain lines that e.g., can be detected by a line detector
		 * The lines to evaluate are match to the set of ground truth lines.
		 * @param linesGroundTruth The ground truth lines, at least one
		 * @param linesEvaluation The lines to be evaluated, at least one
		 * @param perfectMatchAngleThreshold The maximal angle between lines have a perfect match, in radian, with range [0, PI/2]
		 * @param perfectMatchPixelThreshold The maximal distance between corresponding end points of a perfect match, in pixels (defined in the domain of the line coordinates), with range [0, infinity)
		 * @param matchAngleThreshold The maximal angle between lines having a general match, in radian, with range [0, PI/2]
		 * @param matchCloseToLinePixelThreshold The maximal distance between two lines having a general match, in pixels (defined in the domain of the line coordinates), with range [0, infinity)
		 * @param partialMatchNonOverlappingPixelThreshold The maximal amount an evaluation line may exceed a ground truth line in a partial match, in pixels (defined in the domain of the line coordinates), with range [0, infinity)
		 * @param complexMatchMaximalGapPixelThreshold The maximal gap between valid evaluation lines for a complex match, in pixels (defined in the domain of the line coordinates), with range [0, infinity)
		 * @return The resulting set of matches
		 * @tparam T The data type of a scalar, either 'float' or 'double'
		 */
		template <typename T>
		static LineMatchMap evaluateLineSegments(const std::unordered_map<Id, FiniteLineT2<T>>& linesGroundTruth, const std::unordered_map<Id, FiniteLineT2<T>>& linesEvaluation, const T perfectMatchAngleThreshold = NumericT<T>::deg2rad(2), const T perfectMatchPixelThreshold = T(2), const T matchAngleThreshold = NumericT<T>::deg2rad(5), const T matchCloseToLinePixelThreshold = T(3), const T partialMatchNonOverlappingPixelThreshold = T(25), const T complexMatchMaximalGapPixelThreshold = T(15));

		/**
		 * Evaluates the overall quality of line matches.
		 * @param linesGroundTruth The ground truth lines, at least one
		 * @param linesEvaluation The lines to be evaluated, at least one
		 * @param lineMatches The line matches that are found between the ground truth lines and evaluation lines
		 * @param coverage The resulting overall match coverage for all ground truth lines, with range [0, 1]
		 * @param medianAngle The resulting median angle of all matches, in radian, with range [0, PI/2]
		 * @param medianDistance The resulting median distance of all matches, with range [0, infinity)
		 * @param countPerfectMatches The resulting number of provided perfect matches, with range [0, lineMatches.size()]
		 * @param countPartialMatches The resulting number of provided partial matches, with range [0, lineMatches.size()]
		 * @param countComplexMatches The resulting number of provided complex matches, with range [0, lineMatches.size()]
		 * @param notCoveredGroundTruthLines The resulting number of ground truth lines not part of the matches, with range [0, linesGroundTruth.size() - 1]
		 * @param notCoveredEvaluationLines The resulting number of evaluation lines not part of the matches, with range [0, linesEvaluation.size() - 1]
		 * @param notCoveredGroundTruthLineIds Optional resulting ids of all ground truth lines not covered by a corresponding evaluation line
		 * @param notCoveredEvaluationLineIds Optional resulting ids of all evaluation lines not covered by a corresponding ground truth line
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar, either 'float' or 'double'
		 */
		template <typename T>
		static bool evaluateLineMatches(const std::unordered_map<Id, FiniteLineT2<T>>& linesGroundTruth, const std::unordered_map<Id, FiniteLineT2<T>>& linesEvaluation, const LineMatchMap& lineMatches, double& coverage, double& medianAngle, double& medianDistance, size_t& countPerfectMatches, size_t& countPartialMatches, size_t& countComplexMatches, size_t& notCoveredGroundTruthLines, size_t& notCoveredEvaluationLines, IdSet* notCoveredGroundTruthLineIds = nullptr, IdSet* notCoveredEvaluationLineIds = nullptr);

	protected:

		/**
		 * Determines the union of segments resulting from projecting several finite lines onto a unique finite line of interest.
		 * The segments are specified by the location of the projected points on the unique finite line of interest.
		 * @param lineOfInterest The line of interest on which the segments will be determined, must be valid
		 * @param lineIdsToProject The ids of all lines for which the projected segments will be determined
		 * @param lines The map mapping the ids given in 'lineIdsToProject' to finite lines
		 * @return The resulting union of segments, may be an empty union
		 */
		template <typename T>
		static SegmentUnion<T> determineProjectedSegmentUnion(const FiniteLineT2<T>& lineOfInterest, const IdSet& lineIdsToProject, const std::unordered_map<Id, FiniteLineT2<T>>& lines);

		/**
		 * Determines a complex match for a given ground truth line.
		 * @param linesGroundTruth The ground truth lines, at least one
		 * @param linesEvaluation The lines to be evaluated, at least one
		 * @param groundTruthToEvaluationMap The map mapping ids of ground truth lines to similar/overlapping evaluation lines
		 * @param evaluationToGroundTruthMap The map mapping ids of evaluation lines to to similar/overlapping ground truth lines, actually the reverse mapping of 'groundTruthToEvaluationMap'
		 * @param groundTruthId The id of the ground truth line for which the complex match will be determined
		 * @param groundTruthEvaluationSet The set of pairs of line ids for which a mapping in 'groundTruthToEvaluationMap' exists, this set is used to speed up the process
		 * @param complexMatchMaximalGapPixelThreshold The maximal gap between valid evaluation lines, in pixels (defined in the domain of the line coordinates), with range [0, infinity)
		 */
		template <typename T>
		static LineMatchRef determineComplexMatch(const std::unordered_map<Id, FiniteLineT2<T>>& linesGroundTruth, const std::unordered_map<Id, FiniteLineT2<T>>& linesEvaluation, const IdToIdSetMap& groundTruthToEvaluationMap, const IdToIdSetMap evaluationToGroundTruthMap, const Id groundTruthId, const Id64Set& groundTruthEvaluationSet, const T complexMatchMaximalGapPixelThreshold);

		/**
		 * Combines two (32 bit) ids to one 64 bit value.
		 * @param firstId The first id to combine
		 * @param secondId The second id to combine
		 * @return The resulting 64 bit value
		 */
		static inline unsigned long long combineIds(const Id& firstId, const Id& secondId);
};

inline LineEvaluator::LineMatch::LineMatch(const Id sourceId) :
	sourceId_(sourceId)
{
	// nothing to do here
}

inline LineEvaluator::LineMatch::~LineMatch()
{
	// nothing to do here
}

inline LineEvaluator::Id LineEvaluator::LineMatch::sourceId() const
{
	return sourceId_;
}

inline LineEvaluator::PerfectLineMatch::PerfectLineMatch(const Id sourceId, const Id targetId, const double angle, const double maximalDistance) :
	LineMatch(sourceId),
	targetId_(targetId),
	angle_(angle),
	maximalDistance_(maximalDistance)
{
	ocean_assert(NumericD::isInsideRange(0, angle, NumericD::pi_2()));
}

inline LineEvaluator::PerfectLineMatch::MatchType LineEvaluator::PerfectLineMatch::matchType() const
{
	return MT_PERFECT;
}

inline LineEvaluator::Id LineEvaluator::PerfectLineMatch::targetId() const
{
	return targetId_;
}

inline double LineEvaluator::PerfectLineMatch::angle() const
{
	return angle_;
}

inline double LineEvaluator::PerfectLineMatch::maximalDistance() const
{
	return maximalDistance_;
}

inline LineEvaluator::PartialLineMatch::PartialLineMatch(const Id sourceId, const IdSet& targetIds, const double coverage, const double medianAngle, const double medianDistance) :
	LineMatch(sourceId),
	targetIds_(targetIds),
	coverage_(coverage),
	medianAngle_(medianAngle),
	medianDistance_(medianDistance)
{
	ocean_assert(coverage >= 0.0);
	ocean_assert(medianAngle >= 0.0 && medianAngle <= NumericD::pi_2());
	ocean_assert(medianDistance >= 0.0);
}

inline LineEvaluator::PartialLineMatch::PartialLineMatch(const Id sourceId, IdSet&& targetIds, const double coverage, const double medianAngle, const double medianDistance) :
	LineMatch(sourceId),
	targetIds_(std::move(targetIds)),
	coverage_(coverage),
	medianAngle_(medianAngle),
	medianDistance_(medianDistance)
{
	ocean_assert(coverage >= 0.0);
	ocean_assert(medianAngle >= 0.0 && medianAngle <= NumericD::pi_2());
	ocean_assert(medianDistance >= 0.0);
}

inline LineEvaluator::PartialLineMatch::MatchType LineEvaluator::PartialLineMatch::matchType() const
{
	return MT_PARTIAL;
}

inline const LineEvaluator::IdSet& LineEvaluator::PartialLineMatch::targetIds() const
{
	return targetIds_;
}

inline double LineEvaluator::PartialLineMatch::coverage() const
{
	return coverage_;
}

inline double LineEvaluator::PartialLineMatch::medianAngle() const
{
	return medianAngle_;
}

inline double LineEvaluator::PartialLineMatch::medianDistance() const
{
	return medianDistance_;
}

inline LineEvaluator::ComplexLineMatch::ComplexLineMatch(const Id sourceId, const IdSet& targetIds, const double coverage, const double medianAngle, const double medianDistance, const IdSet& connectedSourceIds, const IdSet& connectedTargetIds) :
	PartialLineMatch(sourceId, targetIds, coverage, medianAngle, medianDistance),
	connectedSourceIds_(connectedSourceIds),
	connectedTargetIds_(connectedTargetIds)
{
	// nothing to do here
}

inline LineEvaluator::ComplexLineMatch::MatchType LineEvaluator::ComplexLineMatch::matchType() const
{
	return MT_COMPLEX;
}

inline const LineEvaluator::IdSet& LineEvaluator::ComplexLineMatch::connectedSourceIds() const
{
	return connectedSourceIds_;
}

inline const LineEvaluator::IdSet& LineEvaluator::ComplexLineMatch::connectedTargetIds() const
{
	return connectedTargetIds_;
}

template <typename T>
bool LineEvaluator::areLinesOverlapping(const FiniteLineT2<T>& lineGroundTruth, const FiniteLineT2<T>& lineEvaluation, const T angleThresholdCos, const T distanceThresholdPixels, DistanceMeasure distanceMeasure, T* projectedLength, T* outOfBorderDistance0, T* outOfBorderDistance1, T* locationOnLine0, T* locationOnLine1)
{
	ocean_assert(lineGroundTruth.isValid() && lineEvaluation.isValid());

	ocean_assert(angleThresholdCos >= T(0) && angleThresholdCos <= T(1)); // angleThresholdCos = cos(angleThreshold)
	ocean_assert(distanceThresholdPixels >= T(0));

	// let's check whether both lines are almost parallel

	const T absCosineValue = NumericT<T>::abs(lineGroundTruth.direction() * lineEvaluation.direction());

	if (absCosineValue < angleThresholdCos)
	{
		return false;
	}

	const T sqrDistanceThresholdPixels = distanceThresholdPixels * distanceThresholdPixels;

	T internalSqrDistance = NumericT<T>::maxValue();

	if (distanceMeasure == DM_PROJECTED_ONTO_EVALUATION_LINE || distanceMeasure == DM_PROJECTED_ONTO_EACH_OTHER)
	{
		const VectorT2<T> pointOnInfiniteEvaluation0 = lineEvaluation.nearestPointOnInfiniteLine(lineGroundTruth.point0());
		const VectorT2<T> pointOnInfiniteEvaluation1 = lineEvaluation.nearestPointOnInfiniteLine(lineGroundTruth.point1());

		// the maximum of the distance between end points and projected end points
		internalSqrDistance = std::max(pointOnInfiniteEvaluation0.sqrDistance(lineGroundTruth.point0()), pointOnInfiniteEvaluation1.sqrDistance(lineGroundTruth.point1()));

		if (distanceMeasure == DM_PROJECTED_ONTO_EVALUATION_LINE)
		{
			// we may stop already if the distance is outside the threshold

			if (internalSqrDistance > sqrDistanceThresholdPixels)
			{
				return false;
			}
		}
	}

	T internalOutOfBoundaryDistanceOnGroundTruth0, internalLocationOnGroundTruth0;
	const VectorT2<T> pointOnInfiniteGroundTruth0 = lineGroundTruth.nearestPointOnInfiniteLine(lineEvaluation.point0(), &internalOutOfBoundaryDistanceOnGroundTruth0, &internalLocationOnGroundTruth0);

	T internalOutOfBoundaryDistanceOnGroundTruth1, internalLocationOnGroundTruth1;
	const VectorT2<T> pointOnInfiniteGroundTruth1 = lineGroundTruth.nearestPointOnInfiniteLine(lineEvaluation.point1(), &internalOutOfBoundaryDistanceOnGroundTruth1, &internalLocationOnGroundTruth1);

	if (distanceMeasure != DM_PROJECTED_ONTO_EVALUATION_LINE)
	{
		ocean_assert(distanceMeasure == DM_PROJECTED_ONTO_GROUND_TRUTH || distanceMeasure == DM_PROJECTED_ONTO_EACH_OTHER);

		const T sqrDistanceOnEvaluationLine = std::max(pointOnInfiniteGroundTruth0.sqrDistance(lineEvaluation.point0()), pointOnInfiniteGroundTruth1.sqrDistance(lineEvaluation.point1()));

		if (distanceMeasure == DM_PROJECTED_ONTO_GROUND_TRUTH)
		{
			// we may stop already if the distance is outside the threshold

			if (sqrDistanceOnEvaluationLine > sqrDistanceThresholdPixels)
			{
				return false;
			}

			internalSqrDistance = sqrDistanceOnEvaluationLine;
		}
		else
		{
			// we take the smallest of both distances

			if (sqrDistanceOnEvaluationLine < internalSqrDistance)
			{
				internalSqrDistance = sqrDistanceOnEvaluationLine;
			}

			// we may stop already if the distance is outside the threshold

			if (internalSqrDistance > sqrDistanceThresholdPixels)
			{
				return false;
			}
		}
	}

	ocean_assert(internalSqrDistance != NumericT<T>::maxValue());

	// both lines are close to each other, further the angle is almost similar
	// now let's find out whether the evaluation line is within the boundaries of the finite ground truth line

	Utilities::sortLowestToFront2(internalOutOfBoundaryDistanceOnGroundTruth0, internalOutOfBoundaryDistanceOnGroundTruth1);
	ocean_assert(internalOutOfBoundaryDistanceOnGroundTruth0 <= internalOutOfBoundaryDistanceOnGroundTruth1);

	if (internalOutOfBoundaryDistanceOnGroundTruth1 < 0 || internalOutOfBoundaryDistanceOnGroundTruth0 > 0)
	{
		// the line is completely outside of the boundary of the ground truth line
		return false;
	}

	Utilities::sortLowestToFront2(internalLocationOnGroundTruth0, internalLocationOnGroundTruth1);
	ocean_assert(internalLocationOnGroundTruth0 <= internalLocationOnGroundTruth1);

	if (projectedLength)
	{
		*projectedLength = internalLocationOnGroundTruth1 - internalLocationOnGroundTruth0;

		ocean_assert(NumericT<T>::isWeakEqual(pointOnInfiniteGroundTruth0.distance(pointOnInfiniteGroundTruth1), *projectedLength));
	}

	if (outOfBorderDistance0)
	{
		*outOfBorderDistance0 = internalOutOfBoundaryDistanceOnGroundTruth0;
	}

	if (outOfBorderDistance1)
	{
		*outOfBorderDistance1 = internalOutOfBoundaryDistanceOnGroundTruth1;
	}

	if (locationOnLine0)
	{
		*locationOnLine0 = internalLocationOnGroundTruth0;
	}

	if (locationOnLine1)
	{
		*locationOnLine1 = internalLocationOnGroundTruth1;
	}

	return true;
}

template <typename T>
void LineEvaluator::determineOverlappingAmount(const FiniteLineT2<T>& lineGroundTruth, const FiniteLineT2<T>& lineEvaluation, T* projectedLength, T* outOfBorderDistance0, T* outOfBorderDistance1, T* locationOnLine0, T* locationOnLine1)
{
	ocean_assert(lineGroundTruth.isValid() && lineEvaluation.isValid());

	T internalOutOfBoundaryDistanceOnGroundTruth0, internalLocationOnGroundTruth0;
	lineGroundTruth.nearestPointOnInfiniteLine(lineEvaluation.point0(), &internalOutOfBoundaryDistanceOnGroundTruth0, &internalLocationOnGroundTruth0);

	T internalOutOfBoundaryDistanceOnGroundTruth1, internalLocationOnGroundTruth1;
	lineGroundTruth.nearestPointOnInfiniteLine(lineEvaluation.point1(), &internalOutOfBoundaryDistanceOnGroundTruth1, &internalLocationOnGroundTruth1);

	Utilities::sortLowestToFront2(internalOutOfBoundaryDistanceOnGroundTruth0, internalOutOfBoundaryDistanceOnGroundTruth1);
	Utilities::sortLowestToFront2(internalLocationOnGroundTruth0, internalLocationOnGroundTruth1);

	if (projectedLength)
	{
		*projectedLength = internalLocationOnGroundTruth1 - internalLocationOnGroundTruth0;
	}

	if (outOfBorderDistance0)
	{
		*outOfBorderDistance0 = internalOutOfBoundaryDistanceOnGroundTruth0;
	}

	if (outOfBorderDistance1)
	{
		*outOfBorderDistance1 = internalOutOfBoundaryDistanceOnGroundTruth1;
	}

	if (locationOnLine0)
	{
		*locationOnLine0 = internalLocationOnGroundTruth0;
	}

	if (locationOnLine1)
	{
		*locationOnLine1 = internalLocationOnGroundTruth1;
	}
}

template <typename T>
void LineEvaluator::determineSimilarity(const FiniteLineT2<T>& lineGroundTruth, const FiniteLineT2<T>& lineEvaluation, const DistanceMeasure distanceMeasure, T& angle, T& distance)
{
	ocean_assert(lineGroundTruth.isValid() && lineEvaluation.isValid());

	const T absCosineValue = NumericT<T>::abs(lineGroundTruth.direction() * lineEvaluation.direction());
	angle = NumericT<T>::acos(absCosineValue);

	T sqrDistance = NumericT<T>::maxValue();

	if (distanceMeasure == DM_PROJECTED_ONTO_EVALUATION_LINE || distanceMeasure == DM_PROJECTED_ONTO_EACH_OTHER)
	{
		const VectorT2<T> pointOnInfiniteEvaluation0 = lineEvaluation.nearestPointOnInfiniteLine(lineGroundTruth.point0());
		const VectorT2<T> pointOnInfiniteEvaluation1 = lineEvaluation.nearestPointOnInfiniteLine(lineGroundTruth.point1());

		// the maximum of the distance between end points and projected end points
		sqrDistance = std::max(pointOnInfiniteEvaluation0.sqrDistance(lineGroundTruth.point0()), pointOnInfiniteEvaluation1.sqrDistance(lineGroundTruth.point1()));

		if (distanceMeasure == DM_PROJECTED_ONTO_EVALUATION_LINE)
		{
			distance = NumericT<T>::sqrt(sqrDistance);
			return;
		}
	}

	ocean_assert(distanceMeasure == DM_PROJECTED_ONTO_GROUND_TRUTH || distanceMeasure == DM_PROJECTED_ONTO_EACH_OTHER);

	const VectorT2<T> pointOnInfiniteGroundTruth0 = lineGroundTruth.nearestPointOnInfiniteLine(lineEvaluation.point0());
	const VectorT2<T> pointOnInfiniteGroundTruth1 = lineGroundTruth.nearestPointOnInfiniteLine(lineEvaluation.point1());

	const T sqrDistanceOnEvaluationLine = std::max(pointOnInfiniteGroundTruth0.sqrDistance(lineEvaluation.point0()), pointOnInfiniteGroundTruth1.sqrDistance(lineEvaluation.point1()));

	if (sqrDistanceOnEvaluationLine < sqrDistance)
	{
		sqrDistance = sqrDistanceOnEvaluationLine;
	}

	distance = NumericT<T>::sqrt(sqrDistance);
}

template <typename T>
LineEvaluator::LineMatchMap LineEvaluator::evaluateLineSegments(const std::unordered_map<Id, FiniteLineT2<T>>& linesGroundTruth, const std::unordered_map<Id, FiniteLineT2<T>>& linesEvaluation, const T perfectMatchAngleThreshold, const T perfectMatchPixelThreshold, const T matchAngleThreshold, const T matchCloseToLinePixelThreshold, const T partialMatchNonOverlappingPixelThreshold, const T complexMatchMaximalGapPixelThreshold)
{
	ocean_assert(!linesGroundTruth.empty() && !linesEvaluation.empty());

	ocean_assert(perfectMatchAngleThreshold >= 0 && perfectMatchAngleThreshold <= NumericT<T>::pi_2());
	ocean_assert(perfectMatchPixelThreshold >= 0);

	ocean_assert(matchAngleThreshold >= 0 && matchAngleThreshold <= NumericT<T>::pi_2());
	ocean_assert(matchCloseToLinePixelThreshold >= 0);

	ocean_assert(partialMatchNonOverlappingPixelThreshold >= 0);
	ocean_assert(complexMatchMaximalGapPixelThreshold >= 0);

	typedef std::unordered_map<Id, FiniteLineT2<T>> LineMap;

	// first, we determine a mapping from ground truth lines to connected evaluation lines (all lines which are almost similar and partially overlapping)
	// we can use this map to check for valid matching candidates

	IdToIdSetMap groundTruthToEvaluationMap;
	IdToIdSetMap evaluationToGroundTruthMap;
	Id64Set groundTruthEvaluationSet;

	const T perfectMatchAngleThresholdCos = NumericT<T>::cos(perfectMatchAngleThreshold);
	const T matchAngleThresholdCos = NumericT<T>::cos(matchAngleThreshold);

	for (typename LineMap::const_iterator iG = linesGroundTruth.cbegin(); iG != linesGroundTruth.cend(); ++iG)
	{
		const Id& groundTruthId = iG->first;
		const FiniteLineT2<T>& lineGroundTruth = iG->second;
		ocean_assert(lineGroundTruth.isValid());

		for (typename LineMap::const_iterator iE = linesEvaluation.cbegin(); iE != linesEvaluation.cend(); ++iE)
		{
			const Id& evaluationId = iE->first;
			const FiniteLineT2<T>& lineEvaluation = iE->second;
			ocean_assert(lineEvaluation.isValid());

			if (areLinesOverlapping(lineGroundTruth, lineEvaluation, matchAngleThresholdCos, matchCloseToLinePixelThreshold, DM_PROJECTED_ONTO_EACH_OTHER))
			{
				// for each ground truth line we store the corresponding overlapping/similar evaluation line

				groundTruthToEvaluationMap[groundTruthId].insert(evaluationId);

				// just a set storing that the ground truth line is connected with the evaluation line
				groundTruthEvaluationSet.insert(combineIds(groundTruthId, evaluationId));

				// also, we store the reverse mapping
				evaluationToGroundTruthMap[evaluationId].insert(groundTruthId);
			}
		}
	}

	// now we try to find a corresponding match for each ground truth line

	LineMatchMap lineMatches;

	for (typename LineMap::const_iterator iG = linesGroundTruth.cbegin(); iG != linesGroundTruth.cend(); ++iG)
	{
		const Id& groundTruthId = iG->first;
		const FiniteLineT2<T>& lineGroundTruth = iG->second;

		// whenever we have a valid complex match, we do not need to investage any remaining evaluation line, as they are covered within the complex match already
		LineMatchRef complexMatch;

		SegmentUnion<T> partialMatchUnion;
		IdSet targetIds;

		for (typename LineMap::const_iterator iE = linesEvaluation.cbegin(); !complexMatch && iE != linesEvaluation.cend(); ++iE)
		{
			const Id& evaluationId = iE->first;
			const FiniteLineT2<T>& lineEvaluation = iE->second;

			if (groundTruthEvaluationSet.find(combineIds(groundTruthId, evaluationId)) != groundTruthEvaluationSet.cend())
			{
				T projectedLengthEvaluationLine;
				T outOfBoundaryDistance0, outOfBoundaryDistance1;
				T locationOnLine0, locationOnLine1;

				ocean_assert(areLinesOverlapping(lineGroundTruth, lineEvaluation, matchAngleThresholdCos, matchCloseToLinePixelThreshold, DM_PROJECTED_ONTO_EACH_OTHER));
				determineOverlappingAmount(lineGroundTruth, lineEvaluation, &projectedLengthEvaluationLine, &outOfBoundaryDistance0, &outOfBoundaryDistance1, &locationOnLine0, &locationOnLine1);

				const T projectedNonOverlappingLength = NumericT<T>::abs(outOfBoundaryDistance0) + NumericT<T>::abs(outOfBoundaryDistance1);

				if (projectedNonOverlappingLength <= partialMatchNonOverlappingPixelThreshold)
				{
					const T projectedOverlappingLength = projectedLengthEvaluationLine - projectedNonOverlappingLength;

					// the projected overlapping length cannot be longer than the length of the ground truth line
					ocean_assert_and_suppress_unused(projectedOverlappingLength <= lineGroundTruth.length() + NumericT<T>::weakEps(), projectedOverlappingLength);

					ocean_assert(locationOnLine0 <= locationOnLine1);
					partialMatchUnion.addSegment(locationOnLine0, locationOnLine1);

					targetIds.insert(evaluationId);
				}
				else
				{
					// the line segment is similar to the ground truth line but extends the ground truth line significantly
					// thus, we need to check whether other ground truth lines support the line segment

					complexMatch = determineComplexMatch(linesGroundTruth, linesEvaluation, groundTruthToEvaluationMap, evaluationToGroundTruthMap, groundTruthId, groundTruthEvaluationSet, complexMatchMaximalGapPixelThreshold);

					// the for loop will stop immediately
				}
			}
		}

		if (complexMatch)
		{
			// **TODO** a complex match may also be a perfect match or a partial match

			lineMatches.insert(std::make_pair(groundTruthId, complexMatch));
		}
		else
		{
			if (partialMatchUnion)
			{
				ocean_assert(!targetIds.empty());

				LineMatchRef lineMatch;

				if (targetIds.size() == 1)
				{
					// we may have found a perfect match

					const Id evaluationId = *targetIds.begin();
					const FiniteLineT2<T>& lineEvaluation = linesEvaluation.find(evaluationId)->second;

					// let's check whether both lines are almost parallel
					const T absCosineValue = NumericT<T>::abs(lineGroundTruth.direction() * lineEvaluation.direction());

					if (absCosineValue >= perfectMatchAngleThresholdCos)
					{
						if (lineGroundTruth.isEqual(lineEvaluation, perfectMatchPixelThreshold))
						{
							const T projectedDistancePoint0 = lineGroundTruth.nearestPointOnInfiniteLine(lineEvaluation.point0()).distance(lineEvaluation.point0());
							const T projectedDistancePoint1 = lineGroundTruth.nearestPointOnInfiniteLine(lineEvaluation.point1()).distance(lineEvaluation.point1());

							const T maximalProjectedDistance = std::max(projectedDistancePoint0, projectedDistancePoint1);

							lineMatch = std::make_shared<PerfectLineMatch>(groundTruthId, evaluationId, NumericT<T>::acos(absCosineValue), maximalProjectedDistance);
						}
					}
				}

				if (!lineMatch)
				{
					// we do not have a perfect match, so just a partial match

					const T lengthGroundTruthLine = lineGroundTruth.length();
					const T lengthMatch = partialMatchUnion.unionSize();

					ocean_assert(NumericT<T>::isNotEqualEps(lengthGroundTruthLine) && NumericT<T>::isNotEqualEps(lengthMatch));

					const T matchCoverage = lengthMatch / lengthGroundTruthLine;

					std::vector<T> angles(targetIds.size());
					std::vector<T> distances(targetIds.size());

					const FiniteLineT2<T>& groundTruthLine = linesGroundTruth.find(groundTruthId)->second;

					size_t n = 0;
					for (const Id& targetId : targetIds)
					{
						const FiniteLineT2<T>& evaluationLine = linesEvaluation.find(targetId)->second;

						determineSimilarity(groundTruthLine, evaluationLine, DM_PROJECTED_ONTO_EACH_OTHER, angles[n], distances[n]);
						++n;
					}

					const double medianAngle = double(Median::median<T>(angles.data(), angles.size()));
					const double medianDistance = double(Median::median<T>(distances.data(), distances.size()));

					lineMatch = std::make_shared<PartialLineMatch>(groundTruthId, std::move(targetIds), matchCoverage, medianAngle, medianDistance);
				}

				lineMatches.insert(std::make_pair(groundTruthId, lineMatch));
			}
			else
			{
				ocean_assert(targetIds.empty());
			}
		}
	}

	ocean_assert(lineMatches.size() <= linesGroundTruth.size());

	return lineMatches;
}

template <typename T>
bool LineEvaluator::evaluateLineMatches(const std::unordered_map<Id, FiniteLineT2<T>>& linesGroundTruth, const std::unordered_map<Id, FiniteLineT2<T>>& linesEvaluation, const LineMatchMap& lineMatches, double& coverage, double& medianAngle, double& medianDistance, size_t& countPerfectMatches, size_t& countPartialMatches, size_t& countComplexMatches, size_t& notCoveredGroundTruthLines, size_t& notCoveredEvaluationLines, IdSet* notCoveredGroundTruthLineIds, IdSet* notCoveredEvaluationLineIds)
{
	if (lineMatches.empty())
	{
		return false;
	}

	ocean_assert(lineMatches.size() <= linesGroundTruth.size());
	if (lineMatches.size() > linesGroundTruth.size())
	{
		return false;
	}

	double sumLengthGroundTruth = 0.0;

	for (typename std::unordered_map<Id, FiniteLineT2<T>>::const_iterator iG = linesGroundTruth.cbegin(); iG != linesGroundTruth.cend(); ++iG)
	{
		sumLengthGroundTruth += double(iG->second.length());
	}

	ocean_assert(sumLengthGroundTruth > 0.0);
	if (sumLengthGroundTruth <= 0.0)
	{
		return false;
	}

	countPerfectMatches = 0;
	countPartialMatches = 0;
	countComplexMatches = 0;

	double sumLengthMatches = 0.0;

	std::vector<double> angles;
	angles.reserve(lineMatches.size());

	std::vector<double> distances;
	distances.reserve(lineMatches.size());

	IdSet coveredEvaluationLineIds;

	for (typename LineMatchMap::const_iterator iMatch = lineMatches.cbegin(); iMatch != lineMatches.cend(); ++iMatch)
	{
		const LineMatchRef& match = iMatch->second;
		ocean_assert(match);

		const typename std::unordered_map<Id, FiniteLineT2<T>>::const_iterator iG = linesGroundTruth.find(match->sourceId());

		ocean_assert(iG != linesGroundTruth.cend());
		if (iG == linesGroundTruth.cend())
		{
			return false;
		}

		const FiniteLineT2<T>& groundTruthLine = iG->second;
		ocean_assert(groundTruthLine.isValid());

		const double lengthGroundTruth = double(groundTruthLine.length());

		switch (match->matchType())
		{
			case LineMatch::MT_PERFECT:
			{
				const PerfectLineMatch& perfectMatch = dynamic_cast<const PerfectLineMatch&>(*match);

				sumLengthMatches += double(lengthGroundTruth);
				angles.push_back(perfectMatch.angle());
				distances.push_back(perfectMatch.maximalDistance());

				coveredEvaluationLineIds.insert(perfectMatch.targetId());

				countPerfectMatches++;

				break;
			}

			case LineMatch::MT_PARTIAL:
			{
				const PartialLineMatch& partialMatch = dynamic_cast<const PartialLineMatch&>(*match);

				sumLengthMatches += partialMatch.coverage() * lengthGroundTruth;
				angles.push_back(partialMatch.medianAngle());
				distances.push_back(partialMatch.medianDistance());

				coveredEvaluationLineIds.insert(partialMatch.targetIds().cbegin(), partialMatch.targetIds().cend());

				countPartialMatches++;

				break;
			}

			case LineMatch::MT_COMPLEX:
			{
				const ComplexLineMatch& complexMatch = dynamic_cast<const ComplexLineMatch&>(*match);

				sumLengthMatches += complexMatch.coverage() * lengthGroundTruth;
				angles.push_back(complexMatch.medianAngle());
				distances.push_back(complexMatch.medianDistance());

				coveredEvaluationLineIds.insert(complexMatch.targetIds().cbegin(), complexMatch.targetIds().cend());

				countComplexMatches++;

				break;
			}

			default:
				ocean_assert(false && "Invalid type!");
				return false;
		}
	}

	ocean_assert(NumericT<T>::isNotEqualEps(sumLengthGroundTruth));
	coverage = sumLengthMatches / sumLengthGroundTruth;

	medianAngle = Median::median(angles.data(), angles.size());
	medianDistance = Median::median(distances.data(), distances.size());

	ocean_assert(coveredEvaluationLineIds.size() <= linesEvaluation.size());
	if (coveredEvaluationLineIds.size() > linesEvaluation.size())
	{
		return false;
	}

	notCoveredGroundTruthLines = linesGroundTruth.size() - lineMatches.size();

	notCoveredEvaluationLines = linesEvaluation.size() - coveredEvaluationLineIds.size();

	if (notCoveredGroundTruthLineIds)
	{
		notCoveredGroundTruthLineIds->clear();

		for (typename std::unordered_map<Id, FiniteLineT2<T>>::const_iterator iG = linesGroundTruth.cbegin(); iG != linesGroundTruth.cend(); ++iG)
		{
			if (lineMatches.find(iG->first) == lineMatches.cend())
			{
				notCoveredGroundTruthLineIds->insert(iG->first);
			}
		}
	}

	if (notCoveredEvaluationLineIds)
	{
		notCoveredEvaluationLineIds->clear();

		for (typename std::unordered_map<Id, FiniteLineT2<T>>::const_iterator iE = linesEvaluation.cbegin(); iE != linesEvaluation.cend(); ++iE)
		{
			if (coveredEvaluationLineIds.find(iE->first) == coveredEvaluationLineIds.cend())
			{
				notCoveredEvaluationLineIds->insert(iE->first);
			}
		}
	}

	return true;
}

template <typename T>
SegmentUnion<T> LineEvaluator::determineProjectedSegmentUnion(const FiniteLineT2<T>& lineOfInterest, const IdSet& lineIdsToProject, const std::unordered_map<Id, FiniteLineT2<T>>& lines)
{
	ocean_assert(lineOfInterest.isValid());
	ocean_assert(!lineIdsToProject.empty());

	SegmentUnion<T> segmentUnion;

	for (const Id& lineIdToProject : lineIdsToProject)
	{
		ocean_assert(lines.find(lineIdToProject) != lines.cend());

		const FiniteLineT2<T>& lineToProject = lines.find(lineIdToProject)->second;
		ocean_assert(lineToProject.isValid());

		T locationOnLineOfInterest0;
		lineOfInterest.nearestPointOnInfiniteLine(lineToProject.point0(), nullptr, &locationOnLineOfInterest0);

		T locationOnLineOfInterest1;
		lineOfInterest.nearestPointOnInfiniteLine(lineToProject.point1(), nullptr, &locationOnLineOfInterest1);

		Utilities::sortLowestToFront2(locationOnLineOfInterest0, locationOnLineOfInterest1);

		segmentUnion.addSegment(locationOnLineOfInterest0, locationOnLineOfInterest1);
	}

	return segmentUnion;
}

template <typename T>
LineEvaluator::LineMatchRef LineEvaluator::determineComplexMatch(const std::unordered_map<Id, FiniteLineT2<T>>& linesGroundTruth, const std::unordered_map<Id, FiniteLineT2<T>>& linesEvaluation, const IdToIdSetMap& groundTruthToEvaluationMap, const IdToIdSetMap evaluationToGroundTruthMap, const Id groundTruthId, const Id64Set& groundTruthEvaluationSet, const T complexMatchMaximalGapPixelThreshold)
{
	ocean_assert(complexMatchMaximalGapPixelThreshold >= 0);

	/*
	 * A complex match between lines is given whenever we do not have one or several evaluation line(s) for a ground truth line:
	 *
	 * A valid complex match:
	 * ground truth lines:   +++++++++++ ++++++++++++++++++++++++++++++ ++++++++++++
	 *   evaluation lines: ------------------------  -----------------------------
	 *
	 * An invalid complex match
	 * ground truth lines:          +++++++++++ ++++++++++++++++++++++++++++++ ++++++++++++
	 *   evaluation lines: ------------------------------  -----------------------------
	 *                     ^^^^^^^^^
	 *             (out-of-boundary to large)
	 *
	 * Thus, we have to determine all sibling ground truth lines (connected via evaluation lines).
	 * Afterwards, we can determine the coverage of ground truth line (based on individual evaluation lines)
	 */

	// first we gather all ground truth lines and all evaluation lines which are connected (almost similar and partially overlapping)
	// we start at the ground truth line of interest, determine all connected evaluation lines
	//    for each evaluation line, we determine connected ground truth line and restart the process for (new) ground truth lines

	IdSet connectedGroundTruthIds;
	IdSet connectedEvaluationIds;

	std::vector<Id> groundTruthIdStack(1, groundTruthId);

	while (!groundTruthIdStack.empty())
	{
		const Id currentGroundTruthId = groundTruthIdStack.back();
		groundTruthIdStack.pop_back();

		connectedGroundTruthIds.insert(currentGroundTruthId);

		const IdToIdSetMap::const_iterator iMappingsG2E = groundTruthToEvaluationMap.find(currentGroundTruthId);

		if (iMappingsG2E != groundTruthToEvaluationMap.cend())
		{
			for (const Id& connectedEvaluationId : iMappingsG2E->second)
			{
				const IdSet::const_iterator iConnectedEvaluation = connectedEvaluationIds.find(connectedEvaluationId);

				if (iConnectedEvaluation == connectedEvaluationIds.cend())
				{
					connectedEvaluationIds.insert(connectedEvaluationId);

					const IdToIdSetMap::const_iterator iMappingsE2G = evaluationToGroundTruthMap.find(connectedEvaluationId);

					if (iMappingsE2G != evaluationToGroundTruthMap.cend())
					{
						for (const Id& connectedGroundTruthId : iMappingsE2G->second)
						{
							if (connectedGroundTruthIds.find(connectedGroundTruthId) == connectedGroundTruthIds.cend())
							{
								groundTruthIdStack.push_back(connectedGroundTruthId);
							}
						}
					}
				}
			}
		}
	}

	// no we have all lines that are connected with each other (sibling ground truth lines, and sibling evaluation lines)
	// we need to determine all evaluation lines which are invalid: evaluation lines not fully covered by a corresponding ground truth line

	IdSet validEvaluationIds;

	for (const Id& connectedEvaluationId : connectedEvaluationIds)
	{
		const FiniteLineT2<T>& connectedEvaluationLine = linesEvaluation.find(connectedEvaluationId)->second;

		const SegmentUnion<T> connectedEvaluationUnion = determineProjectedSegmentUnion<T>(connectedEvaluationLine, connectedGroundTruthIds, linesGroundTruth);

		const T lengthConnectedEvaluationLine = connectedEvaluationLine.length();
		const SegmentUnion<T> clampedConnectedEvaluationUnion = connectedEvaluationUnion.intersection(0, lengthConnectedEvaluationLine);

		if (clampedConnectedEvaluationUnion)
		{
			const T frontGap = clampedConnectedEvaluationUnion.segments().begin()->first;
			ocean_assert(frontGap >= T(0));

			const T backGap = lengthConnectedEvaluationLine - clampedConnectedEvaluationUnion.segments().rbegin()->second;
			ocean_assert(backGap >= T(0));

			const T maximalGap = std::max(frontGap, std::max(backGap, clampedConnectedEvaluationUnion.maximalGap()));

			if (maximalGap < complexMatchMaximalGapPixelThreshold)
			{
				ocean_assert(validEvaluationIds.find(connectedEvaluationId) == validEvaluationIds.end());
				validEvaluationIds.insert(connectedEvaluationId);
			}
		}
	}

	// now we can handle our given ground truth line
	// we simply determine the coverage based on all valid evaluation ids

	// NOTE: we do not use the gathered information for other ground truth lines; however, this could improve performance

	const Id& connectedGroundTruthId = groundTruthId;

	const FiniteLineT2<T>& connectedGroundTruthLine = linesGroundTruth.find(connectedGroundTruthId)->second;
	const T lengthConnectedGroundTruthLine = connectedGroundTruthLine.length();

	SegmentUnion<T> connectedPartialMatch;
	IdSet connectedTargetIds;

	std::vector<T> angles;
	angles.reserve(validEvaluationIds.size());

	std::vector<T> distances;
	distances.reserve(validEvaluationIds.size());

	for (const Id& validEvaluationId : validEvaluationIds)
	{
		const FiniteLineT2<T>& validEvaluationLine = linesEvaluation.find(validEvaluationId)->second;

		if (groundTruthEvaluationSet.find(combineIds(connectedGroundTruthId, validEvaluationId)) != groundTruthEvaluationSet.cend())
		{
			// both lines are connected, so we need to determine the coverage

			T locationOnLine0;
			connectedGroundTruthLine.nearestPointOnInfiniteLine(validEvaluationLine.point0(), nullptr, &locationOnLine0);

			T locationOnLine1;
			connectedGroundTruthLine.nearestPointOnInfiniteLine(validEvaluationLine.point1(), nullptr, &locationOnLine1);

			Utilities::sortLowestToFront2(locationOnLine0, locationOnLine1);

			// as we have a complex matching, we do not consider any coverage outside the ground truth line

			locationOnLine0 = minmax(T(0), locationOnLine0, lengthConnectedGroundTruthLine);
			locationOnLine1 = minmax(T(0), locationOnLine1, lengthConnectedGroundTruthLine);

			ocean_assert(locationOnLine0 <= locationOnLine1);
			if (locationOnLine0 < locationOnLine1)
			{
				connectedPartialMatch.addSegment(locationOnLine0, locationOnLine1);
				connectedTargetIds.insert(validEvaluationId);

				T angle, distance;
				determineSimilarity(connectedGroundTruthLine, validEvaluationLine, DM_PROJECTED_ONTO_EACH_OTHER, angle, distance);

				angles.push_back(angle);
				distances.push_back(distance);
			}
		}
	}

	if (connectedPartialMatch)
	{
		ocean_assert(!connectedTargetIds.empty());

		const T lengthMatch = connectedPartialMatch.unionSize();

		ocean_assert(NumericT<T>::isNotEqualEps(lengthConnectedGroundTruthLine) && NumericT<T>::isNotEqualEps(lengthMatch));

		const T matchCoverage = lengthMatch / lengthConnectedGroundTruthLine;

		const double medianAngle = double(Median::median<T>(angles.data(), angles.size()));
		const double medianDistance = double(Median::median<T>(distances.data(), distances.size()));

		return std::make_shared<ComplexLineMatch>(connectedGroundTruthId, connectedTargetIds, matchCoverage, medianAngle, medianDistance, connectedGroundTruthIds, validEvaluationIds);
	}

	return LineMatchRef();
}

inline unsigned long long LineEvaluator::combineIds(const Id& firstId, const Id& secondId)
{
	static_assert(sizeof(Id) * 2 == sizeof(unsigned long long), "Invalid data type!");

	return (unsigned long long)firstId | ((unsigned long long)secondId << 32ull);
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_LINE_EVALUATOR_H
