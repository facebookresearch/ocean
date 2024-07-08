/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_INTERPOLATION_H
#define META_OCEAN_MATH_INTERPOLATION_H

#include "ocean/math/Math.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Pose.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"

#include <map>

namespace Ocean
{

/**
 * This class implements data interpolation functions.
 * An interpolation value of 0 provides the first value, 1 provides the second value.<br>
 * @ingroup math
 */
class Interpolation
{
	protected:

		/**
		 * This class implements a helper linear interpolation helper class allowing to overload the interpolation function for specific data types.
		 * @tparam T Data type of the values to be interpolated
		 */
		template <typename T>
		class LinearInterpolation
		{
			public:

				/**
				 * Performs a linear interpolation between two values.
				 * @param v0 First value
				 * @param v1 Second value
				 * @param t Interpolation factor, with range [0, 1]
				 * @return Resulting interpolation value
				 * @tparam TFactor Data type of the interpolation value
				 */
				template <typename TFactor>
				static inline T interpolate(const T& v0, const T& v1, const TFactor& t);
		};

	public:

		/**
		 * Performs a linear interpolation between two values.
		 * @param v0 First value corresponding to the interpolation factor t = 0
		 * @param v1 Second value corresponding to the interpolation factor t = 1
		 * @param t Interpolation factor, with range [0, 1]
		 * @return Resulting interpolation value
		 * @tparam T Data type of the values to be interpolated
		 * @tparam TFactor Data type of the interpolation value
		 */
		template <typename T, typename TFactor>
		static inline T linear(const T& v0, const T& v1, const TFactor& t);

		/**
		 * Performs a bi-linear interpolation between four values within a 2x2 neighborhood.
		 * The layout of the four values are defined as follows:<br>
		 * <pre>
		 * v00 v01
		 * v10 v11
		 * </pre>
		 * @param v00 Top left value
		 * @param v01 Top right value
		 * @param v10 Bottom left value
		 * @param v11 Bottom right value
		 * @param tx Horizontal (left - right) interpolation factor, with range [0, 1]
		 * @param ty Vertical (top - bottom) interpolation factor, with range [0, 1]
		 * @return Resulting interpolation value
		 * @tparam T Data type of the values to be interpolated
		 * @tparam TFactor Data type of the interpolation value
		 */
		template <typename T, typename TFactor>
		static inline T bilinear(const T& v00, const T& v01, const T& v10, const T& v11, const TFactor& tx, const TFactor& ty);

		/**
		 * Performs a bi-linear interpolation between at most four values within a 2x2 neighborhood.
		 * This function allows the interpolation between a subset of the given four values.<br>
		 * Thus, boolean statements for each individual value define whether the corresponding value is applied for interpolation or not.<br>
		 * If only one value is provided, than the 'interpolation' result is the value itself without investigating the interpolation factors.<br>
		 * The layout of the four values are defined as follows:<br>
		 * <pre>
		 * v00 v01
		 * v10 v11
		 * </pre>
		 * @param v00 Top left value
		 * @param v01 Top right value
		 * @param v10 Bottom left value
		 * @param v11 Bottom right value
		 * @param valid00 True, if the value v00 is valid and has to be applied for interpolation
		 * @param valid01 True, if the value v01 is valid and has to be applied for interpolation
		 * @param valid10 True, if the value v10 is valid and has to be applied for interpolation
		 * @param valid11 True, if the value v11 is valid and has to be applied for interpolation
		 * @param tx Horizontal (left - right) interpolation factor, with range [0, 1]
		 * @param ty Vertical (top - bottom) interpolation factor, with range [0, 1]
		 * @return Resulting interpolation value
		 * @tparam T Data type of the values to be interpolated
		 * @tparam TFactor Data type of the interpolation value
		 */
		template <typename T, typename TFactor>
		static inline T bilinear(const T& v00, const T& v01, const T& v10, const T& v11, const bool valid00, const bool valid01, const bool valid10, const bool valid11, const TFactor& tx, const TFactor& ty);

