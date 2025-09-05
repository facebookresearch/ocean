/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_METRIC_SIZE_H
#define META_OCEAN_CV_CALIBRATION_METRIC_SIZE_H

#include "ocean/cv/calibration/Calibration.h"

#include "ocean/base/String.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements an object holding a metric size like width, height, length, or thickness.
 * Internally, the size is stored in millimeter but can be converted to and from any other unit.
 * @ingroup cvcalibration
 */
class MetricSize
{
	public:

		/**
		 * Definition of different unit types.
		 */
		enum UnitType : uint32_t
		{
			/// An invalid unit type.
			UT_INVALID = 0u,
			/// Millimeter as unit.
			UT_MILLIMETER,
			/// Inch as unit.
			UT_INCH // 1 inch == 25.4 mm
		};

		/**
		 * Definition of different paper types.
		 */
		enum PaperType : uint32_t
		{
			/// An invalid paper type.
			PT_INVALID = 0u,
			/// The paper is a DIN A3.
			PT_DIN_A3,
			/// The paper is a DIN A4.
			PT_DIN_A4,
			/// The paper is a letter.
			PT_LETTER,
			/// The paper is a tabloid aka ledger.
			PT_TABLOID
		};

	public:

		/**
		 * Creates a new object creating an invalid metric size.
		 */
		MetricSize() = default;

		/**
		 * Creates a new metric size object.
		 * @param value The size value, with range (-infinity, infinity)
		 * @param unitType The unit type of the given value, must be valid
		 */
		MetricSize(const double value, const UnitType unitType);

		/**
		 * Returns the size value in a specific unit.
		 * @param unitType The unit type in which the value will be returned, must be valid
		 * @return The size value in the specified unit
		 */
		double value(const UnitType unitType) const;

		/**
		 * Returns the size value in a specific unit as string.
		 * @param unitType The unit type in which the value will be returned, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @return The size value in the specified unit as string
		 */
		inline std::string value(const UnitType unitType, const unsigned int precision) const;

		/**
		 * Returns whether the size value is zero.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether this size object holds a valid value.
		 * @return True, if so
		*/
		inline bool isValid() const;

		/**
		 * Adds two metric size objects.
		 * @param value The second value to add
		 * @return The resulting sum of both sizes
		 */
		inline MetricSize operator+(const MetricSize& value) const;

		/**
		 * Adds and assigns two metric size objects.
		 * @param value The second value to add
		 * @return The resulting sum of both sizes
		 */
		inline MetricSize& operator+=(const MetricSize& value);

		/**
		 * Subtracts two metric size objects.
		 * @param value The second value to subtract
		 * @return The resulting difference of both sizes
		 */
		inline MetricSize operator-(const MetricSize& value) const;

		/**
		 * Subtracts and assigns two metric size objects.
		 * @param value The second value to subtract
		 * @return The resulting difference of both sizes
		 */
		inline MetricSize& operator-=(const MetricSize& value);

		/**
		 * Multiplies this metric size object with a scalar factor.
		 * @param value The multiplication factor, with range (-infinity, infinity)
		 * @return The resulting product
		 */
		inline MetricSize operator*(const double factor) const;

		/**
		 * Multiplies and assigns this metric object with a scalar factor.
		 * @param value The multiplication factor, with range (-infinity, infinity)
		 * @return The resulting product
		 */
		inline MetricSize& operator*=(const double factor);

		/**
		 * Divides this metric size object by a scalar factor.
		 * @param factor The division factor, must not be zero
		 * @return The resulting quotient
		 */
		inline MetricSize operator/(const double factor) const;

		/**
		 * Divides and assigns this metric size object by a scalar factor.
		 * @param factor The division factor, must not be zero
		 * @return The resulting quotient
		 */
		inline MetricSize& operator/=(const double factor);

		/**
		 * Divides two metric size objects and returns the ratio.
		 * @param value The second value to divide, must not be zero
		 * @return The resulting ratio as scalar value
		 */
		inline double operator/(const MetricSize& value) const;

		/**
		 * Returns whether this metric size object is smaller than a second one.
		 * @param value The second metric size object to compare
		 * @return True, if so
		 */
		inline bool operator<(const MetricSize& value) const;

