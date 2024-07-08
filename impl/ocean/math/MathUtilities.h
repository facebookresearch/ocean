/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_MATH_UTILITIES_H
#define META_OCEAN_MATH_MATH_UTILITIES_H

#include "ocean/math/Math.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

/**
 * This class implements utility functions for the math library.
 * @ingroup math
 */
class MathUtilities
{
	public:

		/**
		 * Transforms several objects (e.g., 3D vector or 4D vectors) by a transformation object (e.g., a 3x3 matrix or 4x4 matrix) using the multiplication operator.
		 * @param transformation The transformation to be used (e.g., a matrix)
		 * @param objects The object to be transformed (e.g. vectors)
		 * @param results The resulting transformed objects, the size of the vector will be adjusted to match the size of the provided objects
		 * @param worker Optional worker object to distribute the computation
		 * @param minimalObjectsPerThread Minimal number of objects assigned to one internal thread, with range [1, infinity)
		 */
		template <typename TTransformation, typename TObject>
		static inline void transform(const TTransformation& transformation, const std::vector<TObject>& objects, std::vector<TObject>& results, Worker* worker = nullptr, const unsigned int minimalObjectsPerThread = 1000u);

		/**
		 * Encodes a float value to four uint8 values.
		 * @param value The float value  to decompose, with range [0, 1]
		 * @param decomposedValues The resulting four uint8 values, must be valid
		 */
		static inline void encodeFloatToUint8(const float value, uint8_t* decomposedValues);

		/**
		 * Decodes four uint8 values into a float value
		 * @param decomposedValues The four uint8 values to decompose, must be valid
		 * @return The resulting decomposed float value, with range [0, 1]
		 */
		static inline float decodeFloatFromUint8(const uint8_t* decomposedValues);

	protected:

		/**
		 * Transforms a subset of several objects (e.g., 3D vector or 4D vectors) by a transformation object (e.g., a 3x3 matrix or 4x4 matrix) using the multiplication operator.
		 * @param transformation The transformation to be used (e.g., a matrix)
		 * @param objects The object to be transformed (e.g. vectors)
		 * @param results The resulting transformed objects, the size of the vector will be adjusted to match the size of the provided objects
		 * @param firstObject The first object to be handled
		 * @param numberObjects The number of objects to be handled
		 */
		template <typename TTransformation, typename TObject>
		static inline void transformSubset(const TTransformation* transformation, const TObject* objects, TObject* results, const unsigned int firstObject, const unsigned int numberObjects);
};

template <typename TTransformation, typename TObject>
inline void MathUtilities::transform(const TTransformation& transformation, const std::vector<TObject>& objects, std::vector<TObject>& results, Worker* worker, const unsigned int minimalObjectsPerThread)
{
	ocean_assert(minimalObjectsPerThread >= 1u);
	results.resize(objects.size());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(transformSubset<TTransformation, TObject>, &transformation, objects.data(), results.data(), 0u, 0u), 0u, (unsigned int)objects.size(), 3u, 4u, minimalObjectsPerThread);
	}
	else
	{
		transformSubset<TTransformation, TObject>(&transformation, objects.data(), results.data(), 0u, (unsigned int)objects.size());
	}
}

inline void MathUtilities::encodeFloatToUint8(const float value, uint8_t* decomposedValues)
{
	ocean_assert(0.0f <= value && value <= 1.0f);
	ocean_assert(decomposedValues != nullptr);

	const float value1 = value * 255.0f;
	const float value2 = value * 255.0f * 255.0f; // 255^2
	const float value3 = value * 255.0f * 255.0f * 255.0f; // 255^3

	const int int0 = int(value1);
	const int int1 = int((value1 - float(int(value1))) * 255.0f);
	const int int2 = int((value2 - float(int(value2))) * 255.0f);
	const int int3 = int((value3 - float(int(value3))) * 255.0f);

	ocean_assert(int0 >= 0 && int0 <= 255);
	ocean_assert(int1 >= 0 && int1 <= 255);
	ocean_assert(int2 >= 0 && int2 <= 255);
	ocean_assert(int3 >= 0 && int3 <= 255);

	decomposedValues[0] = uint8_t(int0);
	decomposedValues[1] = uint8_t(int1);
	decomposedValues[2] = uint8_t(int2);
	decomposedValues[3] = uint8_t(int3);
}

float MathUtilities::decodeFloatFromUint8(const uint8_t* decomposedValues)
{
	ocean_assert(decomposedValues != nullptr);

	return float(decomposedValues[0]) / 255.0f
				+ float(decomposedValues[1]) / (255.0f * 255.0f)
				+ float(decomposedValues[2]) / (255.0f * 255.0f * 255.0f)
				+ float(decomposedValues[3]) / (255.0f * 255.0f * 255.0f * 255.0f);
}

template <>
inline void MathUtilities::transformSubset<SquareMatrixF3, VectorF3>(const SquareMatrixF3* transformation, const VectorF3* objects, VectorF3* results, const unsigned int firstObject, const unsigned int numberObjects)
{
	ocean_assert(transformation != nullptr && objects != nullptr && results != nullptr);

	SquareMatrixF3::multiply(*transformation, objects + firstObject, results + firstObject, size_t(numberObjects));
}

template <>
inline void MathUtilities::transformSubset<SquareMatrixD4, VectorD4>(const SquareMatrixD4* transformation, const VectorD4* objects, VectorD4* results, const unsigned int firstObject, const unsigned int numberObjects)
{
	ocean_assert(transformation != nullptr && objects != nullptr && results != nullptr);

	SquareMatrixD4::multiply(*transformation, objects + firstObject, results + firstObject, size_t(numberObjects));
}

template <>
inline void MathUtilities::transformSubset<SquareMatrixF4, VectorF4>(const SquareMatrixF4* transformation, const VectorF4* objects, VectorF4* results, const unsigned int firstObject, const unsigned int numberObjects)
{
	ocean_assert(transformation != nullptr && objects != nullptr && results != nullptr);

	SquareMatrixF4::multiply(*transformation, objects + firstObject, results + firstObject, size_t(numberObjects));
}

template <typename TTransformation, typename TObject>
inline void MathUtilities::transformSubset(const TTransformation* transformation, const TObject* objects, TObject* results, const unsigned int firstObject, const unsigned int numberObjects)
{
	ocean_assert(transformation != nullptr && objects != nullptr && results != nullptr);

	for (unsigned int n = firstObject; n < firstObject + numberObjects; ++n)
	{
		results[n] = *transformation * objects[n];
	}
}

}

#endif // META_OCEAN_MATH_MATH_UTILITIES_H
