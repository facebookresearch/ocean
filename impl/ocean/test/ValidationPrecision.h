/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_VALIDATION_PRECISION_H
#define META_OCEAN_TEST_VALIDATION_PRECISION_H

#include "ocean/test/Test.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomGenerator.h"

#include <cmath>

namespace Ocean
{

namespace Test
{

/**
 * This class implements a helper class to validate the precision of algorithms or objects across multiple iterations.
 * The main purpose is to allow a configurable percentage of iterations to be imprecise while still considering the overall test as successful.
 * This class inherits from Validation and extends it with precision-based validation capabilities.
 * As this class inherits from Validation, the entire validation can also be invalidated using base class methods (e.g., OCEAN_EXPECT_TRUE, OCEAN_SET_FAILED) for errors unrelated to precision.
 * @see Validation, ScopedIteration.
 * @ingroup test
 */
class ValidationPrecision : public Validation
{
	public:

		/**
		 * Creates a new scoped iteration representing one iteration in a precision test.
		 * By default each iteration is considered accurate; call setInaccurate() if the iteration is not precise enough.
		 */
		class ScopedIteration
		{
			public:

				/**
				 * Creates a new scoped iteration associated with a validation object.
				 */
				explicit inline ScopedIteration(ValidationPrecision& ValidationPrecision);

				/**
				 * Destructs this scoped object and forwards the information to the owning validation.
				 */
				inline ~ScopedIteration();

				/**
				 * Sets this iteration to be not precise enough.
				 */
				inline void setInaccurate();

				/**
				 * Sets this iteration to be not precise enough.
				 * This function will also write a message to the error log.
				 * @param expected The expected value
				 * @param actual The actual value
				 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
				 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
				 * @tparam T The data type of the expected and actual value
				 */
				template <typename T>
				inline void setInaccurate(const T& expected, const T& actual, const char* file, const int line);

			protected:

				/**
				 * Disabled default constructor.
				 */
				ScopedIteration() = delete;

				/**
				 * Disabled copy constructor.
				 */
				ScopedIteration(const ScopedIteration&) = delete;

				/**
				 * Disabled move constructor.
				 */
				ScopedIteration(const ScopedIteration&&) = delete;

			protected:

				/// True, if the iteration is accurate; False, if the iteration was not accurate enough.
				bool accurate_ = true;

				/// The owner of this scoped object.
				ValidationPrecision& validationPrecision_;
		};

	public:

		/**
		 * Creates a new precision-based validation object with specified threshold.
		 * @param threshold The necessary percent of accurate iterations necessary for a successful verification, with range (0, 1]
		 * @param minimumIterations The minimum number of iterations necessary for a successful verification, with range [1, infinity)
		 */
		explicit inline ValidationPrecision(const double threshold, const unsigned int minimumIterations = 1u);

		/**
		 * Creates a new validation object associated with a random generator, by default the verified has succeeded.
		 * @param threshold The necessary percent of accurate iterations necessary for a successful verification, with range (0, 1]
		 * @param randomGenerator The random generator which will be used during verification
		 */
		inline ValidationPrecision(const double threshold, RandomGenerator& randomGenerator, const unsigned int minimumIterations = 1u);

		/**
		 * Explicitly adds a new iteration which is either accurate or not.
		 * @param accurate True, if the iteration was precise enough; False, if the iteration was not precise enough
		 * @see ScopedIteration.
		 */
		inline void addIteration(const bool accurate);

		/**
		 * Explicitly adds new iterations for which the amount of accurate iterations is known.
		 * @param accurateIterations The number of accurate iterations, with range [0, iterations]
		 * @param iterations The number of new iterations, with range [1, infinity)
		 * @see ScopedIteration.
		 */
		inline void addIterations(const size_t accurateIterations, const size_t iterations);

		/**
		 * Returns the number of iterations in which the precision has been determined.
		 * @return The number of iterations, with range [0, infinity)
		 */
		inline uint64_t iterations() const;

		/**
		 * Returns the necessary iterations to allow determining success or failure based on the specified success threshold.
		 * @return The number of necessary iterations, with range [1, infinity)
		 */
		inline uint64_t necessaryIterations() const;

