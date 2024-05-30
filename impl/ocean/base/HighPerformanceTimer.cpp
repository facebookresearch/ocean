/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Median.h"
#include "ocean/base/String.h"

#include <deque>

#if defined(_WINDOWS)
	#include <winsock2.h>
	#include <windows.h>
#elif defined(__APPLE__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wimport-preprocessor-directive-pedantic"
	#include <TargetConditionals.h>
	#include <mach/mach_time.h>
	#pragma clang diagnostic pop
#else
#include <ctime>
#endif

namespace Ocean
{

HighPerformanceTimer::HighPerformanceTimer() :
	ticksStart_(ticks())
{
	// nothing to do here
}

double HighPerformanceTimer::seconds() const
{
	if (ticksStart_ < Ticks(0))
	{
		return 0.0;
	}

	return double(ticks() - ticksStart_) / double(precision());
}

double HighPerformanceTimer::mseconds() const
{
	if (ticksStart_ < Ticks(0))
	{
		return 0.0;
	}

	ocean_assert(precision() != Ticks(0));
	return 1000.0 * double(ticks() - ticksStart_) / double(precision());
}

double HighPerformanceTimer::yseconds() const
{
	ocean_assert(precision() != Ticks(0));
	return 1000000.0 * double(ticks() - ticksStart_) / double(precision());
}

double HighPerformanceTimer::nseconds() const
{
	ocean_assert(precision() != Ticks(0));
	return 1000000000.0 * double(ticks() - ticksStart_) / double(precision());
}

HighPerformanceTimer::Ticks HighPerformanceTimer::precision()
{
	static Ticks resolution = ticksPerSecond();
	ocean_assert(resolution == ticksPerSecond());

	ocean_assert(resolution != Ticks(0));
	return resolution;
}

HighPerformanceTimer::Ticks HighPerformanceTimer::ticks()
{
#ifdef OCEAN_USE_EXTERNAL_PRIVACY_CONFIRM_TICKS

	return Ticks(HighPerformanceTimer_externalPrivacyConfirmTicks());

#else // OCEAN_USE_EXTERNAL_PRIVACY_CONFIRM_TICKS

	#ifdef _WINDOWS

		ocean_assert(sizeof(Ticks) == sizeof(LARGE_INTEGER));

		LARGE_INTEGER largeInteger;
		const BOOL result = QueryPerformanceCounter(&largeInteger);
		ocean_assert_and_suppress_unused(result == TRUE, result);

		Ticks value;
		memcpy(&value, &largeInteger, sizeof(value));

		return value;

	#elif defined(__APPLE__)

		#if defined(OCEAN_BUILD_WITH_BUCK) && defined(OCEAN_PLATFORM_BUILD_APPLE_IOS)
			#error HighPerformanceTimer: The privacy confirm ticks function must be used when building in this configuration
		#endif

		return Ticks(mach_absolute_time());

	#else

		struct timespec time;
		clock_gettime(CLOCK_REALTIME, &time);

		return Ticks(time.tv_sec) * Ticks(1000000000) + Ticks(time.tv_nsec);

	#endif

#endif // OCEAN_USE_EXTERNAL_PRIVACY_CONFIRM_TICKS
}

HighPerformanceTimer::Ticks HighPerformanceTimer::ticksPerSecond()
{
	Ticks value = Ticks(1);

#ifdef _WINDOWS

	ocean_assert(sizeof(Ticks) == sizeof(LARGE_INTEGER));
	const BOOL result = QueryPerformanceFrequency((LARGE_INTEGER*)&value);
	ocean_assert_and_suppress_unused(result == TRUE, result);

#elif defined(__APPLE__)

	mach_timebase_info_data_t info;
	mach_timebase_info(&info);

	ocean_assert(info.numer != 0 && info.denom != 0);

	if (info.numer != 0 && info.denom != 0)
	{
		value = Ticks(1000000000) / (info.numer / info.denom);
	}

#else

	struct timespec resolution;
	clock_getres(CLOCK_REALTIME, &resolution);

	ocean_assert(resolution.tv_sec == 0);
	ocean_assert(resolution.tv_nsec != 0);

	if (resolution.tv_nsec != 0)
	{
		value = Ticks(1000000000) / resolution.tv_nsec;
	}

#endif

	return value;
}

HighPerformanceStatistic::HighPerformanceStatistic()
{
	measurements_.reserve(1024);
}

double HighPerformanceStatistic::average() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return total_ / double(measurements_.size());
}

