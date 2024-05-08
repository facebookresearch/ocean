/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/CalibrationPatternDetector.h"
#include "ocean/cv/detector/LineDetectorHough.h"

#include "ocean/base/Subset.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/geometry/VanishingProjection.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

CalibrationPatternDetector::CalibrationPatternDetector(const unsigned int horizontalBoxes, const unsigned int verticalBoxes) :
	horizontalBoxes_(horizontalBoxes),
	verticalBoxes_(verticalBoxes),
	horizontalEdges_(horizontalBoxes * 2u),
	verticalEdges_(verticalBoxes * 2u)
{
	// nothing to do here
}

bool CalibrationPatternDetector::detectPattern(const Frame& frame, Worker* worker)
{
	if (!isValid())
	{
		Log::error() << "CalibrationPatternDetector: Invalid pattern parameters.";
		return false;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		Log::error() << "CalibrationPatternDetector: Unsupported pixel format.";
		return false;
	}

	// if we can use the calibration points from the previous frame
	if (!pattern_.isEmpty())
	{
		if (detectCalibrationPatternWithPreviousCorners(yFrame, frame.timestamp(), worker))
		{
			return true;
		}
	}

	return detectCalibrationPatternWithoutKnowledge(yFrame, frame.timestamp(), worker);
}

void CalibrationPatternDetector::release()
{
	pattern_ = Pattern();
}

bool CalibrationPatternDetector::detectCalibrationPatternWithPreviousCorners(const Frame& yFrame, const Timestamp timestamp, Worker* worker)
{
	if (trackingThresholdIndex_ > 0u)
	{
		--trackingThresholdIndex_;
	}

	const std::vector<TrackingThresholds> trackingThresholds =
	{
		TrackingThresholds(640u, 480u, 70, 11),
		TrackingThresholds(640u, 480u, 50, 15),
		TrackingThresholds(640u, 480u, 30, 17)
	};

	while (true)
	{
		ocean_assert(trackingThresholdIndex_ < trackingThresholds.size());

		const unsigned int cornerThreshold = trackingThresholds[trackingThresholdIndex_].harrisCornerThreshold();
		const Scalar cornerDistance = trackingThresholds[trackingThresholdIndex_].maximalCornerDistance(yFrame.width(), yFrame.height());

		// using Harris detector to find strong corners
		HarrisCorners harrisCorners;
		HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), cornerThreshold, false, harrisCorners, true, worker);

		if (detectCalibrationPatternWithPreviousCorners(harrisCorners, cornerDistance, timestamp))
		{
			return true;
		}

		if (trackingThresholdIndex_ + 1u < trackingThresholds.size())
		{
			++trackingThresholdIndex_;
		}
		else
		{
			return false;
		}
	}
}

