/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TEST_DATA_H
#define META_OCEAN_TEST_TEST_DATA_H

#include "ocean/test/Test.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Value.h"

namespace Ocean
{

namespace Test
{

// Forward declaration.
class TestData;

/**
 * Definition of a shared pointer holding a TestData object.
 * @ingroup test
 */
using SharedTestData = std::shared_ptr<TestData>;

/**
 * This class holds the test data necessary for one test iteration combining input data and potential expected test results.
 * @ingroup test
 */
class OCEAN_TEST_EXPORT TestData
{
	public:

		/**
		 * Definition of individual data types.
		 */
		enum DataType : uint32_t
		{
			/// Invalid data type
			DT_INVALID = 0u,
			/// The data is a value.
			DT_VALUE,
			/// The data is an image.
			DT_IMAGE
		};

	public:

		/**
		 * Default constructor.
		 */
		TestData() = default;

		/**
		 * Creates a new test data object holding a value.
		 * @param value The value of the new object, must be valid
		 * @param expectation The optional test expectation for the new object
		 */
		explicit TestData(Value&& value, Value&& expectation = Value());

		/**
		 * Creates a new test data object holding an image.
		 * @param image The image of the new object, must be valid
		 * @param expectation The optional test expectation for the new object
		 */
		explicit TestData(Frame&& image, Value&& expectation);

		/**
		 * Returns the data type of this test data object.
		 * @return The object's data type
		 */
		inline DataType dataType() const;

		/**
		 * Returns the value of this object.
		 * Ensure that `dataType() == DT_VALUE` before calling this function.
		 * @return The object's value
		 */
		inline const Value& value() const;

		/**
		 * Returns the image of this object.
		 * Ensure that `dataType() == DT_IMAGE` before calling this function.
		 * @return The object's image
		 */
		inline const Frame& image() const;

		/**
		 * Returns the potential expectation of this test object.
		 * @return The object's expectation, invalid if no expectation is defined
		 */
		inline const Value& expectation() const;

		/**
		 * Returns whether this object is valid and holds valid test data.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this object is valid and holds valid test data.
		 * @return True, if so
		 */
		inline explicit operator bool() const;

	protected:

		/// The data type of this test object.
		DataType dataType_ = DT_INVALID;

		/// The object's value, invalid if not defined.
		Value value_;

		//// The object's image, invalid if not defined.
		Frame image_;

		/// The object's expectation, invalid if not defined.
		Value expectation_;
};

inline TestData::DataType TestData::dataType() const
{
	return dataType_;
}

inline const Value& TestData::value() const
{
	ocean_assert(dataType_ == DT_VALUE);

	return value_;
}

inline const Frame& TestData::image() const
{
	ocean_assert(dataType_ == DT_IMAGE);

	return image_;
}

inline const Value& TestData::expectation() const
{
	ocean_assert(dataType_ != DT_INVALID);

	return expectation_;
}

inline bool TestData::isValid() const
{
	return dataType_ != DT_INVALID;
}

inline TestData::operator bool() const
{
	return isValid();
}

}

}

#endif // META_OCEAN_TEST_TEST_DATA_H