double HighPerformanceStatistic::median() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return Median::constMedian(measurements_.data(), measurements_.size());
}

double HighPerformanceStatistic::medianMseconds() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return median() * 1000.0;
}

double HighPerformanceStatistic::percentile(double value) const
{
	ocean_assert(value >= 0.0 && value <= 1.0);

	if (measurements_.empty())
	{
		return -1.0;
	}

	Measurements copyMeasurements(measurements_);
	std::sort(copyMeasurements.begin(), copyMeasurements.end());

	const size_t index = size_t(double(copyMeasurements.size() - 1) * value + 0.5);
	ocean_assert(index < copyMeasurements.size());

	return copyMeasurements[index];
}

double HighPerformanceStatistic::percentileMseconds(double value) const
{
	ocean_assert(value >= 0.0 && value <= 1.0);

	if (measurements_.empty())
	{
		return -1.0;
	}

	return percentile(value) * 1000.0;
}

void HighPerformanceStatistic::start()
{
	ocean_assert(started_ == false);

	timer_.start();

	started_ = true;
}

void HighPerformanceStatistic::startIf(const bool value)
{
	if (value)
	{
		start();
	}
}

void HighPerformanceStatistic::stop()
{
	ocean_assert(started_ == true);

	const double measurement = timer_.seconds();

	best_ = min(best_, measurement);
	worst_ = max(worst_, measurement);

	total_ += measurement;
	measurements_.emplace_back(measurement);

	started_ = false;
}

void HighPerformanceStatistic::stopIf(const bool value)
{
	if (value)
	{
		stop();
	}
}

void HighPerformanceStatistic::skip()
{
	ocean_assert(started_ == true);

	started_ = false;
}

void HighPerformanceStatistic::skipIf(const bool value)
{
	if (value)
	{
		skip();
	}
}

void HighPerformanceStatistic::reset()
{
	ocean_assert(started_ == false);

	best_ = DBL_MAX;
	worst_ = -DBL_MAX;
	total_ = 0.0;
	measurements_.clear();
	started_ = false;
}

std::string HighPerformanceStatistic::toString(const unsigned int precision) const
{
	if (measurements_.empty())
	{
		return "No measurements";
	}

	const double performanceBest = best();
	const double performanceAverage = average();
	const double performanceMedian = median();
	const double performanceWorst = worst();

	double unitFactor = 1.0;
	std::string unit = "s";

	if (performanceBest < 0.0001) // using microseconds when < 0.1ms
	{
		unitFactor = 1000000.0;
		unit = "ys";
	}
	else if (performanceBest < 10.0) // using milliseconds when < 10s
	{
		unitFactor = 1000.0;
		unit = "ms";
	}

	return "Best: " + String::toAString(performanceBest * unitFactor, precision) + unit + ", "
				+ "average: " + String::toAString(performanceAverage * unitFactor, precision) + unit + ", "
				+ "median: " + String::toAString(performanceMedian * unitFactor, precision) + unit + ", "
				+ "worst: " + String::toAString(performanceWorst * unitFactor, precision) + unit;
}

HighPerformanceStatistic& HighPerformanceStatistic::operator+=(const HighPerformanceStatistic& right)
{
	ocean_assert(started_ == false);
	ocean_assert(right.started_ == false);

	measurements_.reserve(measurements_.size() + right.measurements_.size());

	for (const double& rightMeasurement : right.measurements_)
	{
		if (rightMeasurement < best_)
		{
			best_ = rightMeasurement;
		}

		if (rightMeasurement > worst_)
		{
			worst_ = rightMeasurement;
		}

		total_ += rightMeasurement;

		measurements_.emplace_back(rightMeasurement);
	}

	return *this;
}