		/**
		 * Performs a cubic (Catmull-Rom) interpolation between mainly two values while two further supporting points are requested.
		 * @param v0 Supporting value in front of the first value
		 * @param v1 First value
		 * @param v2 Second value
		 * @param v3 Supporting value behind the second value
		 * @param t Interpolation factor, with range [0, 1]
		 * @return Resulting interpolation value
		 * @tparam T Data type of the values to be interpolated
		 * @tparam TFactor Data type of the interpolation value
		 */
		template <typename T, typename TFactor>
		static inline T cubic(const T& v0, const T& v1, const T& v2, const T& v3, const TFactor& t);

		/**
		 * Performs a Lagrange interpolation for a polynomial with degree 2.
		 * @param x0 Position of the first supporting value, must not be identical with x1
		 * @param y0 First supporting value
		 * @param x1 Position of the second supporting value, must not be identical with x0
		 * @param y1 Second supporting value
		 * @param x Position for that the interpolation value is requested
		 * @return Resulting Lagrange intgerpolatino value
		 * @tparam T Data type of the supporting values
		 * @tparam TKey Data type of the positions of the supporting values
		 */
		template <typename T, typename TKey>
		static T lagrange2(const TKey& x0, const T& y0, const TKey& x1, const T& y1, const TKey& x);

		/**
		 * Performs a Lagrange interpolation for a polynomial with degree 3.
		 * @param x0 Position of the first supporting value, must not be identical with x1 or x2
		 * @param y0 First supporting value
		 * @param x1 Position of the second supporting value, must not be identical with x0 or x2
		 * @param y1 Second supporting value
		 * @param x2 Position of the third supporting value, must not be identical with x0 or x1
		 * @param y2 Third supporting value
		 * @param x Position for that the interpolation value is requested
		 * @return Resulting Lagrange intgerpolatino value
		 * @tparam T Data type of the supporting values
		 * @tparam TKey Data type of the positions of the supporting values
		 */
		template <typename T, typename TKey>
		static T lagrange3(const TKey& x0, const T& y0, const TKey& x1, const T& y1, const TKey& x2, const T& y2, const TKey& x);
};

/**
 * This class implements an interpolation map allowing to determine an interplated value between several individual key values.
 * @tparam TKey The data type of the stored keys
 * @tparam TValue The data type of the stored values
 * @tparam TFactor The data type of the interpolation factor
 * @ingroup math
 */
template <typename TKey, typename TValue, typename TFactor>
class InterpolationMap
{
	public:

		/**
		 * Definition of a map mapping keys to values.
		 */
		typedef std::map<TKey, TValue> ValueMap;

		/**
		 * Definition of a function pointer for a function providing a linear interpolation.
		 * The first parameter defines the first interpolation value.<br>
		 * The second parameter defines the second interpolation value.<br>
		 * The third parameter defines the interpolation factor, with range [0, 1].<br>
		 */
		typedef TValue (*LinearInterpolationFunction)(const TValue&, const TValue&, const TFactor&);

	public:

		/**
		 * Default constructor.
		 */
		inline InterpolationMap();

		/**
		 * Copy constructor for interpolation maps.
		 * @param interpolationMap The interpolation map to copy
		 */
		inline InterpolationMap(const InterpolationMap<TKey, TValue, TFactor>& interpolationMap);

		/**
		 * Move constructor for interpolation maps.
		 * @param interpolationMap The interpolation map to move
		 */
		inline InterpolationMap(InterpolationMap<TKey, TValue, TFactor>&& interpolationMap) noexcept;

		/**
		 * Creates a new interpolation map object by a given value map.
		 * @param valueMap The value map that will be stored in this object
		 */
		inline InterpolationMap(const ValueMap& valueMap);

		/**
		 * Creates a new interpolation map object by a given value map.
		 * @param valueMap The value map that will be moved to this object
		 */
		inline InterpolationMap(ValueMap&& valueMap);

		/**
		 * Returns the number of elements (pairs of keys and values) of this interpolation map.
		 * @return The number of stored elements
		 */
		inline size_t size() const;

		/**
		 * Returns whether this interpolation map object is empty.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Clears all elements (pairs of keys and values) of this interpolation map object.
		 */
		inline void clear();

