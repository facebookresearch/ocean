/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/BoundingBox.h"
#include "ocean/math/SquareMatrix3.h"

#include "ocean/base/Utilities.h"

namespace Ocean
{

bool BoundingBox::positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	const Vector3& rayPoint(ray.point());
	const Vector3& rayDirection(ray.direction());

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// front size (z+)
		if (rayDirection.z() < -Numeric::eps() && rayPoint.z() >= higher_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, 1) < 0);

			// p + t * d == box
			// t = (box - p) / d
			/*const Scalar t = (higher_.z() - ray.point().z()) / ray.direction().z();
			const Scalar x = ray.point().x() + ray.direction().x() * t;
			const Scalar y = ray.point().y() + ray.direction().y() * t;

			if (x >= lower_.x() && x <= higher_.x()
				&& y >= lower_.y() && y <= higher_.y())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(x, y, higher_.z());
				return true;
			}*/

			// avoid division
			const Scalar xdz = rayPoint.x() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz <= lower_.x() * rayDirection.z() && xdz >= higher_.x() * rayDirection.z()
					&& ydz <= lower_.y() * rayDirection.z() && ydz >= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (higher_.z() - ray.point().z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, higher_.z());
				return true;
			}
		}

		// back size (z-)
		else if (ray.direction().z() > Numeric::eps() && ray.point().z() <= lower_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, -1) < 0);

			/*const Scalar t = (lower_.z() - ray.point().z()) / ray.direction(). z();
			const Scalar x = ray.point().x() + ray.direction().x() * t;
			const Scalar y = ray.point().y() + ray.direction().y() * t;

			if (x >= lower_.x() && x <= higher_.x()
				&& y >= lower_.y() && y <= higher_.y())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(x, y, lower_.z());
				return true;
			}*/

			const Scalar xdz = rayPoint.x() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz >= lower_.x() * rayDirection.z() && xdz <= higher_.x() * rayDirection.z()
					&& ydz >= lower_.y() * rayDirection.z() && ydz <= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (lower_.z() - rayPoint.z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, lower_.z());
				return true;
			}
		}
	}

	if ((rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// right size (x+)
		if (ray.direction().x() < -Numeric::eps() && ray.point().x() >= higher_.x())
		{
			ocean_assert(ray.direction() * Vector3(1, 0, 0) < 0);

			/*const Scalar t = (higher_.x() - ray.point().x()) / ray.direction().x();
			const Scalar y = ray.point().y() + ray.direction().y() * t;
			const Scalar z = ray.point().z() + ray.direction().z() * t;

			if (y >= lower_.y() && y <= higher_.y()
				&& z >= lower_.z() && z <= higher_.z())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(higher_.x(), y, z);
				return true;
			}*/

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx <= lower_.y() * rayDirection.x() && ydx >= higher_.y() * rayDirection.x()
					&& zdx <= lower_.z() * rayDirection.x() && zdx >= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (higher_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(higher_.x(), ydx * _dx, zdx * _dx);
				return true;
			}
		}

		// left size (x-)
		else if (ray.direction().x() > Numeric::eps() && ray.point().x() <= lower_.x())
		{
			ocean_assert(ray.direction() * Vector3(-1, 0, 0) < 0);

			/*const Scalar t = (lower_.x() - ray.point().x()) / ray.direction().x();
			const Scalar y = ray.point().y() + ray.direction().y() * t;
			const Scalar z = ray.point().z() + ray.direction().z() * t;

			if (y >= lower_.y() && y <= higher_.y()
				&& z >= lower_.z() && z <= higher_.z())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(lower_.x(), y, z);
				return true;
			}*/

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx >= lower_.y() * rayDirection.x() && ydx <= higher_.y() * rayDirection.x()
					&& zdx >= lower_.z() * rayDirection.x() && zdx <= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (lower_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(lower_.x(), ydx * _dx, zdx * _dx);
				return true;
			}
		}
	}

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z()))
	{
		// top size (y+)
		if (ray.direction().y() < -Numeric::eps() && ray.point().y() >= higher_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, 1, 0) < 0);

			/*const Scalar t = (higher_.y() - ray.point().y()) / ray.direction().y();
			const Scalar x = ray.point().x() + ray.direction().x() * t;
			const Scalar z = ray.point().z() + ray.direction().z() * t;

			if (x >= lower_.x() && x <= higher_.x()
				&& z >= lower_.z() && z <= higher_.z())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(x, higher_.y(), z);
				return true;
			}*/

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy <= lower_.x() * rayDirection.y() && xdy >= higher_.x() * rayDirection.y()
					&& zdy <= lower_.z() * rayDirection.y() && zdy >= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (higher_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, higher_.y(), zdy * _dy);
				return true;
			}
		}

		// bottom size (y-)
		else if (ray.direction().y() > Numeric::eps() && ray.point().y() <= lower_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, -1, 0) < 0);

			/*const Scalar t = (lower_.y() - ray.point().y()) / ray.direction().y();
			const Scalar x = ray.point().x() + ray.direction().x() * t;
			const Scalar z = ray.point().z() + ray.direction().z() * t;

			if (x >= lower_.x() && x <= higher_.x()
				&& z >= lower_.z() && z <= higher_.z())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(x, lower_.y(), z);
				return true;
			}*/

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy >= lower_.x() * rayDirection.y() && xdy <= higher_.x() * rayDirection.y()
					&& zdy >= lower_.z() * rayDirection.y() && zdy <= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (lower_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, lower_.y(), zdy * _dy);
				return true;
			}
		}
	}

	return false;
}