bool HighPerformanceBenchmark::start()
{
	const ScopedLock scopedLock(lock_);

	if (isRunning_)
	{
		return false;
	}

	isRunning_ = true;

	return true;
}

bool HighPerformanceBenchmark::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return false;
	}

	isRunning_ = false;

	return true;
}

void HighPerformanceBenchmark::reset()
{
	const ScopedLock scopedLock(lock_);

	measurementMap_.clear();
}

bool HighPerformanceBenchmark::isRunning() const
{
	const ScopedLock scopedLock(lock_);

	return isRunning_;
}

HighPerformanceBenchmark::MeasurementMap HighPerformanceBenchmark::measurementMap() const
{
	const ScopedLock scopedLock(lock_);

	MeasurementMap measurementMap(measurementMap_);

	return measurementMap;
}

std::vector<std::string> HighPerformanceBenchmark::report(const std::string& referenceCategory) const
{
	const ScopedLock scopedLock(lock_);

	if (measurementMap_.empty())
	{
		return std::vector<std::string>();
	}

	typedef std::multimap<double, std::string> SortedMap;

	double referenceTotalSeconds = -1.0;

	if (!referenceCategory.empty())
	{
		const MeasurementMap::const_iterator i = measurementMap_.find(referenceCategory);
		ocean_assert(i != measurementMap_.cend() && "Category does not exist!");

		if (i != measurementMap_.cend())
		{
			double sumSeconds = 0.0;

			for (const double& measurement : i->second)
			{
				sumSeconds += measurement;
			}

			referenceTotalSeconds = sumSeconds;
		}
	}

	SortedMap sortedMap;
	for (MeasurementMap::const_iterator i = measurementMap_.cbegin(); i != measurementMap_.cend(); ++i)
	{
		double sumSeconds = 0.0;

		for (const double& measurement : i->second)
		{
			sumSeconds += measurement;
		}

		sortedMap.insert(std::make_pair(sumSeconds, i->first));
	}

	std::vector<std::vector<std::string>> tokenMatrix;

	if (referenceTotalSeconds > 0.0)
	{
		tokenMatrix.emplace_back(std::vector<std::string>{"Name", "Percent", "Best (ms)", "Average (ms)", "Worst (ms)", "P50 (ms)", "P90 (ms)", "P95 (ms)", "P99 (ms)", "Measurements", "Total (ms)"});
	}
	else
	{
		tokenMatrix.emplace_back(std::vector<std::string>{"Name", "Best (ms)", "Average (ms)", "Worst (ms)", "P50 (ms)", "P90 (ms)", "P95 (ms)", "P99 (ms)", "Measurements", "Total (ms)"});
	}

	for (SortedMap::const_reverse_iterator i = sortedMap.crbegin(); i != sortedMap.crend(); ++i)
	{
		const std::string& name = i->second;

		ocean_assert(measurementMap_.find(name) != measurementMap_.cend());

		MeasurementsSeconds sortedMeasurementsSeconds(measurementMap_.find(name)->second);
		std::sort(sortedMeasurementsSeconds.begin(), sortedMeasurementsSeconds.end());

		if (sortedMeasurementsSeconds.empty())
		{
			continue;
		}

		const double totalSeconds = i->first;

		const double averageSeconds = totalSeconds / double(sortedMeasurementsSeconds.size());

		const double best = sortedMeasurementsSeconds.front() * 1000.0;
		const double average = averageSeconds * 1000.0;
		const double worst = sortedMeasurementsSeconds.back() * 1000.0;

		const double percentile50 = sortedMeasurementsSeconds[sortedMeasurementsSeconds.size() * 50 / 100] * 1000.0;
		const double percentile90 = sortedMeasurementsSeconds[sortedMeasurementsSeconds.size() * 90 / 100] * 1000.0;
		const double percentile95 = sortedMeasurementsSeconds[sortedMeasurementsSeconds.size() * 95 / 100] * 1000.0;
		const double percentile99 = sortedMeasurementsSeconds[sortedMeasurementsSeconds.size() * 99 / 100] * 1000.0;

		const size_t measurements = sortedMeasurementsSeconds.size();

		std::vector<std::string> rowTokens;

		if (referenceTotalSeconds > 0.0)
		{
			const double percent = totalSeconds / referenceTotalSeconds;

			rowTokens = {name, String::toAString(percent * 100.0, 2u) + "%", String::toAString(best, 2u), String::toAString(average, 2u), String::toAString(worst, 2u), String::toAString(percentile50, 2u), String::toAString(percentile90, 2u), String::toAString(percentile95, 2u), String::toAString(percentile99, 2u), String::toAString(measurements), String::toAString(totalSeconds * 1000.0, 2u)};
		}
		else
		{
			rowTokens = {name, String::toAString(best, 2u), String::toAString(average, 2u), String::toAString(worst, 2u), String::toAString(percentile50, 2u), String::toAString(percentile90, 2u), String::toAString(percentile95, 2u), String::toAString(percentile99, 2u), String::toAString(measurements), String::toAString(totalSeconds * 1000.0, 2u)};
		}

		ocean_assert(!tokenMatrix.empty() && rowTokens.size() == tokenMatrix.front().size());
		tokenMatrix.emplace_back(std::move(rowTokens));
	}

	// Find the max. column width to align the tokens when printing them.
	ocean_assert(!tokenMatrix.empty());
	std::vector<size_t> maximumColumnWidths(tokenMatrix.front().size(), 0);

	for (size_t rowIndex = 0; rowIndex < tokenMatrix.size(); ++rowIndex)
	{
		const std::vector<std::string>& rowTokens = tokenMatrix[rowIndex];
		ocean_assert(rowTokens.size() == maximumColumnWidths.size());

		for (size_t columnIndex = 0; columnIndex < rowTokens.size(); ++columnIndex)
		{
			maximumColumnWidths[columnIndex] = std::max(maximumColumnWidths[columnIndex], rowTokens[columnIndex].size());
		}
	}

	// Create a column-aligned report.
	std::vector<std::string> result;

	for (size_t rowIndex = 0; rowIndex < tokenMatrix.size(); ++rowIndex)
	{
		const std::vector<std::string>& rowTokens = tokenMatrix[rowIndex];

		std::string rowString;

		for (size_t columnIndex = 0; columnIndex < rowTokens.size(); ++columnIndex)
		{
			ocean_assert(rowTokens[columnIndex].size() <= maximumColumnWidths[columnIndex]);

			std::string cellString;

			if (rowTokens[columnIndex].size() < maximumColumnWidths[columnIndex])
			{
				const std::string padding(maximumColumnWidths[columnIndex] - rowTokens[columnIndex].size(), ' ');

				if (rowIndex == 0 || columnIndex == 0)
				{
					// Append
					cellString = rowTokens[columnIndex] + padding;
				}
				else
				{
					// Prepend
					cellString = padding + rowTokens[columnIndex];
				}
			}
			else
			{
				cellString = rowTokens[columnIndex];
			}

			if (columnIndex + 1 < rowTokens.size())
			{
				cellString += " | ";
			}

			rowString += cellString;
		}

		result.emplace_back(std::move(rowString));
	}

	return result;
}

