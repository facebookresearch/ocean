/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_HIGH_PERFORMANCE_TIMER_H
#define META_OCEAN_BASE_HIGH_PERFORMANCE_TIMER_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Value.h"

#include <cfloat>
#include <numeric>

namespace Ocean
{

/**
 * This class implements a high performance timer.
 * Use this timer to measure time durations with high accuracy.<br>
 * The measurement can be very helpful to detect performance bottlenecks.<br>
 * Use the HighPerformanceTimer::precision() to retrieve the possible accuracy of this high performance timer.<br>
 * The implementation is platform dependent.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT HighPerformanceTimer
{
	public:

		/**
		 * Definition of CPU ticks.
		 */
		typedef int64_t Ticks;

	public:

		/**
		 * Creates a new timer and starts the time measurement.
		 */
		HighPerformanceTimer();

		/**
		 * (Re-)starts the time measurement.
		 */
		inline void start();

		/**
		 * Returns the measured time since the timer has been started in seconds.
		 * @return Measured seconds
		 */
		double seconds() const;

		/**
		 * Returns the measured time since the timer has been started in milliseconds.
		 * @return Measured milliseconds
		 */
		double mseconds() const;

		/**
		 * Returns the measured time since the timer has been started in microseconds.
		 * @return Measured microseconds
		 */
		double yseconds() const;

		/**
		 * Returns the measured time since the timer has been started in nanoseconds.
		 * @return Measured nanoseconds
		 */
		double nseconds() const;

		/**
		 * Returns the precision of the timer.
		 * @return Countable ticks per seconds
		 */
		static Ticks precision();

		/**
		 * Returns the recent CPU ticks.
		 * @return Recent CPU ticks
		 */
		static Ticks ticks();

		/**
		 * Converts a given CPU tick into seconds regarding the resolution of the timer.
		 * @param ticks The ticks to convert
		 * @return The Number of seconds matching with the given CPU ticks
		 */
		static inline double ticks2seconds(const Ticks ticks);

	protected:

		/**
		 * Returns the resolution of the timer in ticks per second
		 * @return Ticks per second
		 */
		static Ticks ticksPerSecond();

	protected:

		/// The number of CPU ticks when starting the timer.
		Ticks ticksStart_ = Ticks(0);
};

#ifdef OCEAN_USE_EXTERNAL_PRIVACY_CONFIRM_TICKS

/**
 * Returns the recent CPU ticks which will contain a random offset which is constant through the execution of the process.
 * @return Recent CPU ticks with random offset
 */
uint64_t HighPerformanceTimer_externalPrivacyConfirmTicks();

#endif // OCEAN_USE_EXTERNAL_PRIVACY_CONFIRM_TICKS

