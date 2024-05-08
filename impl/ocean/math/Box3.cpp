/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/Box3.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

template <typename T>
BoxT3<T>::BoxT3(const VectorT3<T>& lower, const VectorT3<T>& higher) :
	lower_(lower),
	higher_(higher)
{
	// nothing to do here
}

template <typename T>
BoxT3<T>::BoxT3(const VectorsT3<T>& points) :
	BoxT3(points.data(), points.size())
{
	// nothing to do here
}

template <typename T>
BoxT3<T>::BoxT3(const VectorT3<T>* points, const size_t number) :
	lower_(NumericT<T>::maxValue(), NumericT<T>::maxValue(), NumericT<T>::maxValue()),
	higher_(NumericT<T>::minValue(), NumericT<T>::minValue(), NumericT<T>::minValue())
{
	ocean_assert(points != nullptr && number > 0);

	for (size_t n = 0; n < number; ++n)
	{
		const VectorT3<T>& point = points[n];

		lower_.x() = min(point.x(), lower_.x());
		lower_.y() = min(point.y(), lower_.y());
		lower_.z() = min(point.z(), lower_.z());

		higher_.x() = max(point.x(), higher_.x());
		higher_.y() = max(point.y(), higher_.y());
		higher_.z() = max(point.z(), higher_.z());
	}

	for (size_t n = 0; n < number; ++n)
	{
		ocean_assert(isInside(points[n]));
	}

	ocean_assert(isValid());
}

template <typename T>
BoxT3<T>::BoxT3(const VectorT3<T>& center, const T xSize, const T ySize, const T zSize)
{
	ocean_assert(xSize >= T(0) && ySize >= T(0) && zSize >= T(0));

	const T xSize_2 = xSize * T(0.5);
	const T ySize_2 = ySize * T(0.5);
	const T zSize_2 = zSize * T(0.5);

	lower_ = center - VectorT3<T>(xSize_2, ySize_2, zSize_2);
	higher_ = center + VectorT3<T>(xSize_2, ySize_2, zSize_2);

	ocean_assert(center.isEqual(this->center(), NumericT<T>::weakEps()));

	ocean_assert(NumericT<T>::isWeakEqual(xSize, xDimension()));
	ocean_assert(NumericT<T>::isWeakEqual(ySize, yDimension()));
	ocean_assert(NumericT<T>::isWeakEqual(zSize, zDimension()));
}

template <typename T>
VectorT3<T> BoxT3<T>::center() const
{
	return VectorT3<T>((higher_.x() + lower_.x()) * T(0.5),
					(higher_.y() + lower_.y()) * T(0.5),
					(higher_.z() + lower_.z()) * T(0.5));
}

template <typename T>
T BoxT3<T>::sqrDiagonal() const
{
	return higher_.sqrDistance(lower_);
}

template <typename T>
T BoxT3<T>::diagonal() const
{
	return higher_.distance(lower_);
}

template <typename T>
T BoxT3<T>::xDimension() const
{
	ocean_assert(isValid());
	return higher_.x() - lower_.x();
}

template <typename T>
T BoxT3<T>::yDimension() const
{
	ocean_assert(isValid());
	return higher_.y() - lower_.y();
}

template <typename T>
T BoxT3<T>::zDimension() const
{
	ocean_assert(isValid());
	return higher_.z() - lower_.z();
}

template <typename T>
VectorT3<T> BoxT3<T>::dimension() const
{
	ocean_assert(isValid());

	return VectorT3<T>(xDimension(), yDimension(), zDimension());
}

template <typename T>
bool BoxT3<T>::isPoint(VectorT3<T>* point) const
{
	if (isValid() && NumericT<T>::isEqual(higher_.x(), lower_.x()) && NumericT<T>::isEqual(higher_.y(), lower_.y()) && NumericT<T>::isEqual(higher_.z(), lower_.z()))
	{
		if (point)
		{
			*point = higher_;
		}

		return true;
	}

	return false;
}