bool CalibrationPatternDetector::detectCalibrationPatternWithoutKnowledge(const Frame& yFrame, const Timestamp timestamp, Worker* worker)
{
	if (detectionThresholdIndex_ > 0u)
	{
		--detectionThresholdIndex_;
	}

	const std::vector<DetectionThresholds> detectionThresholds =
	{
		DetectionThresholds(640u, 480u, 70, 7, Numeric::deg2rad(3), Numeric::deg2rad(0.5)),
		DetectionThresholds(640u, 480u, 60, 11, Numeric::deg2rad(4), Numeric::deg2rad(1)),
		DetectionThresholds(640u, 480u, 50, 15, Numeric::deg2rad(5), Numeric::deg2rad(1.5)),
		DetectionThresholds(640u, 480u, 30, 20, Numeric::deg2rad(6), Numeric::deg2rad(2))
	};

	while (true)
	{
		ocean_assert(detectionThresholdIndex_ < detectionThresholds.size());

		const unsigned int cornerThreshold = detectionThresholds[detectionThresholdIndex_].harrisCornerThreshold();
		const Scalar maxCornerDistance = detectionThresholds[detectionThresholdIndex_].maximalCornerDistance(yFrame.width(), yFrame.height());
		const Scalar maxParallelAngle = detectionThresholds[detectionThresholdIndex_].maximalParallelAngle();
		const Scalar orientationError = detectionThresholds[detectionThresholdIndex_].orientationError();

		// using Harris detector to find strong corners
		HarrisCorners harrisCorners;
		if (!HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), cornerThreshold, false, harrisCorners, true, worker))
		{
			return false;
		}

		if (harrisCorners.size() >= horizontalBoxes_ * verticalBoxes_ * 4u)
		{
			if (detectCalibrationPatternWithoutKnowledge(yFrame, harrisCorners, maxCornerDistance, maxParallelAngle, orientationError, timestamp, worker))
			{
				return true;
			}
		}

		if (detectionThresholdIndex_ + 1u < detectionThresholds.size())
		{
			++detectionThresholdIndex_;
		}
		else
		{
			// we have one last change: we simply reduce the resolution of the frame
			// so that we will receive better Harris corners in smooth regions (e.g., because of motion blur or out-of-focus issues)
			// however, the accuracy of the corners will not be optimal

			Frame yFrameHalf;
			if (!CV::FrameShrinker::downsampleByTwo11(yFrame, yFrameHalf, worker))
			{
				return false;
			}

			harrisCorners.clear();
			if (!HarrisCornerDetector::detectCorners(yFrameHalf.constdata<uint8_t>(), yFrameHalf.width(), yFrameHalf.height(), yFrameHalf.paddingElements(), cornerThreshold, false, harrisCorners, true, worker))
			{
				return false;
			}

			if (harrisCorners.size() >= horizontalBoxes_ * verticalBoxes_ * 4u)
			{
				// we need to upsample the locations of the Harris corners again
				for (HarrisCorners::iterator i = harrisCorners.begin(); i != harrisCorners.end(); ++i)
				{
					i->setObservation(i->observation() * Scalar(2), i->distortionState());
				}

				if (detectCalibrationPatternWithoutKnowledge(yFrame, harrisCorners, maxCornerDistance, maxParallelAngle, orientationError, timestamp, worker))
				{
					if (detectCalibrationPatternWithPreviousCorners(yFrame, timestamp, worker))
					{
						return true;
					}
				}
			}

			return false;
		}
	}
}

bool CalibrationPatternDetector::detectCalibrationPatternWithPreviousCorners(const HarrisCorners& corners, const Scalar maxCornerDistance, const Timestamp timestamp)
{
	const Scalar maxCornerDistanceSqr = Numeric::sqr(maxCornerDistance);

	IndexSet32 indexSet;
	PatternRows newPatternRows(verticalEdges_, PatternRow(horizontalEdges_));

	for (size_t c = 0; c < newPatternRows.size(); ++c)
	{
		const PatternRow& row = pattern_.rows()[c];
		PatternRow& newRow = newPatternRows[c];

		for (size_t r = 0; r < row.size(); ++r)
		{
			const Vector2& lastPosition = row[r];

			Scalar minSqr = Numeric::maxValue();
			unsigned int minIndex = (unsigned int)(-1);

			for (size_t n = 0; n < corners.size(); ++n)
			{
				if (indexSet.find((unsigned int)n) == indexSet.end())
				{
					const Scalar sqr = (lastPosition - corners[n].observation()).sqr();

					if (sqr < maxCornerDistanceSqr && sqr < minSqr)
					{
						minIndex = (unsigned int)(n);
						minSqr = sqr;
					}
				}
			}

			if (minIndex != (unsigned int)(-1))
			{
				indexSet.insert(minIndex);
				newRow[r] = corners[minIndex].observation();
			}
			else
			{
				return false;
			}
		}
	}

	pattern_ = Pattern(newPatternRows, timestamp);
	return true;
}