HighPerformanceBenchmark::Category::Category(const std::string& categoryName, const MeasurementsSeconds& measurements, const std::string& categoryNameDelimiter) :
	categoryName_(categoryName),
	categoryNameDelimiter_(categoryNameDelimiter)
{
	ocean_assert(!categoryName_.empty());
	ocean_assert(!measurements.empty());

	sortedMeasurementsSeconds_ = measurements;
	std::sort(sortedMeasurementsSeconds_.begin(), sortedMeasurementsSeconds_.end());
}

bool HighPerformanceBenchmark::Category::addSubCategory(const std::string& newCategoryName, const MeasurementsSeconds& measurements)
{
	ocean_assert(!newCategoryName.empty());
	ocean_assert(!measurements.empty());

	const std::string categoryPrefix = categoryName_ + categoryNameDelimiter_;

	if (newCategoryName.rfind(categoryPrefix, 0) == 0) // >= C++20: starts_with()
	{
		// The input category name is a sub-category of this category; before adding it, though, let's
		// check if it can be further subsumed under any of the existing sub-categories of the current
		// category.
		for (Category& subCategory : subCategories_)
		{
			if (subCategory.addSubCategory(newCategoryName, measurements))
			{
				return true;
			}
		}

		// Add the new category as a sub-category of the current category
		subCategories_.emplace_back(newCategoryName, measurements, categoryNameDelimiter_);

		return true;
	}

	return false;
}