/**
 * This class implements a simple module gathering high performance timer statistics.
 * The class is not thread-safe.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT HighPerformanceStatistic
{
	public:

		/**
		 * Definition of a vector storing performance measurements.
		 */
		typedef std::vector<double> Measurements;

		/**
		 * This class defines a scoped high performance statistic module.
		 * Use this scoped module in combination with a high performance statistic object to add a new performance measurement during the existence of this module.
		 * @ingroup base
		 */
		class OCEAN_BASE_EXPORT ScopedStatistic
		{
			public:

				/**
				 * Creates a new scoped statistic object and starts a new measurement.
				 * @param performance High performance statistic object receiving the scoped measurement.
				 */
				inline explicit ScopedStatistic(HighPerformanceStatistic& performance);

				/**
				 * Destructs a scoped statistic object and stops the measurement.
				 */
				inline ~ScopedStatistic();

				/**
				 * Explicitly releases the object and does not wait until the scope ends.
				 */
				inline void release();

			private:

				/**
				 * Deleted copy constructor.
				 */
				ScopedStatistic(const ScopedStatistic&) = delete;

				/**
				 * Deleted copy operator.
				 * @return Reference to this object
				 */
				ScopedStatistic& operator=(const ScopedStatistic&) = delete;

			private:

				/// High performance statistic object receiving the measurement value.
				HighPerformanceStatistic* statisticPerformance;
		};

	public:

		/**
		 * Creates a new statistic module.
		 */
		HighPerformanceStatistic();

		/**
		 * Returns the first measurement time in seconds.
		 * @return First measurement time, in [sec]
		 */
		inline double first() const;

		/**
		 * Returns the first measurement time in milliseconds.
		 * @return First measurement time, in [ms]
		 */
		inline double firstMseconds() const;

		/**
		 * Returns the second measurement time in seconds.
		 * @return Second measurement time, in [sec]
		 */
		inline double second() const;

		/**
		 * Returns the second measurement time in milliseconds.
		 * @return Second measurement time, in [ms]
		 */
		inline double secondMseconds() const;

		/**
		 * Returns the best measurement time in seconds.
		 * @return Best measurement time, in [sec]
		 */
		inline double best() const;

		/**
		 * Returns the best measurement time in milliseconds.
		 * @return Best measurement time, in [ms]
		 */
		inline double bestMseconds() const;

		/**
		 * Returns the worst measurement time in seconds.
		 * @return Worst measurement time, in [sec]
		 */
		inline double worst() const;

		/**
		 * Returns the worst measurement time in milliseconds.
		 * @return Worst measurement time, in [ms]
		 */
		inline double worstMseconds() const;

		/**
		 * Returns the last measurement time in seconds.
		 * @return Last measurement time, in [sec]
		 */
		inline double last() const;

		/**
		 * Returns the last (most recent measurement time in milliseconds.
		 * @return Last measurement time, in [ms]
		 */
		inline double lastMseconds() const;

		/**
		 * Returns the average measurement time in seconds.
		 * @return Average measurement time, in [sec]
		 */
		double average() const;

		/**
		 * Returns the average measurement time in milliseconds.
		 * @return Average measurement time, in [ms]
		 */
		inline double averageMseconds() const;

		/**
		 * Returns the average number of CPU cycles needed for one operation.
		 * @param operations The number of operation that has been invoked during each measurement, with range [1, infinity)
		 * @param clockRate The number of clock cycles per second of the CPU, e.g., 3.8GHz, with range [1, infinity)
		 * @return The average number of CPU cycles needed, with range (0, infinity), -1 if no valid measurement exists
		 */
		inline double averageCyclesPerOperation(const double operations, const double clockRate = 3800000000.0) const;

		/**
		 * Returns the median measurement time in seconds.
		 * @return Median measurement time, in [sec]
		 */
		double median() const;

		/**
		 * Returns the median measurement time in milliseconds.
		 * @return Median measurement time, in [ms]
		 */
		double medianMseconds() const;

		/**
		 * Returns a specific percentile (e.g., P50 = median, P90, P95, etc.) measurement time in seconds.
		 * @param value The percentile to be returned, with range [0, 1]
		 * @return The measurement for the specified percentile, in seconds
		 */
		double percentile(double value) const;

		/**
		 * Returns a specific percentile (e.g., P50 = median, P90, P95, etc.) measurement time in milliseconds.
		 * @param value The percentile to be returned, with range [0, 1]
		 * @return The measurement for the specified percentile, in milliseconds
		 */
		double percentileMseconds(double value) const;

		/**
		 * Returns the total measurement time in seconds.
		 * @return Total measurement time, in [sec]
		 */
		inline double total() const;

		/**
		 * Returns the total measurement time in milliseconds.
		 * @return Total measurement time, in [ms]
		 */
		inline double totalMseconds() const;

		/**
		 * Returns the current (still) running measurement time in seconds.
		 * @return Current still running measurement time, in [sec]
		 */
		inline double running() const;

		/**
		 * Returns the current (still) running measurement time in milliseconds.
		 * @return Current still running measurement time, in [ms]
		 */
		inline double runningMseconds() const;

		/**
		 * Returns the number of measurements.
		 * @return Measurement numbers
		 */
		inline size_t measurements() const;

		/**
		 * Returns whether currently a measurement is running.
		 * @return True, if so
		 */
		inline bool isRunning() const;

		/**
		 * Starts a new measurement.
		 */
		void start();

		/**
		 * Starts a new measurement if the given value is True, otherwise nothing happens.
		 * @param value True; to start a new measurement; False, to ignore this call
		 */
		void startIf(const bool value);

		/**
		 * Stops a measurement.
		 */
		void stop();

		/**
		 * Stops a measurement.
		 * @param value True; to stop the measurement; False, to ignore this call
		 */
		void stopIf(const bool value);

		/**
		 * Skips a started measurement.
		 * The measurement will not be voted.
		 */
		void skip();

		/**
		 * Skips a started measurement.
		 * The measurement will not be voted.
		 * @param value True; to skip the measurement; False, to ignore this call
		 */
		void skipIf(const bool value);

		/**
		 * Resets all gathered statistics.
		 */
		void reset();

		/**
		 * Returns a string with the relevant performance information of this statistic object.
		 * @param precision The number of decimal places displayed, with range [1, infinity)
		 * @return The resulting string
		 */
		std::string toString(const unsigned int precision = 2u) const;

		/**
		 * Returns whether at least one measurement has been done.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Adds measurements from another object to this statistic object.
		 * @param right The statistic object of which the measurements will be added to this object
		 * @return Reference to this object
		 */
		HighPerformanceStatistic& operator+=(const HighPerformanceStatistic& right);

	private:

		/// High performance timer.
		HighPerformanceTimer timer_;

		/// The individual measurements in order as measured.
		Measurements measurements_;

		/// Best measurement time in seconds.
		double best_ = DBL_MAX;

		/// Worst measurement time in seconds.
		double worst_ = -DBL_MAX;

		/// Entire measurement time in seconds.
		double total_ = 0.0;

		/// State determining whether one measurement is active currently.
		bool started_ = false;
};

