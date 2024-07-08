/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_RGBA_COLOR_H
#define META_OCEAN_MATH_RGBA_COLOR_H

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
typedef std::vector<RGBAColor> RGBAColors;

/**
 * This class implements a color defined by red, green, blue and alpha parameters.
 * The color space is a cube with the alpha parameter as fourth dimension.<br>
 * All color parameters should lie inside the range [0.0, 1.0]. However, values higher than 1.0 are allowed.<br>
 * The alpha value must lie inside the range [0.0, 1.0].<br>
 * To ensure that all values are in the mentioned range normalize the color.<br>
 * Use the isNormalized function to check whether all parameters lie inside the range [0.0, 1.0]<br>
 * An alpha value 1 means fully opaque, 0 means fully transparent. Therefore the alpha value specifies the amount of opaqueness.<br>
 * The internal element order is: red, green, blue, alpha.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT RGBAColor
{
	public:

		/**
		 * Definition of a internal used data type.
		 */
		typedef float Type;

	public:

		/**
		 * Creates a new color object with fully opaque white color.
		 */
		RGBAColor();

		/**
		 * Creates a new color object with white color.
		 * @param valid True, to create a valid white color; False, to create an invalid color object
		 */
		explicit RGBAColor(const bool valid);

		/**
		 * Creates a new opaque color object by three given parameters.
		 * Because the color is opaque the alpha value is set to 1.
		 * @param red The red value with range [0.0, 1.0], however higher values than 1.0 are allowed
		 * @param green The green value with range [0.0, 1.0], however higher values than 1.0 are allowed
		 * @param blue The blue value with range [0.0, 1.0], however higher values than 1.0 are allowed
		 */
		RGBAColor(const float red, const float green, const float blue);

		/**
		 * Creates a new color object by four given parameters.
		 * @param red The red value with range [0.0, 1.0], however higher values than 1.0 are allowed
		 * @param green The green value with range [0.0, 1.0], however higher values than 1.0 are allowed
		 * @param blue The blue value with range [0.0, 1.0], however higher values than 1.0 are allowed
		 * @param alpha The alpha value with range [0.0, 1.0], 0 means fully transparent, 1 means fully opaque
		 */
		RGBAColor(const float red, const float green, const float blue, const float alpha);

		/**
		 * Creates a new color object for a given color temperature.
		 * The resulting RGB value is based on an approximation.
		 * @param colorTemperature The color temperature for which the color object will be created, in kelvin, with range [1000, 40000]
		 */
		explicit RGBAColor(const float colorTemperature);

		/**
		 * Creates a new color object by an array with at least four elements.
		 * @param arrayValue Array with the color parameters
		 */
		explicit RGBAColor(const float* arrayValue);

		/**
		 * Creates a new color object by a given HSVAColor object.
		 * @param color HSVAColor object to create a RGBAColor object from
		 */
		explicit RGBAColor(const HSVAColor& color);

		/**
		 * Returns the red parameter
		 * @return Red parameter with range [0.0, 1.0], however higher values than 1.0 are allowed
		 */
		[[nodiscard]] inline float red() const;

		/**
		 * Returns the green parameter
		 * @return Green parameter with range [0.0, 1.0], however higher values than 1.0 are allowed
		 */
		[[nodiscard]] inline float green() const;

		/**
		 * Returns the blue parameter
		 * @return Blue parameter with range [0.0, 1.0], however higher values than 1.0 are allowed
		 */
		[[nodiscard]] inline float blue() const;

		/**
		 * Returns the alpha parameter
		 * An alpha value of 0 means fully transparent, 1 means fully opaque.
		 * @return Alpha parameter with range [0.0, 1.0]
		 */
		[[nodiscard]] inline float alpha() const;

		/**
		 * Sets the red parameter.
		 * @param red New red parameter with range [0.0, 1.0], however higher values than 1.0 are allowed
		 * @return True, if succeeded
		 */
		bool setRed(const float red);

		/**
		 * Sets the green parameter.
		 * @param green New green parameter with range [0.0, 1.0], however higher values than 1.0 are allowed
		 * @return True, if succeeded
		 */
		bool setGreen(const float green);

		/**
		 * Sets the blue parameter.
		 * @param blue New blue parameter with range [0.0, 1.0], however higher values than 1.0 are allowed
		 * @return True, if succeeded
		 */
		bool setBlue(const float blue);

		/**
		 * Sets the alpha parameter.
		 * An alpha value of 0 means fully transparent, 1 means fully opaque.
		 * @param alpha New alpha parameter with range [0.0, 1.0]
		 * @return True, if succeeded
		 */
		bool setAlpha(const float alpha);

		/**
		 * Returns the damped color object.
		 * The damping factor will be multiplied with each color component and optional with the alpha component.
		 * @param factor The damping factor to apply with range [0, infinity)
		 * @param includeAlpha True, to damp the alpha value; False, to leave the alpha value unchanged
		 * @return Damped color object
		 */
		[[nodiscard]] inline RGBAColor damped(const float factor, const bool includeAlpha = false) const;

		/**
		 * Damps this color object.
		 * The damping factor will be multiplied with each color component and optional with the alpha component.
		 * @param factor The damping factor to apply with range [0, infinity)
		 * @param includeAlpha True, to damp the alpha value; False, to leave the alpha value unchanged
		 */
		inline void damp(const float factor, const bool includeAlpha = false);

		/**
		 * Combines two color objects by adding the individual color components.
		 * The alpha component will be averaged.
		 * @param color The color to combine with this color, must be valid
		 * @return The combined color object
		 */
		[[nodiscard]] RGBAColor combined(const RGBAColor& color) const;

		/**
		 * Combines two color objects by adding the individual color components.
		 * The alpha component will be averaged.
		 * @param color The color to combine with this color, must be valid
		 */
		void combine(const RGBAColor& color);

		/**
		 * Returns whether all parameters are valid.
		 * All color parameters must be positive and the alpha value must lie inside the range [0.0, 1.0]
		 * @return True, if so
		 */
		[[nodiscard]] bool isValid() const;

		/**
		 * Returns whether all parameters lie inside the range [0.0, 1.0].
		 * Beware: The normalization of a color object has nothing in common with the normalization of e.g. a vector object.
		 * @return True, if so
		 */
		[[nodiscard]] bool isNormalized() const;

		/**
		 * Returns the normalized color object.
		 * If no color value is larger than 1.0 no color parameter will be changed.<br>
		 * During normalization all color values are scaled with the same factor so that the larges color value will be 1.0.<br>
		 * @return Normalized color object
		 */
		[[nodiscard]] RGBAColor normalized() const;

		/**
		 * Normalizes this color object.
		 * If no color value is larger than 1.0 nothing will be done.<br>
		 * During normalization all color values are scaled with the same factor so that the largest color value will be 1.0.<br>
		 */
		void normalize();

		/**
		 * Returns the clamped color object.
		 * All color values will be clamped independently into the range [0.0, 1.0].
		 * @return Clamped color object
		 */
		[[nodiscard]] RGBAColor clamped() const;

		/**
		 * Clamps this color object.
		 * All color values will be clamped independently into the range [0.0, 1.0].
		 */
		void clamp();

		/**
		 * Returns whether the color is black, independent of the alpha channel.
		 * @return True, if so
		 */
		[[nodiscard]] inline bool isBlack() const;

		/**
		 * Returns whether the color is white, independent of the alpha channel.
		 * @return True, if so
		 */
		[[nodiscard]] inline bool isWhite() const;

		/**
		 * Returns whether the color is fully opaque.
		 * @return True, if so
		 */
		[[nodiscard]] inline bool isOpaque() const;

		/**
		 * Returns whether the two colors are equal up to a specified epsilon.
		 * @param color The second color to compare, must be valid
		 * @param epsilon The epsilon value for comparison, with range [0, 1)
		 * @param includeAlpha True, to compare the alpha components; False, to ignore the alpha components
		 * @return True, if so
		 */
		[[nodiscard]] inline bool isEqual(const RGBAColor& color, const float epsilon, const bool includeAlpha = true) const;

		/**
		 * Returns whether two colors are identical up to a small epsilon.
		 * @param right The second color value to compare
		 * @return True, if so
		 */
		[[nodiscard]] bool operator==(const RGBAColor& right) const;

		/**
		 * Returns whether two colors are not identical up to a small epsilon.
		 * @param right The second color value to compare
		 * @return True, if so
		 */
		[[nodiscard]] inline bool operator!=(const RGBAColor& right) const;

		/**
		 * Returns the inverted color, the alpha parameter will be untouched.
		 * Make sure that the color object is normalized.
		 * @return Inverted color
		 */
		[[nodiscard]] RGBAColor operator-() const;

