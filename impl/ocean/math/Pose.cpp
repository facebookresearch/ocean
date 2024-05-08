/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/Pose.h"
#include "ocean/math/Interpolation.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/SuccessionSubset.h"

namespace Ocean
{

template <typename T>
PoseT<T>::PoseT()
{
	values_[0] = T(0);
	values_[1] = T(0);
	values_[2] = T(0);
	values_[3] = T(0);
	values_[4] = T(0);
	values_[5] = T(0);
}

template <typename T>
PoseT<T>::PoseT(const VectorT3<T>& translation)
{
	values_[0] = translation[0];
	values_[1] = translation[1];
	values_[2] = translation[2];

	values_[3] = T(0);
	values_[4] = T(0);
	values_[5] = T(0);
}

template <typename T>
PoseT<T>::PoseT(const EulerT<T>& euler)
{
	ocean_assert(euler.isValid());

	values_[0] = T(0);
	values_[1] = T(0);
	values_[2] = T(0);

	const RotationT<T> rotation(euler);
	ocean_assert(rotation.isValid());

	values_[3] = rotation[0] * rotation[3];
	values_[4] = rotation[1] * rotation[3];
	values_[5] = rotation[2] * rotation[3];
}

template <typename T>
PoseT<T>::PoseT(const QuaternionT<T>& quaternion)
{
	ocean_assert(quaternion.isValid());

	values_[0] = T(0);
	values_[1] = T(0);
	values_[2] = T(0);

	const RotationT<T> rotation(quaternion);
	ocean_assert(rotation.isValid());

	values_[3] = rotation[0] * rotation[3];
	values_[4] = rotation[1] * rotation[3];
	values_[5] = rotation[2] * rotation[3];
}

template <typename T>
PoseT<T>::PoseT(const RotationT<T>& rotation)
{
	ocean_assert(rotation.isValid());

	values_[0] = T(0);
	values_[1] = T(0);
	values_[2] = T(0);

	values_[3] = rotation[0] * rotation[3];
	values_[4] = rotation[1] * rotation[3];
	values_[5] = rotation[2] * rotation[3];
}

template <typename T>
PoseT<T>::PoseT(const HomogenousMatrixT4<T>& transformation)
{
	ocean_assert(transformation.rotationMatrix().isOrthonormal(NumericT<T>::weakEps()));

	const VectorT3<T> translation(transformation.translation());
	const RotationT<T> rotation(transformation.rotationMatrix());

	values_[0] = translation[0];
	values_[1] = translation[1];
	values_[2] = translation[2];

	values_[3] = rotation[0] * rotation[3];
	values_[4] = rotation[1] * rotation[3];
	values_[5] = rotation[2] * rotation[3];
}

template <typename T>
PoseT<T>::PoseT(const T* values)
{
	ocean_assert(values != nullptr);

	values_[0] = values[0];
	values_[1] = values[1];
	values_[2] = values[2];
	values_[3] = values[3];
	values_[4] = values[4];
	values_[5] = values[5];
}

template <typename T>
PoseT<T>::PoseT(const T tx, const T ty, const T tz, const T rx, const T ry, const T rz)
{
	values_[0] = tx;
	values_[1] = ty;
	values_[2] = tz;

	values_[3] = rx;
	values_[4] = ry;
	values_[5] = rz;
}

template <typename T>
PoseT<T>::PoseT(const VectorT3<T>& translation, const EulerT<T>& euler)
{
	ocean_assert(euler.isValid());

	values_[0] = translation[0];
	values_[1] = translation[1];
	values_[2] = translation[2];

	const RotationT<T> rotation(euler);
	ocean_assert(rotation.isValid());

	values_[3] = rotation[0] * rotation[3];
	values_[4] = rotation[1] * rotation[3];
	values_[5] = rotation[2] * rotation[3];
}

template <typename T>
PoseT<T>::PoseT(const VectorT3<T>& translation, const QuaternionT<T>& quaternion)
{
	ocean_assert(quaternion.isValid());

	values_[0] = translation[0];
	values_[1] = translation[1];
	values_[2] = translation[2];

	const RotationT<T> rotation(quaternion);
	ocean_assert(rotation.isValid());

	values_[3] = rotation[0] * rotation[3];
	values_[4] = rotation[1] * rotation[3];
	values_[5] = rotation[2] * rotation[3];
}

template <typename T>
PoseT<T>::PoseT(const VectorT3<T>& translation, const RotationT<T>& rotation)
{
	ocean_assert(rotation.isValid());

	values_[0] = translation[0];
	values_[1] = translation[1];
	values_[2] = translation[2];

	values_[3] = rotation[0] * rotation[3];
	values_[4] = rotation[1] * rotation[3];
	values_[5] = rotation[2] * rotation[3];
}

template <typename T>
QuaternionT<T> PoseT<T>::orientation() const
{
	if (NumericT<T>::isEqualEps(values_[3]) && NumericT<T>::isEqualEps(values_[4]) && NumericT<T>::isEqualEps(values_[5]))
	{
		return QuaternionT<T>();
	}

	const VectorT3<T> axis(values_ + 3);
	const T angle = axis.length();

	ocean_assert(NumericT<T>::isNotEqualEps(angle));

	return QuaternionT<T>(axis / angle, angle);
}

template <typename T>
HomogenousMatrixT4<T> PoseT<T>::transformation() const
{
	return HomogenousMatrixT4<T>(translation(), orientation());
}

template <typename T>
T PoseT<T>::angle() const
{
	const VectorT3<T> axis(values_ + 3);

	return axis.length();
}

template <typename T>
bool PoseT<T>::isNull() const
{
	return NumericT<T>::isEqualEps(values_[0]) && NumericT<T>::isEqualEps(values_[1]) && NumericT<T>::isEqualEps(values_[2])
				&& NumericT<T>::isEqualEps(values_[3]) && NumericT<T>::isEqualEps(values_[4]) && NumericT<T>::isEqualEps(values_[5]);
}

template <typename T>
PoseT<T> PoseT<T>::operator+(const PoseT<T>& pose) const
{
	return PoseT<T>(values_[0] + pose.values_[0], values_[1] + pose.values_[1], values_[2] + pose.values_[2],
						values_[3] + pose.values_[3], values_[4] + pose.values_[4], values_[5] + pose.values_[5]);
}

template <typename T>
PoseT<T>& PoseT<T>::operator+=(const PoseT<T>& pose)
{
	values_[0] += pose.values_[0];
	values_[1] += pose.values_[1];
	values_[2] += pose.values_[2];
	values_[3] += pose.values_[3];
	values_[4] += pose.values_[4];
	values_[5] += pose.values_[5];

	return *this;
}

template <typename T>
PoseT<T> PoseT<T>::operator-(const PoseT<T>& pose) const
{
	return PoseT<T>(values_[0] - pose.values_[0], values_[1] - pose.values_[1], values_[2] - pose.values_[2],
						values_[3] - pose.values_[3], values_[4] - pose.values_[4], values_[5] - pose.values_[5]);
}

template <typename T>
PoseT<T>& PoseT<T>::operator-=(const PoseT<T>& pose)
{
	values_[0] -= pose.values_[0];
	values_[1] -= pose.values_[1];
	values_[2] -= pose.values_[2];
	values_[3] -= pose.values_[3];
	values_[4] -= pose.values_[4];
	values_[5] -= pose.values_[5];

	return *this;
}

template <typename T>
PoseT<T> PoseT<T>::operator*(const T value) const
{
	return PoseT<T>(values_[0] * value, values_[1] * value, values_[2] * value, values_[3] * value, values_[4] * value, values_[5] * value);
}

template <typename T>
PoseT<T>& PoseT<T>::operator*=(const T value)
{
	values_[0] *= value;
	values_[1] *= value;
	values_[2] *= value;
	values_[3] *= value;
	values_[4] *= value;
	values_[5] *= value;

	return *this;
}

template <typename T>
Indices32 PoseT<T>::representativePoses(const PosesT<T>& poses, const size_t subsetSize, const size_t explicitIndexOffset)
{
	ocean_assert(subsetSize >= 1 && subsetSize <= poses.size());

	ocean_assert(NumericT<unsigned int>::isInsideValueRange(subsetSize));

	if (subsetSize >= poses.size())
	{
		return createIndices<unsigned int>(poses.size(), (unsigned int)(explicitIndexOffset));
	}

	SuccessionSubset<T, 6> successionPoses((typename SuccessionSubset<T, 6>::Object*)(poses.data()), poses.size());
	const typename SuccessionSubset<T, 6>::Indices indices(successionPoses.subset(subsetSize));

	Indices32 indices32(indices.size());

	for (size_t n = 0; n < indices.size(); ++n)
	{
		indices32[n] = Index32(indices[n] + explicitIndexOffset);
	}

	return indices32;
}

template <typename T>
Indices32 PoseT<T>::representativePoses(const HomogenousMatrixT4<T>* poseMatrices, const size_t size, const size_t subsetSize, const size_t explicitIndexOffset)
{
	ocean_assert(subsetSize >= 1);

	ocean_assert(NumericT<unsigned int>::isInsideValueRange(subsetSize));

	if (subsetSize >= size)
	{
		return createIndices<unsigned int>(size, (unsigned int)(explicitIndexOffset));
	}

	PosesT<T> poses;
	poses.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		poses.emplace_back(poseMatrices[n]);
	}

	return representativePoses(poses, subsetSize, explicitIndexOffset);
}

template <typename T>
HomogenousMatrixT4<T> PoseT<T>::linearPoseInterpolation(const HomogenousMatrixT4<T>& pose0, const HomogenousMatrixT4<T>& pose1, const T& factor)
{
	ocean_assert(pose0.isValid() && pose1.isValid());
	ocean_assert(factor >= 0 && factor <= 1);

	ocean_assert(pose0.orthonormalRotationMatrix() == pose0.rotationMatrix());
	ocean_assert(pose1.orthonormalRotationMatrix() == pose1.rotationMatrix());

	// we handle the special interpolation cases ensuring that we return the exact camera poses
	if (factor <= T(0))
	{
		return pose0;
	}

	if (factor >= T(1))
	{
		return pose1;
	}

	return HomogenousMatrixT4<T>(Interpolation::linear(std::make_pair(pose0.translation(), pose0.rotation()), std::make_pair(pose1.translation(), pose1.rotation()), factor));
}

template class OCEAN_MATH_EXPORT PoseT<float>;
template class OCEAN_MATH_EXPORT PoseT<double>;

}
