/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_CALIBRATION_PATTERN_DETECTOR_H
#define META_OCEAN_CV_DETECTOR_CALIBRATION_PATTERN_DETECTOR_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/CameraCalibration.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements a detector for calibration patterns.
 * The calibration pattern is composed by a rectangular grid of black quadratic boxes.<br>
 * All boxes must have the same size the white space between the boxes must match the box size.<br>
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT CalibrationPatternDetector
{
	public:

		/**
		 * Definition of a (row) vector holding 2D positions.
		 */
		typedef std::vector<Vector2> PatternRow;

		/**
		 * Definition of a vector holding rows.
		 */
		typedef std::vector<PatternRow> PatternRows;

		/**
		 * Redefinition of a calibration pattern.
		 */
		typedef Geometry::CameraCalibration::Pattern Pattern;

	protected:

		/**
		 * Definition of a threshold container for tracking parameters.
		 */
		class TrackingThresholds
		{
			public:

				/**
				 * Creates a new threshold object.
				 * @param width The width of the camera frame for which the threshold is specified in pixel, with range [1, infinity)
				 * @param height The height of the camera frame for which the threshold is specified in pixel, with range [1, infinity)
				 * @param harrisCorners Strength threshold for Harris corners
				 * @param maximalDistance Maximal pixel distance between predicted and real corner, with range (0, infinity)
				 */
				inline TrackingThresholds(const unsigned int width, const unsigned int height, const unsigned int harrisCorners, const Scalar maximalDistance);

				/**
				 * Returns the strength threshold of the Harris corner.
				 * @return The threshold of the corner detector
				 */
				inline unsigned int harrisCornerThreshold() const;

				/**
				 * Returns the maximal distance between predicted and real corner, in pixel.
				 * @param width The width of the camera frame for which the distance is requested in pixel, with range [1, infinity)
				 * @param height The height of the camera frame for which the distance is requested in pixel, with range [1, infinity)
				 * @return The maximal distance, with range (0, infinity)
				 */
				inline Scalar maximalCornerDistance(const unsigned int width, const unsigned int height) const;

			protected:

				/// The inverted diagonal distance between two opposite corners inside the camera frame.
				const Scalar invFrameDiagonal_;

				/// Strength threshold for Harris corners.
				const unsigned int harrisCornerThreshold_;

				/// Maximal pixel distance between predicted and real corner.
				const Scalar maximalCornerDistance_;
		};

		/**
		 * Definition of a threshold container for detection parameters.
		 */
		class DetectionThresholds : public TrackingThresholds
		{
			public:

				/**
				 * Creates a new threshold object.
				 * @param width The width of the camera frame for which the threshold is specified in pixel, with range [1, infinity)
				 * @param height The height of the camera frame for which the threshold is specified in pixel, with range [1, infinity)
				 * @param harrisCorners Strength threshold for Harris corners
				 * @param maximalDistance Maximal pixel distance between predicted and real corner, with range (0, infinity)
				 * @param maximalParallelAngle Maximal angle between two lines so that they still count as parallel
				 * @param orientationError The expected orientation error each provided line can have e.g., due to measurement/detection inaccuracies, in radian with range [0, PI/2)
				 */
				inline DetectionThresholds(const unsigned int width, const unsigned int height, const unsigned int harrisCorners, const Scalar maximalDistance, const Scalar maximalParallelAngle, const Scalar orientationError);

				/**
				 * Returns the maximal angle between two lines so that they still count as parallel.
				 * @return The maximal angle in radian, with range [0, PI/2)
				 */
				inline Scalar maximalParallelAngle() const;

				/**
				 * The expected orientation error each provided line can have e.g., due to measurement/detection inaccuracies, in radian with range [0, PI/2)
				 * @return The expected error in radian, with range [0, PI/2)
				 */
				inline Scalar orientationError() const;

			protected:

				/// Maximal angle between two lines so that they still count as parallel in radian, with range [0, PI/2).
				const Scalar maximalParallelAngle_;

				/// The expected orientation error each provided line can have e.g., due to measurement/detection inaccuracies, in radian with range [0, PI/2).
				const Scalar orientationError_;
		};

	public:

		/**
		 * Creates a new calibration pattern detector object.
		 * A common calibration pattern may have e.g. 5 horizontal and 7 vertical boxes while each box has a size of 0.02m.<br>
		 * Thus, overall 35 boxes are defined fitting to a size of a DIN A4 size.
		 * @param horizontalBoxes Number of horizontal boxes, with range [1, infinity)
		 * @param verticalBoxes Number of vertical boxes, with range [1, infinity)
		 */
		CalibrationPatternDetector(const unsigned int horizontalBoxes, const unsigned int verticalBoxes);

		/**
		 * Returns the number of horizontal boxes.
		 * @return Horizontal boxes
		 */
		inline unsigned int horizontalBoxes() const;

		/**
		 * Returns the number of vertical boxes.
		 * @return Vertical boxes
		 */
		inline unsigned int verticalBoxes() const;

		/**
		 * Returns the number of horizontal edges.
		 * Each horizontal box will provide two horizontal edges.<br>
		 * Beware: In this meaning the direction of these edges is not horizontal.
		 * @return Number of horizontal edges
		 */
		inline unsigned int horizontalEdges() const;

		/**
		 * Returns the number of vertical edges.
		 * Each vertical box will provide two vertical edges.<br>
		 * Beware: In this meaning the direction of these edges is not horizontal.
		 * @return Number of vertical edges
		 */
		inline unsigned int verticalEdges() const;

		/**
		 * Returns the most recent pattern information (corners of the calibration pattern) associated with the last image frame<br>
		 * in which a pattern could be detected successfully.
		 * @return Most recent pattern rows
		 */
		inline const Pattern& pattern() const;

		/**
		 * Detects the calibration pattern in a new image frame.
		 * @param frame New frame to be used for pattern detection
		 * @param worker Optional worker object to be used to distribute the computation to several CPU cores.
		 * @return True, if a pattern could be detected inside the new frame
		 */
		bool detectPattern(const Frame& frame, Worker* worker = nullptr);

		/**
		 * Releases the information of the previously found calibration pattern.
		 */
		void release();

		/**
		 * Returns whether the detector holds valid parameters.
		 * @return True, if so
		 */
		inline bool isValid() const;

	private:

		/**
		 * Re-detects the calibration pattern using the calibration corners form the previous frame.
		 * @param yFrame Grayscale frame of the input frame
		 * @param timestamp Frame timestamp to be used as pattern timestamp if the pattern could be detected
		 * @param worker Optional worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		bool detectCalibrationPatternWithPreviousCorners(const Frame& yFrame, const Timestamp timestamp, Worker* worker);

		/**
		 * Detects the calibration pattern without any previous information.
		 * @param yFrame Grayscale frame of the input frame
		 * @param timestamp Frame timestamp to be used as pattern timestamp if the pattern could be detected
		 * @param worker Optional worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		bool detectCalibrationPatternWithoutKnowledge(const Frame& yFrame, const Timestamp timestamp, Worker* worker);

		/**
		 * Re-detects the calibration pattern using the calibration corners form the previous frame.
		 * @param harrisCorners Already detector Harris corners
		 * @param maxCornerDistance Maximal distance between previous and new corners in pixel to count as the same corner
		 * @param timestamp Frame timestamp to be used as pattern timestamp if the pattern could be detected
		 * @return True, if succeeded
		 */
		bool detectCalibrationPatternWithPreviousCorners(const HarrisCorners& harrisCorners, const Scalar maxCornerDistance, const Timestamp timestamp);

		/**
		 * Detects the calibration pattern without any previous information.
		 * @param yFrame Grayscale frame of the input frame
		 * @param harrisCorners Already detector Harris corners
		 * @param maxCornerDistance Maximal distance between previous and new corners in pixel to count as the same corner
		 * @param maxParallelAngle Maximal angle between two lines so that they still count as parallel, with range [0, PI/2)
		 * @param orientationError The expected orientation error each provided line can have e.g., due to measurement/detection inaccuracies, in radian with range [0, PI/2)
		 * @param timestamp Frame timestamp to be used as pattern timestamp if the pattern could be detected
		 * @param worker Optional worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		bool detectCalibrationPatternWithoutKnowledge(const Frame& yFrame, const HarrisCorners& harrisCorners, const Scalar maxCornerDistance, const Scalar maxParallelAngle, const Scalar orientationError, const Timestamp timestamp, Worker* worker);

		/**
		 * Sorts lines regarding to the signed distance to a given point.
		 * @param lines The lines to be sorted, further the directions of some lines may flip
		 * @param point The point to which the distance is determined
		 */
		static void sortLinesAccordingDistance(Lines2& lines, const Vector2& point = Vector2(0, 0));

	private:

		/// Number of horizontal boxes.
		unsigned int horizontalBoxes_ = 0u;

		/// Number of vertical boxes.
		unsigned int verticalBoxes_ = 0u;

		/// Number of horizontal edges.
		unsigned int horizontalEdges_ = 0u;

		/// Number of vertical edges.
		unsigned int verticalEdges_ = 0u;

		/// Holds the most recent pattern rows.
		Pattern pattern_;

		/// Index of the recently used tracking threshold parameters.
		unsigned int trackingThresholdIndex_ = 0u;

		/// Index of the recently used tracking detection parameters.
		unsigned int detectionThresholdIndex_ = 0u;
};