#if 0 // temporary disabled

		/**
		 * Scalar multiplication operator for the first three components.
		 * Beware: Color values can be large than 1.0 after multiplication.
		 * @param factor Positive component wise multiplication factor
		 * @return Multiplied color
		 * @see isNormalized(), normalize(), clamp()
		 */
		RGBAColor operator*(const float factor) const;

		/**
		 * Scalar multiplication and assign operator for the first three components.
		 * Beware: Color values can be large than 1.0 after multiplication.
		 * @param factor Positive component wise multiplication factor
		 * @return Reference to this object
		 * @see isNormalized(), normalize(), clamp()
		 */
		RGBAColor& operator*=(const float factor);

		/**
		 * Color addition operator for all four components.
		 * Each color component is added with the corresponding color component.
		 * @param color The color object to be used for multiplication
		 * @return Multiplied color
		 * @see isNormalized(), normalize(), clamp()
		 */
		inline RGBAColor operator+(const RGBAColor& color) const;

		/**
		 * Color addition operator for all four components.
		 * Each color component is added with the corresponding color component.
		 * @param color The color object to be used for multiplication
		 * @return Reference to this object
		 * @see isNormalized(), normalize(), clamp()
		 */
		inline RGBAColor& operator+=(const RGBAColor& color);

#endif

		/**
		 * Color multiplication operator for all four components.
		 * Each color component is multiplied with the corresponding color component.
		 * @param color The color object to be used for multiplication
		 * @return Multiplied color
		 * @see isNormalized(), normalize(), clamp()
		 */
		inline RGBAColor operator*(const RGBAColor& color) const;

		/**
		 * Color multiplication operator for all four components.
		 * Each color component is multiplied with the corresponding color component.
		 * @param color The color object to be used for multiplication
		 * @return Reference to this object
		 * @see isNormalized(), normalize(), clamp()
		 */
		inline RGBAColor& operator*=(const RGBAColor& color);

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

		/**
		 * Returns the four elements of this color.
		 * @return The color value with order: red, green, blue, alpha
		 */
		[[nodiscard]] inline const float* data() const;

	protected:

		/// The four values of the color object, with order red, green, blue, alpha.
		float values_[4];
};