/**
 * The HighPerformanceBenchmark object allows to benchmark algorithms with individual categories.
 * Benchmarking needs to be started before it can be used.<br>
 * The class is thread-safe.
 * This class creates flat and/or hierarchical reports. In the case of the former, the categories are sorted by their CPU time. For hierarchical
 * reports, the category names can be expanded using a delimiter, for example `Foo::Bar` where `Bar` is a sub-category of `Foo`. In this report the
 * top-level categories (like `Foo`) will be sorted by their total CPU time which is accumulated over all of their sub-categories (like `Foo::Bar`).
 *
 * Usage Example:
 * <pre>
 * void SomeClass::computeSomething(...)
 * {
 *     HighPerformanceBenchmark::ScopedCategory scopedBenchmark("AlgorithmName");
 *     function0();
 *     function1();
 *     ...
 * }
 *
 * void SomeClass::function0(...)
 * {
 *     HighPerformanceBenchmark::ScopedCategory scopedBenchmark("AlgorithmName::Function0");
 *     ...
 * }
 *
 * void SomeClass::function1(...)
 * {
 *     HighPerformanceBenchmark::ScopedCategory scopedBenchmark("AlgorithmName::Function1");
 *     utilityFunction0();
 *     ...
 * }
 *
 * void Utility::utilityFunction0(...)
 * {
 *     HighPerformanceBenchmark::ScopedCategory scopedBenchmark("UtilityFunction0");
 *     ...
 * }
 *
 * int main()
 * {
 *     HighPerformanceBenchmark::get().start();
 *
 *     while (keepLooping)
 *     {
 *         SomeClass::computeSomething();
 *     }
 *
 *     HighPerformanceBenchmark::get().stop();
 *
 *     // Get the normal or hierarchical performance report
 *     const std::vector<std::string> report = HighPerformanceBenchmark::get().report();
 *     const std::vector<std::string> reportWithHierarchies = HighPerformanceBenchmark::get().reportWithHierarchies();
 * }
 * </pre>
 *
 * which will result in something similar to the following for the normal report:
 * <pre>
 * Name                     | ...
 * AlgorithmName            | ...
 * UtilityFunction0         | ...
 * AlgorithmName::Function0 | ...
 * AlgorithmName::Function1 | ...
 * </pre>
 * and for the hierarchical report the output will be similar to this (sub-categories will be appear indented):
 * <pre>
 * Name             | ...
 * AlgorithmName    | ...
 *   Function0      | ...
 *   Function1      | ...
 * UtilityFunction0 | ...
 * </pre>
 * Of course, the actual order of the categories depends on their proportional contribution to the overall measured CPU time
 * @ingroup base
 */