inline CalibrationPatternDetector::TrackingThresholds::TrackingThresholds(const unsigned int width, const unsigned int height, const unsigned int harrisCorner, const Scalar maximalDistance) :
	invFrameDiagonal_(Numeric::ratio(Scalar(1), Vector2(Scalar(width), Scalar(height)).length())),
	harrisCornerThreshold_(harrisCorner),
	maximalCornerDistance_(maximalDistance)
{
	ocean_assert(width != 0u && height != 0u);
}

inline unsigned int CalibrationPatternDetector::TrackingThresholds::harrisCornerThreshold() const
{
	return harrisCornerThreshold_;
}

inline Scalar CalibrationPatternDetector::TrackingThresholds::maximalCornerDistance(const unsigned int width, const unsigned int height) const
{
	ocean_assert(width != 0u && height != 0u);

	const Scalar currentDiagonal(Vector2(Scalar(width), Scalar(height)).length());
	ocean_assert(Numeric::isNotEqualEps(currentDiagonal));

	return maximalCornerDistance_ * currentDiagonal * invFrameDiagonal_;
}

inline CalibrationPatternDetector::DetectionThresholds::DetectionThresholds(const unsigned int width, const unsigned int height, const unsigned int harrisCorner, const Scalar maximalDistance, const Scalar maximalParallelAngle, const Scalar orientationError) :
	TrackingThresholds(width, height, harrisCorner, maximalDistance),
	maximalParallelAngle_(maximalParallelAngle),
	orientationError_(orientationError)
{
	ocean_assert(maximalParallelAngle_ >= 0 && maximalParallelAngle_ < Numeric::pi_2());
	ocean_assert(orientationError_ >= 0 && orientationError_ < Numeric::pi_2());
}

inline Scalar CalibrationPatternDetector::DetectionThresholds::maximalParallelAngle() const
{
	ocean_assert(maximalParallelAngle_ >= 0 && maximalParallelAngle_ < Numeric::pi_2());
	return maximalParallelAngle_;
}

inline Scalar CalibrationPatternDetector::DetectionThresholds::orientationError() const
{
	ocean_assert(orientationError_ >= 0 && orientationError_ < Numeric::pi_2());
	return orientationError_;
}

inline unsigned int CalibrationPatternDetector::horizontalBoxes() const
{
	return horizontalBoxes_;
}

inline unsigned int CalibrationPatternDetector::verticalBoxes() const
{
	return verticalBoxes_;
}

inline unsigned int CalibrationPatternDetector::horizontalEdges() const
{
	return horizontalEdges_;
}

inline unsigned int CalibrationPatternDetector::verticalEdges() const
{
	return verticalEdges_;
}

inline bool CalibrationPatternDetector::isValid() const
{
	return horizontalBoxes_ > 0u && verticalBoxes_ > 0u;
}

inline const CalibrationPatternDetector::Pattern& CalibrationPatternDetector::pattern() const
{
	return pattern_;
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_CALIBRATION_PATTERN_DETECTOR_H