const HighPerformanceBenchmark::Categories& HighPerformanceBenchmark::Category::subCategories() const
{
	return subCategories_;
}

const std::string& HighPerformanceBenchmark::Category::categoryName() const
{
	return categoryName_;
}

double HighPerformanceBenchmark::Category::computeRecursiveSumSeconds() const
{
	double sumSeconds = std::accumulate(sortedMeasurementsSeconds_.begin(), sortedMeasurementsSeconds_.end(), 0.0);

	for (const Category& subCategory : subCategories_)
	{
		sumSeconds += subCategory.computeRecursiveSumSeconds();
	}

	return sumSeconds;
}

bool HighPerformanceBenchmark::Category::reportAsTokenMatrix(std::vector<std::vector<Value>>& tokenMatrix, const double referenceSeconds, const unsigned numberIndentationSpace, const std::string& categoryNameDelimiter, const bool addColumnDescriptions, const bool valuesAsStrings, const bool includeSubCategories) const
{
	std::vector<std::vector<Value>> localTokenMatrix;

	if (addColumnDescriptions)
	{
		localTokenMatrix.emplace_back(std::vector<Value>{Value("Name"), Value("Percent"), Value("Best (ms)"), Value("Average (ms)"), Value("Worst (ms)"), Value("P50 (ms)"), Value("P90 (ms)"), Value("P95 (ms)"), Value("P99 (ms)"), Value("Measurements"), Value("Total (ms)")});
	}

	const double topLevelCategoryTotalSeconds = computeRecursiveSumSeconds();

	if (topLevelCategoryTotalSeconds <= 0.0)
	{
		ocean_assert(false && "Invalid values for the CPU times");
		return false;
	}

	const double referenceSecondsToUse = referenceSeconds > 0.0 ? referenceSeconds : topLevelCategoryTotalSeconds;
	ocean_assert(referenceSecondsToUse > 0.0);

	typedef std::deque<std::pair<const Category*, unsigned int>> CategoryIndentationStack;

	CategoryIndentationStack categoryStack;
	categoryStack.emplace_front(std::make_pair(this, numberIndentationSpace));

	while (!categoryStack.empty())
	{
		const Category* currentCategory = categoryStack.front().first;
		const unsigned int currentIndentation = categoryStack.front().second;

		ocean_assert(currentCategory != nullptr);

		categoryStack.pop_front();

		// Create a shortened and indented version of the category name
		std::string shortenedCategoryName;

		const std::string& currentCategoryName = currentCategory->categoryName();
		const size_t lastDelimiterPos = currentCategoryName.find_last_of(categoryNameDelimiter);

		if (lastDelimiterPos != std::string::npos && lastDelimiterPos + 1 < currentCategoryName.size())
		{
			shortenedCategoryName = currentCategoryName.substr(lastDelimiterPos + 1);
		}
		else
		{
			shortenedCategoryName = currentCategoryName;
		}

		ocean_assert(!shortenedCategoryName.empty());

		const std::string indentedCategoryName = std::string(currentIndentation, ' ') + shortenedCategoryName;
		ocean_assert(!indentedCategoryName.empty());

		// Generate the information for the current category name

		const double currentSumSeconds = currentCategory->computeRecursiveSumSeconds();

		if (currentSumSeconds <= 0.0 || currentSumSeconds > topLevelCategoryTotalSeconds)
		{
			ocean_assert(false && "Invalid values for the CPU times");
			return false;
		}

		const double percentage = currentSumSeconds / referenceSecondsToUse;

		const std::vector<double>& currentSortedMeasurementsSeconds = currentCategory->sortedMeasurementsSeconds_;

		const double averageSeconds = std::accumulate(currentSortedMeasurementsSeconds.begin(), currentSortedMeasurementsSeconds.end(), 0.0) / double(currentSortedMeasurementsSeconds.size());

		const double best = currentSortedMeasurementsSeconds.front() * 1000.0;
		const double average = averageSeconds * 1000.0;
		const double worst = currentSortedMeasurementsSeconds.back() * 1000.0;

		const double percentile50 = currentSortedMeasurementsSeconds[currentSortedMeasurementsSeconds.size() * 50 / 100] * 1000.0;
		const double percentile90 = currentSortedMeasurementsSeconds[currentSortedMeasurementsSeconds.size() * 90 / 100] * 1000.0;
		const double percentile95 = currentSortedMeasurementsSeconds[currentSortedMeasurementsSeconds.size() * 95 / 100] * 1000.0;
		const double percentile99 = currentSortedMeasurementsSeconds[currentSortedMeasurementsSeconds.size() * 99 / 100] * 1000.0;

		const size_t measurements = currentSortedMeasurementsSeconds.size();

		if (valuesAsStrings)
		{
			localTokenMatrix.emplace_back(std::vector<Value>{Value(indentedCategoryName), Value(String::toAString(percentage * 100.0, 2u) + "%"), Value(String::toAString(best, 2u)), Value(String::toAString(average, 2u)), Value(String::toAString(worst, 2u)), Value(String::toAString(percentile50, 2u)), Value(String::toAString(percentile90, 2u)), Value(String::toAString(percentile95, 2u)), Value(String::toAString(percentile99, 2u)), Value(String::toAString(measurements)), Value(String::toAString(currentSumSeconds * 1000.0, 2u))});
		}
		else
		{
			localTokenMatrix.emplace_back(std::vector<Value>{Value(indentedCategoryName), Value(percentage * 100.0), Value(best), Value(average), Value(worst), Value(percentile50), Value(percentile90), Value(percentile95), Value(percentile99), Value((int64_t)measurements), Value(currentSumSeconds * 1000.0)});
		}

		ocean_assert(localTokenMatrix.front().size() == localTokenMatrix.back().size() && "Every row must have the same number of elements");

		if (includeSubCategories)
		{
			// Continue to generate the information for the (sorted) sub-categories
			typedef std::multimap<double, const Category*> SortedCategoryMap;

			SortedCategoryMap sortedCategoryMap;
			for (Categories::const_reverse_iterator i = currentCategory->subCategories_.crbegin(); i != currentCategory->subCategories_.crend(); ++i)
			{
				sortedCategoryMap.insert(std::make_pair(i->computeRecursiveSumSeconds(), &(*i)));
			}

			for (SortedCategoryMap::const_iterator i = sortedCategoryMap.cbegin(); i != sortedCategoryMap.cend(); ++i)
			{
				// Add the sub-categories, which are sorted by their sums of seconds in ascending order, onto the category stack in reverse order (to process those with large sums of seconds first).
				ocean_assert(i->second != nullptr);
				categoryStack.emplace_front(i->second, currentIndentation + 2u);
			}
		}
	}

	ocean_assert(!localTokenMatrix.empty());
	ocean_assert(!localTokenMatrix.front().empty());

#ifdef OCEAN_DEBUG
	for (const std::vector<Value>& rowToken : localTokenMatrix)
	{
		ocean_assert(rowToken.size() == localTokenMatrix.front().size());
	}
#endif

	tokenMatrix = std::move(localTokenMatrix);

	return true;
}

