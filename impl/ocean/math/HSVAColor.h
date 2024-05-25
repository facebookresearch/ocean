/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_HSVA_COLOR_H
#define META_OCEAN_MATH_HSVA_COLOR_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"

namespace Ocean
{

// Forward declaration.
class HSVAColor;

// Forward declaration.
class RGBAColor;

/**
 * Definition of a vector holding rgba color objects.
 * @ingroup math
 */
typedef std::vector<HSVAColor> HSVAColors;

/**
 * This class implements a color defined by hue, saturation, value and alpha parameters.
 * The color space is cone with hue as angle, saturation as radius and value as height,
 * the alpha parameter is the fourth dimension.<br>
 * Hue must be in the range [0, 2Pi).<br>
 * Saturation, value, and alpha must be in the range [0.0, 1.0].<br>
 * An alpha value 1 means fully opaque, 0 means fully transparent. Therefore the alpha value specifies the amount of opaqueness.<br>
 * The internal element order is: hue, saturation, value, alpha.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT HSVAColor
{
	public:

		/**
		 * Creates a new HSVAColor object with default parameters for a white and fully opaque color.
		 */
		HSVAColor();

		/**
		 * Creates a new opaque HSVAColor object by three given parameters.
		 * @param hue The hue parameter [0, 2PI)
		 * @param saturation The saturation parameter [0.0, 1.0]
		 * @param value The value parameter [0.0, 1.0]
		 */
		HSVAColor(const float hue, const float saturation, const float value);

		/**
		 * Creates a new HSVAColor object by four given parameters.
		 * @param hue The hue parameter [0, 2PI)
		 * @param saturation The saturation parameter [0.0, 1.0]
		 * @param value The value parameter [0.0, 1.0]
		 * @param alpha The alpha value with range [0.0, 1.0], 0 means fully transparent, 1 means fully opaque
		 */
		HSVAColor(const float hue, const float saturation, const float value, const float alpha);

		/**
		 * Creates a new HSVAColor object by an array with at least four elements.
		 * @param valueArray Array with parameters
		 */
		explicit HSVAColor(const float* valueArray);

		/**
		 * Creates a new HSVAColor object by a given RGBAColor object.
		 * @param color RGBAColor object to create the new color from
		 */
		explicit HSVAColor(const RGBAColor& color);

		/**
		 * Returns the hue parameter
		 * @return Hue parameter [0, 2Pi)
		 */
		[[nodiscard]] inline float hue() const;

		/**
		 * Returns the saturation parameter
		 * @return Saturation parameter [0.0, 1.0]
		 */
		[[nodiscard]] inline float saturation() const;

		/**
		 * Returns the value parameter
		 * @return Value parameter [0.0, 1.0]
		 */
		[[nodiscard]] inline float value() const;

		/**
		 * Returns the alpha parameter
		 * An alpha value of 0 means fully transparent, 1 means fully opaque.
		 * @return Alpha parameter with range [0.0, 1.0]
		 */
		[[nodiscard]] inline float alpha() const;

		/**
		 * Sets the hue parameter.
		 * @param hue New hue parameter [0, 2PI)
		 * @return True, if succeeded
		 */
		bool setHue(const float hue);

		/**
		 * Sets the saturation parameter.
		 * @param saturation New saturation parameter [0.0, 1.0]
		 * @return True, if succeeded
		 */
		bool setSaturation(const float saturation);

		/**
		 * Sets the value parameter.
		 * @param value New value parameter [0.0, 1.0]
		 * @return True, if succeeded
		 */
		bool setValue(const float value);

		/**
		 * Sets the alpha parameter.
		 * An alpha value of 0 means fully transparent, 1 means fully opaque.
		 * @param alpha New alpha parameter with range [0.0, 1.0]
		 * @return True, if succeeded
		 */
		bool setAlpha(const float alpha);

		/**
		 * Interpolates two colors linear.
		 * The given parameter specifies the amount of the second color.
		 * @param color Second color for interpolation
		 * @param factor Interpolation factor with range [0.0, 1.0]
		 * @return Interpolated color
		 */
		[[nodiscard]] HSVAColor interpolate(const HSVAColor& color, const float factor) const;

		/**
		 * Returns whether the color holds valid parameters.
		 * @return True, if so
		 */
		[[nodiscard]] bool isValid() const;