bool BoundingBox::positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	const Vector3& rayPoint(ray.point());
	const Vector3& rayDirection(ray.direction());

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// front size (z+)
		if (ray.direction().z() < -Numeric::eps() && ray.point().z() >= higher_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, 1) < 0);

			const Scalar xdz = rayPoint.x() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz <= lower_.x() * rayDirection.z() && xdz >= higher_.x() * rayDirection.z()
					&& ydz <= lower_.y() * rayDirection.z() && ydz >= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (higher_.z() - ray.point().z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, higher_.z());
				normal = Vector3(0, 0, 1);
				return true;
			}
		}

		// back size (z-)
		else if (ray.direction().z() > Numeric::eps() && ray.point().z() <= lower_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, -1) < 0);

			const Scalar xdz = rayPoint.x() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz >= lower_.x() * rayDirection.z() && xdz <= higher_.x() * rayDirection.z()
					&& ydz >= lower_.y() * rayDirection.z() && ydz <= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (lower_.z() - rayPoint.z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, lower_.z());
				normal = Vector3(0, 0, -1);
				return true;
			}
		}
	}

	if ((rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// right size (x+)
		if (ray.direction().x() < -Numeric::eps() && ray.point().x() >= higher_.x())
		{
			ocean_assert(ray.direction() * Vector3(1, 0, 0) < 0);

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx <= lower_.y() * rayDirection.x() && ydx >= higher_.y() * rayDirection.x()
					&& zdx <= lower_.z() * rayDirection.x() && zdx >= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (higher_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(higher_.x(), ydx * _dx, zdx * _dx);
				normal = Vector3(1, 0, 0);
				return true;
			}
		}

		// left size (x-)
		else if (ray.direction().x() > Numeric::eps() && ray.point().x() <= lower_.x())
		{
			ocean_assert(ray.direction() * Vector3(-1, 0, 0) < 0);

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx >= lower_.y() * rayDirection.x() && ydx <= higher_.y() * rayDirection.x()
					&& zdx >= lower_.z() * rayDirection.x() && zdx <= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (lower_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(lower_.x(), ydx * _dx, zdx * _dx);
				normal = Vector3(-1, 0, 0);
				return true;
			}
		}
	}

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z()))
	{
		// top size (y+)
		if (ray.direction().y() < -Numeric::eps() && ray.point().y() >= higher_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, 1, 0) < 0);

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy <= lower_.x() * rayDirection.y() && xdy >= higher_.x() * rayDirection.y()
					&& zdy <= lower_.z() * rayDirection.y() && zdy >= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (higher_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, higher_.y(), zdy * _dy);
				normal = Vector3(0, 1, 0);
				return true;
			}
		}

		// bottom size (y-)
		else if (ray.direction().y() > Numeric::eps() && ray.point().y() <= lower_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, -1, 0) < 0);

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy >= lower_.x() * rayDirection.y() && xdy <= higher_.x() * rayDirection.y()
					&& zdy >= lower_.z() * rayDirection.y() && zdy <= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (lower_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, lower_.y(), zdy * _dy);
				normal = Vector3(0, -1, 0);
				return true;
			}
		}
	}

	return false;
}

