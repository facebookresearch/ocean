/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_TRANSITION_HISTORY_H
#define OCEAN_CV_DETECTOR_BULLSEYES_TRANSITION_HISTORY_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

/**
 * This class implements a simple history for previous pixel transitions (a sliding window of pixel transitions) used for bullseye detection.
 * The history tracks intensity differences (deltas) between adjacent pixels to identify transitions from black to white (or vice versa).
 * The delta is computed as the signed difference between successive pixel intensities, with range [-255, 255].
 * @ingroup cvdetectorbullseyes
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT TransitionHistory
{
	public:

		/**
		 * Creates a new history object.
		 */
		TransitionHistory() = default;

		/**
		 * Returns the history with window size 1.
		 * @return The previous intensity difference (delta) between adjacent pixels, with range [-255, 255]
		 */
		int history1();

		/**
		 * Returns the history with window size 2.
		 * @return The sum of the previous two intensity differences (deltas), with range [-510, 510]
		 */
		int history2();

		/**
		 * Returns the history with window size 3.
		 * @return The sum of the previous three intensity differences (deltas), with range [-765, 765]
		 */
		int history3();

		/**
		 * Adds a new intensity difference (delta) as the most recent history entry.
		 * Existing history entries will be shifted back by one position (deltaMinus1 becomes deltaMinus2, etc.).
		 * @param newDelta The new intensity difference to be added, with range [-255, 255]
		 */
		void push(const int newDelta);

		/**
		 * Resets the history object to its initial state (all deltas set to zero).
		 */
		void reset();

		/**
		 * Checks whether the given pixel is a transition-to-black pixel (whether the direct left neighbor is a bright pixel).
		 * @param pixel The pixel to be checked, must be valid
		 * @param history The history object containing information about previous pixels
		 * @param deltaThreshold The intensity difference threshold between successive pixels to count as a transition, with range [0, 255]
		 * @return True, if so
		 */
		static bool isTransitionToBlack(const uint8_t* pixel, TransitionHistory& history, const int deltaThreshold = defaultDeltaThreshold());

		/**
		 * Checks whether the given pixel is a transition-to-white pixel (whether the direct left neighbor is a dark pixel).
		 * @param pixel The pixel to be checked, must be valid
		 * @param history The history object containing information about previous pixels
		 * @param deltaThreshold The intensity difference threshold between successive pixels to count as a transition, with range [0, 255]
		 * @return True, if so
		 */
		static bool isTransitionToWhite(const uint8_t* pixel, TransitionHistory& history, const int deltaThreshold = defaultDeltaThreshold());

		/**
		 * Returns the default intensity threshold between two successive pixels to count as a transition from black to white (or vice versa).
		 * The delta (intensity difference) is computed as the absolute difference between adjacent pixel intensities.
		 * @return The default threshold value, with range [0, 255]
		 */
		constexpr static int defaultDeltaThreshold();

	protected:

		/// The previous intensity difference (delta) between adjacent pixels, with range [-255, 255]
		int deltaMinus1 = 0;

		/// The second previous intensity difference (delta) between adjacent pixels, with range [-255, 255]
		int deltaMinus2 = 0;

		/// The third previous intensity difference (delta) between adjacent pixels, with range [-255, 255]
		int deltaMinus3 = 0;
};

constexpr int TransitionHistory::defaultDeltaThreshold()
{
	return 20;
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_TRANSITION_HISTORY_H