class OCEAN_BASE_EXPORT HighPerformanceBenchmark : public Singleton<HighPerformanceBenchmark>
{
	friend class Singleton<HighPerformanceBenchmark>;

	public:

		/**
		 * Definition of a vector holding measurements in seconds.
		 */
		typedef std::vector<double> MeasurementsSeconds;

		/**
		 * Definition of a map mapping category names to measurements.
		 */
		typedef std::unordered_map<std::string, MeasurementsSeconds> MeasurementMap;

		/// Forward declaration
		class Category;

		/// Typedef for a vector of categories
		typedef std::vector<Category> Categories;

		/**
		 * This class defines a hierarchical category
		 * This class is used to group categories based on their names into a hierarchy. A hierarchy of categories is created by appending the name of a
		 * sub-category to the name of category, using a delimiter between both names, for example "Foo::Bar" is a sub-category of "Foo". This process can
		 * be repeated recursively ("Foo::Bar::Baz"). Categories without a delimiter in their name - or which cannot be matched otherwise - will be
		 * considered top-level categories ("Foo").
		 * @ingroup base
		 */
		class Category
		{
			public:

				/**
				 * Deleted default constructor.
				 */
				Category() = delete;

				/**
				 * Constructor
				 * @param categoryName The name of the category, must be valid
				 * @param measurementsSeconds The measurements in seconds that have been made by `HighPerformanceBenchmark` for this category, must be valid and non-empty
				 * @param categoryNameDelimiter The delimiter that separate the levels of hierarchy in the category names.
				 */
				Category(const std::string& categoryName, const MeasurementsSeconds& measurementsSeconds, const std::string& categoryNameDelimiter);

				/**
				 * Adds a sub-category to this category
				 * @param subCategoryName The name of the category that will be added as a sub-category to this category, must be valid
				 * @param measurementsSeconds The measurements in seconds that have been made by `HighPerformanceBenchmark` for the new sub-category, must be valid and non-empty
				 */
				bool addSubCategory(const std::string& subCategoryName, const MeasurementsSeconds& measurementsSeconds);

				/**
				 * Returns the sub-categories of this category
				 * @return The sub-categories
				 */
				const Categories& subCategories() const;

				/**
				 * Returns the name of this category
				 * @return The name
				 */
				const std::string& categoryName() const;

				/**
				 * Computes the sum of all measurements in this category and all of its sub-categories
				 * @return The sum
				 */
				double computeRecursiveSumSeconds() const;

				/**
				 * Creates a performance report as a matrix of string tokens
				 * The token matrix is a means to determine the max. column widths in the final report. This alignment step does not happen here, cf. `HighPerformanceBenchmark::reportWithHierarchies()`
				 * @param tokenMatrix The resulting matrix of tokens.
				 * @param referenceSeconds The optional number of seconds that should be used to compute the percentage of CPU runtime of this categories (and it's sub-categories); for values <= 0.0 the percentage will be computed internally for this category only, range: (-infinity, infinity)
				 * @param numberIndentationSpace The optional number of spaces that should be prepended to the names of the categories in the final report (to visualize the hierarchy)
				 * @param categoryNameDelimiter The delimiter that separate the levels of hierarchy in the category names.
				 * @param addColumnDescriptions True, adds a row to the resulting token matrix that contains human-readable descriptions of the columns of the matrix, otherwise no descriptions will be added
				 * @param valuesAsStrings True, all cells will be of type string; False, column descriptions and category names will be reported as string and all other values as double or integer.
				 * @param includeSubCategories True, all sub-categories of the this category will be added to the token matrix as well; False, they will be ignored
				 * @return True, if the generation of the token matrix was successful, otherwise false
				 * @sa HighPerformanceBenchmark::reportWithHierarchies()
				 */
				bool reportAsTokenMatrix(std::vector<std::vector<Value>>& tokenMatrix, const double referenceSeconds = 0.0, const unsigned numberIndentationSpace = 0u, const std::string& categoryNameDelimiter = "::", const bool addColumnDescriptions = false, const bool valuesAsStrings = true, const bool includeSubCategories = true) const;