bool CalibrationPatternDetector::detectCalibrationPatternWithoutKnowledge(const Frame& yFrame, const HarrisCorners& corners, const Scalar maxCornerDistance, const Scalar maxParallelAngle, const Scalar orientationError, const Timestamp timestamp, Worker* worker)
{
	ocean_assert(yFrame);
	ocean_assert(maxCornerDistance >= 0 && maxParallelAngle >= 0 && maxParallelAngle < Numeric::pi_2());

	const Scalar maxCornerDistanceSqr = Numeric::sqr(maxCornerDistance);

	// detects the strong lines inside the given gray frame
	LineDetectorHough::InfiniteLines lines;
	LineDetectorHough::detectLinesWithAdaptiveThreshold(yFrame, LineDetectorHough::FT_SOBEL, LineDetectorHough::FR_HORIZONTAL_VERTICAL, lines, nullptr, true, Scalar(8), 51u, 4u, 5u, true, worker);
	std::sort(lines.rbegin(), lines.rend());

	const unsigned int minimalEdges = horizontalEdges_ + verticalEdges_;
	if (lines.size() < minimalEdges)
	{
		return false;
	}

	const Scalar minimalStrength = lines[minimalEdges - 1u].strength() * Scalar(0.5);

	for (unsigned int n = minimalEdges; n < lines.size(); ++n)
	{
		if (lines[n].strength() < minimalStrength)
		{
			lines.resize(n);
			break;
		}
	}

	// now we extract the two largest groups of perspectively parallel lines
	// i.e., we seek for two groups of lines either projectively parallel (parallel in the frame), or perspectively parallel (all having a common vanishing point

	const Lines2 cornerAlgnedLines = LineDetectorHough::InfiniteLine::cornerAlignedLines(lines.data(), lines.size(), yFrame.width(), yFrame.height());

	const Indices32 largestSubset = Geometry::VanishingProjection::perspectiveParallelLines(ConstArrayAccessor<Line2>(cornerAlgnedLines), maxParallelAngle, orientationError, Numeric::deg2rad(35));
	Lines2 largestLineGroup = Subset::subset(cornerAlgnedLines, largestSubset);

	if (largestLineGroup.size() < max(horizontalEdges_, verticalEdges_))
	{
		return false;
	}

	const Lines2 remainingLines = Subset::invertedSubset(cornerAlgnedLines, IndexSet32(largestSubset.cbegin(), largestSubset.cend()));
	const Indices32 secondSubset = Geometry::VanishingProjection::perspectiveParallelLines(ConstArrayAccessor<Line2>(remainingLines), maxParallelAngle, orientationError, Numeric::deg2rad(35));
	Lines2 secondLargestLineGroup = Subset::subset(remainingLines, secondSubset);

	if (secondLargestLineGroup.size() < min(horizontalEdges_, verticalEdges_))
	{
		return false;
	}

	Lines2& horizontalLines = (horizontalEdges_ >= verticalEdges_) ? largestLineGroup : secondLargestLineGroup;
	Lines2& verticalLines = (horizontalEdges_ >= verticalEdges_) ? secondLargestLineGroup : largestLineGroup;
	ocean_assert(&horizontalLines != &verticalLines);


	// now we eliminate all lines too far away from Harris corners
	Lines2 horizontalCalibrationLines, verticalCalibrationLines;
	horizontalCalibrationLines.reserve(horizontalLines.size());
	verticalCalibrationLines.reserve(verticalLines.size());

	for (unsigned int n = 0u; n < 2u; ++n)
	{
		horizontalCalibrationLines.clear();
		verticalCalibrationLines.clear();

		for (Lines2::const_iterator iL = horizontalLines.begin(); iL != horizontalLines.end(); ++iL)
		{
			unsigned int nearCorners = 0u;
			for (HarrisCorners::const_iterator iC = corners.begin(); iC != corners.end(); ++iC)
			{
				const Scalar sqrDistance = iL->sqrDistance(iC->observation());

				if (sqrDistance <= maxCornerDistanceSqr)
					++nearCorners;
			}

			if (nearCorners >= verticalEdges_)
			{
				horizontalCalibrationLines.push_back(*iL);
			}
		}

		for (Lines2::const_iterator iL = verticalLines.begin(); iL != verticalLines.end(); ++iL)
		{
			unsigned int nearCorners = 0u;
			for (HarrisCorners::const_iterator iC = corners.begin(); iC != corners.end(); ++iC)
			{
				const Scalar sqrDistance = iL->sqrDistance(iC->observation());

				if (sqrDistance <= maxCornerDistanceSqr)
				{
					++nearCorners;
				}
			}

			if (nearCorners >= horizontalEdges_)
			{
				verticalCalibrationLines.push_back(*iL);
			}
		}

		if (horizontalCalibrationLines.size() == horizontalEdges_ || verticalCalibrationLines.size() == verticalEdges_)
		{
			break;
		}

		// now we check whether we need to swap the two groups of lines for another check

		if (horizontalLines.size() < verticalEdges_ || verticalLines.size() < horizontalEdges_)
		{
			break;
		}

		std::swap(horizontalLines, verticalLines);
	}

	if (horizontalCalibrationLines.size() != horizontalEdges_ || verticalCalibrationLines.size() != verticalEdges_)
	{
		return false;
	}

	ocean_assert(!horizontalCalibrationLines.empty() && !verticalCalibrationLines.empty());

	// now we sort all lines according to their location, we use the distance to the origin

	sortLinesAccordingDistance(horizontalCalibrationLines, Vector2(0, 0));
	sortLinesAccordingDistance(verticalCalibrationLines, Vector2(0, 0));

	if (horizontalCalibrationLines.size() < horizontalEdges_ || verticalCalibrationLines.size() < verticalEdges_)
	{
		return false;
	}

	// now we check whether we have a (unique) Harris corner close to every intersection between horizontal and vertical lines

	HarrisCorners patternCorners;
	IndexSet32 indexSet;

	patternCorners.reserve(horizontalEdges_ * verticalEdges_);

	for (Lines2::const_iterator v = verticalCalibrationLines.begin(); v != verticalCalibrationLines.end(); ++v)
	{
		for (Lines2::const_iterator h = horizontalCalibrationLines.begin(); h != horizontalCalibrationLines.end(); ++h)
		{
			Vector2 intersection;
			if (h->intersection(*v, intersection))
			{
				Scalar minSqr = Numeric::maxValue();
				unsigned int minIndex = (unsigned int)(-1);

				for (size_t n = 0; n < corners.size(); ++n)
				{
					if (indexSet.find((unsigned int)n) == indexSet.end())
					{
						const Scalar sqr = (intersection - corners[n].observation()).sqr();

						if (sqr < maxCornerDistanceSqr && sqr < minSqr)
						{
							minIndex = (unsigned int)(n);
							minSqr = sqr;
						}
					}
				}

				if (minIndex != (unsigned int)(-1))
				{
					indexSet.insert(minIndex);
					patternCorners.push_back(corners[minIndex]);
				}
			}
		}
	}

	if (patternCorners.size() != horizontalEdges_ * verticalEdges_)
	{
		return false;
	}

	PatternRows patternRows(verticalEdges_, PatternRow(horizontalEdges_));

	unsigned int index = 0u;
	for (PatternRows::iterator iC = patternRows.begin(); iC != patternRows.end(); ++iC)
	{
		for (PatternRow::iterator iR = iC->begin(); iR != iC->end(); ++iR)
		{
			*iR = patternCorners[index++].observation();
		}
	}

	const Vector3 position(patternCorners[0].observation(), 0);
	const Vector3 positionHorizontal(patternCorners[horizontalEdges_ - 1].observation(), 0);
	const Vector3 positionDiagonal(patternCorners[verticalEdges_  * horizontalEdges_ - 1].observation(), 0);

	const Vector3 directionHorizonal(positionHorizontal - position);
	const Vector3 directionDiagonal(positionDiagonal - position);

	const Vector3 up(directionDiagonal.cross(directionHorizonal));

	if (up.z() > 0)
	{
		for (unsigned int n = 0; n < patternRows.size() / 2; ++n)
		{
			patternRows[n].swap(patternRows[patternRows.size() - n - 1]);
		}
	}

	pattern_ = Pattern(patternRows, timestamp);

	return true;
}

