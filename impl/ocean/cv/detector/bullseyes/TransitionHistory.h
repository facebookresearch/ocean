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
 * This class implements a simple history for previous pixel transitions (a sliding window of pixel transitions) that used for the bullseye detection.
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
			 * @return The previous delta
			 */
			int history1();

			/**
			 * Returns the history with window size 2.
			 * @return The sum of the previous two deltas
			 */
			int history2();

			/**
			 * Returns the history with window size 3.
			 * @return The sum of the previous three deltas
			 */
			int history3();

			/**
			 * Adds a new delta object as most recent history.
			 * Existing history objects will be moved by one pixel.
			 * @param newDelta The new delta object to be added
			 */
			void push(const int newDelta);

			/**
			 * Resets the history object.
			 */
			void reset();

		protected:

			/// The previous delta.
			int deltaMinus1 = 0;

			/// The second previous delta.
			int deltaMinus2 = 0;

			/// The third previous delta.
			int deltaMinus3 = 0;
};

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_TRANSITION_HISTORY_H