		/**
		 * Returns whether the number of iterations is not yet sufficient to determine a success or failure.
		 * @return True, if more iterations are required
		 */
		inline bool needMoreIterations() const;

		/**
		 * Returns if this validation has succeeded.
		 * @return True, if so; False, if the validation has failed
		 */
		[[nodiscard]] inline bool succeeded() const override;

		/**
		 * Returns the accuracy of all iterations.
		 * Beware: Ensure that at least one iteration has been added.
		 * @return The validation's accuracy, in percent, with range [0, 1]
		 */
		[[nodiscard]] inline double accuracy() const;

		/**
		 * Returns the defined threshold.
		 * @return The validation object's precision threshold, with range [0, 1]
		 */
		inline double threshold() const;

		/**
		 * Returns whether this validation object has been set to failed explicitly.
		 * This function is intended for internal use only, use succeeded() instead.
		 * @return True, if so
		 * @see succeeded().
		 */
		[[nodiscard]] inline bool hasSetFailed() const;

	protected:

		/// The necessary percent of accurate iterations necessary for a successful verification, with range (0, 1]
		double threshold_ = 1.0;

		/// The number of iterations needed to determine success or failure.
		uint64_t necessaryIterations_ = 0ull;

		/// The overall number of iterations which have been added.
		uint64_t iterations_ = 0ull;