inline float RGBAColor::red() const
{
	return values_[0];
}

inline float RGBAColor::green() const
{
	return values_[1];
}

inline float RGBAColor::blue() const
{
	return values_[2];
}

inline float RGBAColor::alpha() const
{
	return values_[3];
}

inline RGBAColor RGBAColor::damped(const float factor, const bool includeAlpha) const
{
	ocean_assert(isValid());
	ocean_assert(factor >= 0.0f);

	if (includeAlpha)
	{
		return RGBAColor(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3] * factor);
	}
	else
	{
		return RGBAColor(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3]);
	}
}

inline void RGBAColor::damp(const float factor, const bool includeAlpha)
{
	ocean_assert(isValid());
	ocean_assert(factor >= 0.0f);

	values_[0] *= factor;
	values_[1] *= factor;
	values_[2] *= factor;

	if (includeAlpha)
	{
		values_[3] *= factor;
	}
}

inline bool RGBAColor::isBlack() const
{
	return values_[0] == 0.0f && values_[1] == 0.0f && values_[2] == 0.0f;
}

inline bool RGBAColor::isWhite() const
{
	return values_[0] == 1.0f && values_[1] == 1.0f && values_[2] == 1.0f;
}

inline bool RGBAColor::isOpaque() const
{
	ocean_assert(isValid());
	return values_[3] == 1.0f;
}

