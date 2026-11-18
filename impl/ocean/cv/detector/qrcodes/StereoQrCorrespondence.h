/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include <cstdint>
#include <vector>

namespace Ocean
{
namespace CV
{
namespace Detector
{
namespace QRCodes
{

/** Selects stereo observations without sharing a finder pattern in either image. */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT StereoQrCorrespondence
{
	public:

		struct Candidate
		{
			uint16_t findersA = 0;
			uint16_t findersB = 0;
			double cost = 0.0;
		};

		/**
		 * Maximizes the number of disjoint candidates, then minimizes their total cost.
		 * @param candidates Candidates with three finder bits per image and finite nonnegative costs
		 * @param selected Indices of selected candidates in ascending order, empty on failure
		 * @param maxSearchNodes Maximum search nodes; an exhausted search reports failure, not a partial assignment
		 * @param maxCandidateVisits Maximum candidate validation, bound and filtering visits
		 * @return True when the optimal assignment was established within both search budgets; false on invalid input, aggregate-cost overflow or budget exhaustion
		 */
		static bool select(const std::vector<Candidate>& candidates, std::vector<size_t>& selected, const size_t maxSearchNodes = 16384, const size_t maxCandidateVisits = 262144);
};

} // namespace QRCodes
} // namespace Detector
} // namespace CV
} // namespace Ocean