				/**
				 * Recursively sorts a list of categories by their total CPU times in descending order
				 * @param categories The categories that will be sorted
				 */
				static void sort(Categories& categories);

				/**
				 * Compares the recursive CPU times of two categories
				 * @param category0 The first category that will be used in this comparison, must be valid
				 * @param category1 The second category that will be used in this comparison, must be valid
				 * @return True if the CPU time of the first category is equal or larger than the second, otherwise false
				 */
				static bool greaterCpuTime(const Category& category0, const Category& category1);

			protected:

				/// The human-readable name of this category, e.g., "Foo"
				std::string categoryName_;

				/// The sorted measurements for this category
				MeasurementsSeconds sortedMeasurementsSeconds_;

				/// The delimiter that is used to separate different levels of the hierarchy in the human-readable category name, e.g. "::"
				std::string categoryNameDelimiter_;

				/// The list of sub-categories, e.g. "Foo::Bar", "Foo::Bar::Test", "Foo::Baz", etc.
				std::vector<Category> subCategories_;
		};

	public:

		/**
		 * This class implements a scoped benchmark category.
		 * There must not exist more than one object for each category at the same time.
		 */
		class OCEAN_BASE_EXPORT ScopedCategory
		{
			public:

				/**
				 * Creates a new scoped category with specific name.
				 * Benchmarking will be active as long as the object exists (the execution time of the category will be increased as long as the object exists).
				 * @param name The name of the category, must be valid
				 */
				explicit inline ScopedCategory(std::string name);

				/**
				 * Destructs the scoped category.
				 * Benchmarking for this category will end.
				 */
				inline ~ScopedCategory();

				/**
				 * Explicitly skips benchmarking for this category before the actual scope ends, e.g., if a function did not finish due to an error.
				 */
				inline void skip();

				/**
				 * Explicitly ends benchmarking for this category before the actual scope ends.
				 */
				inline void release();

				/**
				 * Changes the benchmarking category, releases the current category and creates a new one.
				 * @param name The new name of the category, empty to release the current category without creating a new one
				 */
				inline void change(std::string name);

			protected:

				/**
				 * Not existing copy constructor.
				 * @param scopedCategory The object that would be copied
				 */
				ScopedCategory(const ScopedCategory& scopedCategory) = delete;

				/**
				 * Not existing copy constructor.
				 * @param scopedCategory The object that would be copied
				 */
				ScopedCategory& operator=(const ScopedCategory& scopedCategory) = delete;

			protected:

				/// The name of the benchmark category.
				std::string name_;

				/// The CPU ticks when the benchmark of this category started.
				HighPerformanceTimer::Ticks startTicks_;
		};

	public:

		/**
		 * Starts benchmarking.
		 * @return True, if succeeded
		 * @see stop(), isRunning().
		 */
		bool start();

		/**
		 * Stops benchmarking.
		 * @return True, if succeeded
		 * @see start(), isRunning().
		 */
		bool stop();

		/**
		 * Rests all benchmark categories and measurements.
		 */
		void reset();

		/**
		 * Returns whether benchmarking is currently active; False by default.
		 * @return True, if so
		 */
		bool isRunning() const;

		/**
		 * Returns the map with category names and measurement objects.
		 * @return The measurement map
		 */
		MeasurementMap measurementMap() const;

		/**
		 * Creates a performance report as a readable string.
		 * @param referenceCategory Optional reference category for to add relative performance values to the report
		 * @return The report as readable string, one string for each line in the report
		 */
		std::vector<std::string> report(const std::string& referenceCategory = std::string()) const;