		/**
		 * Returns whether this metric size object is smaller or equal than a second one.
		 * @param value The second metric size object to compare
		 * @return True, if so
		 */
		inline bool operator<=(const MetricSize& value) const;

		/**
		 * Returns whether this metric size object is greater than a second one.
		 * @param value The second metric size object to compare
		 * @return True, if so
		 */
		inline bool operator>(const MetricSize& value) const;

		/**
		 * Returns whether this metric size object is greater or equal than a second one.
		 * @param value The second metric size object to compare
		 * @return True, if so
		 */
		inline bool operator>=(const MetricSize& value) const;

		/**
		 * Returns whether two metric size objects hold the same size values.
		 * @param value The second metric size object to compare
		 * @return True, if so
		 */
		inline bool operator==(const MetricSize& value) const;

		/**
		 * Returns whether two metric size objects do not hold the same size values.
		 * @param value The second metric size object to compare
		 * @return True, if so
		 */
		inline bool operator!=(const MetricSize& value) const;

		/**
		 * Returns the metric width and height of a paper.
		 * @param paperType The type of the paper, must be valid
		 * @return True, if succeeded
		 */
		static bool determinePaperSize(const PaperType paperType, MetricSize& width, MetricSize& height);

		/**
		 * Returns the string of a unit type.
		 * @param unitType The unit type to return as string
		 * @return The unit type as string, 'Invalid' if invalid or unknown
		 */
		static std::string translateUnitType(const UnitType unitType);

		/**
		 * Translates the string of a paper type to a paper type value.
		 * @param paperType The string of the paper type
		 * @return The resulting paper type, PT_INVALID if invalid or unknown
		 */
		static PaperType translatePaperType(const std::string& paperType);

		/**
		 * Translates the paper type value to a readable string.
		 * @param paperType The paper type to translate
		 * @return The readable paper type as string, 'invalid' if invalid or unknown
		 */
		static std::string translatePaperType(const PaperType paperType);

	protected:

		/**
		 * Converts the size value from one unit to another unit.
		 * @param sourceValue The source value in the specified source unit to convert, with range (-infinity, infinity)
		 * @return The resulting value in the specified target unit, with range (-infinity, infinity)
		 * @tparam TSourceUnit The source unit in which the source value is defined
		 * @tparam TTargetUnit The target unit in which the target value will be returned
		 */
		template <UnitType TSourceUnit, UnitType TTargetUnit>
		static double convert(const double sourceValue);

		/**
		 * Converts the size value from one unit to another unit.
		 * @param sourceValue The source value in the specified source unit to convert, with range (-infinity, infinity)
		 * @param targetUnit The unit in which the target value will be returned, must be valid
		 * @return The resulting value in the specified target unit, with range (-infinity, infinity)
		 * @tparam TSourceUnit The source unit in which the source value is defined
		 */
		template <UnitType TSourceUnit>
		static double convert(const double sourceValue, const UnitType targetUnit);

		/**
		 * Converts the metric size defined in one unit to another unit.
		 * @param sourceUnit The unit in which the source value is defined, must be valid
		 * @param sourceValue The source value in the specified unit, with range (-infinity, infinity)
		 * @param targetUnit The unit in which the target value will be returned, must be valid
		 * @return The converted value in the specified target unit, with range (-infinity, infinity)
		 */
		static double convert(const UnitType sourceUnit, const double sourceValue, const UnitType targetUnit);

	protected:

		/// The metric size in millimeter, with range (-infinity, infinity).
		double valueMillimeter_ = NumericD::minValue();
};

inline std::string MetricSize::value(const UnitType unitType, const unsigned int precision) const
{
	return "\"" + String::toAString(value(unitType), precision) + translateUnitType(unitType) + "\"";
}

inline bool MetricSize::isNull() const
{
	return Numeric::isEqualEps(valueMillimeter_);
}

inline bool MetricSize::isValid() const
{
	return valueMillimeter_ != NumericD::minValue();
}

inline MetricSize MetricSize::operator+(const MetricSize& value) const
{
	return MetricSize(valueMillimeter_ + value.valueMillimeter_, UT_MILLIMETER);
}

inline MetricSize& MetricSize::operator+=(const MetricSize& value)
{
	valueMillimeter_ += value.valueMillimeter_;

	return *this;
}