template <typename T>
bool BoxT3<T>::isPlanar(PlaneT3<T>& plane) const
{
	if (isValid())
	{
		if (NumericT<T>::isEqual(higher_.x(), lower_.x()))
		{
			plane = PlaneT3<T>(VectorT3<T>(higher_.x(), 0, 0), VectorT3<T>(1, 0, 0));
			return true;
		}

		if (NumericT<T>::isEqual(higher_.y(), lower_.y()))
		{
			plane = PlaneT3<T>(VectorT3<T>(0, higher_.y(), 0), VectorT3<T>(0, 1, 0));
			return true;
		}

		if (NumericT<T>::isEqual(higher_.z(), lower_.z()))
		{
			plane = PlaneT3<T>(VectorT3<T>(0, 0, higher_.z()), VectorT3<T>(0, 0, 1));
			return true;
		}
	}

	return false;
}

template <typename T>
const VectorT3<T>& BoxT3<T>::lower() const
{
	return lower_;
}

template <typename T>
const VectorT3<T>& BoxT3<T>::higher() const
{
	return higher_;
}

template <typename T>
bool BoxT3<T>::isInside(const VectorT3<T>& point, const T eps) const
{
	ocean_assert(isValid());
	ocean_assert(eps >= T(0));

	return !(point.x() < lower_.x() - eps || point.x() > higher_.x() + eps
			|| point.y() < lower_.y() - eps  || point.y() > higher_.y() + eps
			|| point.z() < lower_.z() - eps  || point.z() > higher_.z() + eps);
}

template <typename T>
bool BoxT3<T>::isOnSurface(const VectorT3<T>& point, const T epsilon) const
{
	ocean_assert(epsilon >= 0);

	return ((NumericT<T>::isEqual(lower_.x(), point.x(), epsilon) || NumericT<T>::isEqual(higher_.x(), point.x(), epsilon)) && NumericT<T>::isInsideRange(lower_.y(), point.y(), higher_.y(), epsilon) && NumericT<T>::isInsideRange(lower_.z(), point.z(), higher_.z(), epsilon))
				|| ((NumericT<T>::isEqual(lower_.y(), point.y(), epsilon) || NumericT<T>::isEqual(higher_.y(), point.y(), epsilon)) && NumericT<T>::isInsideRange(lower_.x(), point.x(), higher_.x(), epsilon) && NumericT<T>::isInsideRange(lower_.z(), point.z(), higher_.z(), epsilon))
				|| ((NumericT<T>::isEqual(lower_.z(), point.z(), epsilon) || NumericT<T>::isEqual(higher_.z(), point.z(), epsilon)) && NumericT<T>::isInsideRange(lower_.x(), point.x(), higher_.x(), epsilon) && NumericT<T>::isInsideRange(lower_.y(), point.y(), higher_.y(), epsilon));
}

template <typename T>
bool BoxT3<T>::hasIntersection(const LineT3<T>& ray, const HomogenousMatrixT4<T>& box_T_ray) const
{
	ocean_assert(box_T_ray.isValid());
	ocean_assert(ray.isValid());

	return hasIntersection(LineT3<T>(box_T_ray * ray.point(), box_T_ray.rotationMatrix(ray.direction())));
}