bool BoundingBox::positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal, Vector2& textureCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	const Scalar invDimensionX = xDimension() > Numeric::eps() ? Scalar(1) / xDimension() : Scalar(0);
	const Scalar invDimensionY = yDimension() > Numeric::eps() ? Scalar(1) / yDimension() : Scalar(0);
	const Scalar invDimensionZ = zDimension() > Numeric::eps() ? Scalar(1) / zDimension() : Scalar(0);

	const Vector3& rayPoint(ray.point());
	const Vector3& rayDirection(ray.direction());

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// front size (z+)
		if (ray.direction().z() < -Numeric::eps() && ray.point().z() >= higher_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, 1) < 0);

			const Scalar xdz = rayPoint.x() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz <= lower_.x() * rayDirection.z() && xdz >= higher_.x() * rayDirection.z()
					&& ydz <= lower_.y() * rayDirection.z() && ydz >= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (higher_.z() - ray.point().z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, higher_.z());
				normal = Vector3(0, 0, 1);
				textureCoordinate = Vector2(minmax(Scalar(0), (position.x() - lower_.x()) * invDimensionX, Scalar(1)), minmax(Scalar(0), (position.y() - lower_.y()) * invDimensionY, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}

		// back size (z-)
		else if (ray.direction().z() > Numeric::eps() && ray.point().z() <= lower_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, -1) < 0);

			const Scalar xdz = rayPoint.x() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz >= lower_.x() * rayDirection.z() && xdz <= higher_.x() * rayDirection.z()
					&& ydz >= lower_.y() * rayDirection.z() && ydz <= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (lower_.z() - rayPoint.z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, lower_.z());
				normal = Vector3(0, 0, -1);
				textureCoordinate = Vector2(minmax(Scalar(0), 1 - (position.x() - lower_.x()) * invDimensionX, Scalar(1)), minmax(Scalar(0), (position.y() - lower_.y()) * invDimensionY, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}
	}

	if ((rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// right size (x+)
		if (ray.direction().x() < -Numeric::eps() && ray.point().x() >= higher_.x())
		{
			ocean_assert(ray.direction() * Vector3(1, 0, 0) < 0);

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx <= lower_.y() * rayDirection.x() && ydx >= higher_.y() * rayDirection.x()
					&& zdx <= lower_.z() * rayDirection.x() && zdx >= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (higher_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(higher_.x(), ydx * _dx, zdx * _dx);
				normal = Vector3(1, 0, 0);
				textureCoordinate = Vector2(minmax(Scalar(0), 1 - (position.z() - lower_.z()) * invDimensionZ, Scalar(1)), minmax(Scalar(0), (position.y() - lower_.y()) * invDimensionY, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}

		// left size (x-)
		else if (ray.direction().x() > Numeric::eps() && ray.point().x() <= lower_.x())
		{
			ocean_assert(ray.direction() * Vector3(-1, 0, 0) < 0);

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx >= lower_.y() * rayDirection.x() && ydx <= higher_.y() * rayDirection.x()
					&& zdx >= lower_.z() * rayDirection.x() && zdx <= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (lower_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(lower_.x(), ydx * _dx, zdx * _dx);
				normal = Vector3(-1, 0, 0);
				textureCoordinate = Vector2(minmax(Scalar(0), (position.z() - lower_.z()) * invDimensionZ, Scalar(1)), minmax(Scalar(0), (position.y() - lower_.y()) * invDimensionY, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}
	}

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z()))
	{
		// top size (y+)
		if (ray.direction().y() < -Numeric::eps() && ray.point().y() >= higher_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, 1, 0) < 0);

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy <= lower_.x() * rayDirection.y() && xdy >= higher_.x() * rayDirection.y()
					&& zdy <= lower_.z() * rayDirection.y() && zdy >= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (higher_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, higher_.y(), zdy * _dy);
				normal = Vector3(0, 1, 0);
				textureCoordinate = Vector2(minmax(Scalar(0), (position.x() - lower_.x()) * invDimensionX, Scalar(1)), minmax(Scalar(0), 1 - (position.z() - lower_.z()) * invDimensionZ, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}

		// bottom size (y-)
		else if (ray.direction().y() > Numeric::eps() && ray.point().y() <= lower_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, -1, 0) < 0);

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy >= lower_.x() * rayDirection.y() && xdy <= higher_.x() * rayDirection.y()
					&& zdy >= lower_.z() * rayDirection.y() && zdy <= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (lower_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, lower_.y(), zdy * _dy);
				normal = Vector3(0, -1, 0);
				textureCoordinate = Vector2(minmax(Scalar(0), (position.x() - lower_.x()) * invDimensionX, Scalar(1)), minmax(Scalar(0), (position.z() - lower_.z()) * invDimensionZ, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}
	}

	return false;
}

bool BoundingBox::positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	const Vector3& rayPoint(ray.point());
	const Vector3& rayDirection(ray.direction());

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// front size (z+)
		if (ray.direction().z() > Numeric::eps() && ray.point().z() <= higher_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, 1) > 0);

			// p + t * d == box
			// t = (box - p) / d
			/*const Scalar t = (higher_.z() - ray.point().z()) / ray.direction().z();
			const Scalar x = ray.point().x() + ray.direction().x() * t;
			const Scalar y = ray.point().y() + ray.direction().y() * t;

			if (x >= lower_.x() && x <= higher_.x()
				&& y >= lower_.y() && y <= higher_.y())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(x, y, higher_.z());
				return true;
			}*/

			// avoid division
			const Scalar xdz = rayPoint.x() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz >= lower_.x() * rayDirection.z() && xdz <= higher_.x() * rayDirection.z()
					&& ydz >= lower_.y() * rayDirection.z() && ydz <= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (higher_.z() - ray.point().z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, higher_.z());
				return true;
			}
		}

		// back size (z-)
		else if (ray.direction().z() < -Numeric::eps() && ray.point().z() >= lower_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, -1) > 0);

			/*const Scalar t = (lower_.z() - ray.point().z()) / ray.direction(). z();
			const Scalar x = ray.point().x() + ray.direction().x() * t;
			const Scalar y = ray.point().y() + ray.direction().y() * t;

			if (x >= lower_.x() && x <= higher_.x()
				&& y >= lower_.y() && y <= higher_.y())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(x, y, lower_.z());
				return true;
			}*/

			const Scalar xdz = rayPoint.x() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz <= lower_.x() * rayDirection.z() && xdz >= higher_.x() * rayDirection.z()
					&& ydz <= lower_.y() * rayDirection.z() && ydz >= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (lower_.z() - rayPoint.z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, lower_.z());
				return true;
			}
		}
	}

	if ((rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// right size (x+)
		if (ray.direction().x() > Numeric::eps() && ray.point().x() <= higher_.x())
		{
			ocean_assert(ray.direction() * Vector3(1, 0, 0) > 0);

			/*const Scalar t = (higher_.x() - ray.point().x()) / ray.direction().x();
			const Scalar y = ray.point().y() + ray.direction().y() * t;
			const Scalar z = ray.point().z() + ray.direction().z() * t;

			if (y >= lower_.y() && y <= higher_.y()
				&& z >= lower_.z() && z <= higher_.z())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(higher_.x(), y, z);
				return true;
			}*/

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx >= lower_.y() * rayDirection.x() && ydx <= higher_.y() * rayDirection.x()
					&& zdx >= lower_.z() * rayDirection.x() && zdx <= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (higher_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(higher_.x(), ydx * _dx, zdx * _dx);
				return true;
			}
		}

		// left size (x-)
		else if (ray.direction().x() < -Numeric::eps() && ray.point().x() >= lower_.x())
		{
			ocean_assert(ray.direction() * Vector3(-1, 0, 0) > 0);

			/*const Scalar t = (lower_.x() - ray.point().x()) / ray.direction().x();
			const Scalar y = ray.point().y() + ray.direction().y() * t;
			const Scalar z = ray.point().z() + ray.direction().z() * t;

			if (y >= lower_.y() && y <= higher_.y()
				&& z >= lower_.z() && z <= higher_.z())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(lower_.x(), y, z);
				return true;
			}*/

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx <= lower_.y() * rayDirection.x() && ydx >= higher_.y() * rayDirection.x()
					&& zdx <= lower_.z() * rayDirection.x() && zdx >= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (lower_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(lower_.x(), ydx * _dx, zdx * _dx);
				return true;
			}
		}
	}

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z()))
	{
		// top size (y+)
		if (ray.direction().y() > Numeric::eps() && ray.point().y() <= higher_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, 1, 0) > 0);

			/*const Scalar t = (higher_.y() - ray.point().y()) / ray.direction().y();
			const Scalar x = ray.point().x() + ray.direction().x() * t;
			const Scalar z = ray.point().z() + ray.direction().z() * t;

			if (x >= lower_.x() && x <= higher_.x()
				&& z >= lower_.z() && z <= higher_.z())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(x, higher_.y(), z);
				return true;
			}*/

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy >= lower_.x() * rayDirection.y() && xdy <= higher_.x() * rayDirection.y()
					&& zdy >= lower_.z() * rayDirection.y() && zdy <= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (higher_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, higher_.y(), zdy * _dy);
				return true;
			}
		}

		// bottom size (y-)
		else if (ray.direction().y() < -Numeric::eps() && ray.point().y() >= lower_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, -1, 0) > 0);

			/*const Scalar t = (lower_.y() - ray.point().y()) / ray.direction().y();
			const Scalar x = ray.point().x() + ray.direction().x() * t;
			const Scalar z = ray.point().z() + ray.direction().z() * t;

			if (x >= lower_.x() && x <= higher_.x()
				&& z >= lower_.z() && z <= higher_.z())
			{
				distance = t;
				ocean_assert(t >= 0);

				position = Vector3(x, lower_.y(), z);
				return true;
			}*/

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy <= lower_.x() * rayDirection.y() && xdy >= higher_.x() * rayDirection.y()
					&& zdy <= lower_.z() * rayDirection.y() && zdy >= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (lower_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, lower_.y(), zdy * _dy);
				return true;
			}
		}
	}

	return false;
}