void HighPerformanceBenchmark::Category::sort(Categories& categories)
{
	std::sort(categories.begin(), categories.end(), greaterCpuTime);

	for (Category& category : categories)
	{
		sort(category.subCategories_);
	}
}

bool HighPerformanceBenchmark::Category::greaterCpuTime(const Category& category0, const Category& category1)
{
	return category0.computeRecursiveSumSeconds() >= category1.computeRecursiveSumSeconds();
}

bool HighPerformanceBenchmark::reportWithHierarchies(std::vector<std::string>& report, const std::string& referenceCategory, const std::string& categoryNameDelimiter) const
{
	const ScopedLock scopedLock(lock_);

	if (measurementMap_.empty())
	{
		return false;
	}

	// Create a report for a hierarchy of categories.
	const Categories categories = createCategoryHierarchy(measurementMap_, categoryNameDelimiter);

	if (categories.empty())
	{
		ocean_assert(false && "The list of categories must not be empty.");
		return false;
	}

	// Generate a matrix that contains all the report information as strings; once all cells are known, they can be aligned (next step)
	std::vector<std::vector<Value>> tokenMatrix;

	if (!createTokenMatrixFromCategoryHierarchy(categories, referenceCategory, categoryNameDelimiter, /* valuesAsStrings */ true, tokenMatrix))
	{
		return false;
	}

	// Given the matrix of tokens, find the max. column width to align the tokens when printing them.
	std::vector<size_t> maximumColumnWidths(tokenMatrix.front().size(), 0);

	for (size_t rowIndex = 0; rowIndex < tokenMatrix.size(); ++rowIndex)
	{
		const std::vector<Value>& rowTokens = tokenMatrix[rowIndex];
		ocean_assert(rowTokens.size() == maximumColumnWidths.size());

		for (size_t columnIndex = 0; columnIndex < rowTokens.size(); ++columnIndex)
		{
			ocean_assert(rowTokens[columnIndex].isString());
			maximumColumnWidths[columnIndex] = std::max(maximumColumnWidths[columnIndex], rowTokens[columnIndex].stringValue().size());
		}
	}

	// Create a column-aligned report.
	std::vector<std::string> result;

	for (size_t rowIndex = 0; rowIndex < tokenMatrix.size(); ++rowIndex)
	{
		const std::vector<Value>& rowTokens = tokenMatrix[rowIndex];

		std::string rowString;

		for (size_t columnIndex = 0; columnIndex < rowTokens.size(); ++columnIndex)
		{
			ocean_assert(rowTokens[columnIndex].isString());
			ocean_assert(rowTokens[columnIndex].stringValue().size() <= maximumColumnWidths[columnIndex]);
			const std::string& cellString = rowTokens[columnIndex].stringValue();

			std::string alignedCellString;

			if (cellString.size() < maximumColumnWidths[columnIndex])
			{
				const std::string padding(maximumColumnWidths[columnIndex] - cellString.size(), ' ');

				if (rowIndex == 0 || columnIndex == 0)
				{
					// Align left
					alignedCellString = cellString + padding;
				}
				else
				{
					// Align right
					alignedCellString = padding + cellString;
				}
			}
			else
			{
				alignedCellString = cellString;
			}

			if (columnIndex + 1 < rowTokens.size())
			{
				alignedCellString += " | ";
			}

			rowString += alignedCellString;
		}

		result.emplace_back(std::move(rowString));
	}

	report = std::move(result);

	return true;
}

