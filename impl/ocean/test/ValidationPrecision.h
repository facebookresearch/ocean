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

namespace Ocean
{

namespace Test
{

/**
 * This class implements a helper class to validate the accuracy of tests.
 * @see Validation.
 * @ingroup test
 */
class ValidationPrecision
{
	public:

		/**
		 * Creates a new scoped iteration.
		 * By default the iteration is accurate enough.<br>
		 * Call setInaccurate() for an iteration which is not precise enough.
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
		 */
		explicit inline ValidationPrecision(const double threshold);

		/**
		 * Creates a new validation object associated with a random generator, by default the verified has succeeded.
		 * @param threshold The necessary percent of accurate iterations necessary for a successful verification, with range (0, 1]
		 * @param randomGenerator The random generator which will be used during verification
		 */
		inline ValidationPrecision(const double threshold, RandomGenerator& randomGenerator);

		/**
		 * Destructs this validation object.
		 */
		inline ~ValidationPrecision();

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
		 * Explicitly sets the validation to be failed.
		 * Setting this validation to be failed will result in a failed validation even if all iteration were precise enough.
		 * @see succeeded().
		 */
		inline void setFailed();

		/**
		 * Explicitly sets the validation to be failed.
		 * Setting this validation to be failed will result in a failed validation even if all iteration were precise enough.
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 */
		inline void setFailed(const char* file, const int line);

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
		[[nodiscard]] inline bool succeeded() const;

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

		/**
		 * Returns a string containing the random generator's initial seed, if any
		 * @return The string with initial seed test, empty if no random generator is associated with this validation object
		 */
		inline std::string randomGeneratorOutput() const;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		ValidationPrecision(const ValidationPrecision&) = delete;

		/**
		 * Sets the succeeded state to false.
		 */
		inline void setSucceededFalse();

	protected:

		/// The necessary percent of accurate iterations necessary for a successful verification, with range (0, 1]
		double threshold_ = 1.0;

		/// True, if the validation has succeeded; False, if the validation has failed.
		bool succeeded_ = true;

		/// The number of iterations needed to determine success or failure.
		uint64_t necessaryIterations_ = 0ull;

		/// The overall number of iterations which have been added.
		uint64_t iterations_ = 0ull;

		/// The number of iterations which were precise enough, with range [0, iterations_]
		uint64_t accurateIterations_ = 0ull;

		/// Optional random generator object which will be used during validation.
		RandomGenerator* randomGenerator_ = nullptr;

#ifdef OCEAN_DEBUG
		/// True, if the success state of this validation has been checked.
		mutable bool succeededChecked_ = false;
#endif // OCEAN_DEBUG
};

#ifndef OCEAN_SET_INACCURATE
	#define OCEAN_SET_INACCURATE(scopedIteration, expected, actual) scopedIteration.setInaccurate(expected, actual, __FILE__, __LINE__);
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

inline ValidationPrecision::ValidationPrecision(const double threshold)
{
	ocean_assert(threshold > 0.0 && threshold <= 1.0);

	if (threshold > 0.0 && threshold <= 1.0)
	{
		threshold_ = threshold;

		const double failureRate = 1.0 - threshold_;
		ocean_assert(failureRate > 0.0);

		const double idealIterations = 1.0 / failureRate;

		ocean_assert(idealIterations <= 1000000000u);
		necessaryIterations_ = std::max(1u, (unsigned int)(std::ceil(idealIterations) * 2.0));
	}
	else
	{
		setSucceededFalse();
	}
}

inline ValidationPrecision::ValidationPrecision(const double threshold, RandomGenerator& randomGenerator) :
	ValidationPrecision(threshold)
{
	randomGenerator_ = &randomGenerator;
}

inline ValidationPrecision::~ValidationPrecision()
{
#ifdef OCEAN_DEBUG
	ocean_assert(succeededChecked_ && "The verifier has not been check for success");
#endif
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

inline void ValidationPrecision::setFailed()
{
	setSucceededFalse();
}

inline void ValidationPrecision::setFailed(const char* file, const int line)
{
	setSucceededFalse();

	ocean_assert(file != nullptr);
	if (file != nullptr)
	{
#ifdef OCEAN_USE_GTEST
		std::cerr << "\nValidationPrecision::setFailed() in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
#else
		Log::error() << "ValidationPrecision::setFailed() in '" << file << "', in line " << line << randomGeneratorOutput();
#endif
	}
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

inline std::string ValidationPrecision::randomGeneratorOutput() const
{
	if (randomGenerator_ != nullptr)
	{
		return ", with random generator initial seed '" + String::toAString(randomGenerator_->initialSeed()) + "'";
	}

	return std::string();
}

inline void ValidationPrecision::setSucceededFalse()
{
	succeeded_ = false;
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