template <typename T>
bool BoxT3<T>::hasIntersection(const LineT3<T>& ray) const
{
	ocean_assert(ray.isValid());

	const VectorT3<T>& rayPoint(ray.point());
	const VectorT3<T>& rayDirection(ray.direction());

	// front size (z+)
	if (rayDirection.z() < -NumericT<T>::eps())
	{
		ocean_assert(ray.direction() * VectorT3<T>(0, 0, 1) < 0);

		// p + t * d == box
		// t = (box - p) / d
		/*const T t = (higher_.z() - ray.point().z()) / ray.direction().z();
		const T x = ray.point().x() + ray.direction().x() * t;
		const T y = ray.point().y() + ray.direction().y() * t;

		if (x >= lower_.x() && x <= higher_.x()
			&& y >= lower_.y() && y <= higher_.y())
			return true;*/

		const T xdz = rayPoint.x() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.x();
		const T ydz = rayPoint.y() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.y();

		if (xdz <= lower_.x() * rayDirection.z() && xdz >= higher_.x() * rayDirection.z()
				&& ydz <= lower_.y() * rayDirection.z() && ydz >= higher_.y() * rayDirection.z())
		{
			return true;
		}
	}

	// back size (z-)
	if (rayDirection.z() > NumericT<T>::eps())
	{
		ocean_assert(ray.direction() * VectorT3<T>(0, 0, -1) < 0);

		/*const T t = (lower_.z() - ray.point().z()) / ray.direction(). z();
		const T x = ray.point().x() + ray.direction().x() * t;
		const T y = ray.point().y() + ray.direction().y() * t;

		if (x >= lower_.x() && x <= higher_.x()
			&& y >= lower_.y() && y <= higher_.y())
			return true;*/

		const T xdz = rayPoint.x() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.x();
		const T ydz = rayPoint.y() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.y();

		if (xdz >= lower_.x() * rayDirection.z() && xdz <= higher_.x() * rayDirection.z()
				&& ydz >= lower_.y() * rayDirection.z() && ydz <= higher_.y() * rayDirection.z())
		{
			return true;
		}
	}

	// right size (x+)
	if (rayDirection.x() < -NumericT<T>::eps())
	{
		ocean_assert(ray.direction() * VectorT3<T>(1, 0, 0) < 0);

		/*const T t = (higher_.x() - ray.point().x()) / ray.direction().x();
		const T y = ray.point().y() + ray.direction().y() * t;
		const T z = ray.point().z() + ray.direction().z() * t;

		if (y >= lower_.y() && y <= higher_.y()
			&& z >= lower_.z() && z <= higher_.z())
			return true;*/

		const T ydx = rayPoint.y() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.y();
		const T zdx = rayPoint.z() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.z();

		if (ydx <= lower_.y() * rayDirection.x() && ydx >= higher_.y() * rayDirection.x()
				&& zdx <= lower_.z() * rayDirection.x() && zdx >= higher_.z() * rayDirection.x())
		{
			return true;
		}
	}

	// left size (x-)
	if (rayDirection.x() > NumericT<T>::eps())
	{
		ocean_assert(ray.direction() * VectorT3<T>(-1, 0, 0) < 0);

		/*const T t = (lower_.x() - ray.point().x()) / ray.direction().x();
		const T y = ray.point().y() + ray.direction().y() * t;
		const T z = ray.point().z() + ray.direction().z() * t;

		if (y >= lower_.y() && y <= higher_.y()
			&& z >= lower_.z() && z <= higher_.z())
			return true;*/

		const T ydx = rayPoint.y() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.y();
		const T zdx = rayPoint.z() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.z();

		if (ydx >= lower_.y() * rayDirection.x() && ydx <= higher_.y() * rayDirection.x()
				&& zdx >= lower_.z() * rayDirection.x() && zdx <= higher_.z() * rayDirection.x())
		{
			return true;
		}
	}

	// top size (y+)
	if (rayDirection.y() < -NumericT<T>::eps())
	{
		ocean_assert(ray.direction() * VectorT3<T>(0, 1, 0) < 0);

		/*const T t = (higher_.y() - ray.point().y()) / ray.direction().y();
		const T x = ray.point().x() + ray.direction().x() * t;
		const T z = ray.point().z() + ray.direction().z() * t;

		if (x >= lower_.x() && x <= higher_.x()
			&& z >= lower_.z() && z <= higher_.z())
			return true;*/

		const T xdy = rayPoint.x() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.x();
		const T zdy = rayPoint.z() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.z();

		if (xdy <= lower_.x() * rayDirection.y() && xdy >= higher_.x() * rayDirection.y()
				&& zdy <= lower_.z() * rayDirection.y() && zdy >= higher_.z() * rayDirection.y())
		{
			return true;
		}
	}

	// bottom size (y-)
	if (rayDirection.y() > NumericT<T>::eps())
	{
		ocean_assert(ray.direction() * VectorT3<T>(0, -1, 0) < 0);

		/*const T t = (lower_.y() - ray.point().y()) / ray.direction().y();
		const T x = ray.point().x() + ray.direction().x() * t;
		const T z = ray.point().z() + ray.direction().z() * t;

		if (x >= lower_.x() && x <= higher_.x()
			&& z >= lower_.z() && z <= higher_.z())
			return true;*/

		const T xdy = rayPoint.x() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.x();
		const T zdy = rayPoint.z() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.z();

		if (xdy >= lower_.x() * rayDirection.y() && xdy <= higher_.x() * rayDirection.y()
				&& zdy >= lower_.z() * rayDirection.y() && zdy <= higher_.z() * rayDirection.y())
		{
			return true;
		}
	}

	return false;
}

