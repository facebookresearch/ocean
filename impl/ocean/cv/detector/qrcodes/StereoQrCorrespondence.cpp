/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/StereoQrCorrespondence.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <limits>

namespace Ocean
{
namespace CV
{
namespace Detector
{
namespace QRCodes
{
namespace
{

constexpr size_t kMaxSelections = std::numeric_limits<uint16_t>::digits / 3;

unsigned int bitCount(const uint16_t bits)
{
	return static_cast<unsigned int>(std::bitset<16>(bits).count());
}

class AssignmentSearch
{
	public:

		AssignmentSearch(const std::vector<StereoQrCorrespondence::Candidate>& candidates, const size_t nodes, const size_t work) :
			candidates_(candidates), nodes_(nodes), work_(work)
		{
		}

		bool visit(const uint16_t blockedA, const uint16_t usedB, const size_t depth, const double cost)
		{
			if (nodes_ == 0 || !charge(candidates_.size()))
			{
				return false;
			}
			--nodes_;
			if (!cannotImprove(depth, cost))
			{
				best_ = current_;
				bestSize_ = depth;
				bestCost_ = cost;
			}
			uint16_t unionA = 0;
			uint16_t unionB = 0;
			for (const auto& candidate : candidates_)
			{
				if ((candidate.findersA & blockedA) == 0 && (candidate.findersB & usedB) == 0)
				{
					unionA |= candidate.findersA;
					unionB |= candidate.findersB;
				}
			}
			const size_t upper = depth + std::min(bitCount(unionA), bitCount(unionB)) / 3;
			if (cannotImprove(upper, cost))
			{
				return true;
			}
			const uint16_t pivot = uint16_t(unionA & uint16_t(~unionA + 1));
			for (size_t index = 0; index < candidates_.size(); ++index)
			{
				if (!charge(1))
				{
					return false;
				}
				const auto& candidate = candidates_[index];
				if ((candidate.findersA & pivot) == 0 || (candidate.findersA & blockedA) != 0 || (candidate.findersB & usedB) != 0)
				{
					continue;
				}
				if (candidate.cost > std::numeric_limits<double>::max() - cost)
				{
					return false;
				}
				current_[depth] = index;
				if (!visit(uint16_t(blockedA | candidate.findersA), uint16_t(usedB | candidate.findersB), depth + 1, cost + candidate.cost))
				{
					return false;
				}
				if (cannotImprove(upper, cost))
				{
					return true;
				}
			}
			return visit(uint16_t(blockedA | pivot), usedB, depth, cost);
		}

		std::vector<size_t> selected() const
		{
			return std::vector<size_t>(best_.begin(), best_.begin() + bestSize_);
		}

	private:

		bool charge(const size_t visits)
		{
			if (visits > work_)
			{
				return false;
			}
			work_ -= visits;
			return true;
		}

		bool cannotImprove(const size_t upper, const double cost) const
		{
			return upper < bestSize_ || (upper == bestSize_ && cost >= bestCost_);
		}

		const std::vector<StereoQrCorrespondence::Candidate>& candidates_;
		size_t nodes_;
		size_t work_;
		std::array<size_t, kMaxSelections> current_{};
		std::array<size_t, kMaxSelections> best_{};
		size_t bestSize_ = 0;
		double bestCost_ = 0.0;
};

} // namespace

bool StereoQrCorrespondence::select(const std::vector<Candidate>& candidates, std::vector<size_t>& selected, const size_t maxSearchNodes, const size_t maxCandidateVisits)
{
	selected.clear();
	if (candidates.size() > maxCandidateVisits)
	{
		return false;
	}
	for (const Candidate& candidate : candidates)
	{
		if (bitCount(candidate.findersA) != 3 || bitCount(candidate.findersB) != 3 || !std::isfinite(candidate.cost) || candidate.cost < 0.0)
		{
			return false;
		}
	}
	AssignmentSearch search(candidates, maxSearchNodes, maxCandidateVisits - candidates.size());
	if (!search.visit(0, 0, 0, 0.0))
	{
		return false;
	}
	selected = search.selected();
	std::sort(selected.begin(), selected.end());
	return true;
}

} // namespace QRCodes
} // namespace Detector
} // namespace CV
} // namespace Ocean