		/**
		 * Returns whether the two colors are equal up to a specified epsilon.
		 * @param color The second color to compare, must be valid
		 * @param epsilon The epsilon value for comparison, with range [0, 1)
		 * @param includeAlpha True, to compare the alpha components; False, to ignore the alpha components
		 * @return True, if so
		 */
		[[nodiscard]] inline bool isEqual(const HSVAColor& color, const float epsilon, const bool includeAlpha = true) const;

		/**
		 * Returns whether two colors are identical up to a small epsilon.
		 * @param right Right color
		 * @return True, if so
		 */
		[[nodiscard]] bool operator==(const HSVAColor& right) const;

		/**
		 * Returns whether two colors are not identical up to a small epsilon.
		 * @param right The right color
		 * @return True, if so
		 */
		[[nodiscard]] inline bool operator!=(const HSVAColor& right) const;

		/**
		 * Element access operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to access [0, 3]
		 * @return Element of the vector
		 */
		inline const float& operator[](const unsigned int index) const;

		/**
		 * Element access operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to access [0, 3]
		 * @return Element of the vector
		 */
		inline float& operator[](const unsigned int index);

		/**
		 * Element access operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to access [0, 3]
		 * @return Element of the vector
		 */
		inline const float& operator()(const unsigned int index) const;

		/**
		 * Element access operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to access [0, 3]
		 * @return Element of the vector
		 */
		inline float& operator()(const unsigned int index);

		/**
		 * Access operator.
		 * @return Pointer to the elements
		 */
		inline const float* operator()() const;

		/**
		 * Access operator.
		 * @return Pointer to the elements
		 */
		inline float* operator()();

	protected:

		/// The four elements of the color.
		float values_[4];
};

inline float HSVAColor::hue() const
{
	return values_[0];
}

inline float HSVAColor::saturation() const
{
	return values_[1];
}

inline float HSVAColor::value() const
{
	return values_[2];
}

inline float HSVAColor::alpha() const
{
	return values_[3];
}

inline bool HSVAColor::isEqual(const HSVAColor& color, const float epsilon, const bool includeAlpha) const
{
	ocean_assert(isValid());
	ocean_assert(color.isValid());

	ocean_assert(epsilon >= 0.0f && epsilon < 1.0f);

	return NumericF::isEqual(values_[0], color.values_[0], epsilon)
			&& NumericF::isEqual(values_[1], color.values_[1], epsilon)
			&& NumericF::isEqual(values_[2], color.values_[2], epsilon)
			&& (!includeAlpha || NumericF::isEqual(values_[3], color.values_[3], epsilon));
}

inline bool HSVAColor::operator!=(const HSVAColor& right) const
{
	return !(*this == right);
}

inline const float& HSVAColor::operator[](const unsigned int index) const
{
	ocean_assert(index < 4u);
	return values_[index];
}

inline float& HSVAColor::operator[](const unsigned int index)
{
	ocean_assert(index < 4u);
	return values_[index];
}

inline const float& HSVAColor::operator()(const unsigned int index) const
{
	ocean_assert(index < 4u);
	return values_[index];
}

inline float& HSVAColor::operator()(const unsigned int index)
{
	ocean_assert(index < 4u);
	return values_[index];
}

inline const float* HSVAColor::operator()() const
{
	return values_;
}

inline float* HSVAColor::operator()()
{
	return values_;
}

inline std::ostream& operator<<(std::ostream& stream, const HSVAColor& hsvaColor)
{
	stream << "[" << hsvaColor.hue() << ", " << hsvaColor.saturation() << ", " << hsvaColor.value() << ", " << hsvaColor.alpha() << "]";

	return stream;
}

template <bool tActive>
inline MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const HSVAColor& hsvaColor)
{
	return messageObject << "[" << hsvaColor.hue() << ", " << hsvaColor.saturation() << ", " << hsvaColor.value() << ", " << hsvaColor.alpha() << "]";
}

template <bool tActive>
inline MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const HSVAColor& hsvaColor)
{
	return messageObject << "[" << hsvaColor.hue() << ", " << hsvaColor.saturation() << ", " << hsvaColor.value() << ", " << hsvaColor.alpha() << "]";
}

}

#endif // META_OCEAN_MATH_HSVA_COLOR_H
