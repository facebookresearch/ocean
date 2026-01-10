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
			EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE
		};

	public:

		/**
		 * Draws a bullseye candidate detected in a row.
		 * The function visualizes the five alternating segments (black-white-black-white-black) that form a potential bullseye pattern.
		 * @param yFrame The grayscale frame in which the candidate was detected, must be valid with FORMAT_Y8
		 * @param yRow The row index where the candidate was detected, with range [0, yFrame.height())
		 * @param segmentStart The x-coordinate where the first black segment starts, with range [0, yFrame.width())
		 * @param segment1Size The size of the first black segment in pixels, with range [1, infinity)
		 * @param segment2Size The size of the first white segment in pixels, with range [1, infinity)
		 * @param segment3Size The size of the center black segment (dot) in pixels, with range [1, infinity)
		 * @param segment4Size The size of the second white segment in pixels, with range [1, infinity)
		 * @param segment5Size The size of the second black segment in pixels, with range [1, infinity)
		 */
		void drawBullseyeCandidateInRow(const Frame& yFrame, const unsigned int yRow, const unsigned int segmentStart, const unsigned int segment1Size, const unsigned int segment2Size, const unsigned int segment3Size, const unsigned int segment4Size, const unsigned int segment5Size);
};

}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYES_DEBUG_ELEMENTS_H