template <typename T>
bool BoxT3<T>::hasIntersection(const LineT3<T>& ray, const T epsPerDistance) const
{
	ocean_assert(ray.isValid());
	ocean_assert(epsPerDistance >= 0);

	// we approximate the distance instead of applying the sqrt
	const T distance = std::max(std::max(NumericT<T>::abs(ray.point().x() - center().x()), NumericT<T>::abs(ray.point().y() - center().y())), NumericT<T>::abs(ray.point().z() - center().z()));

	const T eps = epsPerDistance * distance;

	BoxT3 epsBox;
	epsBox.lower_ = VectorT3<T>(lower_.x() - eps, lower_.y() - eps, lower_.z() - eps);
	epsBox.higher_ = VectorT3<T>(higher_.x() + eps, higher_.y() + eps, higher_.z() + eps);

	return epsBox.hasIntersection(ray);
}

template <typename T>
unsigned int BoxT3<T>::corners(VectorT3<T>* corners) const
{
	ocean_assert(corners != nullptr);

	if (!isValid())
	{
		return 0u;
	}

	if (NumericT<T>::isEqual(higher_.x(), lower_.x()))
	{
		if (NumericT<T>::isEqual(higher_.y(), lower_.y()))
		{
			// if the box holds one points only
			if (NumericT<T>::isEqual(higher_.z(), lower_.z()))
			{
				corners[0] = higher_;
				return 1u;
			}

			// the box holds points on a line
			corners[0] = VectorT3<T>(higher_.x(), higher_.y(), lower_.z());
			corners[1] = higher_;
			return 2u;
		}
		else if (NumericT<T>::isEqual(higher_.z(), lower_.z()))
		{
			// the box holds points on a line
			corners[0] = VectorT3<T>(higher_.x(), lower_.y(), higher_.z());
			corners[1] = higher_;
			return 2u;
		}

		// the box holds points on a YZ plane
		corners[0] = VectorT3<T>(higher_.x(), lower_.y(), lower_.z());
		corners[1] = VectorT3<T>(higher_.x(), lower_.y(), higher_.z());
		corners[2] = VectorT3<T>(higher_.x(), higher_.y(), lower_.z());
		corners[3] = VectorT3<T>(higher_.x(), higher_.y(), higher_.z());
		return 4u;
	}

	if (NumericT<T>::isEqual(higher_.y(), lower_.y()))
	{
		ocean_assert(NumericT<T>::isNotEqual(higher_.x(), lower_.x()));

		// if the box holds points on a line
		if (NumericT<T>::isEqual(higher_.z(), lower_.z()))
		{
			corners[0] = VectorT3<T>(lower_.x(), higher_.y(), higher_.z());
			corners[1] = higher_;
			return 2u;
		}

		// the box holds points on a XZ plane
		corners[0] = VectorT3<T>(lower_.x(), higher_.y(), lower_.z());
		corners[1] = VectorT3<T>(lower_.x(), higher_.y(), higher_.z());
		corners[2] = VectorT3<T>(higher_.x(), higher_.y(), lower_.z());
		corners[3] = VectorT3<T>(higher_.x(), higher_.y(), higher_.z());
		return 4u;
	}

	if (NumericT<T>::isEqual(higher_.z(), lower_.z()))
	{
		ocean_assert(NumericT<T>::isNotEqual(higher_.x(), lower_.x()));
		ocean_assert(NumericT<T>::isNotEqual(higher_.y(), lower_.y()));

		// the box holds points on a XY plane
		corners[0] = VectorT3<T>(lower_.x(), lower_.y(), higher_.z());
		corners[1] = VectorT3<T>(lower_.x(), higher_.y(), higher_.z());
		corners[2] = VectorT3<T>(higher_.x(), lower_.y(), higher_.z());
		corners[3] = VectorT3<T>(higher_.x(), higher_.y(), higher_.z());
		return 4u;
	}

	ocean_assert(NumericT<T>::isNotEqual(higher_.x(), lower_.x()));
	ocean_assert(NumericT<T>::isNotEqual(higher_.y(), lower_.y()));
	ocean_assert(NumericT<T>::isNotEqual(higher_.z(), lower_.z()));

	// the box holds points inside the 3D space
	corners[0] = VectorT3<T>(lower_.x(), lower_.y(), lower_.z());
	corners[1] = VectorT3<T>(lower_.x(), lower_.y(), higher_.z());
	corners[2] = VectorT3<T>(lower_.x(), higher_.y(), lower_.z());
	corners[3] = VectorT3<T>(lower_.x(), higher_.y(), higher_.z());
	corners[4] = VectorT3<T>(higher_.x(), lower_.y(), lower_.z());
	corners[5] = VectorT3<T>(higher_.x(), lower_.y(), higher_.z());
	corners[6] = VectorT3<T>(higher_.x(), higher_.y(), lower_.z());
	corners[7] = VectorT3<T>(higher_.x(), higher_.y(), higher_.z());
	return 8u;
}

