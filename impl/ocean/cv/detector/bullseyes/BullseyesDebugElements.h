/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYES_DEBUG_ELEMENTS_H
#define META_OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYES_DEBUG_ELEMENTS_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"

#include "ocean/base/DebugElements.h"
#include "ocean/base/Frame.h"

#include "ocean/math/Math.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

/**
 * This class implements debug elements for the bullseyes library.
 * Debug elements allow to visualize results and intermediate steps from bullseye detection algorithms.
 * @ingroup cvdetectorbullseyes
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT BullseyesDebugElements final :
	public DebugElements,
	public Singleton<BullseyesDebugElements>
{
	friend class Singleton<BullseyesDebugElements>;

	public:

		/// True, in case debugging is allowed and debugging code will be included into the binary; False, to disable debugging code.
		static constexpr bool allowDebugging_ = true;

		/**
		 * Definition of several debug elements.
		 */
		enum ElementId : uint32_t
		{
			/// An invalid element id.
			EI_INVALID = 0u,

			/// BullseyeDetectorMono: Image visualizing valid segment sequences detected in rows.
			EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE,

			/// BullseyeDetectorMono: Image visualizing bullseye candidates that passed neighborhood verification.
			EI_CHECK_BULLSEYE_IN_NEIGHBORHOOD
		};

		/**
		 * This class implements a scoped hierarchy.
		 * The hierarchy exists as long as this object exists.
		 */
		class ScopedHierarchy : public ScopedHierarchyBase
		{
			public:

				/**
				 * Creates a new scoped object and pushes the given hierarchy.
				 * @param hierarchy The hierarchy to be pushed, must be valid
				 */
				inline ScopedHierarchy(const std::string& hierarchy);
		};

		/**
		 * Returns the hierarchy name for the left camera/frame.
		 * @return The hierarchy name "left"
		 */
		static inline std::string hierarchyNameLeftFrame();

		/**
		 * Returns the hierarchy name for the right camera/frame.
		 * @return The hierarchy name "right"
		 */
		static inline std::string hierarchyNameRightFrame();

	public:

		/**
		 * Sets the current left and right camera frames for debug visualization.
		 * If any element ID is enabled, this function stores the frames for later use in drawing functions.
		 * This function must be called before detection so that draw functions (e.g., drawBullseyeCandidateInRow)
		 * can overlay debug information on the original frames. This is necessary because the detector may
		 * operate on pyramid layers with different resolutions than the original frames.
		 *
		 * Typical usage:
		 * @code
		 * // Before detection: store the original camera frames
		 * BullseyesDebugElements::get().setCameraFrames(leftFrame, rightFrame);
		 *
		 * // Run detection (draw functions are called internally with pyramid-layer coordinates)
		 * for (size_t cameraIndex : {0, 1})
		 * {
		 *     const ScopedHierarchy scopedHierarchy(cameraIndex == 0 ? hierarchyNameLeftFrame() : hierarchyNameRightFrame());
		 *     BullseyeDetectorMono::detectBullseyes(frames[cameraIndex], bullseyes);
		 * }
		 *
		 * // After detection: retrieve debug frames per hierarchy
		 * Frame debugLeft = BullseyesDebugElements::get().element(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE, {hierarchyNameLeftFrame()}, true);
		 * Frame debugRight = BullseyesDebugElements::get().element(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE, {hierarchyNameRightFrame()}, true);
		 * @endcode
		 *
		 * @param leftFrame The left camera frame (Y8 grayscale), will be copied if any element is active
		 * @param rightFrame The right camera frame (Y8 grayscale), will be copied if any element is active
		 */
		void setCameraFrames(const Frame& leftFrame, const Frame& rightFrame);

		/**
		 * Returns the current camera frame for the specified side.
		 * @param left True to get the left camera frame, false to get the right camera frame
		 * @return The camera frame, may be invalid if not set
		 */
		Frame getCameraFrame(const bool left) const;

		/**
		 * Draws a bullseye candidate detected in a row.
		 * The function visualizes the five alternating segments (black-white-black-white-black) that form a potential bullseye pattern.
		 * Uses the stored camera frame based on the current hierarchy (left/right).
		 * @param yRow The row index where the candidate was detected (in pyramid layer coordinates), with range [0, frame.height())
		 * @param segmentStart The x-coordinate where the first black segment starts (in pyramid layer coordinates), with range [0, frame.width())
		 * @param segment1Size The size of the first black segment in pixels (in pyramid layer coordinates), with range [1, infinity)
		 * @param segment2Size The size of the first white segment in pixels (in pyramid layer coordinates), with range [1, infinity)
		 * @param segment3Size The size of the center black segment (dot) in pixels (in pyramid layer coordinates), with range [1, infinity)
		 * @param segment4Size The size of the second white segment in pixels (in pyramid layer coordinates), with range [1, infinity)
		 * @param segment5Size The size of the second black segment in pixels (in pyramid layer coordinates), with range [1, infinity)
		 * @param scale Scale factor for coordinates, e.g., 2^i for pyramid layer i, with range [1, infinity)
		 * @sa setCameraFrames()
		 */
		void drawBullseyeCandidateInRow(const unsigned int yRow, const unsigned int segmentStart, const unsigned int segment1Size, const unsigned int segment2Size, const unsigned int segment3Size, const unsigned int segment4Size, const unsigned int segment5Size, const Scalar scale = Scalar(1));

		/**
		 * Draws a bullseye candidate that passed the neighborhood verification check.
		 * The function visualizes the center point and a circle with the given diameter.
		 * Uses the stored camera frame based on the current hierarchy (left/right).
		 * @param yCenter The y-coordinate of the bullseye center (in pyramid layer coordinates), with range [0, frame.height())
		 * @param xCenter The x-coordinate of the bullseye center (in pyramid layer coordinates), with range [0, frame.width())
		 * @param scale Scale factor for coordinates, e.g., 2^i for pyramid layer i, with range [1, infinity)
		 * @param diameter The diameter of the bullseye in pixels (in pyramid layer coordinates), with range [1, infinity)
		 * @sa setCameraFrames()
		 */
		void drawCheckBullseyeInNeighborhood(const unsigned int yCenter, const unsigned int xCenter, const Scalar scale, const unsigned int diameter);

	protected:

		/// The stored left camera frame for debug visualization.
		Frame leftCameraFrame_;

		/// The stored right camera frame for debug visualization.
		Frame rightCameraFrame_;
};

inline BullseyesDebugElements::ScopedHierarchy::ScopedHierarchy(const std::string& hierarchy) :
	ScopedHierarchyBase(get(), hierarchy)
{
	// Nothing to do here.
}

inline std::string BullseyesDebugElements::hierarchyNameLeftFrame()
{
	return "left";
}

inline std::string BullseyesDebugElements::hierarchyNameRightFrame()
{
	return "right";
}

}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYES_DEBUG_ELEMENTS_H
