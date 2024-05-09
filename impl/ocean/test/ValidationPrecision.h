// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
				~ScopedIteration();

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
				ValidationPrecision& ValidationPrecision_;
		};

	public:

		/**
		 * Default constructor, by default the verified has succeeded.
		 */
		ValidationPrecision() = default;

		/**
		 * Creates a new validation object associated with a random generator, by default the verified has succeeded.
		 * @param randomGenerator The random generator which will be used during verification
		 */
		explicit inline ValidationPrecision(RandomGenerator& randomGenerator);

		/**
		 * Destructs this validation object.
		 */
		inline ~ValidationPrecision();

		/**
		 * Explicitly adds a new iteration which is either accurate or not.
		 * @param accurate True, if the iteration was precise enough; False, if the iteration was not precise enough
		 * @see ScopedIteration.
		 */
		inline void addIteration(const bool accureate);

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
		 * Returns if this validation has succeeded.
		 * @param threshold The necessary percent of accurate iterations necessary for a successful verification, with range (0, 1]
		 * @return True, if so; False, if the validation has failed
		 */
		[[nodiscard]] inline bool succeeded(const double threshold) const;

		/**
		 * Returns the accuracy of all iterations.
		 * Beware: Ensure that at least one iteration has been added.
		 * @return The validation's accuracy, in percent, with range [0, 1]
		 */
		[[nodiscard]] inline double accuracy() const;

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
		 * Sets the succeeded state to false.
		 */
		inline void setSucceededFalse();

	protected:

		/// True, if the validation has succeeded; False, if the validation has failed.
		bool succeeded_ = true;

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

inline ValidationPrecision::ScopedIteration::ScopedIteration(ValidationPrecision& ValidationPrecision) :
	ValidationPrecision_(ValidationPrecision)
{
	// nothing to do here
}

ValidationPrecision::ScopedIteration::~ScopedIteration()
{
	ValidationPrecision_.addIteration(accurate_);
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
		Log::error() << "ScopedIteration::setInaccurate() in '" << file << "', in line " << line << ": expected " << expected << ", got " << actual;
	}
}


inline ValidationPrecision::ValidationPrecision(RandomGenerator& randomGenerator) :
	randomGenerator_(&randomGenerator)
{
	// nothing to do here
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
		Log::error() << "Verifier::setFailed() in '" << file << "', in line " << line;
	}
}

inline uint64_t ValidationPrecision::iterations() const
{
	return iterations_;
}

inline bool ValidationPrecision::succeeded(const double threshold) const
{
#ifdef OCEAN_DEBUG
	succeededChecked_ = true;
#endif

	ocean_assert(threshold > 0.0 && threshold <= 1.0);

	if (threshold <= 0.0 || threshold > 1.0)
	{
		return false;
	}

	ocean_assert(iterations_ != 0ull);

	if (iterations_ == 0u)
	{
		return false;
	}

	ocean_assert(accurateIterations_ <= iterations_);

	const double percent = double(accurateIterations_) / double(iterations_);

	if (percent < threshold)
	{
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
		stream << "FAILED!";
	}
	else
	{
		stream << String::toAString(validation.accuracy() * 100.0, 1u) << "% succeeded.";
	}

	return stream;
}

template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const ValidationPrecision& validation)
{
	if (validation.hasSetFailed())
	{
		messageObject << "FAILED!";
	}
	else
	{
		messageObject << String::toAString(validation.accuracy() * 100.0, 1u) << "% succeeded.";
	}

	return messageObject;
}

template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const ValidationPrecision& validation)
{
	if (validation.hasSetFailed())
	{
		messageObject << "FAILED!";
	}
	else
	{
		messageObject << String::toAString(validation.accuracy() * 100.0, 1u) << "% succeeded.";
	}

	return messageObject;
}

}

}

#endif // META_OCEAN_TEST_VALIDATION_PRECISION_H
