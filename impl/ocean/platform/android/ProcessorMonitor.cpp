/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/ProcessorMonitor.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

ProcessorMonitor::ProcessorMonitor() :
	Thread("ProcessorMonitor Thread"),
	installedCores_(Processor::installedCores())
{
	ocean_assert(installedCores_ > 0u);

	startThread();
}

ProcessorMonitor::~ProcessorMonitor()
{
	stopThread();
}

ProcessorMonitor::FrequencyPairs ProcessorMonitor::monitorData(const Timestamp beginTimestamp, const Timestamp endTimestamp)
{
	const ScopedLock scopedLock(lock_);

	FrequencyPairs frequencyPairs;
	frequencyPairs.reserve(frequencyMap_.size());

	if (double(beginTimestamp) < 0.0)
	{
		for (FrequencyMap::const_iterator i = frequencyMap_.cbegin(); i != frequencyMap_.cend(); ++i)
		{
			frequencyPairs.emplace_back(i->first, i->second);
		}
	}
	else
	{
		for (FrequencyMap::const_iterator i = frequencyMap_.lower_bound(beginTimestamp); i != frequencyMap_.upper_bound(endTimestamp); ++i)
		{
			frequencyPairs.emplace_back(i->first, i->second);
		}
	}

	return frequencyPairs;
}

void ProcessorMonitor::clear()
{
	const ScopedLock scopedLock(lock_);

	frequencyMap_.clear();
}

void ProcessorMonitor::threadRun()
{
	while (!shouldThreadStop())
	{
		const Timestamp now(true);

		if (previousTimestamp_.isInvalid() || now >= previousTimestamp_ + updateRate_)
		{
			Frequencies currentFrequencies(installedCores_);

			for (unsigned int n = 0u; n < installedCores_; ++n)
			{
				currentFrequencies[n] = Processor::currentFrequency(n);
			}

			const ScopedLock scopedLock(lock_);

			frequencyMap_.insert(std::make_pair(now, currentFrequencies));
			previousTimestamp_ = now;
		}
		else
		{
			sleep(1u);
		}
	}
}

void ProcessorStatistic::start()
{
	ProcessorMonitor::get();

	ocean_assert(startTimestamp_.isInvalid());
	startTimestamp_.toNow();
}

void ProcessorStatistic::stop()
{
	ocean_assert(startTimestamp_.isValid());

	const Timestamp stopTimestamp(true);

	const ProcessorMonitor::FrequencyPairs frequencyPairs(ProcessorMonitor::get().monitorData(startTimestamp_, stopTimestamp));
	frequencyPairs_.insert(frequencyPairs_.end(), frequencyPairs.begin(), frequencyPairs.end());

	duration_ += double(stopTimestamp - startTimestamp_);

	if (!frequencyPairs_.empty())
	{
		unsigned int cores = 0u;
		uint64_t frequencies = 0ull;

		unsigned int minFrequency = (unsigned int)(-1);
		unsigned int maxFrequency = 0u;

		for (ProcessorMonitor::FrequencyPair& frequencyPair : frequencyPairs_)
		{
			for (const Processor::Frequency& frequency : frequencyPair.second)
			{
				if (frequency != Processor::invalidFrequency())
				{
					++cores;

					frequencies += uint64_t(frequency);

					if (minFrequency > frequency)
					{
						minFrequency = frequency;
					}

					if (maxFrequency < frequency)
					{
						maxFrequency = frequency;
					}
				}
			}
		}

		if (cores > 0u)
		{
			averageFrequency_ = double(frequencies) / double(cores);
		}

		averageActiveCores_ = double(cores) / double(frequencyPairs_.size());

		minimalFrequency_ = double(minFrequency);
		maximalFrequency_ = double(maxFrequency);

		measurements_ = (unsigned int)frequencyPairs_.size();
	}

	startTimestamp_.toInvalid();
}

void ProcessorStatistic::reset()
{
	ocean_assert(startTimestamp_.isInvalid());

	duration_ = 0.0;
	averageActiveCores_ = -1.0;
	averageFrequency_ = -1.0;
	minimalFrequency_ = -1.0;
	maximalFrequency_ = -1.0;
	measurements_ = 0u;
}

}

}

}