template <typename T>
BoxT3<T> BoxT3<T>::expanded(const VectorT3<T>& offsets) const
{
	ocean_assert(isValid());

	const T xSize = std::max(T(0), xDimension() + offsets.x());
	const T ySize = std::max(T(0), yDimension() + offsets.y());
	const T zSize = std::max(T(0), zDimension() + offsets.z());

	return BoxT3<T>(center(), xSize, ySize, zSize);
}

template <typename T>
BoxT3<T>& BoxT3<T>::expand(const VectorT3<T>& offsets)
{
	ocean_assert(isValid());

	*this = expanded(offsets);

	return *this;
}

template <typename T>
void BoxT3<T>::clear()
{
	higher_ = VectorT3<T>(NumericT<T>::minValue(), NumericT<T>::minValue(), NumericT<T>::minValue());
	lower_ = VectorT3<T>(NumericT<T>::maxValue(), NumericT<T>::maxValue(), NumericT<T>::maxValue());
}

template <typename T>
bool BoxT3<T>::isEqual(const BoxT3& box, const T epsilon) const
{
	ocean_assert(epsilon >= T(0));

	return lower_.isEqual(box.lower_, epsilon) && higher_.isEqual(box.higher_, epsilon);
}

template <typename T>
bool BoxT3<T>::isValid() const
{
	return higher_.x() != NumericT<T>::minValue() && higher_.y() != NumericT<T>::minValue() && higher_.z() != NumericT<T>::minValue()
			&& lower_.x() != NumericT<T>::maxValue() && lower_.y() != NumericT<T>::maxValue() && lower_.z() != NumericT<T>::maxValue();
}

template <typename T>
BoxT3<T> BoxT3<T>::operator*(const T factor) const
{
	ocean_assert(isValid());

	VectorT3<T> halfDimensions((higher_.x() - lower_.x()) * T(0.5),
									(higher_.y() - lower_.y()) * T(0.5),
									(higher_.z() - lower_.z()) * T(0.5));

	const VectorT3<T> center(lower_ + halfDimensions);
	halfDimensions *= factor;

	return BoxT3<T>(center - halfDimensions, center + halfDimensions);
}

template <typename T>
BoxT3<T>& BoxT3<T>::operator*=(const T factor)
{
	ocean_assert(isValid());

	VectorT3<T> halfDimensions((higher_.x() - lower_.x()) * T(0.5),
									(higher_.y() - lower_.y()) * T(0.5),
									(higher_.z() - lower_.z()) * T(0.5));

	const VectorT3<T> center(lower_ + halfDimensions);
	halfDimensions *= factor;

	lower_ = center - halfDimensions;
	higher_ = center + halfDimensions;

	return *this;
}