		/**
		 * Returns the entire internal interpolation map.
		 * @return Interpolation map of this object
		 */
		inline const ValueMap& interpolationMap() const;

		/**
		 * Sets or changes the entire internal interpolation map.
		 * @param interpolationMap The interpolation map that will replace the current one
		 */
		inline void setInterpolationMap(const ValueMap& interpolationMap);

		/**
		 * Sets or changes the entire internal interpolation map.
		 * @param interpolationMap The interpolation map that will be moved to this object
		 */
		inline void setInterpolationMap(ValueMap&& interpolationMap);

		/**
		 * Returns whether this interpolation map object holds a value for a specified key.
		 * @param key Key which is checked
		 * @return True, if so
		 */
		inline bool hasValue(const TKey& key) const;

		/**
		 * Returns the value of this interpolation map object defined by a corresponding key.
		 * If no value exists for this key, the pair of key and value is inserted and the default value is returned.<br>
		 * @param key Key for that the value is returned
		 * @return Value that corresponds to the specified key
		 * @see hasValue().
		 */
		inline TValue& value(const TKey& key);

		/**
		 * Removes an elements (pair of key and value) from this interpolation map object.
		 * @param key The key of the element to be removed
		 * @return True, if the element existed
		 */
		inline bool remove(const TKey& key);

		/**
		 * Inserts a new pair of key and value to this map object.
		 * @param key New key to be inserted
		 * @param value Corresponding value to be instered
		 * @param forceOverwrite True, if an already existing pair with same key will be overwritten by the given pair
		 * @return True, if the given pair has been inserted
		 */
		inline bool insert(const TKey& key, const TValue& value, const bool forceOverwrite = false);

		/**
		 * Inserts a new pair of key and value to this map object.
		 * @param key New key to be inserted
		 * @param value Corresponding value to be moved
		 * @param forceOverwrite True, if an already existing pair with same key will be overwritten by the given pair
		 * @return True, if the given pair has been inserted
		 */
		inline bool insert(const TKey& key, TValue&& value, const bool forceOverwrite = false);

		/**
		 * Returns the linear interpolation of the stored values for a given key.
		 * @param key The key of the value that will be (interpolated and) returned
		 * @return Resulting interpolated value
		 */
		inline TValue linear(const TKey& key) const;

		/**
		 * Returns the linear interpolation of the stored values for a given key and a given interpolation function.
		 * @param key The key of the value that will be (interpolated and) returned
		 * @param interpolationFunction An explicit interpolation function that is applied
		 * @return Resulting interpolated value
		 */
		inline TValue linear(const TKey& key, const LinearInterpolationFunction interpolationFunction) const;

		/**
		 * Returns the linear interpolation of given values for a given key.
		 * @param valueMap The map of values that are used for interpolation
		 * @param key The key of the value that will be (interpolated and) returned
		 * @return Resulting interpolated value
		 */
		static TValue linear(const ValueMap& valueMap, const TKey& key);

		/**
		 * Returns the linear interpolation of given values for a given key and a given interpolation function.
		 * @param valueMap The map of values that are used for interpolation
		 * @param key The key of the value that will be (interpolated and) returned
		 * @param interpolationFunction An explicit interpolation function that is applied
		 * @return Resulting interpolated value
		 */
		static TValue linear(const ValueMap& valueMap, const TKey& key, const LinearInterpolationFunction interpolationFunction);

		/**
		 * Assign operator.
		 * @param object The interpolation map object which will be assigned to this object
		 * @return Reference to this object
		 */
		inline InterpolationMap<TKey, TValue, TFactor>& operator=(const InterpolationMap<TKey, TValue, TFactor>& object);

		/**
		 * Move operator.
		 * @param object The interpolation map object which will be moved to this object
		 * @return Reference to this object
		 */
		inline InterpolationMap<TKey, TValue, TFactor>& operator=(InterpolationMap<TKey, TValue, TFactor>&& object) noexcept;

		/**
		 * Returns whether two interpolation objects are equal.
		 * @param object Second interpoation object
		 * @return True, if so
		 */
		inline bool operator==(const InterpolationMap<TKey, TValue, TFactor>& object) const;