		/**
		 * Creates a performance report for a hierarchy of categories as a human-readable string.
		 *
		 * A hierarchy of categories is created by appending the name of a sub-category to the name of category, using a delimiter between both names,
		 * for example "Foo::Bar" is a sub-category of "Foo". This process can be repeated recursively ("Foo::Bar::Baz"). Categories without a delimiter
		 * in their name - or which cannot be matched otherwise - will be considered top-level categories ("Foo").
		 *
		 * The final report will list either 1) all top-level categories with their subsumed sub-categories (which will be indented) or 2) a specific
		 * top-level category. All categories and sub-categories will be sorted by their total CPU-time in descending order.
		 * @param report The resulting report as a readable string, one string for each line in the report
		 * @param referenceCategory The optional reference category for to add relative performance values to the report
		 * @param categoryNameDelimiter The optional delimiter that is used to separate levels of categories in a hierarchy
		 * @return True, if the generation of the report was successful, otherwise false
		 */
		bool reportWithHierarchies(std::vector<std::string>& report, const std::string& referenceCategory = std::string(), const std::string& categoryNameDelimiter = "::") const;

		/**
		 * Returns the number of measurements of a specific category.
		 * @param category The category for which the number of measurements will be returned
		 * @return The number of measurements, with range [0, infinity)
		 */
		size_t measurements(const std::string& category);

		/**
		 * Creates a hierarchy of categories based on their names from a map of measurements
		 * @param measurementMap The map of measurements (category -> measurements) for which a hierarchy of categories will be created
		 * @param categoryNameDelimiter The delimiter that is used to separate levels of categories in a hierarchy
		 * @return The hierarchy
		 */
		static Categories createCategoryHierarchy(const MeasurementMap& measurementMap, const std::string& categoryNameDelimiter);

		/**
		 * Given a hierarchy of categories with measurements, create a matrix with the performance information
		 * The output of this function is a matrix where each cell contains the information that the final report will contain. This intermediate container makes it easy to print the report with aligned columns.
		 * @param categories The hierarchy of categories for which a token matrix will created, must be valid and not empty.
		 * @param referenceCategory The optional reference category for to add relative performance values to the report
		 * @param categoryNameDelimiter The optional delimiter that is used to separate levels of categories in a hierarchy
		 * @param valuesAsStrings True, all cells will be of type string; False, column descriptions and category names will be reported as string and all other values as double or integer.
		 * @param tokenMatrix The resulting token matrix for all categories.
		 * @return True, if the generation of the token matrix was successful, otherwise false.
		 */
		static bool createTokenMatrixFromCategoryHierarchy(const Categories& categories, const std::string referenceCategory, const std::string& categoryNameDelimiter, const bool valuesAsStrings, std::vector<std::vector<Value>>& tokenMatrix);

	protected:

		/**
		 * Adds a benchmark measurement for a specified category.
		 * @param name The name of the category
		 * @param measurement The benchmark measurement in seconds, with range [0, infinity)
		 */
		void addMeasurement(const std::string& name, const double measurement);

	protected:

		/**
		 * Default constructor.
		 */
		HighPerformanceBenchmark();

		/**
		 * Destructs an object.
		 */
		~HighPerformanceBenchmark();

	protected:

		/// The map mapping category names to their measurement objects.
		MeasurementMap measurementMap_;

		/// True, if benchmarking is running, false by default.
		bool isRunning_;

		/// The lock object.
		mutable Lock lock_;
};

inline HighPerformanceStatistic::ScopedStatistic::ScopedStatistic(HighPerformanceStatistic& performance) :
	statisticPerformance(&performance)
{
	statisticPerformance->start();
}

inline HighPerformanceStatistic::ScopedStatistic::~ScopedStatistic()
{
	release();
}

inline void HighPerformanceStatistic::ScopedStatistic::release()
{
	if (statisticPerformance)
	{
		statisticPerformance->stop();
		statisticPerformance = nullptr;
	}
}

inline void HighPerformanceTimer::start()
{
	ticksStart_ = ticks();
}

inline double HighPerformanceTimer::ticks2seconds(const Ticks ticks)
{
	ocean_assert(precision() != Ticks(0));
	return double(ticks) / double(precision());
}

inline double HighPerformanceStatistic::first() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return measurements_.front();
}

inline double HighPerformanceStatistic::firstMseconds() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return measurements_.front() * 1000.0;
}

inline double HighPerformanceStatistic::second() const
{
	if (measurements_.size() < 2)
	{
		return -1.0;
	}

	return measurements_[1];
}