inline bool RGBAColor::isEqual(const RGBAColor& color, const float epsilon, const bool includeAlpha) const
{
	ocean_assert(isValid());
	ocean_assert(color.isValid());

	ocean_assert(epsilon >= 0.0f && epsilon < 1.0f);

	return NumericF::isEqual(values_[0], color.values_[0], epsilon)
			&& NumericF::isEqual(values_[1], color.values_[1], epsilon)
			&& NumericF::isEqual(values_[2], color.values_[2], epsilon)
			&& (!includeAlpha || NumericF::isEqual(values_[3], color.values_[3], epsilon));
}

inline bool RGBAColor::operator!=(const RGBAColor& right) const
{
	ocean_assert(isValid());
	ocean_assert(right.isValid());

	return !(*this == right);
}

#if 0 // temporary disabled

inline RGBAColor RGBAColor::operator+(const RGBAColor& color) const
{
	ocean_assert(isValid());
	ocean_assert(color.isValid());

	return RGBAColor(values_[0] + color.values_[0], values_[1] + color.values_[1], values_[2] + color.values_[2], values_[3] + color.values_[3], true);
}

inline RGBAColor& RGBAColor::operator+=(const RGBAColor& color)
{
	ocean_assert(isValid());
	ocean_assert(color.isValid());

	values_[0] += color.values_[0];
	values_[1] += color.values_[1];
	values_[2] += color.values_[2];
	values_[3] += color.values_[3];
	ocean_assert(isValid());

	return *this;
}

#endif

inline RGBAColor RGBAColor::operator*(const RGBAColor& color) const
{
	ocean_assert(isValid());
	ocean_assert(color.isValid());

	return RGBAColor(values_[0] * color.values_[0], values_[1] * color.values_[1], values_[2] * color.values_[2], values_[3] * color.values_[3]);
}

inline RGBAColor& RGBAColor::operator*=(const RGBAColor& color)
{
	ocean_assert(isValid());
	ocean_assert(color.isValid());

	values_[0] *= color.values_[0];
	values_[1] *= color.values_[1];
	values_[2] *= color.values_[2];
	values_[3] *= color.values_[3];
	ocean_assert(isValid());

	return *this;
}

inline const float& RGBAColor::operator[](const unsigned int index) const
{
	ocean_assert(index < 4);
	return values_[index];
}

inline float& RGBAColor::operator[](const unsigned int index)
{
	ocean_assert(index < 4);
	return values_[index];
}

inline const float& RGBAColor::operator()(const unsigned int index) const
{
	ocean_assert(index < 4);
	return values_[index];
}

inline float& RGBAColor::operator()(const unsigned int index)
{
	ocean_assert(index < 4);
	return values_[index];
}

inline const float* RGBAColor::operator()() const
{
	return values_;
}

inline float* RGBAColor::operator()()
{
	return values_;
}

inline const float* RGBAColor::data() const
{
	return values_;
}

inline std::ostream& operator<<(std::ostream& stream, const RGBAColor& rgbaColor)
{
	stream << "[" << rgbaColor.red() << ", " << rgbaColor.green() << ", " << rgbaColor.blue() << ", " << rgbaColor.alpha() << "]";

	return stream;
}

template <bool tActive>
inline MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const RGBAColor& rgbaColor)
{
	return messageObject << "[" << rgbaColor.red() << ", " << rgbaColor.green() << ", " << rgbaColor.blue() << ", " << rgbaColor.alpha() << "]";
}

template <bool tActive>
inline MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const RGBAColor& rgbaColor)
{
	return messageObject << "[" << rgbaColor.red() << ", " << rgbaColor.green() << ", " << rgbaColor.blue() << ", " << rgbaColor.alpha() << "]";
}

}

#endif // META_OCEAN_MATH_RGBA_COLOR_H