template <typename T>
BoxT3<T>& BoxT3<T>::operator+=(const VectorT3<T>& point)
{
	lower_.x() = min(point.x(), lower_.x());
	lower_.y() = min(point.y(), lower_.y());
	lower_.z() = min(point.z(), lower_.z());

	higher_.x() = max(point.x(), higher_.x());
	higher_.y() = max(point.y(), higher_.y());
	higher_.z() = max(point.z(), higher_.z());

	ocean_assert(isValid());

	return *this;
}

template <typename T>
BoxT3<T> BoxT3<T>::operator+(const BoxT3<T>& right) const
{
	ocean_assert(isValid() && right.isValid());

	return BoxT3<T>(VectorT3<T>(min(lower_.x(), right.lower_.x()), min(lower_.y(), right.lower_.y()), min(lower_.z(), right.lower_.z())),
						VectorT3<T>(max(higher_.x(), right.higher_.x()), max(higher_.y(), right.higher_.y()), max(higher_.z(), right.higher_.z())));
}

template <typename T>
BoxT3<T>& BoxT3<T>::operator+=(const BoxT3<T>& right)
{
	ocean_assert(right.isValid());

	lower_.x() = min(lower_.x(), right.lower_.x());
	lower_.y() = min(lower_.y(), right.lower_.y());
	lower_.z() = min(lower_.z(), right.lower_.z());

	higher_.x() = max(higher_.x(), right.higher_.x());
	higher_.y() = max(higher_.y(), right.higher_.y());
	higher_.z() = max(higher_.z(), right.higher_.z());

	return *this;
}

template <typename T>
BoxT3<T> BoxT3<T>::operator*(const HomogenousMatrixT4<T>& world_T_box) const
{
	ocean_assert(isValid() && world_T_box.isValid());

	const VectorT3<T> corners[8] =
	{
		VectorT3<T>(lower_.x(), lower_.y(), lower_.z()),
		VectorT3<T>(lower_.x(), lower_.y(), higher_.z()),
		VectorT3<T>(lower_.x(), higher_.y(), lower_.z()),
		VectorT3<T>(lower_.x(), higher_.y(), higher_.z()),
		VectorT3<T>(higher_.x(), lower_.y(), lower_.z()),
		VectorT3<T>(higher_.x(), lower_.y(), higher_.z()),
		VectorT3<T>(higher_.x(), higher_.y(), lower_.z()),
		VectorT3<T>(higher_.x(), higher_.y(), higher_.z())
	};

	VectorT3<T> worldLower(NumericT<T>::maxValue(), NumericT<T>::maxValue(), NumericT<T>::maxValue());
	VectorT3<T> worldHigher(NumericT<T>::minValue(), NumericT<T>::minValue(), NumericT<T>::minValue());

	for (unsigned int n = 0u; n < 8u; ++n)
	{
		const VectorT3<T> worldCorner = world_T_box * corners[n];

		for (unsigned int axis = 0u; axis < 3u; ++axis)
		{
			if (worldCorner[axis] > worldHigher[axis])
			{
				worldHigher[axis] = worldCorner[axis];
			}

			if (worldCorner[axis] < worldLower[axis])
			{
				worldLower[axis] = worldCorner[axis];
			}
		}
	}

	return BoxT3<T>(worldLower, worldHigher);
}

template <typename T>
BoxT3<T>& BoxT3<T>::operator*=(const HomogenousMatrixT4<T>& world_T_box)
{
	ocean_assert(isValid() && world_T_box.isValid());

	*this = *this * world_T_box;

	return *this;
}

template <typename T>
bool BoxT3<T>::operator==(const BoxT3& right) const
{
	return isEqual(right, NumericT<T>::eps());
}

template <typename T>
bool BoxT3<T>::operator!=(const BoxT3& right) const
{
	return !(*this == right);
}

template <typename T>
BoxT3<T>::operator bool() const
{
	return isValid();
}

// Explicit instantiations
template class OCEAN_MATH_EXPORT BoxT3<float>;
template class OCEAN_MATH_EXPORT BoxT3<double>;

}