bool BoundingBox::positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	const Vector3& rayPoint(ray.point());
	const Vector3& rayDirection(ray.direction());

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// front size (z+)
		if (ray.direction().z() > Numeric::eps() && ray.point().z() <= higher_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, 1) > 0);

			// avoid division
			const Scalar xdz = rayPoint.x() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz >= lower_.x() * rayDirection.z() && xdz <= higher_.x() * rayDirection.z()
					&& ydz >= lower_.y() * rayDirection.z() && ydz <= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (higher_.z() - ray.point().z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, higher_.z());
				normal = Vector3(0, 0, 1);
				return true;
			}
		}

		// back size (z-)
		else if (ray.direction().z() < -Numeric::eps() && ray.point().z() >= lower_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, -1) > 0);

			const Scalar xdz = rayPoint.x() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz <= lower_.x() * rayDirection.z() && xdz >= higher_.x() * rayDirection.z()
					&& ydz <= lower_.y() * rayDirection.z() && ydz >= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (lower_.z() - rayPoint.z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, lower_.z());
				normal = Vector3(0, 0, -1);
				return true;
			}
		}
	}

	if ((rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// right size (x+)
		if (ray.direction().x() > Numeric::eps() && ray.point().x() <= higher_.x())
		{
			ocean_assert(ray.direction() * Vector3(1, 0, 0) > 0);

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx >= lower_.y() * rayDirection.x() && ydx <= higher_.y() * rayDirection.x()
					&& zdx >= lower_.z() * rayDirection.x() && zdx <= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (higher_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(higher_.x(), ydx * _dx, zdx * _dx);
				normal = Vector3(1, 0, 0);
				return true;
			}
		}

		// left size (x-)
		else if (ray.direction().x() < -Numeric::eps() && ray.point().x() >= lower_.x())
		{
			ocean_assert(ray.direction() * Vector3(-1, 0, 0) > 0);

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx <= lower_.y() * rayDirection.x() && ydx >= higher_.y() * rayDirection.x()
					&& zdx <= lower_.z() * rayDirection.x() && zdx >= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (lower_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(lower_.x(), ydx * _dx, zdx * _dx);
				normal = Vector3(-1, 0, 0);
				return true;
			}
		}
	}

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z()))
	{
		// top size (y+)
		if (ray.direction().y() > Numeric::eps() && ray.point().y() <= higher_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, 1, 0) > 0);

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy >= lower_.x() * rayDirection.y() && xdy <= higher_.x() * rayDirection.y()
					&& zdy >= lower_.z() * rayDirection.y() && zdy <= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (higher_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, higher_.y(), zdy * _dy);
				normal = Vector3(0, 1, 0);
				return true;
			}
		}

		// bottom size (y-)
		else if (ray.direction().y() < -Numeric::eps() && ray.point().y() >= lower_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, -1, 0) > 0);

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy <= lower_.x() * rayDirection.y() && xdy >= higher_.x() * rayDirection.y()
					&& zdy <= lower_.z() * rayDirection.y() && zdy >= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (lower_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, lower_.y(), zdy * _dy);
				normal = Vector3(0, -1, 0);
				return true;
			}
		}
	}

	return false;
}