inline double HighPerformanceStatistic::secondMseconds() const
{
	if (measurements_.size() < 2)
	{
		return -1.0;
	}

	return measurements_[1] * 1000.0;
}

inline double HighPerformanceStatistic::best() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return best_;
}

inline double HighPerformanceStatistic::bestMseconds() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return best_ * 1000.0;
}

inline double HighPerformanceStatistic::worst() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return worst_;
}

inline double HighPerformanceStatistic::worstMseconds() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return worst_ * 1000.0;
}

inline double HighPerformanceStatistic::last() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return measurements_.back();
}

inline double HighPerformanceStatistic::lastMseconds() const
{
	if (measurements_.empty())
	{
		return -1.0;
	}

	return measurements_.back() * 1000.0;
}

inline double HighPerformanceStatistic::averageMseconds() const
{
	return average() * 1000.0;
}

inline double HighPerformanceStatistic::averageCyclesPerOperation(const double operations, const double clockRate) const
{
	ocean_assert(operations > 0.0 && clockRate > 0.0);

	const double seconds = average();
	if (seconds <= 0)
	{
		return -1;
	}

	const double operationsPerSecond = operations / seconds;
	ocean_assert(operationsPerSecond > 0.0);

	const double cyclesPerOperation = clockRate / operationsPerSecond;

	return cyclesPerOperation;
}

inline double HighPerformanceStatistic::total() const
{
	return total_;
}

inline double HighPerformanceStatistic::totalMseconds() const
{
	return total() * 1000.0;
}

size_t HighPerformanceStatistic::measurements() const
{
	return measurements_.size();
}

inline double HighPerformanceStatistic::running() const
{
	return timer_.seconds();
}

inline double HighPerformanceStatistic::runningMseconds() const
{
	return running() * 1000.0;
}

inline bool HighPerformanceStatistic::isRunning() const
{
	return started_;
}

inline HighPerformanceStatistic::operator bool() const
{
	return !measurements_.empty();
}


inline std::ostream& operator<<(std::ostream& stream, const HighPerformanceStatistic& highPerformanceStatistic)
{
	stream << highPerformanceStatistic.toString();

	return stream;
}

template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const HighPerformanceStatistic& highPerformanceStatistic)
{
	messageObject << highPerformanceStatistic.toString();

	return messageObject;
}

template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const HighPerformanceStatistic& highPerformanceStatistic)
{
	messageObject << highPerformanceStatistic.toString();

	return messageObject;
}

inline HighPerformanceBenchmark::ScopedCategory::ScopedCategory(std::string name) :
	name_(std::move(name)),
	startTicks_(HighPerformanceTimer::ticks())
{
	// nothing to do here
}

inline HighPerformanceBenchmark::ScopedCategory::~ScopedCategory()
{
	release();
}

inline void HighPerformanceBenchmark::ScopedCategory::skip()
{
	name_.clear();
}

inline void HighPerformanceBenchmark::ScopedCategory::release()
{
	if (!name_.empty())
	{
		const HighPerformanceTimer::Ticks stopTicks = HighPerformanceTimer::ticks();
		ocean_assert(startTicks_ <= stopTicks);

		const HighPerformanceTimer::Ticks ticks = stopTicks - startTicks_;

		const double measurement = HighPerformanceTimer::ticks2seconds(ticks);
		HighPerformanceBenchmark::get().addMeasurement(name_, measurement);

		name_.clear();
	}
}

inline void HighPerformanceBenchmark::ScopedCategory::change(std::string name)
{
	if (name_ == name)
	{
		return;
	}

	release();

	if (!name.empty())
	{
		name_ = std::move(name);
		startTicks_ = HighPerformanceTimer::ticks();
	}
}

inline HighPerformanceBenchmark::HighPerformanceBenchmark() :
	isRunning_(false)
{
	// nothing to do here
}

inline HighPerformanceBenchmark::~HighPerformanceBenchmark()
{
	// nothing to do here
}

}

#endif // META_OCEAN_BASE_HIGH_PERFORMANCE_TIMER_H