size_t HighPerformanceBenchmark::measurements(const std::string& category)
{
	const ScopedLock scopedLock(lock_);

	const MeasurementMap::const_iterator i = measurementMap_.find(category);

	if (i != measurementMap_.cend())
	{
		return i->second.size();
	}

	return 0u;
}

HighPerformanceBenchmark::Categories HighPerformanceBenchmark::createCategoryHierarchy(const MeasurementMap& measurementMap, const std::string& categoryNameDelimiter)
{
	std::vector<std::string> categoryNames;
	categoryNames.reserve(measurementMap.size());

	for (MeasurementMap::const_iterator i = measurementMap.cbegin(); i != measurementMap.cend(); ++i)
	{
		categoryNames.emplace_back(i->first);
	}

	std::sort(categoryNames.begin(), categoryNames.end());

	ocean_assert(measurementMap.find(categoryNames.front()) != measurementMap.cend());
	Categories categories = {Category(categoryNames.front(), measurementMap.find(categoryNames.front())->second, categoryNameDelimiter)};

	for (size_t i = 1; i < categoryNames.size(); ++i)
	{
		ocean_assert(measurementMap.find(categoryNames[i]) != measurementMap.cend());
		const MeasurementsSeconds& measurements = measurementMap.find(categoryNames[i])->second;
		ocean_assert(!measurements.empty());

		if (!categories.back().addSubCategory(categoryNames[i], measurements))
		{
			// Add the current element as a new category.
			categories.emplace_back(categoryNames[i], measurements, categoryNameDelimiter);
		}
	}

	// Sort all categories by total CPU time
	Category::sort(categories);

	return categories;
}