		/**
		 * Returns whether two interpolation objects are not equal.
		 * @param object Second interpoation object
		 * @return True, if so
		 */
		inline bool operator!=(const InterpolationMap<TKey, TValue, TFactor>& object) const;

		/**
		 * Returns the value of this interpolation map object defined by a corresponding key.
		 * If no value exists for this key, the pair of key and value is inserted and the default value is returned.<br>
		 * @param key Key for that the value is returned
		 * @return Value that corresponds to the specified key
		 * @see value(), hasValue().
		 */
		inline TValue& operator[](const TKey& key);

	protected:

		/// Map mapping keys to values.
		ValueMap interpolationValueMap;
};

template <typename T>
template <typename TFactor>
inline T Interpolation::LinearInterpolation<T>::interpolate(const T& v0, const T& v1, const TFactor& t)
{
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return v0 * (TFactor(1) - t) + v1 * t;
}

template <>
template <>
inline float Interpolation::LinearInterpolation<float>::interpolate(const float& v0, const float& v1, const double& t)
{
	ocean_assert(NumericT<double>::isInsideRange(0.0, t, 1.0));

	return float(v0 * (1.0 - t)) + float(v1 * t);
}

template <>
template <typename TFactor>
inline HomogenousMatrixT4<float> Interpolation::LinearInterpolation<HomogenousMatrixT4<float>>::interpolate(const HomogenousMatrixT4<float>& v0, const HomogenousMatrixT4<float>& v1, const TFactor& t)
{
	ocean_assert(v0.isValid() && v1.isValid());
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	VectorT3<float> translation0(0, 0, 0), translation1(0, 0, 0);
	VectorT3<float> scale0(0, 0, 0), scale1(0, 0, 0);
	VectorT3<float> shear0(0, 0, 0), shear1(0, 0, 0);

	QuaternionT<float> rotation0, rotation1;

	v0.decompose(translation0, rotation0, scale0, shear0);
	v1.decompose(translation1, rotation1, scale1, shear1);

	return HomogenousMatrixT4<float>(linear(translation0, translation1, t), linear(rotation0, rotation1, t), linear(scale0, scale1, t), linear(shear0, shear1, t));
}

template <>
template <typename TFactor>
inline HomogenousMatrixT4<double> Interpolation::LinearInterpolation<HomogenousMatrixT4<double>>::interpolate(const HomogenousMatrixT4<double>& v0, const HomogenousMatrixT4<double>& v1, const TFactor& t)
{
	ocean_assert(v0.isValid() && v1.isValid());
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	VectorT3<double> translation0(0, 0, 0), translation1(0, 0, 0);
	VectorT3<double> scale0(0, 0, 0), scale1(0, 0, 0);
	VectorT3<double> shear0(0, 0, 0), shear1(0, 0, 0);

	QuaternionT<double> rotation0, rotation1;

	v0.decompose(translation0, rotation0, scale0, shear0);
	v1.decompose(translation1, rotation1, scale1, shear1);

	return HomogenousMatrixT4<double>(linear(translation0, translation1, t), linear(rotation0, rotation1, t), linear(scale0, scale1, t), linear(shear0, shear1, t));
}

template <>
template <typename TFactor>
inline std::pair<VectorT3<float>, QuaternionT<float>> Interpolation::LinearInterpolation< std::pair<VectorT3<float>, QuaternionT<float>> >::interpolate(const std::pair<VectorT3<float>, QuaternionT<float>>& v0, const std::pair<VectorT3<float>, QuaternionT<float>>& v1, const TFactor& t)
{
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return std::make_pair(linear(v0.first, v1.first, t), linear(v0.second, v1.second, t));
}

template <>
template <typename TFactor>
inline std::pair<VectorT3<double>, QuaternionT<double>> Interpolation::LinearInterpolation< std::pair<VectorT3<double>, QuaternionT<double>> >::interpolate(const std::pair<VectorT3<double>, QuaternionT<double>>& v0, const std::pair<VectorT3<double>, QuaternionT<double>>& v1, const TFactor& t)
{
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return std::make_pair(linear(v0.first, v1.first, t), linear(v0.second, v1.second, t));
}

template <>
template <typename TFactor>
inline std::pair<VectorT3<float>, RotationT<float>> Interpolation::LinearInterpolation< std::pair<VectorT3<float>, RotationT<float>> >::interpolate(const std::pair<VectorT3<float>, RotationT<float>>& v0, const std::pair<VectorT3<float>, RotationT<float>>& v1, const TFactor& t)
{
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return std::make_pair(linear(v0.first, v1.first, t), linear(v0.second, v1.second, t));
}

template <>
template <typename TFactor>
inline std::pair<VectorT3<double>, RotationT<double>> Interpolation::LinearInterpolation< std::pair<VectorT3<double>, RotationT<double>> >::interpolate(const std::pair<VectorT3<double>, RotationT<double>>& v0, const std::pair<VectorT3<double>, RotationT<double>>& v1, const TFactor& t)
{
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return std::make_pair(linear(v0.first, v1.first, t), linear(v0.second, v1.second, t));
}

template <>
template <typename TFactor>
inline Pose Interpolation::LinearInterpolation<Pose>::interpolate(const Pose& v0, const Pose& v1, const TFactor& t)
{
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return Pose(v0.translation() * (TFactor(1) - t) + v1.translation() * t, v0.orientation().slerp(v1.orientation(), t));
}

template <>
template <typename TFactor>
inline QuaternionT<float> Interpolation::LinearInterpolation<QuaternionT<float>>::interpolate(const QuaternionT<float>& v0, const QuaternionT<float>& v1, const TFactor& t)
{
	ocean_assert(v0.isValid() && v1.isValid());
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return v0.slerp(v1, float(t));
}

template <>
template <typename TFactor>
inline QuaternionT<double> Interpolation::LinearInterpolation<QuaternionT<double>>::interpolate(const QuaternionT<double>& v0, const QuaternionT<double>& v1, const TFactor& t)
{
	ocean_assert(v0.isValid() && v1.isValid());
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return v0.slerp(v1, double(t));
}

template <>
template <typename TFactor>
inline RotationT<float> Interpolation::LinearInterpolation<RotationT<float>>::interpolate(const RotationT<float>& v0, const RotationT<float>& v1, const TFactor& t)
{
	ocean_assert(v0.isValid() && v1.isValid());
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return RotationT<float>(QuaternionT<float>(v0).slerp(QuaternionT<float>(v1), t));
}

template <>
template <typename TFactor>
inline RotationT<double> Interpolation::LinearInterpolation<RotationT<double>>::interpolate(const RotationT<double>& v0, const RotationT<double>& v1, const TFactor& t)
{
	ocean_assert(v0.isValid() && v1.isValid());
	ocean_assert(NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return RotationT<double>(QuaternionT<double>(v0).slerp(QuaternionT<double>(v1), t));
}

template <typename T, typename TFactor>
inline T Interpolation::linear(const T& v0, const T& v1, const TFactor& t)
{
	ocean_assert(t >= TFactor(0) && t <= TFactor(1));

	if (t <= TFactor(0))
	{
		return v0;
	}
	if (t >= TFactor(1))
	{
		return v1;
	}

	return LinearInterpolation<T>::template interpolate<TFactor>(v0, v1, t);
}

template <typename T, typename TFactor>
inline T Interpolation::bilinear(const T& v00, const T& v01, const T& v10, const T& v11, const TFactor& tx, const TFactor& ty)
{
	return linear(linear(v00, v01, tx), linear(v10, v11, tx), ty);
}

template <typename T, typename TFactor>
inline T Interpolation::bilinear(const T& v00, const T& v01, const T& v10, const T& v11, const bool valid00, const bool valid01, const bool valid10, const bool valid11, const TFactor& tx, const TFactor& ty)
{
	ocean_assert(valid00 || valid01 || valid10 || valid11);

	ocean_assert(valid00 == 1 || valid00 == 0);
	ocean_assert(valid01 == 1 || valid01 == 0);
	ocean_assert(valid10 == 1 || valid10 == 0);
	ocean_assert(valid11 == 1 || valid11 == 0);

	const unsigned int state = (unsigned int)(valid00) | ((unsigned int)(valid01) << 8) | ((unsigned int)(valid10) << 16) | ((unsigned int)(valid11) << 24);

	switch (state)
	{
		// 1 1
		// 1 1
		case 0x01010101:
			return bilinear(v00, v01, v10, v11, tx, ty);

		// 0 1
		// 1 1
		case 0x01010100:
			return linear(v01, linear(v10, v11, tx), ty);

		// 1 0
		// 1 1
		case 0x01010001:
			return linear(v00, linear(v10, v11, tx), ty);

		// 1 1
		// 0 1
		case 0x01000101:
			return linear(linear(v00, v01, tx), v11, ty);

		// 1 1
		// 1 0
		case 0x00010101:
			return linear(linear(v00, v01, tx), v10, ty);

		// 0 0
		// 1 1
		case 0x01010000:
			return linear(v10, v11, tx);

		// 1 0
		// 1 0
		case 0x00010001:
			return linear(v00, v10, ty);

		// 1 1
		// 0 0
		case 0x00000101:
			return linear(v00, v01, tx);

		// 0 1
		// 0 1
		case 0x01000100:
			return linear(v01, v11, ty);

		// 0 1
		// 1 0
		case 0x00010100:
			// position on the diagonal
			return linear(v10, v01, Vector2(tx, 1 - ty).length() * Scalar(0.70710678118654752440084436210485));

		// 1 0
		// 0 1
		case 0x01000001:
			// position on the diagonal
			return linear(v00, v11, Vector2(tx, ty).length() * Scalar(0.70710678118654752440084436210485));

		// 1 0
		// 0 0
		case 0x00000001:
			return v00;

		// 0 1
		// 0 0
		case 0x00000100:
			return v01;

		// 0 0
		// 1 0
		case 0x00010000:
			return v10;

		// 0 0
		// 0 1
		case 0x01000000:
			return v11;
	}

	ocean_assert(false && "Invalid interpolation statement!");
	return T();
}

template <typename T, typename TFactor>
inline T Interpolation::cubic(const T& v0, const T& v1, const T& v2, const T& v3, const TFactor& t)
{
	ocean_assert((std::is_same<T, float>::value) ? NumericT<TFactor>::isInsideWeakRange(TFactor(0), t, TFactor(1)) : NumericT<TFactor>::isInsideRange(TFactor(0), t, TFactor(1)));

	return (v0 * TFactor(-0.5) + v1 * TFactor(1.5) - v2 * TFactor(1.5) + v3 * TFactor(0.5)) * t * t * t
			+ (v0 - v1 * TFactor(2.5) + v2 * TFactor(2) - v3 * TFactor(0.5)) * t * t
			+ (v0 * TFactor(-0.5) + v2 * TFactor(0.5)) * t
			+ v1;
}

template <typename T, typename TKey>
T Interpolation::lagrange2(const TKey& x0, const T& y0, const TKey& x1, const T& y1, const TKey& x)
{
	ocean_assert(NumericT<TKey>::isNotEqual(x0, x1));

	return (y0 * (x1 - x) + y1 * (x - x0)) * (TKey(1) / (x1 - x0));
}

template <typename T, typename TKey>
T Interpolation::lagrange3(const TKey& x0, const T& y0, const TKey& x1, const T& y1, const TKey& x2, const T& y2, const TKey& x)
{
	ocean_assert(NumericT<TKey>::isNotEqual(x0, x1) && NumericT<TKey>::isNotEqual(x0, x2) && NumericT<TKey>::isNotEqual(x1, 2));

	return y0 * ((x - x1) * (x - x2)) * (TKey(1) / ((x0 - x1) * (x0 - x2)))
		+ y1 * ((x - x0) * (x - x2)) * (TKey(1) / ((x1 - x0) * (x1 - x2)))
		+ y2 * ((x - x0) * (x - x1)) * (TKey(1) / ((x2 - x0) * (x2 - x1)));
}

template <typename TKey, typename TValue, typename TFactor>
inline InterpolationMap<TKey, TValue, TFactor>::InterpolationMap()
{
	// nothing to do here
}

template <typename TKey, typename TValue, typename TFactor>
inline InterpolationMap<TKey, TValue, TFactor>::InterpolationMap(const InterpolationMap<TKey, TValue, TFactor>& interpolationMap) :
	interpolationValueMap(interpolationMap.interpolationValueMap)
{
	// nothing to do here
}

template <typename TKey, typename TValue, typename TFactor>
inline InterpolationMap<TKey, TValue, TFactor>::InterpolationMap(InterpolationMap<TKey, TValue, TFactor>&& interpolationMap) noexcept :
	interpolationValueMap(std::move(interpolationMap.interpolationValueMap))
{
	// nothing to do here
}

template <typename TKey, typename TValue, typename TFactor>
inline InterpolationMap<TKey, TValue, TFactor>::InterpolationMap(const ValueMap& valueMap) :
	interpolationValueMap(valueMap)
{
	// nothing to do here
}

template <typename TKey, typename TValue, typename TFactor>
inline InterpolationMap<TKey, TValue, TFactor>::InterpolationMap(ValueMap&& valueMap) :
	interpolationValueMap(std::move(valueMap))
{
	// nothing to do here
}

template <typename TKey, typename TValue, typename TFactor>
inline size_t InterpolationMap<TKey, TValue, TFactor>::size() const
{
	return interpolationValueMap.size();
}

template <typename TKey, typename TValue, typename TFactor>
inline bool InterpolationMap<TKey, TValue, TFactor>::isEmpty() const
{
	return interpolationValueMap.empty();
}

template <typename TKey, typename TValue, typename TFactor>
inline void InterpolationMap<TKey, TValue, TFactor>::clear()
{
	interpolationValueMap.clear();
}

template <typename TKey, typename TValue, typename TFactor>
inline const typename InterpolationMap<TKey, TValue, TFactor>::ValueMap& InterpolationMap<TKey, TValue, TFactor>::interpolationMap() const
{
	return interpolationValueMap;
}

template <typename TKey, typename TValue, typename TFactor>
inline void InterpolationMap<TKey, TValue, TFactor>::setInterpolationMap(const ValueMap& interpolationMap)
{
	interpolationValueMap = interpolationMap;
}

template <typename TKey, typename TValue, typename TFactor>
inline void InterpolationMap<TKey, TValue, TFactor>::setInterpolationMap(ValueMap&& interpolationMap)
{
	interpolationValueMap = std::move(interpolationMap);
}

template <typename TKey, typename TValue, typename TFactor>
inline bool InterpolationMap<TKey, TValue, TFactor>::hasValue(const TKey& key) const
{
	return interpolationValueMap.find(key) != interpolationValueMap.end();
}

template <typename TKey, typename TValue, typename TFactor>
inline TValue& InterpolationMap<TKey, TValue, TFactor>::value(const TKey& key)
{
	return interpolationValueMap[key];
}

template <typename TKey, typename TValue, typename TFactor>
inline bool InterpolationMap<TKey, TValue, TFactor>::remove(const TKey& key)
{
	return interpolationValueMap.erase(key) != 0;
}

template <typename TKey, typename TValue, typename TFactor>
inline bool InterpolationMap<TKey, TValue, TFactor>::insert(const TKey& key, const TValue& value, const bool forceOverwrite)
{
	if (forceOverwrite)
	{
		interpolationValueMap[key] = value;
		return true;
	}

	if (interpolationValueMap.find(key) != interpolationValueMap.end())
	{
		return false;
	}

	interpolationValueMap[key] = value;

	return true;
}

template <typename TKey, typename TValue, typename TFactor>
inline bool InterpolationMap<TKey, TValue, TFactor>::insert(const TKey& key, TValue&& value, const bool forceOverwrite)
{
	if (forceOverwrite)
	{
		interpolationValueMap[key] = std::move(value);
		return true;
	}

	if (interpolationValueMap.find(key) != interpolationValueMap.end())
	{
		return false;
	}

	interpolationValueMap[key] = std::move(value);

	return true;
}

template <typename TKey, typename TValue, typename TFactor>
inline TValue InterpolationMap<TKey, TValue, TFactor>::linear(const TKey& key) const
{
	return linear(interpolationValueMap, key);
}

template <typename TKey, typename TValue, typename TFactor>
inline TValue InterpolationMap<TKey, TValue, TFactor>::linear(const TKey& key, const LinearInterpolationFunction interpolationFunction) const
{
	return linear(interpolationValueMap, key, interpolationFunction);
}

template <typename TKey, typename TValue, typename TFactor>
TValue InterpolationMap<TKey, TValue, TFactor>::linear(const ValueMap& valueMap, const TKey& key)
{
	if (valueMap.empty())
	{
		return TValue();
	}

	const typename ValueMap::const_iterator iHigher = valueMap.lower_bound(key);

	// if the key is larger than the last key
	if (iHigher == valueMap.end())
	{
		ocean_assert(valueMap.rbegin() != valueMap.rend());
		return valueMap.rbegin()->second;
	}

	// if the key is the first element
	if (iHigher == valueMap.begin())
	{
		return iHigher->second;
	}

	// we have at least one key in front of the higher key

	typename ValueMap::const_iterator iLower(iHigher);
	iLower--;

	TFactor width = TFactor(iHigher->first - iLower->first);

	ocean_assert(width > 0);
	ocean_assert(key > iLower->first);

	TFactor interpolationFactor = TFactor(key - iLower->first) / width;
	ocean_assert(interpolationFactor >= TFactor(0) && interpolationFactor <= TFactor(1));

	return Interpolation::linear(iLower->second, iHigher->second, interpolationFactor);
}

template <typename TKey, typename TValue, typename TFactor>
TValue InterpolationMap<TKey, TValue, TFactor>::linear(const ValueMap& valueMap, const TKey& key, const LinearInterpolationFunction interpolationFunction)
{
	ocean_assert(interpolationFunction);

	if (valueMap.empty() || !interpolationFunction)
	{
		return TValue();
	}

	const typename ValueMap::const_iterator iHigher = valueMap.lower_bound(key);

	// if the key is larger than the last key
	if (iHigher == valueMap.end())
	{
		ocean_assert(valueMap.rbegin() != valueMap.rend());
		return valueMap.rbegin()->second;
	}

	// if the key is the first element
	if (iHigher == valueMap.begin())
	{
		return iHigher->second;
	}

	// we have at least one key in front of the higher key

	typename ValueMap::const_iterator iLower(iHigher);
	iLower--;

	TFactor width = TFactor(iHigher->first - iLower->first);

	ocean_assert(width > 0);
	ocean_assert(key > iLower->first);

	TFactor interpolationFactor = TFactor(key - iLower->first) / width;
	ocean_assert(interpolationFactor >= TFactor(0) && interpolationFactor <= TFactor(1));

	return interpolationFunction(iLower->second, iHigher->second, interpolationFactor);
}

template <typename TKey, typename TValue, typename TFactor>
inline InterpolationMap<TKey, TValue, TFactor>& InterpolationMap<TKey, TValue, TFactor>::operator=(const InterpolationMap<TKey, TValue, TFactor>& object)
{
	interpolationValueMap = object.interpolationValueMap;
	return *this;
}

template <typename TKey, typename TValue, typename TFactor>
inline InterpolationMap<TKey, TValue, TFactor>& InterpolationMap<TKey, TValue, TFactor>::operator=(InterpolationMap<TKey, TValue, TFactor>&& object) noexcept
{
	if (this != &object)
	{
		interpolationValueMap = std::move(object.interpolationValueMap);
	}

	return *this;
}

template <typename TKey, typename TValue, typename TFactor>
inline bool InterpolationMap<TKey, TValue, TFactor>::operator==(const InterpolationMap<TKey, TValue, TFactor>& object) const
{
	return interpolationValueMap == object.interpolationValueMap;
}

template <typename TKey, typename TValue, typename TFactor>
inline bool InterpolationMap<TKey, TValue, TFactor>::operator!=(const InterpolationMap<TKey, TValue, TFactor>& object) const
{
	return !(*this == object);
}

template <typename TKey, typename TValue, typename TFactor>
inline TValue& InterpolationMap<TKey, TValue, TFactor>::operator[](const TKey& key)
{
	return value(key);
}

}

#endif // META_OCEAN_MATH_INTERPOLATION_H