		/// The number of iterations which were precise enough, with range [0, iterations_]
		uint64_t accurateIterations_ = 0ull;
};

#ifndef OCEAN_SET_INACCURATE
	#define OCEAN_SET_INACCURATE(scopedIteration, ...) scopedIteration.setInaccurate(__VA_ARGS__, __FILE__, __LINE__);
#endif

inline ValidationPrecision::ScopedIteration::ScopedIteration(ValidationPrecision& ValidationPrecision) :
	validationPrecision_(ValidationPrecision)
{
	// nothing to do here
}

inline ValidationPrecision::ScopedIteration::~ScopedIteration()
{
	validationPrecision_.addIteration(accurate_);
}

inline void ValidationPrecision::ScopedIteration::setInaccurate()
{
	accurate_ = false;
}

template <typename T>
inline void ValidationPrecision::ScopedIteration::setInaccurate(const T& expected, const T& actual, const char* file, const int line)
{
	accurate_ = false;

	ocean_assert(file != nullptr);
	if (file != nullptr)
	{
		Log::error() << "ScopedIteration::setInaccurate() in '" << file << "', in line " << line << ": expected " << expected << ", got " << actual << validationPrecision_.randomGeneratorOutput();
	}
}

inline ValidationPrecision::ValidationPrecision(const double threshold, const unsigned int minimumIterations)
{
	ocean_assert(threshold > 0.0 && threshold <= 1.0);

	if (threshold > 0.0 && threshold <= 1.0)
	{
		threshold_ = threshold;

		const double failureRate = 1.0 - threshold_;
		ocean_assert(failureRate > 0.0);

		const double idealIterations = 1.0 / failureRate;

		ocean_assert(idealIterations <= 1000000000u);
		necessaryIterations_ = std::max(minimumIterations, (unsigned int)(std::ceil(idealIterations) * 2.0));
	}
	else
	{
		setSucceededFalse();
	}
}

inline ValidationPrecision::ValidationPrecision(const double threshold, RandomGenerator& randomGenerator, const unsigned int minimumIterations) :
	Validation(randomGenerator)
{
	ocean_assert(threshold > 0.0 && threshold <= 1.0);

	if (threshold > 0.0 && threshold <= 1.0)
	{
		threshold_ = threshold;

		const double failureRate = 1.0 - threshold_;
		ocean_assert(failureRate > 0.0);

		const double idealIterations = 1.0 / failureRate;

		ocean_assert(idealIterations <= 1000000000u);
		necessaryIterations_ = std::max(minimumIterations, (unsigned int)(std::ceil(idealIterations) * 2.0));
	}
	else
	{
		setSucceededFalse();
	}
}

inline void ValidationPrecision::addIteration(const bool accurate)
{
	++iterations_;

	if (accurate)
	{
		++accurateIterations_;
	}
}

inline void ValidationPrecision::addIterations(const size_t accurateIterations, const size_t iterations)
{
	ocean_assert(accurateIterations <= iterations);
	ocean_assert(iterations >= 1);

	if (accurateIterations > iterations)
	{
		setSucceededFalse();
	}

	accurateIterations_ += accurateIterations;
	iterations_ += iterations;
}

inline uint64_t ValidationPrecision::iterations() const
{
	return iterations_;
}

inline uint64_t ValidationPrecision::necessaryIterations() const
{
	ocean_assert(necessaryIterations_ >= 1ull);

	return necessaryIterations_;
}

inline bool ValidationPrecision::needMoreIterations() const
{
	ocean_assert(necessaryIterations_ >= 1ull);

	return iterations_ < necessaryIterations_;
}

inline bool ValidationPrecision::succeeded() const
{
#ifdef OCEAN_DEBUG
	succeededChecked_ = true;
#endif

	ocean_assert(threshold_ > 0.0 && threshold_ <= 1.0);

	if (threshold_ <= 0.0 || threshold_ > 1.0)
	{
		return false;
	}

	const double percent = accuracy();

	if (percent < threshold_)
	{
#ifdef OCEAN_USE_GTEST

		std::ostream& stream = std::cerr << "\nFAILED with only " << String::toAString(accuracy() * 100.0, 1u) << "%, threshold is " << String::toAString(threshold() * 100.0, 1u) << "%" << randomGeneratorOutput() << "\n";

		if (needMoreIterations())
		{
			stream << "Not enough iterations for the specified success threshold (executed " << iterations_ << " of " << necessaryIterations_ << " necessary iterations)\n";
		}

		stream << std::endl;
#endif
		return false;
	}

	return succeeded_;
}

[[nodiscard]] inline double ValidationPrecision::accuracy() const
{
	ocean_assert(iterations_ != 0ull);

	if (iterations_ == 0u)
	{
		return -1.0;
	}

	ocean_assert(accurateIterations_ <= iterations_);

	const double percent = double(accurateIterations_) / double(iterations_);
	ocean_assert(percent >= 0.0 && percent <= 1.0);

	return percent;
}

inline double ValidationPrecision::threshold() const
{
	return threshold_;
}

[[nodiscard]] inline bool ValidationPrecision::hasSetFailed() const
{
	return succeeded_ == false;
}

inline std::ostream& operator<<(std::ostream& stream, const ValidationPrecision& validation)
{
	if (validation.hasSetFailed())
	{
		stream << "FAILED!" << validation.randomGeneratorOutput();
	}
	else
	{
		if (validation.accuracy() < validation.threshold())
		{
			stream << "FAILED with only " << String::toAString(validation.accuracy() * 100.0, 1u) << "%, threshold is " << String::toAString(validation.threshold() * 100.0, 1u) << "%" << validation.randomGeneratorOutput();
		}
		else
		{
			stream << String::toAString(validation.accuracy() * 100.0, 1u) << "% succeeded.";
		}
	}

	return stream;
}

template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const ValidationPrecision& validation)
{
	if (validation.hasSetFailed())
	{
		messageObject << "FAILED!" << validation.randomGeneratorOutput();
	}
	else
	{
		if (validation.accuracy() < validation.threshold())
		{
			messageObject << "FAILED with only " << String::toAString(validation.accuracy() * 100.0, 1u) << "%, threshold is " << String::toAString(validation.threshold() * 100.0, 1u) << "%" << validation.randomGeneratorOutput();
		}
		else
		{
			messageObject << String::toAString(validation.accuracy() * 100.0, 1u) << "% succeeded.";
		}
	}

	return messageObject;
}

template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const ValidationPrecision& validation)
{
	if (validation.hasSetFailed())
	{
		messageObject << "FAILED!" << validation.randomGeneratorOutput();
	}
	else
	{
		if (validation.accuracy() < validation.threshold())
		{
			messageObject << "FAILED with only " << String::toAString(validation.accuracy() * 100.0, 1u) << "%, threshold is " << String::toAString(validation.threshold() * 100.0, 1u) << "%" << validation.randomGeneratorOutput();
		}
		else
		{
			messageObject << String::toAString(validation.accuracy() * 100.0, 1u) << "% succeeded.";
		}
	}

	return messageObject;
}

}

}

#endif // META_OCEAN_TEST_VALIDATION_PRECISION_H