bool HighPerformanceBenchmark::createTokenMatrixFromCategoryHierarchy(const Categories& categories, const std::string referenceCategory, const std::string& categoryNameDelimiter, const bool valuesAsStrings, std::vector<std::vector<Value>>& tokenMatrix)
{
	if (categories.empty())
	{
		ocean_assert(false && "Invalid category input");
		return false;
	}

	std::vector<std::vector<Value>> localTokenMatrix;

	if (!referenceCategory.empty())
	{
		// Only report a specific category.
		for (const Category& category : categories)
		{
			if (referenceCategory == category.categoryName())
			{
				if (!category.reportAsTokenMatrix(localTokenMatrix, -1.0, 0u, categoryNameDelimiter, /* addColumnDescription */ true, valuesAsStrings, /* includeSubCategories */ true))
				{
					ocean_assert(false && "The token matrix should never be empty!");
					return false;
				}
			}
		}
	}
	else
	{
		double sumSeconds = 0.0;

		for (const Category& category : categories)
		{
			sumSeconds += category.computeRecursiveSumSeconds();
		}

		for (const Category& category : categories)
		{
			const bool addColumnDescriptions = localTokenMatrix.empty();
			std::vector<std::vector<Value>> categoryTokenMatrix;

			if (!category.reportAsTokenMatrix(categoryTokenMatrix, sumSeconds, 0u, categoryNameDelimiter, addColumnDescriptions, valuesAsStrings, /* includeSubCategories */ true))
			{
				ocean_assert(false && "The token matrix should never be empty!");
				return false;
			}

			for (std::vector<Value>& rowTokens : categoryTokenMatrix)
			{
				ocean_assert(localTokenMatrix.empty() || rowTokens.size() == localTokenMatrix.back().size());
				localTokenMatrix.emplace_back(std::move(rowTokens));
			}
		}
	}

	ocean_assert(!localTokenMatrix.empty());
	ocean_assert(!localTokenMatrix.front().empty());

#ifdef OCEAN_DEBUG
	for (const std::vector<Value>& rowToken : localTokenMatrix)
	{
		ocean_assert(rowToken.size() == localTokenMatrix.front().size());
	}
#endif

	tokenMatrix = std::move(localTokenMatrix);

	return true;
}

void HighPerformanceBenchmark::addMeasurement(const std::string& name, const double measurement)
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	MeasurementMap::iterator i = measurementMap_.find(name);

	if (i == measurementMap_.cend())
	{
		measurementMap_.insert(std::make_pair(name, MeasurementsSeconds(1, measurement)));
	}
	else
	{
		i->second.emplace_back(measurement);
	}
}

}