inline MetricSize MetricSize::operator-(const MetricSize& value) const
{
	return MetricSize(valueMillimeter_ - value.valueMillimeter_, UT_MILLIMETER);
}

inline MetricSize& MetricSize::operator-=(const MetricSize& value)
{
	valueMillimeter_ -= value.valueMillimeter_;

	return *this;
}

inline MetricSize MetricSize::operator*(const double factor) const
{
	return MetricSize(valueMillimeter_ * factor, UT_MILLIMETER);
}

inline MetricSize& MetricSize::operator*=(const double factor)
{
	valueMillimeter_ *= factor;

	return *this;
}

inline MetricSize MetricSize::operator/(const double factor) const
{
	ocean_assert(NumericD::isNotEqualEps(factor));

	return MetricSize(valueMillimeter_ / factor, UT_MILLIMETER);
}

inline MetricSize& MetricSize::operator/=(const double factor)
{
	ocean_assert(NumericD::isNotEqualEps(factor));

	valueMillimeter_ /= factor;

	return *this;
}

inline double MetricSize::operator/(const MetricSize& value) const
{
	ocean_assert(value.isValid());

	ocean_assert(NumericD::isNotEqualEps(value.valueMillimeter_));

	return valueMillimeter_ / value.valueMillimeter_;
}

inline bool MetricSize::operator<(const MetricSize& value) const
{
	return valueMillimeter_ < value.valueMillimeter_;
}

inline bool MetricSize::operator<=(const MetricSize& value) const
{
	return valueMillimeter_ <= value.valueMillimeter_;
}

inline bool MetricSize::operator>(const MetricSize& value) const
{
	return valueMillimeter_ > value.valueMillimeter_;
}

inline bool MetricSize::operator>=(const MetricSize& value) const
{
	return valueMillimeter_ >= value.valueMillimeter_;
}

inline bool MetricSize::operator==(const MetricSize& value) const
{
	return NumericD::isEqual(valueMillimeter_, value.valueMillimeter_);
}

inline bool MetricSize::operator!=(const MetricSize& value) const
{
	return NumericD::isNotEqual(valueMillimeter_, value.valueMillimeter_);
}

template <>
inline double MetricSize::convert<MetricSize::UT_MILLIMETER, MetricSize::UT_INCH>(const double sourceValue)
{
	return sourceValue / 25.4;
}

template <>
inline double MetricSize::convert<MetricSize::UT_INCH, MetricSize::UT_MILLIMETER>(const double sourceValue)
{
	return sourceValue * 25.4;
}

template <MetricSize::UnitType TSourceUnit, MetricSize::UnitType TTargetUnit>
inline double MetricSize::convert(const double sourceValue)
{
	static_assert(TSourceUnit == TTargetUnit, "Missing specialization for either TSourceUnit or TTargetUnit");
	static_assert(TSourceUnit != UT_INVALID && TTargetUnit != UT_INVALID, "Invalid unit type!");

	return sourceValue;
}

template <>
inline double MetricSize::convert<MetricSize::UT_MILLIMETER>(const double sourceValue, const UnitType targetUnit)
{
	ocean_assert(targetUnit != UT_INVALID);

	switch (targetUnit)
	{
		case UT_INVALID:
			break;

		case UT_MILLIMETER:
			return convert<UT_MILLIMETER, UT_MILLIMETER>(sourceValue);

		case UT_INCH:
			return convert<UT_MILLIMETER, UT_INCH>(sourceValue);
	}

	ocean_assert(false && "Invalid unit type!");
	return sourceValue;
}

template <>
inline double MetricSize::convert<MetricSize::UT_INCH>(const double sourceValue, const UnitType targetUnit)
{
	ocean_assert(targetUnit != UT_INVALID);

	switch (targetUnit)
	{
		case UT_INVALID:
			break;

		case UT_MILLIMETER:
			return convert<UT_INCH, UT_MILLIMETER>(sourceValue);

		case UT_INCH:
			return convert<UT_INCH, UT_INCH>(sourceValue);
	}

	ocean_assert(false && "Invalid unit type!");
	return sourceValue;
}

template <MetricSize::UnitType TSourceUnit>
inline  double MetricSize::convert(const double sourceValue, const UnitType targetUnit)
{
	static_assert(oceanFalse<TSourceUnit>(), "Missing specialization");

	return sourceValue;
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_METRIC_SIZE_H
