/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_COLOR_CHANNEL_CURVE_H
#define META_OCEAN_CV_ADVANCED_COLOR_CHANNEL_CURVE_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Callback.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * Implements a look-up table to perform fast transformations on a single color channel.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT ColorChannelCurve
{
	public:

		/**
		 * Callback function type that returns a floating point value for the specified unsigned char input color value.
		 * Callback functions of this type can be used to define a color channel curve like gamma correction.
		 */
		typedef Callback<Scalar, unsigned char> TransformationFunction;

		/**
		 * Represents a specific color channel transformation
		 */
		enum TransformationType
		{
			/// Returns input unmodified
			TT_IDENTITY,
			/// Delinearization of SRGB values.
			TT_LINEAR_SRGB
		};

	public:

		/**
		 * Creates a new color channel curve using the specified transformation function preset.
		 * @param type Specifies the type of inbuilt transformation function to use
		 */
		ColorChannelCurve(const TransformationType type);

		/**
		 * Creates a new color channel curve using the specified transformation function.
		 * @param transformFunction Transformation function to use
		 */
		inline ColorChannelCurve(const TransformationFunction& transformFunction);

		/**
		 * Linearizes a given component of an sRGB triplet.
		 * @param value Red, green or blue component of an sRGB triplet in the interval [0;255]
		 * @return Linearized value in the interval [0;1]
		 */
		static inline Scalar linearizeSRGB(const unsigned char value);

		/**
		 * Linearizes a given component of an sRGB triplet.
		 * @param value Red, green or blue component of an sRGB triplet in the interval [0;1]
		 * @return Linearized value in the interval [0;1]
		 */
		static inline Scalar linearizeSRGB(const Scalar value);

		/**
		 * Returns the input values unmodified.
		 * @param value Red, green or blue component of an sRGB triplet in the interval [0;1]
		 * @return unmodified value in the interval [0;1]
		 */
		static inline Scalar identity(const unsigned char value);

		/**
		 * Delinearizes a given component of an sRGB triplet.
		 * @param value Red, green or blue component of an sRGB triplet, with range [0, 1]
		 * @return Delinearized value, with range [0, 255]
		 */
		static unsigned char inline delinearizeSRGB(const Scalar value);

		/**
		 * Transform a byte-sized color component using the transformation function implemented as look-up table.
		 * @param value Input value to be transformed
		 * @return Transformed output value
		 */
		inline Scalar operator()(const unsigned char value) const;

	protected:

		/**
		 * Sets the look-up-table using the specified transformation function.
		 * @param transformFunction Transformation function to use
		 */
		void setTransformation(const TransformationFunction& transformFunction);

	protected:

		/// Maps byte-sized input color values between 0 and 255 to the specified output value.
		Scalar transformData[256];
};

inline ColorChannelCurve::ColorChannelCurve(const TransformationFunction& transformFunction)
{
	setTransformation(transformFunction);
}

inline Scalar ColorChannelCurve::operator()(const unsigned char value) const
{
	return transformData[value];
}

inline unsigned char ColorChannelCurve::delinearizeSRGB(const Scalar value)
{
	ocean_assert(value >= 0 && value <= 1);

	if (value <= Scalar(0.00304))
	{
		const Scalar c = value * Scalar(12.92);

		ocean_assert(c >= 0 && c <= 1);
		return (unsigned char)(c * Scalar(255) + Scalar(0.5));
	}
	else
	{
		const Scalar c = Numeric::pow(value, Scalar(0.4166666666666667)) * Scalar(1.055) - Scalar(0.055); // 0.41666... = 1/2.4

		ocean_assert(c >= 0 && c <= 1);
		return (unsigned char)(c * Scalar(255) + Scalar(0.5));
	}
}

inline Scalar ColorChannelCurve::identity(const unsigned char value)
{
	return value * Scalar(0.00392156862745098);
}

inline Scalar ColorChannelCurve::linearizeSRGB(const unsigned char value)
{
	const Scalar c = value * Scalar(0.00392156862745098); // 0.00392... = 1/255.0

	if (c <= Scalar(0.03928))
		return c * Scalar(0.07739938080495357); // 0.07739... = 1/12.92
	else
		return Numeric::pow((c + Scalar(0.055)) * Scalar(0.9478672985781991), Scalar(2.4)); // 0.94786 = 1/1.055
}

inline Scalar ColorChannelCurve::linearizeSRGB(const Scalar value)
{
	if (value <= Scalar(0.03928))
		return value * Scalar(0.07739938080495357); // 0.07739... = 1/12.92
	else
		return Numeric::pow((value + Scalar(0.055)) * Scalar(0.9478672985781991), Scalar(2.4)); // 0.94786 = 1/1.055
}

}

}

}

#endif  // OCEAN_CV_ADVANCED_COLOR_CHANNEL_CURVE_H