bool BoundingBox::positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal, Vector2& textureCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	const Scalar invDimensionX = xDimension() > Numeric::eps() ? Scalar(1) / xDimension() : Scalar(0);
	const Scalar invDimensionY = yDimension() > Numeric::eps() ? Scalar(1) / yDimension() : Scalar(0);
	const Scalar invDimensionZ = zDimension() > Numeric::eps() ? Scalar(1) / zDimension() : Scalar(0);

	const Vector3& rayPoint(ray.point());
	const Vector3& rayDirection(ray.direction());

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// front size (z+)
		if (ray.direction().z() > Numeric::eps() && ray.point().z() <= higher_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, 1) > 0);

			// avoid division
			const Scalar xdz = rayPoint.x() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (higher_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz >= lower_.x() * rayDirection.z() && xdz <= higher_.x() * rayDirection.z()
					&& ydz >= lower_.y() * rayDirection.z() && ydz <= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (higher_.z() - ray.point().z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, higher_.z());
				normal = Vector3(0, 0, 1);
				textureCoordinate = Vector2(minmax(Scalar(0), (position.x() - lower_.x()) * invDimensionX, Scalar(1)), minmax(Scalar(0), (position.y() - lower_.y()) * invDimensionY, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}

		// back size (z-)
		else if (ray.direction().z() < -Numeric::eps() && ray.point().z() >= lower_.z())
		{
			ocean_assert(ray.direction() * Vector3(0, 0, -1) > 0);

			const Scalar xdz = rayPoint.x() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.x();
			const Scalar ydz = rayPoint.y() * rayDirection.z() + (lower_.z() - rayPoint.z()) * rayDirection.y();

			if (xdz <= lower_.x() * rayDirection.z() && xdz >= higher_.x() * rayDirection.z()
					&& ydz <= lower_.y() * rayDirection.z() && ydz >= higher_.y() * rayDirection.z())
			{
				const Scalar _dz = 1 / rayDirection.z();

				distance = (lower_.z() - rayPoint.z()) * _dz;
				ocean_assert(distance >= 0);

				position = Vector3(xdz * _dz, ydz * _dz, lower_.z());
				normal = Vector3(0, 0, -1);
				textureCoordinate = Vector2(minmax(Scalar(0), 1 - (position.x() - lower_.x()) * invDimensionX, Scalar(1)), minmax(Scalar(0), (position.y() - lower_.y()) * invDimensionY, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}
	}

	if ((rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z())
		&& (rayDirection.y() < 0 || rayPoint.y() <= higher_.y())
		&& (rayDirection.y() > 0 || rayPoint.y() >= lower_.y()))
	{
		// right size (x+)
		if (ray.direction().x() > Numeric::eps() && ray.point().x() <= higher_.x())
		{
			ocean_assert(ray.direction() * Vector3(1, 0, 0) > 0);

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (higher_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx >= lower_.y() * rayDirection.x() && ydx <= higher_.y() * rayDirection.x()
					&& zdx >= lower_.z() * rayDirection.x() && zdx <= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (higher_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(higher_.x(), ydx * _dx, zdx * _dx);
				normal = Vector3(1, 0, 0);
				textureCoordinate = Vector2(minmax(Scalar(0), 1 - (position.z() - lower_.z()) * invDimensionZ, Scalar(1)), minmax(Scalar(0), (position.y() - lower_.y()) * invDimensionY, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}

		// left size (x-)
		else if (ray.direction().x() < -Numeric::eps() && ray.point().x() >= lower_.x())
		{
			ocean_assert(ray.direction() * Vector3(-1, 0, 0) > 0);

			// avoid division
			const Scalar ydx = rayPoint.y() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.y();
			const Scalar zdx = rayPoint.z() * rayDirection.x() + (lower_.x() - rayPoint.x()) * rayDirection.z();

			if (ydx <= lower_.y() * rayDirection.x() && ydx >= higher_.y() * rayDirection.x()
					&& zdx <= lower_.z() * rayDirection.x() && zdx >= higher_.z() * rayDirection.x())
			{
				const Scalar _dx = 1 / rayDirection.x();

				distance = (lower_.x() - ray.point().x()) * _dx;
				ocean_assert(distance >= 0);

				position = Vector3(lower_.x(), ydx * _dx, zdx * _dx);
				normal = Vector3(-1, 0, 0);
				textureCoordinate = Vector2(minmax(Scalar(0), (position.z() - lower_.z()) * invDimensionZ, Scalar(1)), minmax(Scalar(0), (position.y() - lower_.y()) * invDimensionY, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}
	}

	if ((rayDirection.x() < 0 || rayPoint.x() <= higher_.x())
		&& (rayDirection.x() > 0 || rayPoint.x() >= lower_.x())
		&& (rayDirection.z() < 0 || rayPoint.z() <= higher_.z())
		&& (rayDirection.z() > 0 || rayPoint.z() >= lower_.z()))
	{
		// top size (y+)
		if (ray.direction().y() > Numeric::eps() && ray.point().y() <= higher_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, 1, 0) > 0);

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (higher_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy >= lower_.x() * rayDirection.y() && xdy <= higher_.x() * rayDirection.y()
					&& zdy >= lower_.z() * rayDirection.y() && zdy <= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (higher_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, higher_.y(), zdy * _dy);
				normal = Vector3(0, 1, 0);
				textureCoordinate = Vector2(minmax(Scalar(0), (position.x() - lower_.x()) * invDimensionX, Scalar(1)), minmax(Scalar(0), 1 - (position.z() - lower_.z()) * invDimensionZ, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}

		// bottom size (y-)
		else if (ray.direction().y() < -Numeric::eps() && ray.point().y() >= lower_.y())
		{
			ocean_assert(ray.direction() * Vector3(0, -1, 0) > 0);

			// avoid division
			const Scalar xdy = rayPoint.x() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.x();
			const Scalar zdy = rayPoint.z() * rayDirection.y() + (lower_.y() - rayPoint.y()) * rayDirection.z();

			if (xdy <= lower_.x() * rayDirection.y() && xdy >= higher_.x() * rayDirection.y()
					&& zdy <= lower_.z() * rayDirection.y() && zdy >= higher_.z() * rayDirection.y())
			{
				const Scalar _dy = 1 / rayDirection.y();

				distance = (lower_.y() - ray.point().y()) * _dy;
				ocean_assert(distance >= 0);

				position = Vector3(xdy * _dy, lower_.y(), zdy * _dy);
				normal = Vector3(0, -1, 0);
				textureCoordinate = Vector2(minmax(Scalar(0), (position.x() - lower_.x()) * invDimensionX, Scalar(1)), minmax(Scalar(0), (position.z() - lower_.z()) * invDimensionZ, Scalar(1)));
				ocean_assert(textureCoordinate.x() >= 0 && textureCoordinate.x() <= 1 && textureCoordinate.y() >= 0 && textureCoordinate.y() <= 1);

				return true;
			}
		}
	}

	return false;
}

}