void CalibrationPatternDetector::sortLinesAccordingDistance(Lines2& lines, const Vector2& point)
{
	// first we ensure that all directions point into the same direction
	const Line2& referenceLine = lines.front();
	ocean_assert(Numeric::isEqual(referenceLine.direction().length(), 1));
	for (size_t n = 1; n < lines.size(); ++n)
	{
		ocean_assert(Numeric::isEqual(lines[n].direction().length(), 1));

		if (referenceLine.direction() * lines[n].direction() < 0)
		{
			lines[n] = Line2(lines[n].point(), -lines[n].direction());
		}

		ocean_assert(referenceLine.direction() * lines[n].direction() > 0);
	}

	typedef std::map<Scalar, Line2> LineMap;
	LineMap lineMap;

	for (size_t n = 0; n < lines.size(); ++n)
	{
		ocean_assert(Numeric::isEqual(lines[n].direction().perpendicular().length(), 1));

		const Scalar signedDistance = lines[n].direction().perpendicular() * (lines[n].point() - point);
		ocean_assert(Numeric::isEqual(Numeric::abs(signedDistance), lines[n].distance(point)));

		ocean_assert(lineMap.find(signedDistance) == lineMap.cend());
		lineMap[signedDistance] = lines[n];
	}

	lines.clear();
	lines.reserve(lineMap.size());

	for (LineMap::const_iterator i = lineMap.cbegin(); i != lineMap.cend(); ++i)
	{
		lines.emplace_back(i->second);
	}
}

}

}

}
